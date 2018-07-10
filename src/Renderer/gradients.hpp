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
	float mZDivisor[3], mZDivisorXStep, mZDivisorYStep;
	float mDepth[3], mDepthXStep, mDepthYStep;


		template <class T>
		static inline T CalculateStepX(T(&values)[3], float acy, float bcy, float oneOverDX) {
			return ((values[1] - values[2]) * acy - (values[0] - values[2]) * bcy) * oneOverDX;
		}

		template <class T>
		static inline T CalculateStepY(T(&values)[3], float acx, float bcx, float oneOverDY) {
			return ((values[1] - values[2]) * acx - (values[0] - values[2]) * bcx) * oneOverDY;
		}

	public:


		Gradients(const Vertex& a, const Vertex& b, const Vertex& c) {

			mColors[0] = a.color() / a.w();
			mColors[1] = b.color() / b.w();
			mColors[2] = c.color() / c.w();

			mTexCoord[0] = a.texCoord() / a.w();
			mTexCoord[1] = b.texCoord() / b.w();
			mTexCoord[2] = c.texCoord() / c.w();

			mZDivisor[0] = 1.0f / a.w();
			mZDivisor[1] = 1.0f / b.w();
			mZDivisor[2] = 1.0f / c.w();

			mDepth[0] = a.z();
			mDepth[1] = b.z();
			mDepth[2] = c.z();

			float oneOverDX = 1.0f / (((b.x() - c.x()) * (a.y() - c.y())) - ((a.x() - c.x()) * (b.y() - c.y())));
			float oneOverDY = -oneOverDX;

			float acy = a.y() - c.y();
			float bcy = b.y() - c.y();

			float acx = a.x() - c.x();
			float bcx = b.x() - c.x();

			mColorXStep = CalculateStepX(mColors, acy, bcy, oneOverDX);
			mColorYStep = CalculateStepY(mColors, acx, bcx, oneOverDY);

			mTexCoordXStep = CalculateStepX(mTexCoord, acy, bcy, oneOverDX);
			mTexCoordYStep = CalculateStepY(mTexCoord, acx, bcx, oneOverDY);
		
			mZDivisorXStep = CalculateStepX(mZDivisor, acy, bcy, oneOverDX);
			mZDivisorYStep = CalculateStepY(mZDivisor, acx, bcx, oneOverDY);

			mDepthXStep = CalculateStepX(mDepth, acy, bcy, oneOverDX);
			mDepthYStep = CalculateStepY(mDepth, acx, bcx, oneOverDY);
		}

		
		inline const vec4& color(int index) const { return mColors[index]; }
		
		inline const vec4& colorXStep() const { return mColorXStep; }
		
		inline const vec4& colorYStep() const { return mColorYStep; }
		
		inline const vec2& texCoord(int index) const { return mTexCoord[index]; }
		
		inline const vec2& texCoordXStep() const { return mTexCoordXStep; }
		
		inline const vec2& texCoordYStep() const { return mTexCoordYStep; }
		
		inline float zDivisor(int index) const { return mZDivisor[index]; }
		
		inline float zDivisorXStep() const { return mZDivisorXStep; }
		
		inline float zDivisorYStep() const { return mZDivisorYStep; }
		
		inline float depth(int index) const { return mDepth[index]; }
		
		inline float depthXStep() const { return mDepthXStep; }
		
		inline float depthYStep() const { return mDepthYStep; }
};


#endif //GRADIENTS_HPP
