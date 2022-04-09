#pragma once

#include "Azteck/Layer.h"
#include "Azteck/Events/MouseEvent.h"
#include "Azteck/Events/KeyEvent.h"
#include "Azteck/Events/ApplicationEvent.h"

namespace Azteck
{
	class AZTECK_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void onAttach() override;
		void onDetach() override;
		void onUpdate() override;
		void onEvent(Event& event) override;

	private:
		bool onMouseButtonPressedEvent(MouseButtonPressedEvent& e);
		bool onMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
		bool onMouseMovedEvent(MouseMovedEvent& e);
		bool onMouseScrolledEvent(MouseScrolledEvent& e);

		bool onKeyPressedEvent(KeyPressedEvent& e);
		bool onKeyReleasedEvent(KeyReleasedEvent& e);
		bool onKeyTypedEvent(KeyTypedEvent& e);

		bool onWindowResizedEvent(WindowResizedEvent& e);

	private:
		float _time;
	};
}