#pragma once

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
		static std::pair<float, float> getMousePosition();
		static float getMouseX();
		static float getMouseY();
	};
}