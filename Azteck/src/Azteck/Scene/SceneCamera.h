#pragma once

#include "Azteck/Renderer/Camera.h"

namespace Azteck
{
	class SceneCamera : public Camera
	{
	public:
		enum class ProjectionType
		{
			Perspective = 0,
			Orthographic
		};

	public:
		SceneCamera();

		void setOrthographic(float size, float nearClip, float farClip);
		void setPerspective(float fov, float nearClip, float farClip);

		void setViewportSize(uint32_t width, uint32_t height);


		float getVerticalFOV() const { return _perspectiveFOV; }
		void setVerticalFOV(float fov);

		float getPerspectiveNearClip() const { return _perspectiveNearClip; }
		void setPerspectiveNearClip(float nearClip);

		float getPerspectiveFarClip() const { return _perspectiveFarClip; }
		void setPerspectiveFarClip(float farClip);


		float getOrthographicSize() const { return _orthoSize; }
		void setOrthographicSize(float size);

		float getOrthographicNearClip() const { return _orthoNearClip; }
		void setOrthographicNearClip(float nearClip);

		float getOrthographicFarClip() const { return _orthoFarClip; }
		void setOrthographicFarClip(float farClip);

		ProjectionType getProjectionType() const { return _projectionType; }
		void setProjectionType(ProjectionType type);

	private:
		void recalculateProjection();

	private:
		ProjectionType _projectionType = ProjectionType::Orthographic;

		float _orthoSize;
		float _orthoNearClip;
		float _orthoFarClip;

		float _perspectiveFOV;
		float _perspectiveNearClip;
		float _perspectiveFarClip;

		float _aspectRatio;
	};
}
