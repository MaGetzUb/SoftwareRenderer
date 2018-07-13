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

#ifndef CANVAS_HPP 
#define CANVAS_HPP

#include "../System/window.hpp"
#include "../Math/vec4.hpp"
using byte = unsigned char;

class Canvas {

	BITMAPINFO mBufferInfo;
	byte* mBufferMemory;
	unsigned mBufferSize;
	Window* mWindow;

	int mWidth, mHeight;
	int mPitch;

	public:


		Canvas(Window& window);
		~Canvas();

		void resize(int w, int h);

		void swapBuffers() const;

		inline void clear(byte r, byte g, byte b) { clear(0xff << 24 | r << 16 | g << 8 | b); }

		void clear(unsigned int color = 0x0);

		void clear(const vec4& color);

		void clearCheckerboard(int index, const vec4& color);

		int width() const { return mWidth; }
		int height() const { return mHeight; }
		int pitch() const { return mPitch; }

		unsigned int pixel(int x, int y) {
			if(x < 0 || x > (mWidth - 1) || y < 0 || y > (mHeight - 1))
				return 0;
			int index = (x + y*mWidth);
			unsigned int p = *((unsigned int*)mBufferMemory + index);
			return (p & 0xff) << 16 | p & 0x00ff00 | (p & 0xff0000) >> 16;
		}

		void set(int x, int y, unsigned int pixel) {
			int index = (x + y*mWidth);
			*((unsigned int*)mBufferMemory + index) = pixel;
		}

		inline void set(int x, int y, byte r, byte g, byte b) {
			set(x, y, 0xff << 24 | r << 16 | g << 8 | b);
		}

		#ifdef USE_SIMD
		inline void set(int x, int y, const vec4& color) {
			set(x, y, Vec4ToPixel(color));
		}
		#else
		inline void set(int x, int y, const vec4& color) {
			set(x, y, (byte)(color.x * 255.f), (byte)(color.y * 255.f), (byte)(color.z * 255.f));
		}
		#endif 


		unsigned char* buffer() { return mBufferMemory; }

		void blit(int x, int y, int w, int h, unsigned int* buf);


};


#endif //CANVAS_HPP