#pragma once

#include "Core.h"

namespace Azteck
{
	class Input
	{
	public:
		Input(const Input&) = delete;
		Input& operator=(const Input&) = delete;

		inline static bool isKeyPressed(int keyCode) { return _instance->isKeyPressedImpl(keyCode); };

		inline static bool isMouseButtonPressed(int button) { return _instance->isMouseButtonPressedImpl(button); };
		inline static std::pair<float, float> getMousePossition() { return _instance->getMousePossitionImpl(); }
		inline static float getMouseX() { return _instance->getMouseXImpl(); };
		inline static float getMouseY() { return _instance->getMouseYImpl(); };

	protected:
		Input() = default;

		virtual bool isKeyPressedImpl(int keyCode) = 0;

		virtual bool isMouseButtonPressedImpl(int button) = 0;
		virtual std::pair<float, float> getMousePossitionImpl() = 0;
		virtual float getMouseXImpl() = 0;
		virtual float getMouseYImpl() = 0;

	private:
		static Scope<Input> _instance;
	};
}