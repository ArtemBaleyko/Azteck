#pragma once

#include "Azteck/Core/Input.h"

namespace Azteck
{
	class WindowsInput : public Input
	{
	protected:
		virtual bool isKeyPressedImpl(KeyCode keyCode) override;

		virtual bool isMouseButtonPressedImpl(MouseCode button) override;
		virtual std::pair<float, float> getMousePossitionImpl() override;
		virtual float getMouseXImpl() override;
		virtual float getMouseYImpl() override;
	};
}