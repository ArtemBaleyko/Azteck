#include "azpch.h"
#include "SceneCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Azteck
{
	SceneCamera::SceneCamera()
		: _orthoSize(10.0f)
		, _orthoNear(-1.0f)
		, _orthoFar(1.0f)
		, _aspectRatio(1.0f)
	{
		recalculateProjection();
	}

	void SceneCamera::setOrthographic(float size, float nearClip, float farClip)
	{
		_orthoSize = size;
		_orthoNear = nearClip;
		_orthoFar = farClip;
		recalculateProjection();
	}

	void SceneCamera::setViewportSize(uint32_t width, uint32_t height)
	{
		_aspectRatio = (float)width / (float)height;
		recalculateProjection();
	}

	void SceneCamera::setOrthographicSize(float size)
	{
		_orthoSize = size;
		recalculateProjection();
	}

	void SceneCamera::recalculateProjection()
	{
		const float orthoLeft = -_orthoSize * _aspectRatio * 0.5f;
		const float orthoRight = _orthoSize * _aspectRatio * 0.5f;
		const float orthoBottom = -_orthoSize * 0.5f;
		const float orthoTop = _orthoSize * 0.5f;

		_projection = glm::ortho(orthoLeft, orthoRight, orthoBottom,
			orthoTop, _orthoNear, _orthoFar);
	}
}