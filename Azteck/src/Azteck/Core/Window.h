#pragma once

#include "azpch.h"

#include "Azteck/Events/Event.h"

namespace Azteck
{
	struct WindowProps
	{
		WindowProps(const std::string& title = "Azteck Engine",
			uint32_t width = 1280,
			uint32_t height = 720)
			: title(title)
			, width(width)
			, height(height)
		{}

		std::string title;
		uint32_t width;
		uint32_t height;
	};

	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void onUpdate() = 0;

		virtual uint32_t getWidth() const = 0;
		virtual uint32_t getHeight() const = 0;

		virtual void setEventCallback(const EventCallbackFn& callback) = 0;
		virtual void setVSync(bool enabled) = 0;
		virtual bool isVSync() const = 0;

		virtual void* getNativeWindow() const = 0;

		static Scope<Window> create(const WindowProps& props = WindowProps());
	};
}