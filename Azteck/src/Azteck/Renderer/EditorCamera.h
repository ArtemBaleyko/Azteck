#pragma once

#include "Camera.h"
#include "Azteck/Core/Timestep.h"
#include "Azteck/Events/Event.h"
#include "Azteck/Events/MouseEvent.h"

#include <glm/glm.hpp>

namespace Azteck {

	class EditorCamera : public Camera
	{
	public:
		EditorCamera() = default;
		EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);

		void onUpdate(Timestep ts);
		void onEvent(Event& e);

		inline float getDistance() const { return _distance; }
		inline void setDistance(float distance) { _distance = distance; }

		inline void setViewportSize(float width, float height) { _viewportWidth = width; _viewportHeight = height; updateProjection(); }

		const glm::mat4& getViewMatrix() const { return _viewMatrix; }
		glm::mat4 getViewProjection() const { return _projection * _viewMatrix; }

		glm::vec3 getUpDirection() const;
		glm::vec3 getRightDirection() const;
		glm::vec3 getForwardDirection() const;
		const glm::vec3& getPosition() const { return _position; }
		glm::quat getOrientation() const;

		float getPitch() const { return _pitch; }
		float getYaw() const { return _yaw; }
	private:
		void updateProjection();
		void updateView();

		bool onMouseScroll(MouseScrolledEvent& e);

		void mousePan(const glm::vec2& delta);
		void mouseRotate(const glm::vec2& delta);
		void mouseZoom(float delta);

		glm::vec3 calculatePosition() const;

		std::pair<float, float> panSpeed() const;
		float rotationSpeed() const;
		float zoomSpeed() const;
	private:
		float _fov = 45.0f;
		float _aspectRatio = 1.778f;
		float _nearClip = 0.1f;
		float _farClip = 1000.0f;

		glm::mat4 _viewMatrix;
		glm::vec3 _position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 _focalPoint = { 0.0f, 0.0f, 0.0f };

		glm::vec2 _initialMousePosition = { 0.0f, 0.0f };

		float _distance = 10.0f;
		float _pitch = 0.0f;
		float _yaw = 0.0f;

		float _viewportWidth = 1280.0f;
		float _viewportHeight = 720.0f;
	};

}
