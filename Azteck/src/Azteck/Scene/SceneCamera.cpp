#include "azpch.h"
#include "SceneCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Azteck
{
	SceneCamera::SceneCamera()
		: _orthoSize(10.0f)
		, _orthoNearClip(-1.0f)
		, _orthoFarClip(1.0f)
		, _perspectiveFOV(glm::radians(45.0f))
		, _perspectiveNearClip(0.01f)
		, _perspectiveFarClip(1000.0f)
		, _aspectRatio(1.0f)
	{
		recalculateProjection();
	}

	void SceneCamera::setOrthographic(float size, float nearClip, float farClip)
	{
		_projectionType = ProjectionType::Orthographic;
		_orthoSize = size;
		_orthoNearClip = nearClip;
		_orthoFarClip = farClip;
		recalculateProjection();
	}

	void SceneCamera::setPerspective(float fov, float nearClip, float farClip)
	{
		_projectionType = ProjectionType::Perspective;
		_perspectiveFOV = fov;
		_perspectiveNearClip = nearClip;
		_perspectiveFarClip = farClip;
		recalculateProjection();
	}

	void SceneCamera::setViewportSize(uint32_t width, uint32_t height)
	{
		AZ_CORE_ASSERT(width > 0 && height > 0, "Invalid viewport size");

		_aspectRatio = (float)width / (float)height;
		recalculateProjection();
	}

	void SceneCamera::setVerticalFOV(float fov)
	{
		_perspectiveFOV = fov;
		recalculateProjection();
	}

	void SceneCamera::setPerspectiveNearClip(float nearClip)
	{
		_perspectiveNearClip = nearClip;
		recalculateProjection();
	}

	void SceneCamera::setPerspectiveFarClip(float farClip)
	{
		_perspectiveFarClip = farClip;
		recalculateProjection();
	}

	void SceneCamera::setOrthographicSize(float size)
	{
		_orthoSize = size;
		recalculateProjection();
	}

	void SceneCamera::setOrthographicNearClip(float nearClip)
	{
		_orthoNearClip = nearClip;
		recalculateProjection();
	}

	void SceneCamera::setOrthographicFarClip(float farClip)
	{
		_orthoFarClip = farClip;
		recalculateProjection();
	}

	void SceneCamera::setProjectionType(ProjectionType type)
	{
		_projectionType = type;
		recalculateProjection();
	}

	void SceneCamera::recalculateProjection()
	{
		if (_projectionType == ProjectionType::Orthographic)
		{
			const float orthoLeft = -_orthoSize * _aspectRatio * 0.5f;
			const float orthoRight = _orthoSize * _aspectRatio * 0.5f;
			const float orthoBottom = -_orthoSize * 0.5f;
			const float orthoTop = _orthoSize * 0.5f;

			_projection = glm::ortho(orthoLeft, orthoRight, orthoBottom,
				orthoTop, _orthoNearClip, _orthoFarClip);
		}
		else
		{
			_projection = glm::perspective(_perspectiveFOV, _aspectRatio, 
				_perspectiveNearClip, _perspectiveFarClip);
		}
	}
}
