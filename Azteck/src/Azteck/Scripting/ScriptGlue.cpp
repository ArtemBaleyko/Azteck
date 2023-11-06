#include "azpch.h"
#include "ScriptGlue.h"

#include "mono/metadata/object.h"
#include "mono/metadata/reflection.h"

#include "ScriptEngine.h"

#include "Azteck/Core/UUID.h"
#include "Azteck/Core/KeyCodes.h"
#include "Azteck/Core/Input.h"

#include "Azteck/Scene/Scene.h"
#include "Azteck/Scene/Entity.h"

#include "Azteck/Physics/Physics2D.h"

#include "box2d/b2_body.h"

namespace Azteck
{
#define AZ_ADD_INTERNAL_CALL(Name) mono_add_internal_call("Azteck.InternalCalls::" #Name, Name)

	static std::unordered_map<MonoType*, std::function<bool(Entity)>> _entityHasComponentFuncs;

	static void NativeLog(MonoString* string, int parameter)
	{
		char* cStr = mono_string_to_utf8(string);
		std::string str(cStr);
		mono_free(cStr);
		std::cout << str << ", " << parameter << std::endl;
	}

	static void NativeLog_Vector(glm::vec3* parameter, glm::vec3* outResult)
	{
		AZ_CORE_WARN("Value: {0}", *parameter);
		*outResult = glm::normalize(*parameter);
	}

	static float NativeLog_VectorDot(glm::vec3* parameter)
	{
		AZ_CORE_WARN("Value: {0}", *parameter);
		return glm::dot(*parameter, *parameter);
	}

	static MonoObject* GetScriptInstance(UUID entityID)
	{
		return ScriptEngine::getManagedInstance(entityID);
	}

	static bool Entity_HasComponent(UUID entityID, MonoReflectionType* componentType)
	{
		Scene* scene = ScriptEngine::getSceneContext();
		AZ_CORE_ASSERT(scene, "Scene is nullptr");

		Entity entity = scene->getEntityByUUID(entityID);
		AZ_CORE_ASSERT(entity, "Unknown entity");

		MonoType* managedType = mono_reflection_type_get_type(componentType);
		return _entityHasComponentFuncs.at(managedType)(entity);
	}

	static uint64_t Entity_FindEntityByName(MonoString* name)
	{
		char* nameCStr = mono_string_to_utf8(name);

		Scene* scene = ScriptEngine::getSceneContext();
		AZ_CORE_ASSERT(scene, "Scene is nullptr");

		Entity entity = scene->getEntityByName(nameCStr);
		mono_free(nameCStr);

		if (!entity)
			return 0;

		return entity.getUUID();
	}

	static void TransformComponent_GetTranslation(UUID entityID, glm::vec3* outTranslation)
	{
		Scene* scene = ScriptEngine::getSceneContext();
		AZ_CORE_ASSERT(scene, "Scene is nullptr");
		Entity entity = scene->getEntityByUUID(entityID);
		AZ_CORE_ASSERT(entity, "Unknown entity");

		*outTranslation = entity.getComponent<TransformComponent>().translation;
	}

	static void TransformComponent_SetTranslation(UUID entityID, glm::vec3* translation)
	{
		Scene* scene = ScriptEngine::getSceneContext();
		AZ_CORE_ASSERT(scene, "Scene is nullptr");
		Entity entity = scene->getEntityByUUID(entityID);
		AZ_CORE_ASSERT(entity, "Unknown entity");

		entity.getComponent<TransformComponent>().translation = *translation;
	}

	static void Rigidbody2DComponent_ApplyLinearImpulse(UUID entityID, glm::vec2* impulse, glm::vec2* point, bool wake)
	{
		Scene* scene = ScriptEngine::getSceneContext();
		AZ_CORE_ASSERT(scene, "Scene is nullptr");
		Entity entity = scene->getEntityByUUID(entityID);
		AZ_CORE_ASSERT(entity, "Entity is unknown");

		auto& rb2d = entity.getComponent<Rigidbody2DComponent>();
		b2Body* body = (b2Body*)rb2d.runtimeBody;

		body->ApplyLinearImpulse(b2Vec2(impulse->x, impulse->y), b2Vec2(point->x, point->y), wake);
	}

