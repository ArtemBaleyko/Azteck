#pragma once

#include "azpch.h"

#include "Azteck/Core/Core.h"

namespace Azteck
{
	enum class EventType
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum EventCategory
	{
		None = 0,
		EventCategoryApplication	= BIT(0),
		EventCategoryInput			= BIT(1),
		EventCategoryKeyboard		= BIT(2),
		EventCategoryMouse			= BIT(3),
		EventCategoryMouseButton	= BIT(4)
	};

#define EVENT_CLASS_TYPE(type) static EventType getStaticType() { return EventType::type; } \
								virtual EventType getEventType() const override { return getStaticType(); } \
								virtual const char* getName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int getCategoryFlags() const override { return category; }

	class  Event
	{
		friend class EventDispatcher;
	public:
		virtual EventType getEventType() const = 0;
		virtual const char* getName() const = 0;
		virtual int getCategoryFlags() const = 0;
		virtual std::string toString() const { return getName(); }

		inline bool isHandled() const { return _handled; }
		inline bool isInCategory(EventCategory category) { return getCategoryFlags() & category; }

		inline void setHandled(bool isHandled) { _handled = isHandled; }

	protected:
		bool _handled = false;
	};

	class EventDispatcher
	{
	public:
		EventDispatcher(Event& event)
			: _event(event)
		{}

		template<typename T, typename Fn>
		bool dispatch(const Fn& func)
		{
			if (_event.getEventType() == T::getStaticType())
			{
				_event._handled = func(static_cast<T&>(_event));
				return true;
			}

			return false;
		}

	private:
		Event& _event;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.toString();
	}
}