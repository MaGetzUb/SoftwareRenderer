/*
Copyright © 2018, Marko Ranta
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

	1. Redistributions of source code must retain the above copyright
	   notice, this list of conditions and the following disclaimer.

	2. Redistributions in binary form must reproduce the above copyright
	   notice, this list of conditions and the following disclaimer in the
	   documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifdef _WIN32
#include <windows.h>
#include <windowsx.h>
#endif

#include <functional>
#include <iostream>
#include "inputmanager.hpp"





InputManager::InputManager():
	mKeyStates{},
	mButtonStates{},
	mMouseX(0),
	mMouseY(0),
	mMouseZ(0),
	mMouseMoveX(0),
	mMouseMoveY(0),
	mMouseMoveZ(0),
	mTextInput(false),
	mInputChar(0),
#ifdef _WIN32
	mCallbackProcesor([this](Frame& frame, UINT msg, WPARAM wParam, LPARAM lParam) -> LRESULT { return process(frame, msg, wParam, lParam); })
#elif defined(__linux__)
	mCallbackProcesor([this](Frame& frame, const XEvent& event){process(frame, event);})
#endif 
{}

InputManager::~InputManager() {}
	
void InputManager::update() {
	mTextInput = false;
	mInputChar = 0;
	mMouseMoveX = 0;
	mMouseMoveY = 0;
	mMouseMoveZ = 0;
	for(unsigned int i = 0; i < 256; i++)
		mKeyStates[i] &= 0x1; 
	for(unsigned int i = 0; i < 5; i++)
		mButtonStates[i] &= 0x1; 
}

bool InputManager::isKeyDown(unsigned char key) const {
	return mKeyStates[key] & 0x1;
}

bool InputManager::isKeyHit(unsigned char key) const {
	return mKeyStates[key] == 0x3;
}

bool InputManager::isKeyUp(unsigned char key) const {
	return mKeyStates[key] == 0x2;
}

bool InputManager::isMouseDown(unsigned char button) const {
	return mButtonStates[button] & 0x1;
}

bool InputManager::isMouseHit(unsigned char button) const {
	return mButtonStates[button] == 0x3;
}
bool InputManager::isMouseUp(unsigned char button) const {
	return mButtonStates[button] == 0x2; 
}

int InputManager::mouseX() const {
	return mMouseX;
}

int InputManager::mouseY() const {
	return mMouseY;
}

int InputManager::mouseZ() const {
	return mMouseZ;
}

int InputManager::mouseMoveX() const {
	return mMouseMoveX;
}

int InputManager::mouseMoveY() const {
	return mMouseMoveY;
}

int InputManager::mouseMoveZ() const {
	return mMouseMoveZ;
}

bool InputManager::isTextInput(unsigned int& c) const {
	c = mInputChar; 
	return mTextInput; 
}

#ifdef _WIN32
int MapKey(WPARAM key, LPARAM ext);

LRESULT InputManager::process(Frame& frame, UINT msg, WPARAM wparam, LPARAM lparam) {
	
	switch(msg) {
		case WM_MOUSEMOVE:
			/*
			TRACKMOUSEEVENT trackInfo; 
			trackInfo.cbSize = sizeof(trackInfo);
			trackInfo.dwFlags = TME_LEAVE;
			trackInfo.hwndTrack = window.handle(); 
			trackInfo.dwHoverTime = HOVER_DEFAULT;
			TrackMouseEvent(&trackInfo); 
			*/
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP: {
			if(frame.isRawInput()) break;
			int index = -1;
			/*SetCapture(window.handle());*/
			switch(msg) {
				case WM_LBUTTONDOWN:
				case WM_LBUTTONUP:
					mButtonStates[0] ^= 1;
					mButtonStates[0] |= 2;
				break;
				case WM_RBUTTONDOWN:
				case WM_RBUTTONUP:
					mButtonStates[1] ^= 1;
					mButtonStates[1] |= 2;
				break;
				case WM_MBUTTONDOWN:
				case WM_MBUTTONUP:
					mButtonStates[2] ^= 1;
					mButtonStates[2] |= 2;
				break;
			}
			
			int x = GET_X_LPARAM(lparam);
			int y = GET_Y_LPARAM(lparam);
			
			mMouseMoveX = x - mMouseX;
			mMouseMoveY = y - mMouseY; 
			mMouseX = x;
			mMouseY = y;
			return 0;
		} break;
		case WM_MOUSELEAVE:
			if(frame.isRawInput()) break;
			for(int i = 0; i < 0; i++) {
				mButtonStates[i] = 0;
			}
			/*ReleaseCapture();*/
			return 0;
		break;
		case WM_CHAR:
			mTextInput = true;
			mInputChar = (unsigned int)wparam;
			return 0;
		break;
		case WM_KEYDOWN:
		case WM_KEYUP: {
			int key = MapKey(wparam, lparam);
			if(((lparam >> 31) & 1) & (mKeyStates[key] & 0x1)) {
				mKeyStates[key] ^= 1;
				mKeyStates[key] |= 2;
			}
			return 0;
		} break;
		case WM_MOUSEWHEEL:
			mMouseMoveZ = GET_WHEEL_DELTA_WPARAM(wparam) / (SHORT)WHEEL_DELTA;
			mMouseZ += mMouseMoveZ;
			return 0;
		break;
		case WM_CLOSE:
		case WM_ENTERSIZEMOVE:
		case WM_EXITSIZEMOVE:
			for(int i = 0; i < 256; i++) {
				mKeyStates[i] = 0;
			}
			for(int i = 0; i < 5; i++) {
				mButtonStates[i] = 0;
			}
			return 0;
		break;
		case WM_INPUT: {

			POINT p;
			GetCursorPos(&p);
			RECT wr;
			GetWindowRect(frame.handle(), &wr);
			if(p.x < wr.left || p.x > wr.right || p.y < wr.top || p.y > wr.bottom || frame.isClosed() || GetActiveWindow() != frame.handle())
				return 0;

			UINT size;
			GetRawInputData((HRAWINPUT)lparam, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER));
			LPBYTE lpb = (LPBYTE)alloca(size);
			if(GetRawInputData((HRAWINPUT)lparam, RID_INPUT, lpb, &size, sizeof(RAWINPUTHEADER)) != size) {
				std::cerr << "GetRawInput returned invalid size!" << std::endl;
			}

			RAWINPUT* inp = (RAWINPUT*)lpb;
			RAWINPUTHEADER& header = inp->header;
			switch(header.dwType) {
				case RIM_TYPEKEYBOARD: {
					RAWKEYBOARD& rawKeyboard = inp->data.keyboard;

					USHORT scan = rawKeyboard.MakeCode;
					USHORT vkey = rawKeyboard.VKey;

					if(vkey = 0xff) {
						return 0;
					}
					else if(vkey == VK_SHIFT) {
						vkey = MapVirtualKey(scan, MAPVK_VSC_TO_VK_EX);
					}

					int key = MapKey(vkey, (rawKeyboard.Flags & RI_KEY_E0) ? 1 << 6 : 0);

					if((rawKeyboard.Flags & 0x1) != mKeyStates[key]) {
						mKeyStates[key] ^= 1;
						mKeyStates[key] |= 2;
					}

				} break;
				case RIM_TYPEMOUSE: {
					RAWMOUSE& rawMouse = inp->data.mouse;
					#define TestButton( BUTTON ) ((rawMouse.ulButtons & BUTTON) == BUTTON)
					#define WhichButton( BUTTON ) (TestButton( BUTTON##_DOWN ) || TestButton( BUTTON##_UP ))
					#define ButtonPressed( BUTTON ) (TestButton( BUTTON##_DOWN) && !TestButton( BUTTON##_UP ))
					#define UpdateState( ButtonState ) \
							ButtonState ^= 1; \
							ButtonState |= 2

					if(WhichButton(RI_MOUSE_LEFT_BUTTON)) {
						UpdateState(mButtonStates[0]);
					}
					else if(WhichButton(RI_MOUSE_RIGHT_BUTTON)) {
						UpdateState(mButtonStates[1]);
					}
					else if(WhichButton(RI_MOUSE_MIDDLE_BUTTON)) {
						UpdateState(mButtonStates[2]);
					}
					else if(WhichButton(RI_MOUSE_BUTTON_4)) {
						UpdateState(mButtonStates[3]);
					}
					else if(WhichButton(RI_MOUSE_BUTTON_5)) {
						UpdateState(mButtonStates[4]);
					}

					#undef ButtonPressed
					#undef SwitchButton
					#undef TestButton


					ScreenToClient(frame.handle(), &p);

					mMouseX = p.x;
					mMouseY = p.y;
					mMouseMoveX = inp->data.mouse.lLastX;
					mMouseMoveY = inp->data.mouse.lLastY;

					mMouseMoveZ = (SHORT)(rawMouse.usButtonData) / WHEEL_DELTA;
					mMouseZ += mMouseMoveZ;

				} break;
			}
			return 0;
		} break;
	}
	
	if(mCustomMessages) 
		return mCustomMessages(frame, msg, wparam, lparam);
	return frame.defaultWindowProc(msg, wparam, lparam);
}

