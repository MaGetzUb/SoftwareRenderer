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
	
	int mYStart, mYEnd;
	
	float mX, mXStep;
	vec4 mColor, mColorStep;
	vec2 mTexCoord, mTexCoordStep;
	float mZDivisor, mZDivisorStep;
	float mDepth, mDepthStep;
	vec3 mNormal, mNormalStep;

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

			mTexCoord = gradients.texCoord(minYVid) + (gradients.texCoordYStep() * yOffset) + (gradients.texCoordXStep() * xOffset);
			mTexCoordStep = gradients.texCoordYStep() + (gradients.texCoordXStep() * mXStep);

			mZDivisor= gradients.zDivisor(minYVid) + (gradients.zDivisorYStep() * yOffset) + (gradients.zDivisorXStep() * xOffset);
			mZDivisorStep = gradients.zDivisorYStep() + (gradients.zDivisorXStep() * mXStep);

			mDepth = gradients.depth(minYVid) + (gradients.depthYStep() * yOffset) + (gradients.depthXStep() * xOffset);
			mDepthStep = gradients.depthYStep() + (gradients.depthXStep() * mXStep);
		
			mNormal = gradients.normal(minYVid) + (gradients.normalYStep() * yOffset) + (gradients.normalXStep() * xOffset);
			mNormalStep = gradients.normalYStep() + (gradients.normalXStep() * mXStep);
		}
	
		void step() {
			mX += mXStep;
			mColor += mColorStep;
			mTexCoord += mTexCoordStep;
			mZDivisor += mZDivisorStep;
			mDepth += mDepthStep;
			mNormal += mNormalStep;
		}

		inline float x() const { return mX; }

		inline float xStep() const { return mXStep; }
		
		inline int yStart() const { return mYStart; }
		
		inline int yEnd() const { return mYEnd; }
		
		inline float zDivisor() const { return mZDivisor; }

		inline float depth() const { return mDepth; }

		inline const vec4& color() const { return mColor; }

		inline const vec2& texCoord() const { return mTexCoord; }

		inline const vec3& normal() const { return mNormal;  }

};


#endif 