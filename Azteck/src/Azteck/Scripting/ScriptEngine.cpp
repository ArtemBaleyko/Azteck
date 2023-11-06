#include "azpch.h"
#include "ScriptEngine.h"

#include "ScriptGlue.h"

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"
#include "mono/metadata/tabledefs.h"
#include "mono/metadata/mono-debug.h"
#include "mono/metadata/threads.h"

#include "FileWatch.h"

#include "Azteck/Core/Application.h"
#include "Azteck/Core/Timer.h"
#include "Azteck/Core/Buffers.h"
#include "Azteck/Core/FileSystem.h"

namespace Azteck 
{
	static std::unordered_map<std::string, ScriptFieldType> scriptFieldTypeMap =
	{
		{ "System.Single", ScriptFieldType::Float },
		{ "System.Double", ScriptFieldType::Double },
		{ "System.Boolean", ScriptFieldType::Bool },
		{ "System.Char", ScriptFieldType::Char },
		{ "System.Int16", ScriptFieldType::Short },
		{ "System.Int32", ScriptFieldType::Int },
		{ "System.Int64", ScriptFieldType::Long },
		{ "System.Byte", ScriptFieldType::Byte },
		{ "System.UInt16", ScriptFieldType::UShort },
		{ "System.UInt32", ScriptFieldType::UInt },
		{ "System.UInt64", ScriptFieldType::ULong },

		{ "Azteck.Vector2", ScriptFieldType::Vector2 },
		{ "Azteck.Vector3", ScriptFieldType::Vector3 },
		{ "Azteck.Vector4", ScriptFieldType::Vector4 },

		{ "Azteck.Entity", ScriptFieldType::Entity },
	};

	namespace Utils 
	{
		static MonoAssembly* loadMonoAssembly(const std::filesystem::path& assemblyPath, bool loadPDB = false)
		{
			ScopedBuffer fileData = FileSystem::readFileBinary(assemblyPath);

			// NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
			MonoImageOpenStatus status;
			MonoImage* image = mono_image_open_from_data_full(fileData.as<char>(), fileData.size(), 1, &status, 0);

			if (status != MONO_IMAGE_OK)
			{
				const char* errorMessage = mono_image_strerror(status);

				AZ_CORE_ERROR(errorMessage);
				return nullptr;
			}

			if (loadPDB)
			{
				std::filesystem::path pdbPath = assemblyPath;
				pdbPath.replace_extension(".pdb");

				if (std::filesystem::exists(pdbPath))
				{
					ScopedBuffer pdbFileData = FileSystem::readFileBinary(pdbPath);
					mono_debug_open_image_from_memory(image, pdbFileData.as<const mono_byte>(), pdbFileData.size());

					AZ_CORE_INFO("Loaded PDB {}", pdbPath);
				}
			}

			std::string pathString = assemblyPath.string();
			MonoAssembly* assembly = mono_assembly_load_from_full(image, pathString.c_str(), &status, 0);
			mono_image_close(image);

			return assembly;
		}

		void printAssemblyTypes(MonoAssembly* assembly)
		{
			MonoImage* image = mono_assembly_get_image(assembly);
			const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
			int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

			for (int32_t i = 0; i < numTypes; i++)
			{
				uint32_t cols[MONO_TYPEDEF_SIZE];
				mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

				const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
				const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

				AZ_CORE_TRACE("{}.{}", nameSpace, name);
			}
		}

		ScriptFieldType monoTypeToScriptFieldType(MonoType* monoType)
		{
			std::string typeName = mono_type_get_name(monoType);

			auto it = scriptFieldTypeMap.find(typeName);
			if (it == scriptFieldTypeMap.end())
			{
				AZ_CORE_ERROR("Unknown type: {}", typeName);
				return ScriptFieldType::None;
			}

			return it->second;
		}

		const char* ScriptFieldTypeToString(ScriptFieldType type)
		{
			switch (type)
			{
				case ScriptFieldType::Float:   return "Float";
				case ScriptFieldType::Double:  return "Double";
				case ScriptFieldType::Bool:    return "Bool";
				case ScriptFieldType::Char:    return "Char";
				case ScriptFieldType::Byte:    return "Byte";
				case ScriptFieldType::Short:   return "Short";
				case ScriptFieldType::Int:     return "Int";
				case ScriptFieldType::Long:    return "Long";
				case ScriptFieldType::UByte:   return "UByte";
				case ScriptFieldType::UShort:  return "UShort";
				case ScriptFieldType::UInt:    return "UInt";
				case ScriptFieldType::ULong:   return "ULong";
				case ScriptFieldType::Vector2: return "Vector2";
				case ScriptFieldType::Vector3: return "Vector3";
				case ScriptFieldType::Vector4: return "Vector4";
				case ScriptFieldType::Entity:  return "Entity";
			}
			return "<Invalid>";
		}
	}

