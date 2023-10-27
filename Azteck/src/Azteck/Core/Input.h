#pragma once

#include <glm/glm.hpp>

#include "Azteck/Core/Core.h"
#include "Azteck/Core/KeyCodes.h"
#include "Azteck/Core/MouseCodes.h"

namespace Azteck
{
	class Input
	{
	public:
		static bool isKeyPressed(KeyCode keyCode);

		static bool isMouseButtonPressed(MouseCode button);
		static glm::vec2 getMousePosition();
		static float getMouseX();
		static float getMouseY();
	};
}
