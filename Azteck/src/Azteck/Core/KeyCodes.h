#pragma once

namespace Azteck
{
	enum class KeyCode : uint16_t
	{
		// From glfw3.h
		Space = 32,
		Apostrophe = 39, /* ' */
		Comma = 44, /* , */
		Minus = 45, /* - */
		Period = 46, /* . */
		Slash = 47, /* / */

		D0 = 48, /* 0 */
		D1 = 49, /* 1 */
		D2 = 50, /* 2 */
		D3 = 51, /* 3 */
		D4 = 52, /* 4 */
		D5 = 53, /* 5 */
		D6 = 54, /* 6 */
		D7 = 55, /* 7 */
		D8 = 56, /* 8 */
		D9 = 57, /* 9 */

		Semicolon = 59, /* ; */
		Equal = 61, /* = */

		A = 65,
		B = 66,
		C = 67,
		D = 68,
		E = 69,
		F = 70,
		G = 71,
		H = 72,
		I = 73,
		J = 74,
		K = 75,
		L = 76,
		M = 77,
		N = 78,
		O = 79,
		P = 80,
		Q = 81,
		R = 82,
		S = 83,
		T = 84,
		U = 85,
		V = 86,
		W = 87,
		X = 88,
		Y = 89,
		Z = 90,

		LeftBracket = 91,  /* [ */
		Backslash = 92,  /* \ */
		RightBracket = 93,  /* ] */
		GraveAccent = 96,  /* ` */

		World1 = 161, /* non-US #1 */
		World2 = 162, /* non-US #2 */

		/* Function keys */
		Escape = 256,
		Enter = 257,
		Tab = 258,
		Backspace = 259,
		Insert = 260,
		Delete = 261,
		Right = 262,
		Left = 263,
		Down = 264,
		Up = 265,
		PageUp = 266,
		PageDown = 267,
		Home = 268,
		End = 269,
		CapsLock = 280,
		ScrollLock = 281,
		NumLock = 282,
		PrintScreen = 283,
		Pause = 284,
		F1 = 290,
		F2 = 291,
		F3 = 292,
		F4 = 293,
		F5 = 294,
		F6 = 295,
		F7 = 296,
		F8 = 297,
		F9 = 298,
		F10 = 299,
		F11 = 300,
		F12 = 301,
		F13 = 302,
		F14 = 303,
		F15 = 304,
		F16 = 305,
		F17 = 306,
		F18 = 307,
		F19 = 308,
		F20 = 309,
		F21 = 310,
		F22 = 311,
		F23 = 312,
		F24 = 313,
		F25 = 314,

		/* Keypad */
		KP0 = 320,
		KP1 = 321,
		KP2 = 322,
		KP3 = 323,
		KP4 = 324,
		KP5 = 325,
		KP6 = 326,
		KP7 = 327,
		KP8 = 328,
		KP9 = 329,
		KPDecimal = 330,
		KPDivide = 331,
		KPMultiply = 332,
		KPSubtract = 333,
		KPAdd = 334,
		KPEnter = 335,
		KPEqual = 336,

		LeftShift = 340,
		LeftControl = 341,
		LeftAlt = 342,
		LeftSuper = 343,
		RightShift = 344,
		RightControl = 345,
		RightAlt = 346,
		RightSuper = 347,
		Menu = 348
	};

	inline std::ostream& operator<<(std::ostream& os, KeyCode keyCode)
	{
		os << static_cast<int32_t>(keyCode);
		return os;
	}
}

