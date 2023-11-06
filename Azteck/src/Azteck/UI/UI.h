#pragma once

#include <imgui.h>

namespace Azteck::UI
{
	struct ScopedStyleColor
	{
		ScopedStyleColor() = default;

		ScopedStyleColor(ImGuiCol idx, ImVec4 color, bool predicate = true)
			: _shouldBeSet(predicate)
		{
			if (predicate)
				ImGui::PushStyleColor(idx, color);
		}

		ScopedStyleColor(ImGuiCol idx, ImU32 color, bool predicate = true)
			: _shouldBeSet(predicate)
		{
			if (predicate)
				ImGui::PushStyleColor(idx, color);
		}

		~ScopedStyleColor()
		{
			if (_shouldBeSet)
				ImGui::PopStyleColor();
		}

	private:
		bool _shouldBeSet = false;
	};
}
