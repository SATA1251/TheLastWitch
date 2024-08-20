#include <dinput.h>

#pragma comment(lib, "dinput8")
#pragma comment(lib, "dxguid")

enum class KEY
{
	//Ű���� ����Ű
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

	//Ű���� ���ĺ�Ű
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

	// FunctionŰ
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

	//Ű���� ��ȣŰ
	APOSTROPE = DIK_APOSTROPHE,	// ' ��ư
	BACKSLASH = DIK_BACKSLASH,	// \ ��ư
	COMMA = DIK_COMMA,			// , ��ư
	PERIOD = DIK_PERIOD,		// . Ű
	LBRACKET = DIK_LBRACKET,	// [ ��ư
	RBRACKET = DIK_RBRACKET,	// ] ��ư
	EQUALS = DIK_EQUALS,		// = ��ư
	MINUS = DIK_MINUS,			// - ��ư
	GRAVE = DIK_GRAVE,			// ` ��ư
	SEMICOLON = DIK_SEMICOLON,	// ; Ű
	SLASH = DIK_SLASH,			// / Ű

	// ����Ű ���
	DDELETE = DIK_DELETE,
	END = DIK_END,
	HOME = DIK_HOME,
	INSERT = DIK_INSERT,
	PGUP = DIK_PRIOR,			// PgUp ��ư
	PGDOWN = DIK_NEXT,			// PgDown ��ư
	SCRLK = DIK_SCROLL,			// Scroll Lock ��ư
	PAUSE = DIK_PAUSE,			// Pause ��ư
	PRTSC = DIK_SYSRQ,			// PrtSc ��ư

	// ����Ű
	UP = DIK_UP,
	DOWN = DIK_DOWN,
	LEFT = DIK_LEFT,
	RIGHT = DIK_RIGHT,

	// Ű���� ���Ű
	ESCAPE = DIK_ESCAPE,		// esc ��ư
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

	// ���е� ����Ű
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

	// ���� �� ���е� ��ư
	NUMDECIMAL = DIK_DECIMAL,		// ���е��� . ��ư
	NUMADD = DIK_ADD,			// ���е��� + ��ư
	NUMDIVIDE = DIK_DIVIDE,			// ���е��� / ��ư
	NUMMINUS = DIK_SUBTRACT,		// ���е��� - ��ư
	NUMMULTYPLY = DIK_MULTIPLY,		// ���е��� * ��ư
	NUMNUMLCOK = DIK_NUMLOCK,		// ���е��� NumLock ��ư
	NUMENTER = DIK_NUMPADENTER,	//���е��� NUMENTER ��ư
};