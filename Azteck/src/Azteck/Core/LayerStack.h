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
		std::vector<Layer*>::reverse_iterator rbegin() { return _layers.rbegin(); }
		std::vector<Layer*>::reverse_iterator rend() { return _layers.rend(); }
		std::vector<Layer*>::const_iterator cbegin() { return _layers.cbegin(); }
		std::vector<Layer*>::const_iterator cend() { return _layers.cend(); }
		std::vector<Layer*>::const_reverse_iterator crbegin() { return _layers.crbegin(); }
		std::vector<Layer*>::const_reverse_iterator crend() { return _layers.crend(); }

	private:
		std::vector<Layer*> _layers;
		size_t _layerInsertIndex;
	};
}