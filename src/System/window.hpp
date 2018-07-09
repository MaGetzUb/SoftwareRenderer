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

#ifndef WINDOW_HPP 
#define WINDOW_HPP

#include <windows.h>
#include <windowsx.h>
#include <string>
#include <functional>

class Window;
using MessageHandler = std::function<LRESULT(Window&, UINT, WPARAM, LPARAM)>;

class Window {
	
	friend struct WindowClass; 

	HWND mWindowHandle;
	HDC mDeviceContext; 

	unsigned mWidth, mHeight;

	MessageHandler mMessageCallback;

	bool mCloseRequested;

	public:

		Window();
		Window(unsigned width, unsigned height, const std::string& title = "A window");
		~Window(); 

		bool initialize(unsigned width, unsigned height, const std::string& title = "A window");
		bool deinitialize();

		void setTitle(const std::string& title);

		unsigned width() const { return mWidth; }
		unsigned height() const { return mHeight;  }

		bool isClosed() const { return mCloseRequested; }

		HWND handle() { return mWindowHandle; }
		HDC dc() { return mDeviceContext; }

		void setMessageCallback(MessageHandler handler) { mMessageCallback = std::move(handler); }

		void pollEvents();

		LRESULT defaultWindowProc(UINT msg, WPARAM wparam, LPARAM lparam);

	private:

		static LRESULT CALLBACK MessageCallback(HWND window, UINT msg, WPARAM wparam, LPARAM lparam); 

		

};


#endif 