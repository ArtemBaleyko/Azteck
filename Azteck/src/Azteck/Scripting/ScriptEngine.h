#pragma once

#include <filesystem>
#include <string>

#include "Azteck/Scene/Entity.h"

extern "C" {
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoImage MonoImage;
}

namespace Azteck 
{
	class ScriptClass
	{
	public:
		ScriptClass() = default;
		ScriptClass(const std::string& classNamespace, const std::string& className, bool isCore = false);

		MonoObject* instantiate();
		MonoMethod* getMethod(const std::string& name, int parameterCount);
		MonoObject* invokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr);

	private:
		std::string _classNamespace;
		std::string _className;

		MonoClass* _monoClass = nullptr;
	};

	class ScriptInstance
	{
	public:
		ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity);

		void invokeOnCreate();
		void invokeOnUpdate(float ts);
	private:
		Ref<ScriptClass> _scriptClass;

		MonoObject* _instance = nullptr;
		MonoMethod* _constructor = nullptr;
		MonoMethod* _onCreateMethod = nullptr;
		MonoMethod* _onUpdateMethod = nullptr;
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

	private:
		static void initMono();
		static void shutdownMono();

		static MonoObject* instantiateClass(MonoClass* monoClass);

		static void loadAssemblyClasses();

		friend class ScriptClass;
		friend class ScriptInstance;
	};
}
