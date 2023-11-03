#include "azpch.h"
#include "ScriptEngine.h"

#include "ScriptGlue.h"

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"

namespace Azteck 
{
	namespace Utils 
	{
		// TODO: move to FileSystem class
		static char* readBytes(const std::filesystem::path& filepath, uint32_t* outSize)
		{
			std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

			if (!stream)
			{
				// Failed to open the file
				return nullptr; 
			}

			std::streampos end = stream.tellg();
			stream.seekg(0, std::ios::beg);
			uint64_t size = end - stream.tellg();

			if (size == 0)
			{
				// File is empty
				return nullptr;
			}

			char* buffer = new char[size];
			stream.read((char*)buffer, size);
			stream.close();

			*outSize = (uint32_t)size;
			return buffer;
		}

		static MonoAssembly* loadMonoAssembly(const std::filesystem::path& assemblyPath)
		{
			uint32_t fileSize = 0;
			char* fileData = readBytes(assemblyPath, &fileSize);

			// NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
			MonoImageOpenStatus status;
			MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

			if (status != MONO_IMAGE_OK)
			{
				const char* errorMessage = mono_image_strerror(status);

				AZ_CORE_ERROR(errorMessage);
				return nullptr;
			}

			std::string pathString = assemblyPath.string();
			MonoAssembly* assembly = mono_assembly_load_from_full(image, pathString.c_str(), &status, 0);
			mono_image_close(image);

			delete[] fileData;

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

	}

	struct ScriptEngineData
	{
		MonoDomain* rootDomain = nullptr;
		MonoDomain* appDomain = nullptr;

		MonoAssembly* coreAssembly = nullptr;
		MonoImage* coreAssemblyImage = nullptr;

		ScriptClass entityClass;
	};

	static ScriptEngineData* _data = nullptr;

	void ScriptEngine::init()
	{
		_data = new ScriptEngineData();

		initMono();
		loadAssembly("resources/scripts/Azteck-ScriptCore.dll");

		ScriptGlue::registerFunctions();

		// Retrieve and instantiate class (with constructor)
		_data->entityClass = ScriptClass("Azteck", "Entity");

		MonoObject* instance = _data->entityClass.instantiate();

		// Call method
		MonoMethod* printMessageFunc = _data->entityClass.getMethod("PrintMessage", 0);
		_data->entityClass.invokeMethod(instance, printMessageFunc);

		// Call method with param
		MonoMethod* printIntFunc = _data->entityClass.getMethod("PrintInt", 1);

		int value = 5;
		void* param = &value;

		_data->entityClass.invokeMethod(instance, printIntFunc, &param);

		MonoMethod* printIntsFunc = _data->entityClass.getMethod("PrintInts", 2);
		int value2 = 508;
		void* params[2] =
		{
			&value,
			&value2
		};
		_data->entityClass.invokeMethod(instance, printIntsFunc, params);

		MonoString* monoString = mono_string_new(_data->appDomain, "Hello World from C++!");
		MonoMethod* printCustomMessageFunc = _data->entityClass.getMethod("PrintCustomMessage", 1);
		void* stringParam = monoString;
		_data->entityClass.invokeMethod(instance, printCustomMessageFunc, &stringParam);
	}

	void ScriptEngine::shutdown()
	{
		shutdownMono();
		delete _data;
	}


	void ScriptEngine::initMono()
	{
		mono_set_assemblies_path("mono/lib");

		MonoDomain* rootDomain = mono_jit_init("AzteckJITRuntime");
		AZ_CORE_ASSERT(rootDomain, "Root domain is not initialized");

		_data->rootDomain = rootDomain;
	}

	void ScriptEngine::shutdownMono()
	{
		// mono_domain_unload(_data->AppDomain);
		_data->appDomain = nullptr;

		// mono_jit_cleanup(_data->RootDomain);
		_data->rootDomain = nullptr;
	}

	void ScriptEngine::loadAssembly(const std::filesystem::path& filepath)
	{
		_data->appDomain = mono_domain_create_appdomain("AzteckScriptRuntime", nullptr);
		mono_domain_set(_data->appDomain, true);

		_data->coreAssembly = Utils::loadMonoAssembly(filepath);
		_data->coreAssemblyImage = mono_assembly_get_image(_data->coreAssembly);
	}

	MonoObject* ScriptEngine::instantiateClass(MonoClass* monoClass)
	{
		MonoObject* instance = mono_object_new(_data->appDomain, monoClass);
		mono_runtime_object_init(instance);
		return instance;
	}

	ScriptClass::ScriptClass(const std::string& classNamespace, const std::string& className)
		: _classNamespace(classNamespace), _className(className)
	{
		_monoClass = mono_class_from_name(_data->coreAssemblyImage, classNamespace.c_str(), className.c_str());
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
		return mono_runtime_invoke(method, instance, params, nullptr);
	}
}
