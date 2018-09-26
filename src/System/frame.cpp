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
#elif defined(__linux__)
#include <GL/glx.h>
#include <GL/glxext.h>
#include <dlfcn.h>
#endif

#include "frame.hpp" 
#include <iostream>
#include <cassert>


Frame::Frame(unsigned width, unsigned height, const std::string& title) {
	initialize(width, height, title);
}

Frame::~Frame() {
	deinitialize();
}

#ifdef _WIN32

static HGLRC gFirstContext = nullptr;

struct WindowClass {
	WNDCLASS instance;
	HINSTANCE module;
	std::string name;

	WindowClass(): 
		instance{},
		module(GetModuleHandleA(0)),
		name("TestWindowClass")
	{
		instance.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		instance.lpfnWndProc = &Frame::MessageCallback;
		instance.lpszClassName = name.c_str();
		instance.hCursor = LoadCursor(nullptr, IDC_ARROW);
		instance.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
		instance.hInstance = module;
		instance.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		RegisterClassA(&instance);
	}

	~WindowClass() {
		UnregisterClassA(name.c_str(), module);
	}

} gWindowClass;



bool Frame::initialize(unsigned width, unsigned height, const std::string& title) {
	mWidth = width;
	mHeight = height;

	POINT p;
	MONITORINFO mi;
	RECT rect;
	GetCursorPos(&p);

	if(GetPhysicalCursorPos(&p) == TRUE) {
		if(GetMonitorInfo(MonitorFromPoint(p, MONITOR_DEFAULTTOPRIMARY), &mi) == TRUE) {

			int monitorW = mi.rcMonitor.right - mi.rcMonitor.left;
			int monitorH = mi.rcMonitor.bottom - mi.rcMonitor.top;

			rect.left = mi.rcMonitor.left + monitorW / 2;
			rect.top = mi.rcMonitor.top + monitorH / 2;
			rect.left -= (width / 2);
			rect.top -= (height / 2);
			rect.right = rect.left + width;
			rect.bottom = rect.top + height;
			//mOriginalRect = rect;
		} else {
			goto DefaultPosition;
		}
	} else {
	DefaultPosition:
		//Position it in the middle of the main monitor.
		rect.left = GetSystemMetrics(SM_CXSCREEN) / 2 - width / 2;
		rect.top = GetSystemMetrics(SM_CYSCREEN) / 2 - height / 2;
		rect.right = rect.left + width;
		rect.bottom = rect.top + height;
		//mOriginalRect = rect;
	}

	DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
	AdjustWindowRect(&rect, style, FALSE);

	assert(IsWindow(mWindowHandle) == FALSE);
	mWindowHandle = CreateWindowExA(
		0, 
		gWindowClass.name.c_str(), 
		title.c_str(),
		style, 
		rect.left,
		rect.top,
		rect.right - rect.left,
		rect.bottom - rect.top,
		0, 
		0, 
		gWindowClass.module,
		(void*)this
	);

	if(mWindowHandle == nullptr) {
		std::cerr << "Frame creation failed!" << std::endl;
		return false;
	}

	GetClientRect(mWindowHandle, &rect);
	mWidth = rect.right - rect.left;
	mHeight = rect.bottom - rect.top;

	SetActiveWindow(mWindowHandle);

	mDeviceContext = GetDC(mWindowHandle);
	return true;
}

bool Frame::deinitialize() {
	if(wglGetCurrentDC() == mDeviceContext) {
		HGLRC rc = wglGetCurrentContext();
		wglMakeCurrent(mDeviceContext, 0);
		wglDeleteContext(rc);
	}
	ReleaseDC(mWindowHandle, mDeviceContext);
	DestroyWindow(mWindowHandle);
	return true;
}

void Frame::setTitle(const std::string& title) {
	SetWindowTextA(mWindowHandle, title.c_str());
}

void Frame::useRawInput() {

	mMouse.usUsagePage = 0x01;
	mMouse.usUsage = 0x02;
	mMouse.dwFlags = 0;
	mMouse.hwndTarget = mWindowHandle;

	mKeyboard.usUsagePage = 0x01;
	mKeyboard.usUsage = 0x06;
	mKeyboard.dwFlags = 0;
	mKeyboard.hwndTarget = mWindowHandle;

	if(RegisterRawInputDevices(&mMouse, 1, sizeof(mMouse)) == FALSE)
		std::cerr << "Couldn't register mouse RAWINPUT device!" << std::endl;
	
	if(RegisterRawInputDevices(&mKeyboard, 1, sizeof(mKeyboard)) == FALSE)
		std::cerr << "Couldn't register keyboard RAWINPUT device!" << std::endl;


	mRawInput = true;
}


