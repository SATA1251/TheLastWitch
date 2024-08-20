#include <dinput.h>

#pragma comment(lib, "dinput8")
#pragma comment(lib, "dxguid")

enum class KEY
{
	//키보드 숫자키
	_0 = DIK_0,
	_1 = DIK_1,
	_2 = DIK_2,
	_3 = DIK_3,
	_4 = DIK_4,
	_5 = DIK_5,
	_6 = DIK_6,
	_7 = DIK_7,
	_8 = DIK_8,
	_9 = DIK_9,

	//키보드 알파벳키
	A = DIK_A,
	B = DIK_B,
	C = DIK_C,
	D = DIK_D,
	E = DIK_E,
	F = DIK_F,
	G = DIK_G,
	H = DIK_H,
	I = DIK_I,
	J = DIK_J,
	K = DIK_K,
	L = DIK_L,
	M = DIK_M,
	N = DIK_N,
	O = DIK_O,
	P = DIK_P,
	Q = DIK_Q,
	R = DIK_R,
	S = DIK_S,
	T = DIK_T,
	U = DIK_U,
	V = DIK_V,
	W = DIK_W,
	X = DIK_X,
	Y = DIK_Y,
	Z = DIK_Z,

	// Function키
	F1 = DIK_F1,
	F2 = DIK_F2,
	F3 = DIK_F3,
	F4 = DIK_F4,
	F5 = DIK_F5,
	F6 = DIK_F6,
	F7 = DIK_F7,
	F8 = DIK_F8,
	F9 = DIK_F9,
	F10 = DIK_F10,
	F11 = DIK_F11,
	F12 = DIK_F12,

	//키보드 기호키
	APOSTROPE = DIK_APOSTROPHE,	// ' 버튼
	BACKSLASH = DIK_BACKSLASH,	// \ 버튼
	COMMA = DIK_COMMA,			// , 버튼
	PERIOD = DIK_PERIOD,		// . 키
	LBRACKET = DIK_LBRACKET,	// [ 버튼
	RBRACKET = DIK_RBRACKET,	// ] 버튼
	EQUALS = DIK_EQUALS,		// = 버튼
	MINUS = DIK_MINUS,			// - 버튼
	GRAVE = DIK_GRAVE,			// ` 버튼
	SEMICOLON = DIK_SEMICOLON,	// ; 키
	SLASH = DIK_SLASH,			// / 키

	// 방향키 상단
	DDELETE = DIK_DELETE,
	END = DIK_END,
	HOME = DIK_HOME,
	INSERT = DIK_INSERT,
	PGUP = DIK_PRIOR,			// PgUp 버튼
	PGDOWN = DIK_NEXT,			// PgDown 버튼
	SCRLK = DIK_SCROLL,			// Scroll Lock 버튼
	PAUSE = DIK_PAUSE,			// Pause 버튼
	PRTSC = DIK_SYSRQ,			// PrtSc 버튼

	// 방향키
	UP = DIK_UP,
	DOWN = DIK_DOWN,
	LEFT = DIK_LEFT,
	RIGHT = DIK_RIGHT,

	// 키보드 기능키
	ESCAPE = DIK_ESCAPE,		// esc 버튼
	LWINDOW = DIK_LWIN,
	RWINDOW = DIK_RWIN,
	LCTRL = DIK_LCONTROL,
	RCTRL = DIK_RCONTROL,
	LALT = DIK_LALT,
	RALT = DIK_RALT,
	LSHIFT = DIK_LSHIFT,
	RSHIFT = DIK_RSHIFT,
	ENTER = DIK_RETURN,			// Enter
	BACK = DIK_BACK,
	CAPSLOCK = DIK_CAPITAL,
	SPACE = DIK_SPACE,
	TAB = DIK_TAB,
	MENU = DIK_APPS,

	// 넘패드 숫자키
	NUM0 = DIK_NUMPAD0,
	NUM1 = DIK_NUMPAD1,
	NUM2 = DIK_NUMPAD2,
	NUM3 = DIK_NUMPAD3,
	NUM4 = DIK_NUMPAD4,
	NUM5 = DIK_NUMPAD5,
	NUM6 = DIK_NUMPAD6,
	NUM7 = DIK_NUMPAD7,
	NUM8 = DIK_NUMPAD8,
	NUM9 = DIK_NUMPAD9,

	// 숫자 외 넘패드 버튼
	NUMDECIMAL = DIK_DECIMAL,		// 넘패드의 . 버튼
	NUMADD = DIK_ADD,			// 넘패드의 + 버튼
	NUMDIVIDE = DIK_DIVIDE,			// 넘패드의 / 버튼
	NUMMINUS = DIK_SUBTRACT,		// 넘패드의 - 버튼
	NUMMULTYPLY = DIK_MULTIPLY,		// 넘패드의 * 버튼
	NUMNUMLCOK = DIK_NUMLOCK,		// 넘패드의 NumLock 버튼
	NUMENTER = DIK_NUMPADENTER,	//넘패드의 NUMENTER 버튼
};