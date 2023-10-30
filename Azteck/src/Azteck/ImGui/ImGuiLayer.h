#pragma once

#include "Azteck/Core/Layer.h"
#include "Azteck/Events/MouseEvent.h"
#include "Azteck/Events/KeyEvent.h"
#include "Azteck/Events/ApplicationEvent.h"

namespace Azteck
{
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void onAttach() override;
		virtual void onDetach() override;
		virtual void onImGuiRender() override;
		virtual void onEvent(Event& e) override;

		void begin();
		void end();

		inline void setBlockEvents(bool block) { _blockEvents = block; }

		void setDarkThemeColors();

	private:
		bool _blockEvents;
	};
}
