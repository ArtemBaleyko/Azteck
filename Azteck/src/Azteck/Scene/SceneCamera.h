#pragma once

#include "Azteck/Renderer/Camera.h"

namespace Azteck
{
	class SceneCamera : public Camera
	{
	public:
		SceneCamera();

		void setOrthographic(float size, float nearClip, float farClip);
		void setViewportSize(uint32_t width, uint32_t height);

		float getOrthographicSize() const { return _orthoSize; }
		void setOrthographicSize(float size);

	private:
		void recalculateProjection();

	private:
		float _orthoSize;
		float _orthoNear;
		float _orthoFar;

		float _aspectRatio;
	};
}