void Frame::setUpOpenGL() {

	mOpenGLConfiguration = true; //Ensures DeviceContext is not regained after resize.

	PIXELFORMATDESCRIPTOR pfd{};
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_SWAP_EXCHANGE;
	pfd.iLayerType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cAlphaBits = 8;

	int pfmt = ChoosePixelFormat(mDeviceContext, &pfd);
	SetPixelFormat(mDeviceContext, pfmt, &pfd);
	HGLRC glcontext = wglCreateContext(mDeviceContext);

	HGLRC existing = wglGetCurrentContext();

	wglMakeCurrent(mDeviceContext, glcontext);

	if(existing) {
		wglShareLists(existing, glcontext);
	}

}

void Frame::pollEvents() {
	MSG msg;
	while(PeekMessage(&msg, mWindowHandle, NULL, NULL, PM_REMOVE) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

LRESULT Frame::defaultWindowProc(UINT msg, WPARAM wparam, LPARAM lparam) {
	return DefWindowProc(mWindowHandle, msg, wparam, lparam);
}

LRESULT CALLBACK Frame::MessageCallback(HWND windowHandle, UINT msg, WPARAM wparam, LPARAM lparam) {
	if(msg == WM_CREATE) {
		LONG_PTR windowPointer = (LONG_PTR)((CREATESTRUCT*)(lparam))->lpCreateParams;
		SetWindowLongPtr(windowHandle, GWLP_USERDATA, windowPointer);
	}

	Frame* windowPointer = windowHandle ? (Frame*)GetWindowLongPtrA(windowHandle, GWLP_USERDATA) : nullptr;

	if(!windowPointer)
		return DefWindowProc(windowHandle, msg, wparam, lparam);
	switch(msg) {
		case WM_CLOSE:
			windowPointer->mCloseRequested = true;
		break;
		case WM_SIZE: { //Intercept the WM_SIZE message, and
			// regain the device context. 
			if(!windowPointer->mOpenGLConfiguration) {
				ReleaseDC(windowHandle, windowPointer->mDeviceContext);
				windowPointer->mDeviceContext = GetDC(windowHandle);
			}
			// Update also the Frame size. 
			windowPointer->mWidth = GET_X_LPARAM(lparam);
			windowPointer->mHeight = GET_Y_LPARAM(lparam);
		} break;
	}


	if(windowPointer->mMessageCallback)
		return windowPointer->mMessageCallback(*windowPointer, msg, wparam, lparam);

	return DefWindowProc(windowHandle, msg, wparam, lparam);
}

void Frame::present() {
	if(mDeviceContext) SwapBuffers(mDeviceContext);
}

#ifdef NDEBUG
extern int main(int argc, char* argv[]);

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	main(__argc, __argv);
}
#endif 

#elif defined(__linux__)

PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = nullptr;


bool Frame::initialize(unsigned width, unsigned height, const std::string& title) {
	mDisplay = XOpenDisplay(nullptr);
	if(mDisplay == nullptr) return false;
	mRootWindow = DefaultRootWindow(mDisplay);


	if(mRootWindow == NULL) return false;
	XWindowAttributes attribs;
	XGetWindowAttributes(mDisplay, mRootWindow, &attribs);



	mWindowAttributes.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | Button1MotionMask | FocusChangeMask;

	mWindow = XCreateWindow(
		mDisplay, 
		mRootWindow, 
		attribs.width / 2 - width / 2, 
		attribs.height / 2 - height / 2, 
		width, 
		height, 
		0,
		CopyFromParent,
		InputOutput,
		CopyFromParent, 
		CWColormap | CWEventMask, 
		&mWindowAttributes
	);

	mWidth = width;
	mHeight = height;
	XStoreName(mDisplay, mWindow, title.c_str());
	XMapWindow(mDisplay, mWindow);



	return 0;
}

void Frame::setTitle(const std::string& title) {
	XStoreName(mDisplay, mWindow, title.c_str());
}

bool Frame::deinitialize() {

	if(mDeviceContext) {
		glXMakeContextCurrent(mDisplay, mWindow, 0, 0);
		glXDestroyContext(mDisplay, mDeviceContext);
	}
	XDestroyWindow(mDisplay, mWindow);
	XCloseDisplay(mDisplay);
}

GLXFBConfig pickBestConfig(Display* display, int* attribs) {

	int fbCount;
	GLXFBConfig* fbc = glXChooseFBConfig(display, DefaultScreen(display), attribs, &fbCount);


	if(fbCount <= 0) {
		std::cerr << "Couldn't find even single GLXFBConfig!\n";
		abort();
	}

	assert(fbc && "Failed to retrieve a framebuffer config!");

	int bestFbc = -1, worstFbc = -1, bestNumSamples = -1, worstNumSamples = 999;

	for(int i = 0; i < fbCount; i++) {
		XVisualInfo *vi = glXGetVisualFromFBConfig(display, fbc[i]);
		if(vi) {
			int sampleBuffers, numSamples;
			glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLE_BUFFERS, &sampleBuffers);
			glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLES, &numSamples);

			if(bestFbc < 0 || sampleBuffers && numSamples > bestNumSamples) bestFbc = i, bestNumSamples = numSamples;
			if(worstFbc < 0 || !sampleBuffers || numSamples < worstNumSamples) worstFbc = i, worstNumSamples = numSamples;
		}
		XFree(vi);
	}

	return fbc[bestFbc];
}




