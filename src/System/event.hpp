#ifndef EVENT_HPP
#define EVENT_HPP

struct Key {
	enum {
		Undefined = ~0,
		A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
		Esc, Return, Back, Space, LCtrl, LAlt, RCtrl, RAlt, LShift, RShift, Up, Down, Left, Right, Tab, Enter, CapsLock, NumLock, ScrLock, Prtscr, Pause,
		Insert, Home, PageUp, Delete, End, PageDown,
		Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,
		Numpad0, Numpad1, Numpad2, Numpad3, Numpad4, Numpad5, Numpad6, Numpad7, Numpad8, Numpad9,
		F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, Plus, Minus, Add, Substract, Asterisk, Comma, Period,
		KeyCount
	};
};

static const char* KeyName(int key) {

	static const char* keys[] = {
		"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
		"Esc", "Return", "Back", "Space", "LCtrl", "LAlt", "RCtrl", "RAlt", "LShift", "RShift", "Up", "Down", "Left", "Right", "Tab", "Enter", "CapsLock", "NumLock", "ScrLock", "Prtscr", "Pause",
		"Insert", "Home", "PageUp", "Delete", "End", "PageDown",
		"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
		"Numpad 0", "Numpad 1", "Numpad 2", "Numpad 3", "Numpad 4", "Numpad 5", "Numpad 6", "Numpad 7", "Numpad 8", "Numpad 9",
		"F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12", "Plus", "Minus", "Add", "Substract", "Asterisk", "Comma", "Period"
	};

	if(key > Key::Undefined && key < Key::KeyCount) {
		return keys[key];
	}
	return "Undefined";
}


struct Event {

	enum Type {
		MouseMove,
		MousePressed,
		MouseReleased,
		KeyPressed,
		KeyReleased,
		TextInput,
		Resize,
		Close
	} type;

	union {
		struct {
			float x, y;
			float dx, dy;
		} mouseMove;
		struct {
			int keyCode;
		};
		struct {
			int width, height;
		} size;
		char character;
	};
};





#endif 

