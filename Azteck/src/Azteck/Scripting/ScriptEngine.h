#pragma once

#include <filesystem>
#include <string>
#include <map>

#include "Azteck/Scene/Entity.h"

extern "C" {
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoImage MonoImage;
	typedef struct _MonoClassField MonoClassField;
}

namespace Azteck 
{
	enum class ScriptFieldType
	{
		None = 0,
		Float, Double,
		Bool, Char, Byte, Short, Int, Long,
		UByte, UShort, UInt, ULong,
		Vector2, Vector3, Vector4,
		Entity
	};

	struct ScriptField
	{
		ScriptFieldType type;
		std::string name;

		MonoClassField* classField;
	};

	class ScriptClass
	{
	public:
		ScriptClass() = default;
		ScriptClass(const std::string& classNamespace, const std::string& className, bool isCore = false);

		MonoObject* instantiate();
		MonoMethod* getMethod(const std::string& name, int parameterCount);
		MonoObject* invokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr);

		const std::map<std::string, ScriptField> getFields() const { return _fields; }

	private:
		std::string _classNamespace;
		std::string _className;

		std::map<std::string, ScriptField> _fields;

		MonoClass* _monoClass = nullptr;

		friend class ScriptEngine;
	};

	class ScriptInstance
	{
	public:
		ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity);

		void invokeOnCreate();
		void invokeOnUpdate(float ts);

		Ref<ScriptClass> getScriptClass() { return _scriptClass; }

		template<typename T>
		T getFieldValue(const std::string& name)
		{
			bool success = getFieldValueInternal(name, _fieldValueBuffer);
			if (!success)
				return T();

			return *(T*)_fieldValueBuffer;
		}

		template<typename T>
		void setFieldValue(const std::string& name, const T& value)
		{
			setFieldValueInternal(name, &value);
		}

	private:
		bool getFieldValueInternal(const std::string& name, void* buffer);
		bool setFieldValueInternal(const std::string& name, const	void* value);

	private:
		Ref<ScriptClass> _scriptClass;

		MonoObject* _instance = nullptr;
		MonoMethod* _constructor = nullptr;
		MonoMethod* _onCreateMethod = nullptr;
		MonoMethod* _onUpdateMethod = nullptr;

		inline static char _fieldValueBuffer[8];
	};

	class ScriptEngine
	{
	public:
		static void init();
		static void shutdown();

		static void loadAssembly(const std::filesystem::path& filepath);
		static void loadAppAssembly(const std::filesystem::path& filepath);

		static void onRuntimeStart(Scene* scene);
		static void onRuntimeStop();

		static bool entityClassExists(const std::string& fullClassName);
		static void onCreateEntity(Entity entity);
		static void onUpdateEntity(Entity entity, Timestep ts);

		static Scene* getSceneContext();
		static std::unordered_map<std::string, Ref<ScriptClass>> getEntityClasses();

		static MonoImage* getCoreAssemblyImage();

		static Ref<ScriptInstance> getEntityScriptInstance(UUID entityID);

	private:
		static void initMono();
		static void shutdownMono();

		static MonoObject* instantiateClass(MonoClass* monoClass);

		static void loadAssemblyClasses();

		friend class ScriptClass;
		friend class ScriptInstance;
	};
}