void Frame::setUpOpenGL() {

  	
	int glxMajo, glxMino;
	if(!glXQueryVersion(mDisplay, &glxMajo, &glxMino) || ((glxMajo == 1) && (glxMino < 3)) || (glxMajo < 1)) {
		std::cout << "GLX version invalid!\n";
		exit(1);
  	}

	
	glXCreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC)glXGetProcAddress((GLubyte*)"glXCreateContextAttribsARB");

 	static int visualAttribs[] = {
    	GLX_X_RENDERABLE, True,
    	GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
    	GLX_RENDER_TYPE, GLX_RGBA_BIT,
    	GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
    	GLX_RED_SIZE, 8,
    	GLX_GREEN_SIZE, 8,
    	GLX_BLUE_SIZE, 8,
    	GLX_ALPHA_SIZE, 8,
    	GLX_DEPTH_SIZE, 24,
    	GLX_STENCIL_SIZE, 8,
    	GLX_DOUBLEBUFFER, True,
    	//GLX_SAMPLE_BUFFERS, 1,
    	//GLX_SAMPLES, 4,
    	None
    };

	GLXFBConfig conf = pickBestConfig(mDisplay, visualAttribs);

	mVisualInfo = glXGetVisualFromFBConfig(mDisplay, conf);

	mRootWindow = RootWindow(mDisplay, mVisualInfo->screen);

	mColorMap = XCreateColormap(mDisplay, mRootWindow, mVisualInfo->visual, AllocNone);
	
	int numColormaps = XInstallColormap(mDisplay, mColorMap);


	int attribs[] = {
		GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
		GLX_CONTEXT_MINOR_VERSION_ARB, 3,
		#ifdef DEBUG
		GLX_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
		#endif
		GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
		None
	};


	mDeviceContext = glXCreateContextAttribsARB(mDisplay, conf, NULL, True, attribs);
 	glXMakeCurrent(mDisplay, mWindow, mDeviceContext);


}

void Frame::pollEvents() {

	XEvent event;
	while(XPending(mDisplay)) {
		XNextEvent(mDisplay, &event);
		switch(event.type) {
			case Expose: {
				XWindowAttributes attribs;
				XGetWindowAttributes(mDisplay, mWindow, &attribs);
				mWidth = attribs.width;
				mHeight = attribs.height;
				mMessageCallback(*this, event);
			} break;
			case ClientMessage:
				mCloseRequested = true;
				mMessageCallback(*this, event);
			break;
			default:
				mMessageCallback(*this, event);
			break;
		}
	}

}

void Frame::present() {
	if(mDeviceContext) glXSwapBuffers(mDisplay, mWindow);
}

#endif 



