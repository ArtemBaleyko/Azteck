#pragma once

#include "Azteck/Core/Core.h"
#include "Azteck/Core/Layer.h"

namespace Azteck
{
	class LayerStack
	{
	public:
		LayerStack();
		~LayerStack();

		void pushLayer(Layer* layer);
		void pushOverlay(Layer* overlay);
		void popLayer(Layer* layer);
		void popOverlay(Layer* overlay);

		std::vector<Layer*>::iterator begin() { return _layers.begin(); }
		std::vector<Layer*>::iterator end() { return _layers.end(); }

	private:
		std::vector<Layer*> _layers;
		size_t _layerInsertIndex;
	};
}