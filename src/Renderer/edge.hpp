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

#ifndef EDGE_HPP
#define EDGE_HPP

#include "vertex.hpp"
#include "../Math/vec4.hpp"
#include "gradients.hpp"

class Edge {
	
	float mX, mXStep;
	int mYStart, mYEnd;
	
	vec4 mColor, mColorStep;

	public: 

		Edge(const Gradients& gradients, const Vertex& a, const Vertex& b, int minYVid) {
			mYStart = (int)ceilf(a.y());
			mYEnd = (int)ceilf(b.y());

			float yDist = b.y() - a.y();
			float xDist = b.x() - a.x();
			float yOffset = (mYStart - a.y());

			mXStep = xDist / yDist;
			mX = a.x() + mXStep * yOffset;

			float xOffset = (mX - a.x());
			
			mColor = gradients.color(minYVid) + (gradients.colorYStep()*yOffset) + (gradients.colorXStep()*xOffset);
			mColorStep = gradients.colorYStep() + (gradients.colorXStep() * mXStep);
		}
	
		void step() {
			mX += mXStep;
			mColor += mColorStep;
		}

		float x() const { return mX; }
		int yStart() const { return mYStart; }
		int yEnd() const { return mYEnd; }
		const vec4& color() const { return mColor; }
};


#endif 