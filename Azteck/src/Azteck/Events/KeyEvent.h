#pragma once

#include <sstream>

#include "Azteck/Events/Event.h"
#include "Azteck/Core/KeyCodes.h"

namespace Azteck
{
	class KeyEvent : public Event
	{
	public:
		inline KeyCode getKeyCode() const { return _keyCode; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
	protected:
		KeyEvent(KeyCode keyCode)
			: _keyCode(keyCode) {}

	protected:
		KeyCode _keyCode;
	};

	//---------------------------------------------------------------------------------------------
	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(KeyCode keyCode, bool isRepeat = false)
			: KeyEvent(keyCode)
			, _isRepeat(isRepeat)
		{}

		inline bool isRepeat() const { return _isRepeat; }

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << _keyCode << " (repeat = " << _isRepeat << ")";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)
	private:
		bool _isRepeat;
	};

	//---------------------------------------------------------------------------------------------
	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(KeyCode keyCode)
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
	class KeyTypedEvent : public KeyEvent
	{
	public:
		KeyTypedEvent(KeyCode keyCode)
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
