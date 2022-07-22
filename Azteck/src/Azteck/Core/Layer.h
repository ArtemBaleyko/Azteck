#pragma once

#include "Azteck/Core/Core.h"
#include "Azteck/Events/Event.h"

#include "Azteck/Core/Timestep.h"

namespace Azteck
{
	class Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void onAttach() {}
		virtual void onDetach() {}
		virtual void onUpdate(Timestep timestep) {}
		virtual void onImGuiRender() {}
		virtual void onEvent(Event& event) {}

		inline const std::string& getName() const { return _debugName; }
	protected:
		std::string _debugName;
	};
}