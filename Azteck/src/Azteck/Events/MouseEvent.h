#pragma once

#include "Event.h"

namespace Azteck
{
	class AZTECK_API MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(float posX, float posY)
			: _posX(posX)
			, _posY(posY)
		{}

		inline float getX() const { return _posX; }
		inline float getY() const { return _posY; }

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << _posX << ", " << _posY;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	private:
		float _posX;
		float _posY;
	};

	//---------------------------------------------------------------------------------------------
	class AZTECK_API MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(float offsetX, float offsetY)
			: _offsetX(offsetX)
			, _offsetY(offsetY)
		{}

		inline float getOffsetX() const { return _offsetX; }
		inline float getOffsetY() const { return _offsetY; }

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "MouseScrolledEvent: " << _offsetX << ", " << _offsetY;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	private:
		float _offsetX;
		float _offsetY;
	};

	//---------------------------------------------------------------------------------------------
	class AZTECK_API MouseButtonEvent : public Event
	{
	public:
		inline int getMouseButton() const { return _buttonCode; }

		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	protected:
		MouseButtonEvent(int buttonCode)
			: _buttonCode(buttonCode)
		{}

	protected:
		int _buttonCode;
	};

	//---------------------------------------------------------------------------------------------
	class AZTECK_API MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(int buttonCode)
			: MouseButtonEvent(buttonCode)
		{}

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << _buttonCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	//---------------------------------------------------------------------------------------------
	class AZTECK_API MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(int buttonCode)
			: MouseButtonEvent(buttonCode)
		{}

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << _buttonCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};
}