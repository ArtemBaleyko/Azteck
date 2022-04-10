#pragma once

#include "Azteck/Window.h"

#include "GLFW/glfw3.h"

namespace Azteck
{
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void onUpdate() override;

		inline unsigned int getWidth() const override { return _data.width; }
		inline unsigned int getHeight() const override { return _data.height; }

		void setEventCallback(const EventCallbackFn& callback) override;
		void setVSync(bool enabled) override;
		bool isVSync() const override;

		inline virtual void* getNativeWindow() const { return _window; };

	private:
		virtual void init(const WindowProps& props);
		virtual void shutdown();

	private:
		struct WindowData
		{
			std::string title;
			unsigned int width;
			unsigned int height;
			bool vSync;

			EventCallbackFn eventCallback;
		};
		
		GLFWwindow* _window;
		WindowData _data;
	};
}