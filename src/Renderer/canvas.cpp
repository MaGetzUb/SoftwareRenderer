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

#include "canvas.hpp" 

Canvas::Canvas(Window& window):
	mBufferInfo{},
	mBufferMemory(nullptr),
	mBufferSize(0),
	mWindow(&window)
{
	mBufferInfo.bmiHeader.biPlanes = 1;
	mBufferInfo.bmiHeader.biBitCount = 32;
	mBufferInfo.bmiHeader.biCompression = BI_RGB;
}

Canvas::~Canvas() {
	if(mBufferMemory) {
		_aligned_free(mBufferMemory);
	}
}

void Canvas::resize(int w, int h) {
	
	if(mBufferMemory) _aligned_free(mBufferMemory);
	
	mBufferInfo.bmiHeader.biSize = sizeof(mBufferInfo.bmiHeader);
	mWidth = mBufferInfo.bmiHeader.biWidth = w;
	mHeight = -(mBufferInfo.bmiHeader.biHeight = -h);

	unsigned bytesPerPixel = (mBufferInfo.bmiHeader.biBitCount >> 3);
	mBufferSize = w * h * bytesPerPixel;
	mPitch = w * bytesPerPixel;

	mBufferMemory = (byte*)_aligned_malloc(mWidth*mHeight*sizeof(int), 16);

}

void Canvas::swapBuffers() const {
	StretchDIBits(mWindow->dc(), 0, 0, mWindow->width(), mWindow->height(), 0, 0, mWidth, mHeight, mBufferMemory, &mBufferInfo, DIB_RGB_COLORS, SRCCOPY);
}

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