	struct ScriptEngineData
	{
		MonoDomain* rootDomain = nullptr;
		MonoDomain* appDomain = nullptr;

		MonoAssembly* coreAssembly = nullptr;
		MonoImage* coreAssemblyImage = nullptr;

		MonoAssembly* appAssembly = nullptr;
		MonoImage* appAssemblyImage = nullptr;

		std::filesystem::path coreAssemblyFilepath;
		std::filesystem::path appAssemblyFilepath;

		ScriptClass entityClass;

		std::unordered_map<std::string, Ref<ScriptClass>> entityClasses;
		std::unordered_map<UUID, Ref<ScriptInstance>> entityInstances;
		std::unordered_map<UUID, ScriptFieldMap> entityScriptFields;

		Scope<filewatch::FileWatch<std::string>> appAssemblyFileWatcher;
		bool assemblyReloadPending = false;

#ifdef AZ_DEBUG
		bool enableDebugging = true;
#else
		bool enableDebugging = false;
#endif
		// Runtime
		Scene* SceneContext = nullptr;
	};

	static ScriptEngineData* _data = nullptr;

	static void onAppAssemblyFileSystemEvent(const std::string& path, const filewatch::Event change_type)
	{
		if (!_data->assemblyReloadPending && change_type == filewatch::Event::modified)
		{
			_data->assemblyReloadPending = true;

			Application::getInstance().submitToMainThread([path]()
				{
					_data->appAssemblyFileWatcher.reset();
					ScriptEngine::reloadAssembly();

					AZ_CORE_TRACE("{} has been reloaded", path);
				});
		}
	}

	void ScriptEngine::init()
	{
		_data = new ScriptEngineData();

		initMono();
		ScriptGlue::registerFunctions();

		if (!loadAssembly("resources/scripts/Azteck-ScriptCore.dll"))
		{
			AZ_CORE_ERROR("[ScriptEngine] Could not load Azteck-ScriptCore assembly");
			return;
		}

		if (!loadAppAssembly("SandboxProject/assets/scripts/binaries/Sandbox.dll"))
		{
			AZ_CORE_ERROR("[ScriptEngine] Could not load app assembly");
			return;
		}

		loadAssemblyClasses();

		ScriptGlue::registerComponents();

		// Retrieve and instantiate class (with constructor)
		_data->entityClass = ScriptClass("Azteck", "Entity", true);
	}

	void ScriptEngine::shutdown()
	{
		shutdownMono();
		delete _data;
	}


	void ScriptEngine::initMono()
	{
		mono_set_assemblies_path("mono/lib");

		if (_data->enableDebugging)
		{
			const char* argv[2] = {
				"--debugger-agent=transport=dt_socket,address=127.0.0.1:2550,server=y,suspend=n,loglevel=3,logfile=MonoDebugger.log",
				"--soft-breakpoints"
			};

			mono_jit_parse_options(2, (char**)argv);
			mono_debug_init(MONO_DEBUG_FORMAT_MONO);
		}

		MonoDomain* rootDomain = mono_jit_init("AzteckJITRuntime");
		AZ_CORE_ASSERT(rootDomain, "Root domain is not initialized");

		_data->rootDomain = rootDomain;

		if (_data->enableDebugging)
			mono_debug_domain_create(_data->rootDomain);

		mono_thread_set_main(mono_thread_current());
	}

	void ScriptEngine::shutdownMono()
	{
		mono_domain_set(mono_get_root_domain(), false);

		mono_domain_unload(_data->appDomain);
		_data->appDomain = nullptr;

		mono_jit_cleanup(_data->rootDomain);
		_data->rootDomain = nullptr;
	}

	bool ScriptEngine::loadAssembly(const std::filesystem::path& filepath)
	{
		_data->appDomain = mono_domain_create_appdomain("AzteckScriptRuntime", nullptr);
		mono_domain_set(_data->appDomain, true);

		_data->coreAssemblyFilepath = filepath;
		_data->coreAssembly = Utils::loadMonoAssembly(filepath, _data->enableDebugging);

		if (_data->coreAssembly == nullptr)
			return false;

		_data->coreAssemblyImage = mono_assembly_get_image(_data->coreAssembly);

		return true;
	}

