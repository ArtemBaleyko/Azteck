#pragma once

#include "Azteck/Core/Core.h"
#include "Azteck/Core/KeyCodes.h"
#include "Azteck/Core/MouseCodes.h"

namespace Azteck
{
	class Input
	{
	public:
		Input(const Input&) = delete;
		Input& operator=(const Input&) = delete;

		inline static bool isKeyPressed(KeyCode keyCode) { return _instance->isKeyPressedImpl(keyCode); };

		inline static bool isMouseButtonPressed(MouseCode button) { return _instance->isMouseButtonPressedImpl(button); };
		inline static std::pair<float, float> getMousePossition() { return _instance->getMousePossitionImpl(); }
		inline static float getMouseX() { return _instance->getMouseXImpl(); };
		inline static float getMouseY() { return _instance->getMouseYImpl(); };

		static Scope<Input> create();

	protected:
		Input() = default;

		virtual bool isKeyPressedImpl(KeyCode keyCode) = 0;

		virtual bool isMouseButtonPressedImpl(MouseCode button) = 0;
		virtual std::pair<float, float> getMousePossitionImpl() = 0;
		virtual float getMouseXImpl() = 0;
		virtual float getMouseYImpl() = 0;

	private:
		static Scope<Input> _instance;
	};
}