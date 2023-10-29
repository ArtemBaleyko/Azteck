#include "azpch.h"
#include "EditorCamera.h"

#include "Azteck/Core/Input.h"
#include "Azteck/Core/KeyCodes.h"
#include "Azteck/Core/MouseCodes.h"

#include <glfw/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Azteck {

	EditorCamera::EditorCamera(float fov, float aspectRatio, float nearClip, float farClip)
		: _fov(fov), _aspectRatio(aspectRatio), _nearClip(nearClip), _farClip(farClip), Camera(glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip))
	{
		updateView();
	}

	void EditorCamera::updateProjection()
	{
		_aspectRatio = _viewportWidth / _viewportHeight;
		_projection = glm::perspective(glm::radians(_fov), _aspectRatio, _nearClip, _farClip);
	}

	void EditorCamera::updateView()
	{
		// _yaw = _pitch = 0.0f; // Lock the camera's rotation
		_position = calculatePosition();

		glm::quat orientation = getOrientation();
		_viewMatrix = glm::translate(glm::mat4(1.0f), _position) * glm::toMat4(orientation);
		_viewMatrix = glm::inverse(_viewMatrix);
	}

	std::pair<float, float> EditorCamera::panSpeed() const
	{
		float x = std::min(_viewportWidth / 1000.0f, 2.4f); // max = 2.4f
		float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		float y = std::min(_viewportHeight / 1000.0f, 2.4f); // max = 2.4f
		float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

		return { xFactor, yFactor };
	}

	float EditorCamera::rotationSpeed() const
	{
		return 0.8f;
	}

	float EditorCamera::zoomSpeed() const
	{
		float distance = _distance * 0.2f;
		distance = std::max(distance, 0.0f);
		float speed = distance * distance;
		speed = std::min(speed, 100.0f); // max speed = 100
		return speed;
	}

	void EditorCamera::onUpdate(Timestep ts)
	{
		if (Input::isKeyPressed(Key::LeftAlt))
		{
			const glm::vec2& mouse{ Input::getMouseX(), Input::getMouseY() };
			glm::vec2 delta = (mouse - _initialMousePosition) * 0.003f;
			_initialMousePosition = mouse;

			if (Input::isMouseButtonPressed(Mouse::ButtonMiddle))
				mousePan(delta);
			else if (Input::isMouseButtonPressed(Mouse::ButtonLeft))
				mouseRotate(delta);
			else if (Input::isMouseButtonPressed(Mouse::ButtonRight))
				mouseZoom(delta.y);
		}

		updateView();
	}

	void EditorCamera::onEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<MouseScrolledEvent>(AZ_BIND_EVENT_FN(EditorCamera::onMouseScroll));
	}

	bool EditorCamera::onMouseScroll(MouseScrolledEvent& e)
	{
		float delta = e.getOffsetY() * 0.1f;
		mouseZoom(delta);
		updateView();
		return false;
	}

	void EditorCamera::mousePan(const glm::vec2& delta)
	{
		auto [xSpeed, ySpeed] = panSpeed();
		_focalPoint += -getRightDirection() * delta.x * xSpeed * _distance;
		_focalPoint += getUpDirection() * delta.y * ySpeed * _distance;
	}

	void EditorCamera::mouseRotate(const glm::vec2& delta)
	{
		float yawSign = getUpDirection().y < 0 ? -1.0f : 1.0f;
		_yaw += yawSign * delta.x * rotationSpeed();
		_pitch += delta.y * rotationSpeed();
	}

	void EditorCamera::mouseZoom(float delta)
	{
		_distance -= delta * zoomSpeed();
		if (_distance < 1.0f)
		{
			_focalPoint += getForwardDirection();
			_distance = 1.0f;
		}
	}

	glm::vec3 EditorCamera::getUpDirection() const
	{
		return glm::rotate(getOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::vec3 EditorCamera::getRightDirection() const
	{
		return glm::rotate(getOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	glm::vec3 EditorCamera::getForwardDirection() const
	{
		return glm::rotate(getOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	glm::vec3 EditorCamera::calculatePosition() const
	{
		return _focalPoint - getForwardDirection() * _distance;
	}

	glm::quat EditorCamera::getOrientation() const
	{
		return glm::quat(glm::vec3(-_pitch, -_yaw, 0.0f));
	}

}
