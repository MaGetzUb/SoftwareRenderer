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

#ifndef INPUTMANAGER_HPP
#define INPUTMANAGER_HPP
#include "window.hpp"

class InputManager { //An input manager
	
	char mKeyStates[256]; 
	char mButtonStates[5]; 
	
	int mMouseX, mMouseY, mMouseZ;
	int mMouseMoveX, mMouseMoveY, mMouseMoveZ;
	
	bool mTextInput; 
	unsigned int mInputChar; 
	
	MessageHandler mCustomMessages; 
	MessageHandler mCallbackProcesor; 
	
	public:
	
		InputManager();
		~InputManager();
		
		void update(); 
		
		bool isKeyDown(unsigned char key) const;
		bool isKeyHit(unsigned char key) const;
		bool isKeyUp(unsigned char key) const; 
		
		bool isMouseDown(unsigned char key) const;
		bool isMouseHit(unsigned char key) const;
		bool isMouseUp(unsigned char key) const; 
		
		int mouseX() const;
		int mouseY() const;
		int mouseZ() const; 
		
		int mouseMoveX() const;
		int mouseMoveY() const;
		int mouseMoveZ() const; 
		
		inline void setCustomMessageCallback(MessageHandler&& handler) { mCustomMessages = std::move(handler); }
		inline const MessageHandler& callbackProcessor() const { return mCallbackProcesor; };
		
		bool isTextInput(unsigned int& c) const; 
		
		void ignoreMouseMoveEvents(Window& window);
		
		LRESULT process(Window& window, UINT msg, WPARAM wparam, LPARAM lparam);
};

#endif //INPUTMANAGER_HPP