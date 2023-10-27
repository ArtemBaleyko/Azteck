#include "azpch.h"
#include "OrthographicCameraController.h"

#include "Azteck/Core/Input.h"
#include "Azteck/Core/KeyCodes.h"

namespace Azteck
{
	
	OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
		: _aspectRatio(aspectRatio)
		, _zoomLevel(1.0f)
		, _doRotation(rotation)
		, _camera(-_aspectRatio * _zoomLevel, _aspectRatio * _zoomLevel, -_zoomLevel, _zoomLevel)
		, _cameraPosition(0.0f)
		, _cameraRotation(0.0f)
		, _cameraTranslationSpeed(5.0f)
		, _cameraRotationSpeed(180.0f)
	{

	}

	void OrthographicCameraController::onUpdate(Timestep ts)
	{
		AZ_PROFILE_FUNCTION();

		if (Input::isKeyPressed(Key::A))
		{
			_cameraPosition.x -= cos(glm::radians(_cameraRotation)) * _cameraTranslationSpeed * ts;
			_cameraPosition.y -= sin(glm::radians(_cameraRotation)) * _cameraTranslationSpeed * ts;
		}
		else if (Input::isKeyPressed(Key::D))
		{
			_cameraPosition.x += cos(glm::radians(_cameraRotation)) * _cameraTranslationSpeed * ts;
			_cameraPosition.y += sin(glm::radians(_cameraRotation)) * _cameraTranslationSpeed * ts;
		}

		if (Input::isKeyPressed(Key::W))
		{
			_cameraPosition.x += -sin(glm::radians(_cameraRotation)) * _cameraTranslationSpeed * ts;
			_cameraPosition.y += cos(glm::radians(_cameraRotation)) * _cameraTranslationSpeed * ts;
		}
		else if (Input::isKeyPressed(Key::S))
		{
			_cameraPosition.x -= -sin(glm::radians(_cameraRotation)) * _cameraTranslationSpeed * ts;
			_cameraPosition.y -= cos(glm::radians(_cameraRotation)) * _cameraTranslationSpeed * ts;
		}

		if (_doRotation)
		{
			if (Input::isKeyPressed(Key::Q))
				_cameraRotation += _cameraRotationSpeed * ts;
			else if (Input::isKeyPressed(Key::E))
				_cameraRotation -= _cameraRotationSpeed * ts;

			if (_cameraRotation > 180.0f)
				_cameraRotation -= 360.0f;
			else if (_cameraRotation <= -180.0f)
				_cameraRotation += 360.0f;

			_camera.setRotation(_cameraRotation);
		}

		_camera.setPosition(_cameraPosition);

		_cameraTranslationSpeed = _zoomLevel;
	}

	void OrthographicCameraController::onEvent(Event& e)
	{
		AZ_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);

		dispatcher.dispatch<MouseScrolledEvent>(AZ_BIND_EVENT_FN(OrthographicCameraController::onMouseScrolled));
		dispatcher.dispatch<WindowResizedEvent>(AZ_BIND_EVENT_FN(OrthographicCameraController::onWindowResized));
	}

	void OrthographicCameraController::onResize(float width, float height)
	{
		_aspectRatio = width / height;
		_camera.setProjection(-_aspectRatio * _zoomLevel, +_aspectRatio * _zoomLevel, -_zoomLevel, _zoomLevel);
	}

	void OrthographicCameraController::setZoomLevel(float level)
	{
		_zoomLevel = level;
	}

	float OrthographicCameraController::getZoomLevel() const
	{
		return _zoomLevel;
	}

	bool OrthographicCameraController::onMouseScrolled(MouseScrolledEvent& e)
	{
		AZ_PROFILE_FUNCTION();

		_zoomLevel -= e.getOffsetY() * 0.25f;
		_zoomLevel = std::max(_zoomLevel, 0.25f);
		_camera.setProjection(-_aspectRatio * _zoomLevel, +_aspectRatio * _zoomLevel, -_zoomLevel, _zoomLevel);
		return false;
	}

	bool OrthographicCameraController::onWindowResized(WindowResizedEvent& e)
	{
		AZ_PROFILE_FUNCTION();

		onResize((float)e.getWidth(), (float)e.getHeight());
		return false;
	}

}