	static void Rigidbody2DComponent_ApplyLinearImpulseToCenter(UUID entityID, glm::vec2* impulse, bool wake)
	{
		Scene* scene = ScriptEngine::getSceneContext();
		AZ_CORE_ASSERT(scene, "Scene is nullptr");
		Entity entity = scene->getEntityByUUID(entityID);
		AZ_CORE_ASSERT(entity, "Entity is unknown");

		auto& rb2d = entity.getComponent<Rigidbody2DComponent>();
		b2Body* body = (b2Body*)rb2d.runtimeBody;
		body->ApplyLinearImpulseToCenter(b2Vec2(impulse->x, impulse->y), wake);
	}

	static void Rigidbody2DComponent_GetLinearVelocity(UUID entityID, glm::vec2* outLinearVelocity)
	{
		Scene* scene = ScriptEngine::getSceneContext();
		AZ_CORE_ASSERT(scene, "Scene is nullptr");
		Entity entity = scene->getEntityByUUID(entityID);
		AZ_CORE_ASSERT(entity, "Entity is unknown");

		auto& rb2d = entity.getComponent<Rigidbody2DComponent>();
		b2Body* body = (b2Body*)rb2d.runtimeBody;
		const b2Vec2& linearVelocity = body->GetLinearVelocity();
		*outLinearVelocity = glm::vec2(linearVelocity.x, linearVelocity.y);
	}

	static Rigidbody2DComponent::BodyType Rigidbody2DComponent_GetType(UUID entityID)
	{
		Scene* scene = ScriptEngine::getSceneContext();
		AZ_CORE_ASSERT(scene, "Scene is nullptr");
		Entity entity = scene->getEntityByUUID(entityID);
		AZ_CORE_ASSERT(entity, "Entity is unknown");

		auto& rb2d = entity.getComponent<Rigidbody2DComponent>();
		b2Body* body = (b2Body*)rb2d.runtimeBody;
		return Utils::rigidbody2DTypeFromBox2DBody(body->GetType());
	}

	static void Rigidbody2DComponent_SetType(UUID entityID, Rigidbody2DComponent::BodyType bodyType)
	{
		Scene* scene = ScriptEngine::getSceneContext();
		AZ_CORE_ASSERT(scene, "Scene is nullptr");
		Entity entity = scene->getEntityByUUID(entityID);
		AZ_CORE_ASSERT(entity, "Entity is unknown");

		auto& rb2d = entity.getComponent<Rigidbody2DComponent>();
		b2Body* body = (b2Body*)rb2d.runtimeBody;
		body->SetType(Utils::rigidbody2DTypeToBox2DBody(bodyType));
	}

	static bool Input_IsKeyDown(KeyCode keycode)
	{
		return Input::isKeyPressed(keycode);
	}

	template<typename... Component>
	static void registerComponent()
	{
		([]()
			{
				std::string_view typeName = typeid(Component).name();
				size_t pos = typeName.find_last_of(':');
				std::string_view structName = typeName.substr(pos + 1);
				std::string managedTypename = fmt::format("Azteck.{}", structName);

				MonoType* managedType = mono_reflection_type_from_name(managedTypename.data(), ScriptEngine::getCoreAssemblyImage());
				if (!managedType)
				{
					AZ_CORE_ERROR("Could not find component type {}", managedTypename);
					return;
				}
				_entityHasComponentFuncs[managedType] = [](Entity entity) { return entity.hasComponent<Component>(); };
			}(), ...);
	}

	template<typename... Component>
	static void registerComponent(ComponentGroup<Component...>)
	{
		registerComponent<Component...>();
	}

	void ScriptGlue::registerComponents()
	{
		_entityHasComponentFuncs.clear();
		registerComponent(AllComponents{});
	}

	void ScriptGlue::registerFunctions()
	{
		AZ_ADD_INTERNAL_CALL(NativeLog);
		AZ_ADD_INTERNAL_CALL(NativeLog_Vector);
		AZ_ADD_INTERNAL_CALL(NativeLog_VectorDot);

		AZ_ADD_INTERNAL_CALL(GetScriptInstance);
		AZ_ADD_INTERNAL_CALL(Entity_HasComponent);
		AZ_ADD_INTERNAL_CALL(Entity_FindEntityByName);

		AZ_ADD_INTERNAL_CALL(TransformComponent_GetTranslation);
		AZ_ADD_INTERNAL_CALL(TransformComponent_SetTranslation);

		AZ_ADD_INTERNAL_CALL(Rigidbody2DComponent_ApplyLinearImpulse);
		AZ_ADD_INTERNAL_CALL(Rigidbody2DComponent_ApplyLinearImpulseToCenter);

		AZ_ADD_INTERNAL_CALL(Input_IsKeyDown);
	}
}
