#pragma once

#include "Azteck/Events/Event.h"
#include "Azteck/Core/MouseCodes.h"

namespace Azteck
{
	class MouseMovedEvent : public Event
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
	class MouseScrolledEvent : public Event
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
	class MouseButtonEvent : public Event
	{
	public:
		inline MouseCode getMouseButton() const { return _buttonCode; }

		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput | EventCategoryMouseButton)

	protected:
		MouseButtonEvent(MouseCode buttonCode)
			: _buttonCode(buttonCode)
		{}

	protected:
		MouseCode _buttonCode;
	};

	//---------------------------------------------------------------------------------------------
	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(MouseCode buttonCode)
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
	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(MouseCode buttonCode)
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
