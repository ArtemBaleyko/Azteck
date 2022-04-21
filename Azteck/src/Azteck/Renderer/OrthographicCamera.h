#pragma once

#include <glm/glm.hpp>

namespace Azteck
{
	class OrthographicCamera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top);

		inline const glm::vec3& getPosition() const { return _position; };
		void setPosition(const glm::vec3& position);

		inline float getRotation() const { return _rotation; }
		void setRotation(float rotation);

		inline const glm::mat4& getProjectionMatrix() const { return _projectionMatrix; }
		inline const glm::mat4& getViewMatrix() const { return _viewMatrix; }
		inline const glm::mat4& getViewProjectionMatrix() const { return _viewProjectionMatrix; }

	private:
		void recalculateViewMatrix();

	private:
		glm::mat4 _projectionMatrix;
		glm::mat4 _viewMatrix;
		glm::mat4 _viewProjectionMatrix;

		glm::vec3 _position;
		float _rotation;
	};
}