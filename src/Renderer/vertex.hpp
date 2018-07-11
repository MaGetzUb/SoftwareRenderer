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

struct Plane {
	vec3 normal;
	float depth;
};

//Code based on TheBennybox' video tutorial series on software rendering
class Vertex {
	
	vec4 mPosition = { 0.0f, 0.0f, 0.0f, 1.0f };
	vec4 mColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	vec2 mTexCoord = { 0.0f, 0.0f };
	vec3 mNormal = { 0.f, 0.f, 0.f };

	public:

		Vertex() = default;
		
		Vertex(const vec4& v, const vec4& color = vec4(1.f, 1.f, 1.f, 1.f), const vec2& texCoord = vec2(0.f, 0.f), const vec3& normal = vec3(0.f, 0.f, 0.f)) :
			mPosition(v), 
			mColor(color), 
			mTexCoord(texCoord),
			mNormal(normal)
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
			std::swap(mNormal, b.mNormal);
		}
		
		Vertex(const Vertex& v): Vertex(v.mPosition, v.mColor, v.mTexCoord, v.mNormal) {}

		inline Vertex& operator=(const Vertex& b) {
			mPosition = b.mPosition;
			mColor = b.mColor;
			mTexCoord = b.mTexCoord;
			mNormal = b.mNormal;
			return *this;
		}

		inline Vertex& operator=(Vertex&& b) {
			std::swap(mPosition, b.mPosition);
			std::swap(mColor, b.mColor);
			std::swap(mTexCoord, b.mTexCoord);
			std::swap(mNormal, b.mNormal);
			return *this;
		}

		~Vertex() {}

		inline Vertex& transform(mat4 matrix) {
			mPosition = matrix * mPosition;
			mNormal = (matrix * vec4(mNormal, 0.0f)).xyz();
			mNormal.normalize();
			return *this;
		}

		inline Vertex transformed(mat4 matrix) const {
			return Vertex(matrix * mPosition, mColor, mTexCoord, (matrix * vec4(mNormal, 0.f)).xyz());
		}

		inline Vertex& perspectiveDivide() {
			mPosition.x /= mPosition.w;
			mPosition.y /= mPosition.w;
			mPosition.z /= mPosition.w;
			return *this;
		}

		inline Vertex perspectiveDivided() const {
			return Vertex({mPosition.x / mPosition.w, mPosition.y / mPosition.w, mPosition.z / mPosition.w, mPosition.w}, mColor, mTexCoord, mNormal);
		}

		inline Vertex& setPosition(const vec3& pos) { 
			mPosition.x = pos.x;
			mPosition.y = pos.y;
			mPosition.z = pos.z;
			return *this;
		}

		inline void setColor(const vec4& color) { mColor = color; }

		inline void setTexCoord(const vec2& texCoord) { mTexCoord = texCoord; }

		inline void setNormal(const vec3& normal) { mNormal = normal.normalized(); }

		inline const vec4& position() const { return mPosition; }
		
		inline const vec4& color() const { return mColor; }

		inline const vec2& texCoord() const { return mTexCoord; }

		inline const vec3& normal() const { return mNormal; }

		inline Vertex& setW(float w) {
			mPosition.w = w;
			return *this;
		}


		inline vec3 xyz() const { return mPosition.xyz(); }

		inline float xyzwComponent(int index) const { return mPosition[index]; }

		inline float x() const { return mPosition.x; }
		inline float y() const { return mPosition.y; }
		inline float z() const { return mPosition.z; }
		inline float w() const { return mPosition.w; }

		inline bool isInView() const {
			return !(
				mPosition.x < -mPosition.w || 
				mPosition.x >  mPosition.w || 
				mPosition.y < -mPosition.w || 
				mPosition.y >  mPosition.w || 
				mPosition.z < -mPosition.w || 
				mPosition.z >  mPosition.w
			);
		}


		static Vertex Mix(const Vertex& a, const Vertex& b, float amt) {
			Vertex out;
			out.mPosition = a.mPosition * (1.0f - amt) + b.mPosition * amt;
			out.mColor = a.mColor * (1.0f - amt) + b.mColor * amt;
			out.mTexCoord = a.mTexCoord * (1.0f - amt) + b.mTexCoord * amt;
			out.mNormal = a.mNormal * (1.0f - amt) + b.mNormal * amt;
			return out;
		}


};


inline float TriangleAreaDoubled(const Vertex& a, const Vertex& b, const Vertex& c) {

	float x1 = b.x() - a.x();
	float y1 = b.y() - a.y();
	float x2 = c.x() - a.x();
	float y2 = c.y() - a.y();

	return x1 * y2 - x2 * y1;
}

static bool ClipEdge(const Plane& plane, const Vertex& a, const Vertex& b, Vertex& c) {

	c = Vertex();

	vec3 ab = b.xyz() - a.xyz();
	vec3 p = plane.normal * plane.depth;
	vec3 aop = a.xyz() - dot(a.xyz() - p, plane.normal)*plane.normal;
	float amt = dot(ab, aop) / dot(ab, ab);
	if(amt < 0.0 || amt > 1.0) return false;
	
	c = Vertex::Mix(a, b, amt);

	return true;
}


#endif //VERTEX_HPP