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

#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <algorithm>
#include "../Math/matrix.hpp"
#include "../Math/vec4.hpp"
#include "../Math/vec3.hpp"
#include "../Math/vec2.hpp"

//Code based on TheBennybox' video tutorial series on software rendering
class Vertex {
	
	vec4 mPosition = { 0.0f, 0.0f, 0.0f, 1.0f };
	vec4 mColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	vec2 mTexCoord = { 0.0f, 0.0f };

	public:

		Vertex() = default;
		
		Vertex(const vec4& v, const vec4& color = vec4(1.f, 1.f, 1.f, 1.f), const vec2& texCoord = vec2(0.f, 0.f)): 
			mPosition(v), 
			mColor(color), 
			mTexCoord(texCoord) 
		{}

		Vertex(float x, float y, float z, float w): 
			Vertex(vec4{ x, y, z, w }) 
		{}

		Vertex(float x, float y): 
			Vertex(x, y, 0.0f, 1.0f) 
		{}
		
		Vertex(Vertex&& b) { 
			std::swap(mPosition, b.mPosition);
			std::swap(mColor, b.mColor);
			std::swap(mTexCoord, b.mTexCoord);
		}
		
		Vertex(const Vertex& v): Vertex(v.mPosition, v.mColor, v.mTexCoord) {}

		Vertex& operator=(const Vertex& b) {
			mPosition = b.mPosition;
			mColor = b.mColor;
			mTexCoord = b.mTexCoord;
			return *this;
		}

		Vertex& operator=(Vertex&& b) {
			std::swap(mPosition, b.mPosition);
			std::swap(mColor, b.mColor);
			std::swap(mTexCoord, b.mTexCoord);
			return *this;
		}

		~Vertex() {}

		inline Vertex& transform(mat4 matrix) {
			mPosition = matrix * mPosition;
			return *this;
		}

		inline Vertex transformed(mat4 matrix) const {
			return Vertex(matrix * mPosition, mColor, mTexCoord);
		}

		Vertex& perspectiveDivide() {
			mPosition.x /= mPosition.w;
			mPosition.y /= mPosition.w;
			mPosition.z /= mPosition.w;
			return *this;
		}

		Vertex perspectiveDivided() const {
			return Vertex({mPosition.x / mPosition.w, mPosition.y / mPosition.w, mPosition.z / mPosition.w, mPosition.w}, mColor, mTexCoord);
		}

		void setColor(const vec4& color) { mColor = color; }

		void setTexCoord(const vec2& texCoord) { mTexCoord = texCoord; }

		const vec4& color() const { return mColor; }

		const vec2& texCoord() const { return mTexCoord; }

		inline Vertex& setW(float w) {
			mPosition.w = w;
			return *this;
		}

		inline Vertex& setPosition(const vec3& pos) { 
			mPosition.x = pos.x;
			mPosition.y = pos.y;
			mPosition.z = pos.z;
			return *this;
		}

		const vec4& position() const { return mPosition; }
		vec3 xyz() const { return mPosition.xyz(); }


		float x() const { return mPosition.x; }
		float y() const { return mPosition.y; }
		float z() const { return mPosition.z; }
		float w() const { return mPosition.w; }

};

#endif 