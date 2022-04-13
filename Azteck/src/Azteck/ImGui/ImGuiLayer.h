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

		virtual void onAttach() override;
		virtual void onDetach() override;
		virtual void onImGuiRender() override;

		void begin();
		void end();

	private:
		float _time;
	};
}