int MapKey(WPARAM key, LPARAM ext) {
	switch(key) {
		case 'A': return Key::A;
		case 'B': return Key::B;
		case 'C': return Key::C;
		case 'D': return Key::D;
		case 'E': return Key::E;
		case 'F': return Key::F;
		case 'G': return Key::G;
		case 'H': return Key::H;
		case 'I': return Key::I;
		case 'J': return Key::J;
		case 'K': return Key::K;
		case 'L': return Key::L;
		case 'M': return Key::M;
		case 'N': return Key::N;
		case 'O': return Key::O;
		case 'P': return Key::P;
		case 'Q': return Key::Q;
		case 'R': return Key::R;
		case 'S': return Key::S;
		case 'T': return Key::T;
		case 'U': return Key::U;
		case 'V': return Key::V;
		case 'W': return Key::W;
		case 'X': return Key::X;
		case 'Y': return Key::Y;
		case 'Z': return Key::Z;
		case VK_ESCAPE: return Key::Esc; 
		case VK_RETURN: if((ext >> 6) & 1) return Key::Enter; else return Key::Return;
		case VK_BACK: return Key::Back;
		case VK_SPACE: return Key::Space;
		case VK_LCONTROL: return Key::LCtrl;
		case VK_RCONTROL: return Key::RCtrl; 
		case VK_LMENU: return Key::LAlt; 
		case VK_RMENU: return Key::RAlt;
		case VK_LSHIFT: return Key::LShift;
		case VK_RSHIFT: return Key::RShift; 
		case VK_UP: return Key::Up;
		case VK_DOWN: return Key::Down;
		case VK_LEFT: return Key::Left;
		case VK_RIGHT: return Key::Right; 
		case VK_TAB: return Key::Tab;
		case VK_CAPITAL: return Key::CapsLock;
		case VK_NUMLOCK: return Key::NumLock;
		case VK_SCROLL: return Key::ScrLock;
		case VK_SNAPSHOT: return Key::Prtscr;
		case VK_PAUSE: return Key::Pause;
		case VK_INSERT: return Key::Insert;
		case VK_HOME: return Key::Home;
		case VK_PRIOR: return Key::PageUp; 
		case VK_NEXT: return Key::PageDown;
		case VK_DELETE: return Key::Delete;
		case VK_END: return Key::End; 
		case 0x30: return Key::Num0; 
		case 0x31: return Key::Num1; 
		case 0x32: return Key::Num2;
		case 0x33: return Key::Num3;
		case 0x34: return Key::Num4;
		case 0x35: return Key::Num5;
		case 0x36: return Key::Num6;
		case 0x37: return Key::Num7;
		case 0x38: return Key::Num8;
		case 0x39: return Key::Num9;
		case VK_NUMPAD0: return Key::Numpad0;
		case VK_NUMPAD1: return Key::Numpad1;
		case VK_NUMPAD2: return Key::Numpad2;
		case VK_NUMPAD3: return Key::Numpad3;
		case VK_NUMPAD4: return Key::Numpad4;
		case VK_NUMPAD5: return Key::Numpad5;
		case VK_NUMPAD6: return Key::Numpad6;
		case VK_NUMPAD7: return Key::Numpad7;
		case VK_NUMPAD8: return Key::Numpad8;
		case VK_NUMPAD9: return Key::Numpad9;
		case VK_F1: Key::F1;
		case VK_F2: Key::F2;
		case VK_F3: Key::F3;
		case VK_F4: Key::F4;
		case VK_F5: Key::F5;
		case VK_F6: Key::F6;
		case VK_F7: Key::F7;
		case VK_F8: Key::F8;
		case VK_F9: Key::F9;
		case VK_F10: Key::F10;
		case VK_F11: Key::F11;
		case VK_F12: Key::F12;
		case VK_OEM_PLUS: return Key::Plus;
		case VK_OEM_MINUS : return Key::Minus;
		case VK_ADD: return Key::Add;
		case VK_SUBTRACT: return Key::Substract;
		case VK_MULTIPLY: return Key::Asterisk;
		case VK_OEM_COMMA: return Key::Comma;
		case VK_OEM_PERIOD: return Key::Period;
	}
	return Key::Undefined;
}


