#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "Azteck/Scene/SceneCamera.h"
#include "Azteck/Renderer/Texture.h"
#include "Azteck/Core/UUID.h"

namespace Azteck
{
	struct IDComponent
	{
		IDComponent() = default;
		IDComponent(const IDComponent&) = default;

		UUID id;
	};

	struct TagComponent
	{
		TagComponent() = default;
		TagComponent(const TagComponent&) = default;

		TagComponent(const std::string& tag)
			: tag(tag)
		{}

		std::string tag;
	};

	struct TransformComponent
	{
		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;

		TransformComponent(const glm::vec3& translation)
			: translation(translation)
		{}

		glm::mat4 getTransform() const
		{
			glm::mat4 rotationMatrix = glm::toMat4(glm::quat(rotation));

			return glm::translate(glm::mat4(1.0f), translation)
				* rotationMatrix
				* glm::scale(glm::mat4(1.0f), scale);
		}

		glm::vec3 translation{ 0.0f };
		glm::vec3 rotation{ 0.0f };
		glm::vec3 scale{ 1.0f };
	};

	struct SpriteRendererComponent
	{
		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;

		SpriteRendererComponent(const glm::vec4& color)
			: color(color)
		{}

		glm::vec4 color{1.0f};
		Ref<Texture2D> texture;
		float tilingFactor = 1.0f;
	};

	struct CircleRendererComponent
	{
		CircleRendererComponent() = default;
		CircleRendererComponent(const CircleRendererComponent&) = default;

		glm::vec4 color{ 1.0f };
		float thickness = 1.0f;
		float fade = 0.005f;
	};

	struct CameraComponent
	{
		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;

		SceneCamera camera;
		bool primary = true;
		bool fixedAspectRatio = false;
	};

	struct ScriptComponent
	{
		ScriptComponent() = default;
		ScriptComponent(const ScriptComponent&) = default;

		std::string className;
	};

	class ScriptableEntity;

	struct NativeScriptComponent
	{
		ScriptableEntity* instance = nullptr;

		ScriptableEntity* (*instantiateScript)();
		void (*destroyScript)(NativeScriptComponent*);

		template<typename T>
		void bind()
		{
			instantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			destroyScript = [](NativeScriptComponent* nsc) { delete nsc->instance; nsc->instance = nullptr; };
		}
	};

	//Physics
	struct Rigidbody2DComponent
	{
		enum class BodyType
		{
			Static = 0,
			Dynamic,
			Kinematic
		};

		Rigidbody2DComponent() = default;
		Rigidbody2DComponent(const Rigidbody2DComponent&) = default;

		BodyType type = BodyType::Static;
		bool fixedRotation = false;

		// Storage for runtime
		void* runtimeBody = nullptr;
	};

	struct BoxCollider2DComponent
	{
		BoxCollider2DComponent() = default;
		BoxCollider2DComponent(const BoxCollider2DComponent&) = default;

		float density = 1.0f;
		float friction = 0.5f;
		float restitution = 0.0f;
		float restitutionThreshold = 0.5f;

		glm::vec2 offset{ 0.0f };
		glm::vec2 size{ 0.5f };

		// Storage for runtime
		void* runtimeFixture = nullptr;
	};

	struct CircleCollider2DComponent
	{
		CircleCollider2DComponent() = default;
		CircleCollider2DComponent(const CircleCollider2DComponent&) = default;

		glm::vec2 offset = { 0.0f, 0.0f };
		float radius = 0.5f;

		float density = 1.0f;
		float friction = 0.5f;
		float restitution = 0.0f;
		float restitutionThreshold = 0.5f;

		// Storage for runtime
		void* runtimeFixture = nullptr;
	};

	template<typename... Component>
	struct ComponentGroup
	{
	};

	using AllComponents =
		ComponentGroup<TransformComponent, SpriteRendererComponent,
		CircleRendererComponent, CameraComponent, ScriptComponent,
		NativeScriptComponent, Rigidbody2DComponent, BoxCollider2DComponent,
		CircleCollider2DComponent>;
}