	bool ScriptEngine::loadAppAssembly(const std::filesystem::path& filepath)
	{
		_data->appAssemblyFilepath = filepath;
		_data->appAssembly = Utils::loadMonoAssembly(filepath, _data->enableDebugging);

		if (_data->appAssembly == nullptr)
			return false;

		_data->appAssemblyImage = mono_assembly_get_image(_data->appAssembly);

		_data->appAssemblyFileWatcher = createScope<filewatch::FileWatch<std::string>>(filepath.string(), onAppAssemblyFileSystemEvent);
		_data->assemblyReloadPending = false;

		return true;
	}

	void ScriptEngine::reloadAssembly()
	{
		mono_domain_set(mono_get_root_domain(), false);

		mono_domain_unload(_data->appDomain);

		loadAssembly(_data->coreAssemblyFilepath);
		loadAppAssembly(_data->appAssemblyFilepath);
		loadAssemblyClasses();

		ScriptGlue::registerComponents();

		// Retrieve and instantiate class
		_data->entityClass = ScriptClass("Azteck", "Entity", true);
	}

	void ScriptEngine::onRuntimeStart(Scene* scene)
	{
		_data->SceneContext = scene;
	}

	bool ScriptEngine::entityClassExists(const std::string& fullClassName)
	{
		return _data->entityClasses.find(fullClassName) != _data->entityClasses.end();
	}

	void ScriptEngine::onCreateEntity(Entity entity)
	{
		const auto& sc = entity.getComponent<ScriptComponent>();
		if (ScriptEngine::entityClassExists(sc.className))
		{
			UUID entityID = entity.getUUID();

			Ref<ScriptInstance> instance = createRef<ScriptInstance>(_data->entityClasses[sc.className], entity);
			_data->entityInstances[entityID] = instance;

			// Copy field values
			if (_data->entityScriptFields.find(entityID) != _data->entityScriptFields.end())
			{
				const ScriptFieldMap& fieldMap = _data->entityScriptFields.at(entityID);
				for (const auto& [name, fieldInstance] : fieldMap)
					instance->setFieldValueInternal(name, fieldInstance._buffer);
			}

			instance->invokeOnCreate();
		}
	}

	void ScriptEngine::onUpdateEntity(Entity entity, Timestep ts)
	{
		UUID entityUUID = entity.getUUID();
		if (_data->entityInstances.find(entityUUID) != _data->entityInstances.end())
		{
			Ref<ScriptInstance> instance = _data->entityInstances[entityUUID];
			instance->invokeOnUpdate((float)ts);
		}
		else
		{
			AZ_CORE_ERROR("Could not find ScriptInstance for entity {}", entityUUID);
		}
	}

	Scene* ScriptEngine::getSceneContext()
	{
		return _data->SceneContext;
	}

	Ref<ScriptClass> ScriptEngine::getEntityClass(const std::string& name)
	{
		auto it = _data->entityClasses.find(name);
		if (it == _data->entityClasses.end())
			return nullptr;

		return it->second;
	}

	void ScriptEngine::onRuntimeStop()
	{
		_data->SceneContext = nullptr;

		_data->entityInstances.clear();
	}

	std::unordered_map<std::string, Ref<ScriptClass>> ScriptEngine::getEntityClasses()
	{
		return _data->entityClasses;
	}

	ScriptFieldMap& ScriptEngine::getScriptFieldMap(Entity entity)
	{
		AZ_CORE_ASSERT(entity, "Invalid entity");

		UUID entityID = entity.getUUID();
		return _data->entityScriptFields[entityID];
	}

