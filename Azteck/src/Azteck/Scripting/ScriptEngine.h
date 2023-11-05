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
	constexpr int FIELD_VALUE_SIZE = 16;

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

	class ScriptFieldInstance
	{
	public:
		ScriptFieldInstance()
		{
			memset(_buffer, 0, sizeof(_buffer));
		}

		template<typename T>
		T getValue()
		{
			static_assert(sizeof(T) <= FIELD_VALUE_SIZE, "Type too large!");
			return *(T*)_buffer;
		}

		template<typename T>
		void setValue(T value)
		{
			static_assert(sizeof(T) <= FIELD_VALUE_SIZE, "Type too large!");
			memcpy(_buffer, &value, sizeof(T));
		}

		ScriptField field;
	private:
		uint8_t _buffer[FIELD_VALUE_SIZE];

		friend class ScriptEngine;
		friend class ScriptInstance;
	};

	using ScriptFieldMap = std::unordered_map<std::string, ScriptFieldInstance>;

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
			static_assert(sizeof(T) <= FIELD_VALUE_SIZE, "Type too large!");

			bool success = getFieldValueInternal(name, _fieldValueBuffer);
			if (!success)
				return T();

			return *(T*)_fieldValueBuffer;
		}

		template<typename T>
		void setFieldValue(const std::string& name, T value)
		{
			static_assert(sizeof(T) <= FIELD_VALUE_SIZE, "Type too large!");

			setFieldValueInternal(name, &value);
		}

	private:
		bool getFieldValueInternal(const std::string& name, void* buffer);
		bool setFieldValueInternal(const std::string& name, const void* value);

	private:
		Ref<ScriptClass> _scriptClass;

		MonoObject* _instance = nullptr;
		MonoMethod* _constructor = nullptr;
		MonoMethod* _onCreateMethod = nullptr;
		MonoMethod* _onUpdateMethod = nullptr;

		inline static char _fieldValueBuffer[FIELD_VALUE_SIZE];

		friend class ScriptEngine;
		friend class ScriptFieldInstance;
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
		static Ref<ScriptClass> getEntityClass(const std::string& name);
		static std::unordered_map<std::string, Ref<ScriptClass>> getEntityClasses();
		static ScriptFieldMap& getScriptFieldMap(Entity entity);

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

	namespace Utils {

		inline const char* scriptFieldTypeToString(ScriptFieldType fieldType)
		{
			switch (fieldType)
			{
				case ScriptFieldType::None:    return "None";
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

			AZ_CORE_ASSERT(false, "Unknown ScriptFieldType");
			return "None";
		}

		inline ScriptFieldType scriptFieldTypeFromString(std::string_view fieldType)
		{
			if (fieldType == "None")    return ScriptFieldType::None;
			if (fieldType == "Float")   return ScriptFieldType::Float;
			if (fieldType == "Double")  return ScriptFieldType::Double;
			if (fieldType == "Bool")    return ScriptFieldType::Bool;
			if (fieldType == "Char")    return ScriptFieldType::Char;
			if (fieldType == "Byte")    return ScriptFieldType::Byte;
			if (fieldType == "Short")   return ScriptFieldType::Short;
			if (fieldType == "Int")     return ScriptFieldType::Int;
			if (fieldType == "Long")    return ScriptFieldType::Long;
			if (fieldType == "UByte")   return ScriptFieldType::UByte;
			if (fieldType == "UShort")  return ScriptFieldType::UShort;
			if (fieldType == "UInt")    return ScriptFieldType::UInt;
			if (fieldType == "ULong")   return ScriptFieldType::ULong;
			if (fieldType == "Vector2") return ScriptFieldType::Vector2;
			if (fieldType == "Vector3") return ScriptFieldType::Vector3;
			if (fieldType == "Vector4") return ScriptFieldType::Vector4;
			if (fieldType == "Entity")  return ScriptFieldType::Entity;

			AZ_CORE_ASSERT(false, "Unknown ScriptFieldType");
			return ScriptFieldType::None;
		}

	}
}
