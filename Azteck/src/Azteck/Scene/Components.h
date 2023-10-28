#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Azteck/Scene/SceneCamera.h"
#include "Azteck/Scene/ScriptableEntity.h"

namespace Azteck
{
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
			glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotation.x, { 1.0f, 0.0f, 0.0f })
				* glm::rotate(glm::mat4(1.0f), rotation.y, { 0.0f, 1.0f, 0.0f })
				* glm::rotate(glm::mat4(1.0f), rotation.z, { 0.0f, 0.0f, 1.0f });

			return glm::translate(glm::mat4(1.0f), translation)
				* rotationMatrix
				* glm::scale(glm::mat4(1.0f), scale);
		}

		glm::vec3 translation{ 0.0f };
		glm::vec3 rotation{ 0.0f };
		glm::vec3 scale{ 1.0f };
	};

	struct SpriteRenderComponent
	{
		SpriteRenderComponent() = default;
		SpriteRenderComponent(const SpriteRenderComponent&) = default;

		SpriteRenderComponent(const glm::vec4& color)
			: color(color)
		{}

		glm::vec4 color{1.0f};
	};

	struct CameraComponent
	{
		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;

		SceneCamera camera;
		bool primary = true;
		bool fixedAspectRatio = false;
	};

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
}
