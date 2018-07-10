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

#ifndef STARFIELD_HPP
#define STARFIELD_HPP

#include "Renderer/canvas.hpp"
#include <vector>
#include <algorithm>


class Starfield {

	Canvas* mCanvas;

	struct Star { float x, y, z; };

	std::vector<Star> mStars;

	public:

	Starfield(Canvas& canvas, int numStars = 2048) :
		mCanvas(&canvas)
	{
		mStars.reserve(numStars);
		for(int i = 0; i < numStars; i++) {
			mStars.emplace_back(Star{
				(float)(-canvas.width() + 2 * (rand() % canvas.width())),
				(float)(-canvas.height() + 2 * (rand() % canvas.height())),
				(float)(-(rand() % 512))
			});
		}
	}

	void updateAndDraw(float dt) {
		std::sort(mStars.begin(), mStars.end(), [](const Star& a, const Star& b) -> bool {
			return a.z < b.z;
		});
		for(size_t i = 0; i < mStars.size(); i++) {

			float sz = 1.f / (mStars[i].z * 0.1f);
			float sx = mCanvas->width() / 2 + mStars[i].x * sz * 10.f;
			float sy = mCanvas->height() / 2 - mStars[i].y * sz * 10.f;

			mStars[i].z += 10.0f*(dt / 1000.0f);
			if(mStars[i].z > 0) mStars[i].z = -512;
			if(sx >= 0 && sx < mCanvas->width() && sy >= 0 && sy < mCanvas->height()) {
				byte br = (byte)(255.0f*((512.0f + mStars[i].z) / 512.0f));
				mCanvas->set((int)sx, (int)sy, br, br, br);
			}
		}
	}

};

#endif //STARFIELD_HPP