	void ScriptEngine::loadAssemblyClasses()
	{
		_data->entityClasses.clear();

		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(_data->appAssemblyImage, MONO_TABLE_TYPEDEF);

		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
		MonoClass* entityClass = mono_class_from_name(_data->coreAssemblyImage, "Azteck", "Entity");

		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(_data->appAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* className = mono_metadata_string_heap(_data->appAssemblyImage, cols[MONO_TYPEDEF_NAME]);
			std::string fullName;
			if (strlen(nameSpace) != 0)
				fullName = fmt::format("{}.{}", nameSpace, className);
			else
				fullName = className;

			MonoClass* monoClass = mono_class_from_name(_data->appAssemblyImage, nameSpace, className);

			if (monoClass == entityClass)
				continue;

			bool isEntity = mono_class_is_subclass_of(monoClass, entityClass, false);
			if (!isEntity)
				continue;

			Ref<ScriptClass> scriptClass = createRef<ScriptClass>(nameSpace, className);
			_data->entityClasses[fullName] = scriptClass;

			int fieldCount = mono_class_num_fields(monoClass);
			AZ_CORE_WARN("{} has {} fields:", className, fieldCount);
			
			void* iterator = nullptr;
			while (MonoClassField* field = mono_class_get_fields(monoClass, &iterator))
			{
				const char* fieldName = mono_field_get_name(field);
				uint32_t flags = mono_field_get_flags(field);
				if (flags & FIELD_ATTRIBUTE_PUBLIC)
				{
					MonoType* type = mono_field_get_type(field);
					ScriptFieldType fieldType = Utils::monoTypeToScriptFieldType(type);
					AZ_CORE_WARN("  {} ({})", fieldName, Utils::ScriptFieldTypeToString(fieldType));

					scriptClass->_fields[fieldName] = { fieldType, fieldName, field };
				}
			}
		}
	}

	MonoImage* ScriptEngine::getCoreAssemblyImage()
	{
		return _data->coreAssemblyImage;
	}

	Ref<ScriptInstance> ScriptEngine::getEntityScriptInstance(UUID entityID)
	{
		auto it = _data->entityInstances.find(entityID);
		if (it == _data->entityInstances.end())
			return nullptr;

		return it->second;
	}

	MonoObject* ScriptEngine::getManagedInstance(UUID uuid)
	{
		AZ_CORE_ASSERT(_data->entityInstances.find(uuid) != _data->entityInstances.end(), "Entity is not found");
		return _data->entityInstances.at(uuid)->getManagedObject();
	}

	MonoObject* ScriptEngine::instantiateClass(MonoClass* monoClass)
	{
		MonoObject* instance = mono_object_new(_data->appDomain, monoClass);
		mono_runtime_object_init(instance);
		return instance;
	}

	ScriptClass::ScriptClass(const std::string& classNamespace, const std::string& className, bool isCore)
		: _classNamespace(classNamespace), _className(className)
	{
		_monoClass = mono_class_from_name(isCore ? _data->coreAssemblyImage : _data->appAssemblyImage, 
			classNamespace.c_str(), className.c_str());
	}

	MonoObject* ScriptClass::instantiate()
	{
		return ScriptEngine::instantiateClass(_monoClass);
	}

	MonoMethod* ScriptClass::getMethod(const std::string& name, int parameterCount)
	{
		return mono_class_get_method_from_name(_monoClass, name.c_str(), parameterCount);
	}

	MonoObject* ScriptClass::invokeMethod(MonoObject* instance, MonoMethod* method, void** params)
	{
		MonoObject* exception = nullptr;
		return mono_runtime_invoke(method, instance, params, &exception);
	}

	ScriptInstance::ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity)
		: _scriptClass(scriptClass)
	{
		_instance = scriptClass->instantiate();

		_constructor = _data->entityClass.getMethod(".ctor", 1);
		_onCreateMethod = scriptClass->getMethod("OnCreate", 0);
		_onUpdateMethod = scriptClass->getMethod("OnUpdate", 1);

		UUID entityID = entity.getUUID();
		void* param = &entityID;
		_scriptClass->invokeMethod(_instance, _constructor, &param);
	}

	void ScriptInstance::invokeOnCreate()
	{
		if (_onCreateMethod)
			_scriptClass->invokeMethod(_instance, _onCreateMethod);
	}

	void ScriptInstance::invokeOnUpdate(float ts)
	{
		if (_onUpdateMethod)
		{
			void* param = &ts;
			_scriptClass->invokeMethod(_instance, _onUpdateMethod, &param);
		}
	}

	bool ScriptInstance::getFieldValueInternal(const std::string& name, void* buffer)
	{
		const auto& fields = _scriptClass->getFields();

		auto it = fields.find(name);
		if (it == fields.end())
			return false;

		const ScriptField& field = it->second;
		mono_field_get_value(_instance, field.classField, buffer);

		return true;
	}

	bool ScriptInstance::setFieldValueInternal(const std::string& name, const void* value)
	{
		const auto& fields = _scriptClass->getFields();

		auto it = fields.find(name);
		if (it == fields.end())
			return false;

		const ScriptField& field = it->second;
		mono_field_set_value(_instance, field.classField, (void*)value);

		return true;
	}
}
