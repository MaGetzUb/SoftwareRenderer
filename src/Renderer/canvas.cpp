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

#include <cstdlib>
#include "canvas.hpp" 
#include "../System/memory.hpp"

#ifdef __linux__
#include <X11/extensions/Xrender.h>
#endif 

#ifdef _WIN32
Canvas::Canvas(Frame& window):
	mBufferInfo{},
	mBufferMemory(nullptr),
	mBufferSize(0),
	mFrame(&window)
{
	mBufferInfo.bmiHeader.biPlanes = 1;
	mBufferInfo.bmiHeader.biBitCount = 32;
	mBufferInfo.bmiHeader.biCompression = BI_RGB;
}
#elif defined(__linux__)
Canvas::Canvas(Frame& frame):
	mBufferMemory(nullptr),
	mBufferSize(0),
	mFrame(&frame)
{

}

#endif 

Canvas::~Canvas() {
	if(mBufferMemory) {
		free(mBufferMemory);
	}
}

void Canvas::resize(int w, int h) {
	
	if(mBufferMemory) _aligned_free(mBufferMemory);
	
	#if _WIN32

	mBufferInfo.bmiHeader.biSize = sizeof(mBufferInfo.bmiHeader);
	mWidth = mBufferInfo.bmiHeader.biWidth = w;
	mHeight = -(mBufferInfo.bmiHeader.biHeight = -h);

	unsigned bytesPerPixel = (mBufferInfo.bmiHeader.biBitCount >> 3);

	#elif defined(__linux__)

	int bytesPerPixel = 4;
	mWidth = w;
	mHeight = h;

	#endif 

	mBufferSize = w * h * bytesPerPixel;
	mPitch = w * bytesPerPixel;
	mBufferMemory = (byte*)_aligned_malloc(mWidth*mHeight*sizeof(int), 16);

	#ifdef __linux__

	FrameInfo info = mFrame->frameInfo();
	//mPixmap = XCreatePixmap(info.display, info.window, mWidth, mHeight, bitDepth);

	XGCValues gcValues;
	mGC = XCreateGC(info.display, info.window, 0, &gcValues);

	#endif 
}

void Canvas::swapBuffers() const {
	#ifdef _WIN32
		StretchDIBits(mFrame->dc(), 0, 0, mFrame->width(), mFrame->height(), 0, 0, mWidth, mHeight, mBufferMemory, &mBufferInfo, DIB_RGB_COLORS, SRCCOPY);
	#elif defined(__linux__)



	FrameInfo frameInfo = mFrame->frameInfo();
	Display* display = frameInfo.display;
	Window window = frameInfo.window;

	if(mFrameWidth != mFrame->width() || mFrameHeight != mFrame->height()) {
		

		constexpr int bitDepth = 24;
		constexpr int padding = 32;
		constexpr int bytesPerPixel = padding >> 3;

		mFrameWidth = (int)mFrame->width();
		mFrameHeight = (int)mFrame->height();

		if(mImage) XDestroyImage(mImage); //According the docs, this also deletes the allocated bitmap buffer.

		mFramebufferMemory = (byte*)_aligned_malloc(mFrameWidth*mFrameHeight*bytesPerPixel, 16);

		FrameInfo info = mFrame->frameInfo();
		Visual* visual = DefaultVisual(info.display, 0);

		mImage = XCreateImage(info.display, CopyFromParent, bitDepth, ZPixmap, 0, (char*)mFramebufferMemory, mFrameWidth, mFrameHeight, padding, 0);
	}

	#pragma omp parallel for 
	for(int y = 0; y < mFrameHeight; y++)
	for(int x = 0; x < mFrameWidth; x++)
	{
		int sx = mWidth * x / mFrameWidth;
		int sy = mHeight * y / mFrameHeight;
		((unsigned*)mFramebufferMemory)[x + y *  mFrameWidth] = ((unsigned*)mBufferMemory)[sx + sy * mWidth];
	}

	/*
	for(int y = 0; y < mHeight; y++)
	for(int x = 0; x < mWidth; x++)
		((unsigned*)mFramebufferMemory)[x+y*mFrameWidth] = ((unsigned*)mBufferMemory)[x + y * mWidth];
	*/

	XPutImage(display, window, mGC, mImage, 0, 0, 0, 0, mFrameWidth, mFrameHeight);

	
	#endif 
}

#ifdef USE_SIMD
void Canvas::clear(unsigned int color) {
	__m128i pixel = _mm_set1_epi32(color);
	const int pixels = mWidth * mHeight;
	for(int i = 0; i < pixels; i+=4) {
		_mm_store_si128((__m128i*)(mBufferMemory+i*4), pixel);
	}
}

void Canvas::clear(const vec4& color) {
	__m128i pixel = _mm_set1_epi32(Vec4ToPixel(color));
	const int pixels = mWidth * mHeight;
	for(int i = 0; i < pixels; i += 4) {
		_mm_store_si128((__m128i*)(mBufferMemory + i * 4), pixel);
	}
}

#else 

void Canvas::clear(unsigned int color) {
	const int pixels = mWidth * mHeight;
	for(int i = 0; i < pixels; i++) {
		((unsigned int*)mBufferMemory)[i] = color;
	}
}

void Canvas::clear(const vec4& color) {
	const int pixels = mWidth * mHeight;
	unsigned int color = Vec4ToPixel(color);
	for(int i = 0; i < pixels; i++) {
		((unsigned int*)mBufferMemory)[i] = color;
	}
}

#endif 

void Canvas::clearCheckerboard(int index, const vec4& color) {
	int pix = Vec4ToPixel(color);
	int* pixBuf = (int*)mBufferMemory;
	const int pixels = mWidth * mHeight;
	
	for(int i = 0; i < pixels; i += 4) {
		int cy = (i/mWidth)&1;
		int st = !(cy^index);
		pixBuf[i + st] = pix;
		pixBuf[i + 2 + st] = pix;
	}
}
