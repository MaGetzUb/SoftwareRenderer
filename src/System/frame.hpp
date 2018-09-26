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

#ifndef FRAME_HPP 
#define FRAME_HPP

#ifdef _WIN32
#include <windows.h>
#include <windowsx.h>
#elif defined(__linux__) 
#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/glx.h>
#endif 

#include <string>
#include <functional>

class Frame;
#ifdef _WIN32
using MessageHandler = std::function<LRESULT(Frame&, UINT, WPARAM, LPARAM)>;
#elif defined(__linux__)
using MessageHandler = std::function<void(Frame& frame, const XEvent& event)>;

struct FrameInfo {
	Display* display;
	Window window;
};

#endif 



class Frame {
	

#ifdef _WIN32
	friend struct WindowClass; 

	HWND mWindowHandle = nullptr;
	HDC mDeviceContext = nullptr; 
	
	RAWINPUTDEVICE mMouse;
	RAWINPUTDEVICE mKeyboard;
	bool mRawInput;
#elif defined(__linux__)
	GLXContext mDeviceContext;
	Display* mDisplay = nullptr;
	Window mWindow;
	Window mRootWindow;
	XVisualInfo* mVisualInfo = nullptr;
	Colormap mColorMap;
	XSetWindowAttributes mWindowAttributes;
#endif 

	unsigned mWidth = 0, mHeight = 0;

	MessageHandler mMessageCallback;

	bool mCloseRequested = false;
	bool mOpenGLConfiguration = false;

	public:

		Frame() = default;
		Frame(const Frame&) = delete;
		Frame(unsigned width, unsigned height, const std::string& title = "A window");
		~Frame(); 

		bool initialize(unsigned width, unsigned height, const std::string& title = "A window");
		bool deinitialize();

		void setTitle(const std::string& title);

		unsigned width() const { return mWidth; }
		unsigned height() const { return mHeight;  }

		void useRawInput();

		bool isRawInput() const { 
			#ifdef _WIN32
			return mRawInput; 
			#else 
			return false;
			#endif 
		}

		bool isClosed() const { return mCloseRequested; }


		void setUpOpenGL();

		inline void setMessageCallback(MessageHandler&& handler) { mMessageCallback = std::move(handler); }

		inline void setMessageCallback(const MessageHandler& handler) { mMessageCallback = handler; }

		void pollEvents();

		void present();

#ifdef _WIN32

		HWND handle() { return mWindowHandle; }

		HDC dc() { return mDeviceContext; }

		LRESULT defaultWindowProc(UINT msg, WPARAM wparam, LPARAM lparam);
#elif defined(__linux__)
		FrameInfo frameInfo() {
			return FrameInfo{mDisplay, mWindow};
		}
#endif
	
	private:

#ifdef _WIN32
		static LRESULT CALLBACK MessageCallback(HWND window, UINT msg, WPARAM wparam, LPARAM lparam); 
#elif defined(__linux__)

#endif

		

};


#endif //FRAME_HPP
