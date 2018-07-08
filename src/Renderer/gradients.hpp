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

#ifndef GRADIENTS_HPP
#define GRADIENTS_HPP
#include "../Math/vec4.hpp"
#include "vertex.hpp"

class Gradients {
	
	vec4 mColors[3], mColorXStep, mColorYStep;
	vec2 mTexCoord[3], mTexCoordXStep, mTexCoordYStep;

	public:

		Gradients(const Vertex& a, const Vertex& b, const Vertex& c) {

			mColors[0] = a.color();
			mColors[1] = b.color();
			mColors[2] = c.color();

			mTexCoord[0] = a.texCoord();
			mTexCoord[1] = b.texCoord();
			mTexCoord[2] = c.texCoord();

			float oneOverDX = 1.0f / (((b.x() - c.x()) * (a.y() - c.y())) - ((a.x() - c.x()) * (b.y() - c.y())));
			float oneOverDY = -oneOverDX;

			float acx = a.x() - c.x();
			float acy = a.y() - c.y();

			float bcx = b.x() - c.x();
			float bcy = b.y() - c.y();

			mColorXStep = (((mColors[1] - mColors[2]) * acy) - ((mColors[0] - mColors[2]) * bcy)) * oneOverDX;
			mColorYStep = (((mColors[1] - mColors[2]) * acx) - ((mColors[0] - mColors[2]) * bcx)) * oneOverDY;

			mTexCoordXStep = (((mTexCoord[1] - mTexCoord[2]) * acy) - ((mTexCoord[0] - mTexCoord[2]) * bcy)) * oneOverDX;
			mTexCoordYStep = (((mTexCoord[1] - mTexCoord[2]) * acx) - ((mTexCoord[0] - mTexCoord[2]) * bcx)) * oneOverDY;
		}

		const vec4& color(int index) const { return mColors[index]; }
		
		const vec4& colorXStep() const { return mColorXStep; }

		const vec4& colorYStep() const { return mColorYStep; }

		const vec2& texCoord(int index) const { return mTexCoord[index]; }

		const vec2& texCoordXStep() const { return mTexCoordXStep; }

		const vec2& texCoordYStep() const { return mTexCoordYStep; }
};


#endif 
