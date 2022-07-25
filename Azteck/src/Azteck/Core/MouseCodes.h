#pragma once

namespace Azteck
{
	enum class MouseCode : uint16_t
	{
		// From glfw3.h
		Button0 = 0,
		Button1 = 1,
		Button2 = 2,
		Button3 = 3,
		Button4 = 4,
		Button5 = 5,
		Button6 = 6,
		Button7 = 7,

		ButtonLast = Button7,
		ButtonLeft = Button0,
		ButtonRight = Button1,
		ButtonMiddle = Button2
	};

	inline std::ostream& operator<<(std::ostream& os, MouseCode mouseCode)
	{
		os << static_cast<int32_t>(mouseCode);
		return os;
	}
}

#define AZ_MOUSE_BUTTON_0      ::Azteck::MouseCode::Button0
#define AZ_MOUSE_BUTTON_1      ::Azteck::MouseCode::Button1
#define AZ_MOUSE_BUTTON_2      ::Azteck::MouseCode::Button2
#define AZ_MOUSE_BUTTON_3      ::Azteck::MouseCode::Button3
#define AZ_MOUSE_BUTTON_4      ::Azteck::MouseCode::Button4
#define AZ_MOUSE_BUTTON_5      ::Azteck::MouseCode::Button5
#define AZ_MOUSE_BUTTON_6      ::Azteck::MouseCode::Button6
#define AZ_MOUSE_BUTTON_7      ::Azteck::MouseCode::Button7
#define AZ_MOUSE_BUTTON_LAST   ::Azteck::MouseCode::ButtonLast
#define AZ_MOUSE_BUTTON_LEFT   ::Azteck::MouseCode::ButtonLeft
#define AZ_MOUSE_BUTTON_RIGHT  ::Azteck::MouseCode::ButtonRight
#define AZ_MOUSE_BUTTON_MIDDLE ::Azteck::MouseCode::ButtonMiddle
