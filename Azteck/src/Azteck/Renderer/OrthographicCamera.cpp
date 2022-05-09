#include "azpch.h"
#include "OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Azteck
{
	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
		: _projectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f))
		, _viewMatrix(1.0f)
		, _rotation(0.0f)
		, _position(0.0f)
	{
		_viewProjectionMatrix = _projectionMatrix * _viewMatrix;
	}

	void OrthographicCamera::setProjection(float left, float right, float bottom, float top)
	{
		_projectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		_viewProjectionMatrix = _projectionMatrix * _viewMatrix;
	}

	void OrthographicCamera::setPosition(const glm::vec3& position)
	{
		_position = position;
		recalculateViewMatrix();
	}

	void OrthographicCamera::setRotation(float rotation)
	{
		_rotation = rotation;
		recalculateViewMatrix();
	}

	void OrthographicCamera::recalculateViewMatrix()
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), _position);
		transform = glm::rotate(transform, glm::radians(_rotation), glm::vec3(0.0f, 0.0f, 1.0f));

		_viewMatrix = glm::inverse(transform);
		_viewProjectionMatrix = _projectionMatrix * _viewMatrix;
	}
}