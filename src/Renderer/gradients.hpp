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
	vec3 mNormal[3], mNormalXStep, mNormalYStep;

	template <class T>
	static inline T CalculateStepX(T(&values)[3], float cay, float bcy, float oneOverDX) {
		return ((values[1] - values[2]) * cay - (values[0] - values[2]) * bcy) * oneOverDX;
	}

	template <class T>
	static inline T CalculateStepY(T(&values)[3], float acx, float bcx, float oneOverDY) {
		return ((values[1] - values[2]) * acx - (values[0] - values[2]) * bcx) * oneOverDY;
	}

	public:


		Gradients(bool perspectiveCorrected, const Vertex& a, const Vertex& b, const Vertex& c) {
			
			float oneOverW[3] = { 1.f, 1.f, 1.f };
			if(perspectiveCorrected) {
				oneOverW[0] /= a.w();
				oneOverW[1] /= b.w();
				oneOverW[2] /= c.w();
			}
		
			mColors[0] = a.color() * oneOverW[0];
			mColors[1] = b.color() * oneOverW[1];
			mColors[2] = c.color() * oneOverW[2];

			mTexCoord[0] = a.texCoord() * oneOverW[0];
			mTexCoord[1] = b.texCoord() * oneOverW[1];
			mTexCoord[2] = c.texCoord() * oneOverW[2];

			mZDivisor[0] = 1.0f * oneOverW[0];
			mZDivisor[1] = 1.0f * oneOverW[1];
			mZDivisor[2] = 1.0f * oneOverW[2];

			mDepth[0] = a.z();
			mDepth[1] = b.z();
			mDepth[2] = c.z();

			mNormal[0] = a.normal() * oneOverW[0];
			mNormal[1] = b.normal() * oneOverW[1];
			mNormal[2] = c.normal() * oneOverW[2];

			float cax = a.x() - c.x();
			float cay = a.y() - c.y();

			float cbx = b.x() - c.x();
			float cby = b.y() - c.y();

			float oneOverDX = 1.0f / (cbx * cay - cax * cby);
			float oneOverDY = -oneOverDX;

			mColorXStep = CalculateStepX(mColors, cay, cby, oneOverDX);
			mColorYStep = CalculateStepY(mColors, cax, cbx, oneOverDY);

			mTexCoordXStep = CalculateStepX(mTexCoord, cay, cby, oneOverDX);
			mTexCoordYStep = CalculateStepY(mTexCoord, cax, cbx, oneOverDY);
		
			mZDivisorXStep = CalculateStepX(mZDivisor, cay, cby, oneOverDX);
			mZDivisorYStep = CalculateStepY(mZDivisor, cax, cbx, oneOverDY);

			mDepthXStep = CalculateStepX(mDepth, cay, cby, oneOverDX);
			mDepthYStep = CalculateStepY(mDepth, cax, cbx, oneOverDY);
		
			mNormalXStep = CalculateStepX(mNormal, cay, cby, oneOverDX);
			mNormalYStep = CalculateStepY(mNormal, cax, cbx, oneOverDY);
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

		inline const vec3& normal(int index) const { return mNormal[index]; }

		inline const vec3& normalXStep() const { return mNormalXStep; }

		inline const vec3& normalYStep() const { return mNormalYStep; }
};


#endif //GRADIENTS_HPP
