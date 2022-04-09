#pragma once

#include "Event.h"

namespace Azteck
{
	class AZTECK_API KeyEvent : public Event
	{
	public:
		inline int getKeyCode() const { return _keyCode; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
	protected:
		KeyEvent(int keyCode)
			: _keyCode(keyCode) {}

	protected:
		int _keyCode;
	};

	//---------------------------------------------------------------------------------------------
	class AZTECK_API KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(int keyCode, int repeatCount)
			: KeyEvent(keyCode)
			, _repeatCount(repeatCount)
		{}

		inline int getRepeatCount() const { return _repeatCount; }

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << _keyCode << " (" << _repeatCount << " repeats)";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)
	private:
		int _repeatCount;
	};

	//---------------------------------------------------------------------------------------------
	class AZTECK_API KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(int keyCode)
			: KeyEvent(keyCode)
		{}

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << _keyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};

	//---------------------------------------------------------------------------------------------
	class AZTECK_API KeyTypedEvent : public KeyEvent
	{
	public:
		KeyTypedEvent(int keyCode)
			: KeyEvent(keyCode)
		{}

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << _keyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyTyped)
	};
}