/* Printable keys */
#define AZ_KEY_SPACE           ::Azteck::KeyCode::Space
#define AZ_KEY_APOSTROPHE      ::Azteck::KeyCode::Apostrophe    /* ' */
#define AZ_KEY_COMMA           ::Azteck::KeyCode::Comma         /* , */
#define AZ_KEY_MINUS           ::Azteck::KeyCode::Minus         /* - */
#define AZ_KEY_PERIOD          ::Azteck::KeyCode::Period        /* . */
#define AZ_KEY_SLASH           ::Azteck::KeyCode::Slash         /* / */
#define AZ_KEY_0               ::Azteck::KeyCode::D0
#define AZ_KEY_1               ::Azteck::KeyCode::D1
#define AZ_KEY_2               ::Azteck::KeyCode::D2
#define AZ_KEY_3               ::Azteck::KeyCode::D3
#define AZ_KEY_4               ::Azteck::KeyCode::D4
#define AZ_KEY_5               ::Azteck::KeyCode::D5
#define AZ_KEY_6               ::Azteck::KeyCode::D6
#define AZ_KEY_7               ::Azteck::KeyCode::D7
#define AZ_KEY_8               ::Azteck::KeyCode::D8
#define AZ_KEY_9               ::Azteck::KeyCode::D9
#define AZ_KEY_SEMICOLON       ::Azteck::KeyCode::Semicolon     /* ; */
#define AZ_KEY_EQUAL           ::Azteck::KeyCode::Equal         /* = */
#define AZ_KEY_A               ::Azteck::KeyCode::A
#define AZ_KEY_B               ::Azteck::KeyCode::B
#define AZ_KEY_C               ::Azteck::KeyCode::C
#define AZ_KEY_D               ::Azteck::KeyCode::D
#define AZ_KEY_E               ::Azteck::KeyCode::E
#define AZ_KEY_F               ::Azteck::KeyCode::F
#define AZ_KEY_G               ::Azteck::KeyCode::G
#define AZ_KEY_H               ::Azteck::KeyCode::H
#define AZ_KEY_I               ::Azteck::KeyCode::I
#define AZ_KEY_J               ::Azteck::KeyCode::J
#define AZ_KEY_K               ::Azteck::KeyCode::K
#define AZ_KEY_L               ::Azteck::KeyCode::L
#define AZ_KEY_M               ::Azteck::KeyCode::M
#define AZ_KEY_N               ::Azteck::KeyCode::N
#define AZ_KEY_O               ::Azteck::KeyCode::O
#define AZ_KEY_P               ::Azteck::KeyCode::P
#define AZ_KEY_Q               ::Azteck::KeyCode::Q
#define AZ_KEY_R               ::Azteck::KeyCode::R
#define AZ_KEY_S               ::Azteck::KeyCode::S
#define AZ_KEY_T               ::Azteck::KeyCode::T
#define AZ_KEY_U               ::Azteck::KeyCode::U
#define AZ_KEY_V               ::Azteck::KeyCode::V
#define AZ_KEY_W               ::Azteck::KeyCode::W
#define AZ_KEY_X               ::Azteck::KeyCode::X
#define AZ_KEY_Y               ::Azteck::KeyCode::Y
#define AZ_KEY_Z               ::Azteck::KeyCode::Z
#define AZ_KEY_LEFT_BRACKET    ::Azteck::KeyCode::LeftBracket   /* [ */
#define AZ_KEY_BACKSLASH       ::Azteck::KeyCode::Backslash     /* \ */
#define AZ_KEY_RIGHT_BRACKET   ::Azteck::KeyCode::RightBracket  /* ] */
#define AZ_KEY_GRAVE_ACCENT    ::Azteck::KeyCode::GraveAccent   /* ` */
#define AZ_KEY_WORLD_1         ::Azteck::KeyCode::World1        /* non-US #1 */
#define AZ_KEY_WORLD_2         ::Azteck::KeyCode::World2        /* non-US #2 */

