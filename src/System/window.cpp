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
#include "window.hpp" 
#include <iostream>
#include <cassert>


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
		instance.lpfnWndProc = &Window::MessageCallback;
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

Window::Window():
	mWidth(0),
	mHeight(0),
	mCloseRequested(false)
{}

Window::Window(unsigned width, unsigned height, const std::string& title) {
	initialize(width, height, title);
}

Window::~Window() {
	deinitialize();
}

bool Window::initialize(unsigned width, unsigned height, const std::string& title) {
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
		std::cerr << "Window creation failed!" << std::endl;
		return false;
	}

	GetClientRect(mWindowHandle, &rect);
	mWidth = rect.right - rect.left;
	mHeight = rect.bottom - rect.top;

	SetActiveWindow(mWindowHandle);

	mDeviceContext = GetDC(mWindowHandle);

	return true;
}

bool Window::deinitialize() {
	ReleaseDC(mWindowHandle, mDeviceContext);
	DestroyWindow(mWindowHandle);
	return true;
}

void Window::setTitle(const std::string& title) {
	SetWindowTextA(mWindowHandle, title.c_str());
}


void Window::pollEvents() {
	MSG msg;
	while(PeekMessage(&msg, mWindowHandle, NULL, NULL, PM_REMOVE) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

LRESULT Window::defaultWindowProc(UINT msg, WPARAM wparam, LPARAM lparam) {
	return DefWindowProc(mWindowHandle, msg, wparam, lparam);
}

LRESULT CALLBACK Window::MessageCallback(HWND windowHandle, UINT msg, WPARAM wparam, LPARAM lparam) {
	if(msg == WM_CREATE) {
		LONG_PTR windowPointer = (LONG_PTR)((CREATESTRUCT*)(lparam))->lpCreateParams;
		SetWindowLongPtr(windowHandle, GWLP_USERDATA, windowPointer);
	}

	Window* windowPointer = windowHandle ? (Window*)GetWindowLongPtrA(windowHandle, GWLP_USERDATA) : nullptr;

	if(!windowPointer)
		return DefWindowProc(windowHandle, msg, wparam, lparam);
	switch(msg) {
		case WM_CLOSE:
			windowPointer->mCloseRequested = true;
		break;
		case WM_SIZE: { //Intercept the WM_SIZE message, and
			// regain the device context. 
			ReleaseDC(windowHandle, windowPointer->mDeviceContext);
			windowPointer->mDeviceContext = GetDC(windowHandle);
			// Update also the window size. 
			windowPointer->mWidth = GET_X_LPARAM(lparam);
			windowPointer->mHeight = GET_Y_LPARAM(lparam);
		} break;
	}


	if(windowPointer->mMessageCallback)
		return windowPointer->mMessageCallback(*windowPointer, msg, wparam, lparam);

	return DefWindowProc(windowHandle, msg, wparam, lparam);
}