#endif 

#ifdef __linux__

int MapKey(int xkkey) {

	switch(xkkey) {
		case 'a': return Key::A;
		case 'b': return Key::B;
		case 'c': return Key::C;
		case 'd': return Key::D;
		case 'e': return Key::E;
		case 'f': return Key::F;
		case 'g': return Key::G;
		case 'h': return Key::H;
		case 'i': return Key::I;
		case 'j': return Key::J;
		case 'k': return Key::K;
		case 'l': return Key::L;
		case 'm': return Key::M;
		case 'n': return Key::N;
		case 'o': return Key::O;
		case 'p': return Key::P;
		case 'q': return Key::Q;
		case 'r': return Key::R;
		case 's': return Key::S;
		case 't': return Key::T;
		case 'u': return Key::U;
		case 'v': return Key::V;
		case 'w': return Key::W;
		case 'x': return Key::X;
		case 'y': return Key::Y;
		case 'z': return Key::Z;
		case XK_Escape: return Key::Esc; 
		case XK_Return: return Key::Return;
		case XK_BackSpace: return Key::Back;
		case XK_space: return Key::Space;
		case XK_Control_L: return Key::LCtrl;
		case XK_Control_R: return Key::RCtrl; 
		case XK_Alt_L: return Key::LAlt; 
		case XK_Alt_R: return Key::RAlt;
		case XK_Shift_L: return Key::LShift;
		case XK_Shift_R: return Key::RShift; 
		case XK_Up: return Key::Up;
		case XK_Down: return Key::Down;
		case XK_Left: return Key::Left;
		case XK_Right: return Key::Right; 
		case XK_Tab: return Key::Tab;
		case XK_Caps_Lock: return Key::CapsLock;
		case XK_Num_Lock: return Key::NumLock;
		case XK_Scroll_Lock: return Key::ScrLock;
		case XK_Print: return Key::Prtscr;
		case XK_Pause: return Key::Pause;
		case XK_Insert: return Key::Insert;
		case XK_Home: return Key::Home;
		case XK_Page_Up: return Key::PageUp; 
		case XK_Page_Down: return Key::PageDown;
		case XK_Delete: return Key::Delete;
		case XK_End: return Key::End; 
		case XK_0: return Key::Num0; 
		case XK_1: return Key::Num1; 
		case XK_2: return Key::Num2;
		case XK_3: return Key::Num3;
		case XK_4: return Key::Num4;
		case XK_5: return Key::Num5;
		case XK_6: return Key::Num6;
		case XK_7: return Key::Num7;
		case XK_8: return Key::Num8;
		case XK_9: return Key::Num9;
		case XK_KP_0: return Key::Numpad0;
		case XK_KP_1: return Key::Numpad1;
		case XK_KP_2: return Key::Numpad2;
		case XK_KP_3: return Key::Numpad3;
		case XK_KP_4: return Key::Numpad4;
		case XK_KP_5: return Key::Numpad5;
		case XK_KP_6: return Key::Numpad6;
		case XK_KP_7: return Key::Numpad7;
		case XK_KP_8: return Key::Numpad8;
		case XK_KP_9: return Key::Numpad9;
		case XK_F1: return Key::F1;
		case XK_F2: return Key::F2;
		case XK_F3: return Key::F3;
		case XK_F4: return Key::F4;
		case XK_F5: return Key::F5;
		case XK_F6: return Key::F6;
		case XK_F7: return Key::F7;
		case XK_F8: return Key::F8;
		case XK_F9: return Key::F9;
		case XK_F10: return Key::F10;
		case XK_F11: return Key::F11;
		case XK_F12: return Key::F12;
		case XK_plus: return Key::Plus;
		case XK_minus : return Key::Minus;
		case XK_KP_Add: return Key::Add;
		case XK_KP_Subtract: return Key::Substract;
		case XK_KP_Multiply: return Key::Asterisk;
		case XK_comma: return Key::Comma;
		case XK_period: return Key::Period;
	}
	return Key::Undefined;
}