/* Function keys */
#define AZ_KEY_ESCAPE          ::Azteck::KeyCode::Escape
#define AZ_KEY_ENTER           ::Azteck::KeyCode::Enter
#define AZ_KEY_TAB             ::Azteck::KeyCode::Tab
#define AZ_KEY_BACKSPACE       ::Azteck::KeyCode::Backspace
#define AZ_KEY_INSERT          ::Azteck::KeyCode::Insert
#define AZ_KEY_DELETE          ::Azteck::KeyCode::Delete
#define AZ_KEY_RIGHT           ::Azteck::KeyCode::Right
#define AZ_KEY_LEFT            ::Azteck::KeyCode::Left
#define AZ_KEY_DOWN            ::Azteck::KeyCode::Down
#define AZ_KEY_UP              ::Azteck::KeyCode::Up
#define AZ_KEY_PAGE_UP         ::Azteck::KeyCode::PageUp
#define AZ_KEY_PAGE_DOWN       ::Azteck::KeyCode::PageDown
#define AZ_KEY_HOME            ::Azteck::KeyCode::Home
#define AZ_KEY_END             ::Azteck::KeyCode::End
#define AZ_KEY_CAPS_LOCK       ::Azteck::KeyCode::CapsLock
#define AZ_KEY_SCROLL_LOCK     ::Azteck::KeyCode::ScrollLock
#define AZ_KEY_NUM_LOCK        ::Azteck::KeyCode::NumLock
#define AZ_KEY_PRINT_SCREEN    ::Azteck::KeyCode::PrintScreen
#define AZ_KEY_PAUSE           ::Azteck::KeyCode::Pause
#define AZ_KEY_F1              ::Azteck::KeyCode::F1
#define AZ_KEY_F2              ::Azteck::KeyCode::F2
#define AZ_KEY_F3              ::Azteck::KeyCode::F3
#define AZ_KEY_F4              ::Azteck::KeyCode::F4
#define AZ_KEY_F5              ::Azteck::KeyCode::F5
#define AZ_KEY_F6              ::Azteck::KeyCode::F6
#define AZ_KEY_F7              ::Azteck::KeyCode::F7
#define AZ_KEY_F8              ::Azteck::KeyCode::F8
#define AZ_KEY_F9              ::Azteck::KeyCode::F9
#define AZ_KEY_F10             ::Azteck::KeyCode::F10
#define AZ_KEY_F11             ::Azteck::KeyCode::F11
#define AZ_KEY_F12             ::Azteck::KeyCode::F12
#define AZ_KEY_F13             ::Azteck::KeyCode::F13
#define AZ_KEY_F14             ::Azteck::KeyCode::F14
#define AZ_KEY_F15             ::Azteck::KeyCode::F15
#define AZ_KEY_F16             ::Azteck::KeyCode::F16
#define AZ_KEY_F17             ::Azteck::KeyCode::F17
#define AZ_KEY_F18             ::Azteck::KeyCode::F18
#define AZ_KEY_F19             ::Azteck::KeyCode::F19
#define AZ_KEY_F20             ::Azteck::KeyCode::F20
#define AZ_KEY_F21             ::Azteck::KeyCode::F21
#define AZ_KEY_F22             ::Azteck::KeyCode::F22
#define AZ_KEY_F23             ::Azteck::KeyCode::F23
#define AZ_KEY_F24             ::Azteck::KeyCode::F24
#define AZ_KEY_F25             ::Azteck::KeyCode::F25

/* Keypad */
#define AZ_KEY_KP_0            ::Azteck::KeyCode::KP0
#define AZ_KEY_KP_1            ::Azteck::KeyCode::KP1
#define AZ_KEY_KP_2            ::Azteck::KeyCode::KP2
#define AZ_KEY_KP_3            ::Azteck::KeyCode::KP3
#define AZ_KEY_KP_4            ::Azteck::KeyCode::KP4
#define AZ_KEY_KP_5            ::Azteck::KeyCode::KP5
#define AZ_KEY_KP_6            ::Azteck::KeyCode::KP6
#define AZ_KEY_KP_7            ::Azteck::KeyCode::KP7
#define AZ_KEY_KP_8            ::Azteck::KeyCode::KP8
#define AZ_KEY_KP_9            ::Azteck::KeyCode::KP9
#define AZ_KEY_KP_DECIMAL      ::Azteck::KeyCode::KPDecimal
#define AZ_KEY_KP_DIVIDE       ::Azteck::KeyCode::KPDivide
#define AZ_KEY_KP_MULTIPLY     ::Azteck::KeyCode::KPMultiply
#define AZ_KEY_KP_SUBTRACT     ::Azteck::KeyCode::KPSubtract
#define AZ_KEY_KP_ADD          ::Azteck::KeyCode::KPAdd
#define AZ_KEY_KP_ENTER        ::Azteck::KeyCode::KPEnter
#define AZ_KEY_KP_EQUAL        ::Azteck::KeyCode::KPEqual

#define AZ_KEY_LEFT_SHIFT      ::Azteck::KeyCode::LeftShift
#define AZ_KEY_LEFT_CONTROL    ::Azteck::KeyCode::LeftControl
#define AZ_KEY_LEFT_ALT        ::Azteck::KeyCode::LeftAlt
#define AZ_KEY_LEFT_SUPER      ::Azteck::KeyCode::LeftSuper
#define AZ_KEY_RIGHT_SHIFT     ::Azteck::KeyCode::RightShift
#define AZ_KEY_RIGHT_CONTROL   ::Azteck::KeyCode::RightControl
#define AZ_KEY_RIGHT_ALT       ::Azteck::KeyCode::RightAlt
#define AZ_KEY_RIGHT_SUPER     ::Azteck::KeyCode::RightSuper
#define AZ_KEY_MENU            ::Azteck::KeyCode::Menu