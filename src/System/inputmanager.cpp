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

#include <windows.h>
#include <windowsx.h>
#include <functional>
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
	mCallbackProcesor(std::bind(&InputManager::process, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4))
{}

InputManager::~InputManager() {}
	
void InputManager::update() {
	mTextInput = false;
	mInputChar = 0;
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

LRESULT InputManager::process(Window& window, UINT msg, WPARAM wparam, LPARAM lparam) {
	
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
			for(int i = 0; i < 3; i++) {
				if(mButtonStates[i] & 0x2) {
					mButtonStates[2] ^= 1;
					mButtonStates[2] |= 2;
				}
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
			if(((lparam >> 31) & 1) == (mKeyStates[wparam] & 0x1)) {
				mKeyStates[wparam] ^= 1;
				mKeyStates[wparam] |= 2;
			}
			return 0;
		} break;
		case WM_MOUSEWHEEL:
			mMouseMoveZ = GET_WHEEL_DELTA_WPARAM(wparam) / (SHORT)WHEEL_DELTA;
			mMouseZ += mMouseMoveZ;
			return 0;
		break;
	}
	
	if(mCustomMessages) 
		return mCustomMessages(window, msg, wparam, lparam);
	return window.defaultWindowProc(msg, wparam, lparam);
}
