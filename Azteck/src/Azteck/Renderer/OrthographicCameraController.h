#pragma once

#include "Azteck/Renderer/OrthographicCamera.h"
#include "Azteck/Core/Timestep.h"

#include "Azteck/Events/ApplicationEvent.h"
#include "Azteck/Events/MouseEvent.h"

namespace Azteck
{
	class OrthographicCameraController
	{
	public:
		OrthographicCameraController(float aspectRatio, bool rotation = false);

		void onUpdate(Timestep ts);
		void onEvent(Event& e);

		void setZoomLevel(float level);
		float getZoomLevel() const;

		inline OrthographicCamera& getCamera() { return _camera; }
		inline const OrthographicCamera& getCamera() const { return _camera; }

	private:
		bool onMouseScrolled(MouseScrolledEvent& e);
		bool onWindowResized(WindowResizedEvent& e);

	private:
		float _aspectRatio;
		float _zoomLevel;
		glm::vec3 _cameraPosition;
		float _cameraRotation;
		float _cameraTranslationSpeed;
		float _cameraRotationSpeed;

		bool _doRotation;
		OrthographicCamera _camera;
	};
}