void InputManager::process(Frame& frame, const XEvent& event) {

	switch(event.type) {
		case KeyPress:
		case KeyRelease: {
			int lookup, key = XLookupKeysym(&const_cast<XKeyEvent&>(event.xkey), 0);
			lookup = key;
			key = MapKey(key);
			if(event.type == 2+(mKeyStates[key] & 0x1)) {
				mKeyStates[key] ^= 1;
				mKeyStates[key] |= 2;
			}
		} break;
		case MotionNotify:
			mMouseMoveX = event.xmotion.x - mMouseX;
			mMouseMoveY = event.xmotion.y - mMouseY;

			mMouseX = event.xmotion.x;
			mMouseY = event.xmotion.y;
			
		break;
		case ButtonPress:
		case ButtonRelease:

			if(event.xbutton.button < 4) {

				if(event.type == 4+(mButtonStates[event.xbutton.button-1] & 0x1)) {
					mButtonStates[event.xbutton.button-1] ^=1;
					mButtonStates[event.xbutton.button-1] |=2;
				}

			} else if((event.xbutton.button == 4 || event.xbutton.button == 5) && event.type == ButtonPress) {
				int oldZ = mMouseZ;
				mMouseZ += ((int)(event.xbutton.button == 4) - (int)(event.xbutton.button == 5));	
				mMouseMoveZ = mMouseZ - oldZ; 
			}

		break;
	}

	if(mCustomMessages) mCustomMessages(frame, event);

}

#endif 
