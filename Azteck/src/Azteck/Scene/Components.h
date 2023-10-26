#pragma once

#include <glm/glm.hpp>

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

		TransformComponent(const glm::mat4& transform)
			: transform(transform)
		{}

		operator const glm::mat4& () const { return transform; }
		operator glm::mat4& () { return transform; }

		glm::mat4 transform{1.0f};
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
}