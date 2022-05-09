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
		if (Input::isKeyPressed(AZ_KEY_A))
			_cameraPosition.x -= _cameraTranslationSpeed * ts;
		else if (Input::isKeyPressed(AZ_KEY_D))
			_cameraPosition.x += _cameraTranslationSpeed * ts;

		if (Input::isKeyPressed(AZ_KEY_W))
			_cameraPosition.y += _cameraTranslationSpeed * ts;
		else if (Input::isKeyPressed(AZ_KEY_S))
			_cameraPosition.y -= _cameraTranslationSpeed * ts;

		if (_doRotation)
		{
			if (Input::isKeyPressed(AZ_KEY_Q))
				_cameraRotation += _cameraRotationSpeed * ts;
			else if (Input::isKeyPressed(AZ_KEY_E))
				_cameraRotation -= _cameraRotationSpeed * ts;

			_camera.setRotation(_cameraRotation);
		}

		_camera.setPosition(_cameraPosition);

		_cameraTranslationSpeed = _zoomLevel;
	}

	void OrthographicCameraController::onEvent(Event& e)
	{
		EventDispatcher dispatcher(e);

		dispatcher.dispatch<MouseScrolledEvent>(AZ_BIND_EVENT_FN(OrthographicCameraController::onMouseScrolled));
		dispatcher.dispatch<WindowResizedEvent>(AZ_BIND_EVENT_FN(OrthographicCameraController::onWindowResized));
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
		_zoomLevel -= e.getOffsetY() * 0.25f;
		_zoomLevel = std::max(_zoomLevel, 0.25f);
		_camera.setProjection(-_aspectRatio * _zoomLevel, +_aspectRatio * _zoomLevel, -_zoomLevel, _zoomLevel);
		return false;
	}

	bool OrthographicCameraController::onWindowResized(WindowResizedEvent& e)
	{
		_aspectRatio = e.getWidth() / (float)e.getHeight();
		_camera.setProjection(-_aspectRatio * _zoomLevel, +_aspectRatio * _zoomLevel, -_zoomLevel, _zoomLevel);
		return false;
	}

}