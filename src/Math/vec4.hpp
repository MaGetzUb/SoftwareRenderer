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

//Vector 4 class, based on my other project 'FrostCore'. ~MaGetzUb

#ifndef VEC4_HPP
#define VEC4_HPP

#include "math.hpp"
#include "matrix.hpp"


template <class T>
struct tvec3;

template <class T>
struct tvec4;

template<class T>
inline static tvec4<T> operator*(T a, const tvec4<T>& b);

template<class T>
inline static tvec4<T> operator*(const tvec4<T>& a, T b);

template <class T>
struct tvec4 {

	static_assert(std::is_arithmetic<T>::value, "Only arithmetic types allowed for tvec4!");

	union {
		struct { T x, y, z, w; };
		struct { T r, g, b, a; };
		struct { T s, t, p, q; };
	};

	tvec4():
		x((T)0),
		y((T)0),
		z((T)0),
		w((T)0)
	{}

	tvec4(T av, T bv, T cv, T dv):
		x(av),
		y(bv),
		z(cv),
		w(dv)
	{}

	tvec4(T v):
		x(v),
		y(v),
		z(v),
		w(v)
	{}

	tvec4(const tvec3<T>& xyz, T xyz_w = 1):
		x(xyz.x),
		y(xyz.y),
		z(xyz.z),
		w(xyz_w)
	{}

	tvec4(const tvec4<T>& r):
		x(r.x),
		y(r.y),
		z(r.z),
		w(r.w)
	{}

	template<class B>
	explicit tvec4(const tvec4<B>& r):
		x((B)r.x),
		y((B)r.y),
		z((B)r.z),
		w((B)r.w)
	{}

	~tvec4() {}

	T& operator[](int i) {
		return *(((T*)&x)+i);
	}

	const T& operator[](int i) const {
		return *(((T*)&x)+i);
	}

	inline tvec4& operator=(const tvec4<T>& r) {
		x = r.x;
		y = r.y;
		z = r.z;
		w = r.w;
		return *this;
	}

	inline tvec4& operator+=(const tvec4<T>& r) {
		*this = *this + r;
		return *this;
	}

	inline tvec4& operator-=(const tvec4<T>& r) {
		*this = *this - r;
		return *this;
	}

	inline tvec4& operator*=(T r) {
		*this = *this * r;
		return *this;
	}

	inline tvec4& operator/=(T r) {
		*this = *this / b;
		return *this;
	}

	inline T dot(const tvec4& r) const {
		return x*r.x + y*r.y + z*r.z + w*r.w;
	}

	tvec4 mix(const tvec4& r, T amt) const {
		return ((*this)*((T)1 - amt)) + (r * amt);
	}

	tvec3<T> xyz() const { return tvec3<T>(x, y, z);  }

	inline T magnitude() const {
		return dot(*this);
	}

	inline T length() const {
		return sqrt(magnitude());
	}

	inline tvec4& normalize() {
		*this /= length();
		return *this;
	}

	inline tvec4 normalized() {
		return tvec4(*this).normalize();
	}

};

typedef tvec4<float> vec4;
typedef tvec4<double> dvec4;
typedef tvec4<int> ivec4;
typedef tvec4<unsigned> uvec4;



template <class T>
inline tvec4<T> dot(const tvec4<T>& a, const tvec4<T>& b) {
	return a.dot(b);
}

template <class T>
inline tvec4<T> normalize(const tvec4<T>& vec) {
	return vec.normalized();
}

template <class T>
inline static tvec4<T> reorthogonalize(const tvec4<T>& a, const tvec4<T>& b) {
	return normalize(a - dot(a, b) * b);
}

template<class T>
inline static T operator*(const tvec4<T>& a, const tvec4<T>& b) {
	return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}

template<class T>
inline static tvec4<T>& operator-(tvec4<T>& vec) {
	vec.x = -vec.x;
	vec.y = -vec.y;
	vec.z = -vec.z;
	vec.w = -vec.w;
	return vec;
}

template<class T>
inline static bool operator==(const tvec4<T>& a, const tvec4<T>& b) {
	return (a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w);
}

template<class T>
inline static bool operator!=(const tvec4<T>& a, const tvec4<T>& b) {
	return !(a == b);
}


template<class T>
inline static tvec4<T> operator+(const tvec4<T>& a, const tvec4<T>& b) {
	return tvec4<T>(a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w);
}

template<class T>
inline static tvec4<T> operator-(const tvec4<T>& a, const tvec4<T>& b) {
	return tvec4<T>(a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w);
}

template<class T>
inline static tvec4<T> operator/(const tvec4<T>& a, T b) {
	return tvec4<T>(a.x / b, a.y/b, a.z / b, a.w / b);
}

template<class T>
inline static tvec4<T> operator*(const tvec4<T>& a, T b) {
	return tvec4<T>(a.x * b, a.y * b, a.z*b, a.w*b);
}


template<class T>
inline static tvec4<T> operator*(T a, const tvec4<T>& b) {
	return tvec4<T>(a * b.x, a * b.y, a * b.z, a * b.w);
}

template<class T>
inline static tvec4<T> operator*(const tvec4<T>& a, const tvec4<T>& b) {
	return tvec4<T>(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}

template<class T>
inline static std::ostream& operator<<(std::ostream& o, const tvec4<T>& b) {
	o << "x: "<<b.x<<" y: "<<b.y<<" z: "<<b.z<<" w: "<<b.w;
	return o;
}

template<class T>
tvec4<T> mix(const tvec4<T>& a, const tvec4<T>& b, T amt) {
	return a.mix(b, amt);
}

template<class T>
inline static T dot(const tvec4<T>& a, const tvec4<T>& b) {
	return a.dot(b);
}

template <class T>
static tvec4<T> operator*(const  tmat4<T>& m, const tvec4<T>& v) {
	tvec4<T> o;
	o.x = v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0] + v.w * m[3][0];
	o.y = v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1] + v.w * m[3][1];
	o.z = v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2] + v.w * m[3][2];
	o.w = v.x * m[0][3] + v.y * m[1][3] + v.z * m[2][3] + v.w * m[3][3];
	return o;
}

template <class T>
void tmat4<T>::transform(const tvec4<T>& vec) {
	transform(vec.x, vec.y, vec.z, vec.w);
}

typedef tvec4<double> dvec4;
typedef tvec4<float> vec4;
typedef tvec4<int> ivec4;
typedef tvec4<unsigned int> uvec4;

#ifdef USE_SIMD


inline static vec4 operator*(const mat4& m, const vec4& vec) {
	__m128 x = _mm_set1_ps(vec.x);
	__m128 y = _mm_set1_ps(vec.y);
	__m128 z = _mm_set1_ps(vec.z);
	__m128 w = _mm_set1_ps(vec.w);

	__m128 col1 = _mm_set_ps(m[0][0], m[0][1], m[0][2], m[0][3]);
	__m128 col2 = _mm_set_ps(m[1][0], m[1][1], m[1][2], m[1][3]);
	__m128 col3 = _mm_set_ps(m[2][0], m[2][1], m[2][2], m[2][3]);
	__m128 col4 = _mm_set_ps(m[3][0], m[3][1], m[3][2], m[3][3]);
	__m128 out = _mm_add_ps(_mm_add_ps(_mm_mul_ps(x, col1), _mm_mul_ps(y, col2)), _mm_add_ps(_mm_mul_ps(z, col3), _mm_mul_ps(w, col4)));
	return vec4{ out.m128_f32[3], out.m128_f32[2], out.m128_f32[1], out.m128_f32[0] };
}


inline static vec4 operator+(const vec4& a, const vec4& b) {
	__m128 avec = _mm_loadu_ps(&a[0]);
	__m128 bvec = _mm_loadu_ps(&b[0]);
	__m128 out = _mm_add_ps(avec, bvec);
	return  vec4{ out.m128_f32[0], out.m128_f32[1], out.m128_f32[2], out.m128_f32[3] };
}

inline static vec4 operator-(const vec4& a) {
	__m128 avec = _mm_loadu_ps(&a[0]);
	__m128 out = _mm_xor_ps(avec, _mm_set1_ps(-0.0));
	return  vec4{ out.m128_f32[0], out.m128_f32[1], out.m128_f32[2], out.m128_f32[3] };
}

inline static vec4 operator-(const vec4& a, const vec4& b) {
	__m128 avec = _mm_loadu_ps(&a[0]);
	__m128 bvec = _mm_loadu_ps(&b[0]);
	__m128 out = _mm_sub_ps(avec, bvec);
	return  vec4{ out.m128_f32[0], out.m128_f32[1], out.m128_f32[2], out.m128_f32[3] };
}

inline static vec4 operator/(const vec4& a, float b) {
	__m128 avec = _mm_loadu_ps(&a[0]);
	__m128 bvec = _mm_set_ps(b, b, b, b);
	__m128 out = _mm_div_ps(avec, bvec);
	return  vec4{ out.m128_f32[0], out.m128_f32[1], out.m128_f32[2], out.m128_f32[3] };
}

inline static vec4 operator*(const vec4& a, float b) {
	__m128 avec = _mm_loadu_ps(&a[0]);
	__m128 bvec = _mm_set1_ps(b);
	__m128 out = _mm_mul_ps(avec, bvec);
	return  vec4{ out.m128_f32[0], out.m128_f32[1], out.m128_f32[2], out.m128_f32[3] };
}

inline static vec4 operator*(float a, const vec4& b) {
	__m128 avec = _mm_set_ps(a, a, a, a);
	__m128 bvec =_mm_loadu_ps(&b[0]);
	__m128 out = _mm_mul_ps(avec, bvec);
	return  vec4{ out.m128_f32[0], out.m128_f32[1], out.m128_f32[2], out.m128_f32[3] };
}

//Hadamard product!
inline static vec4 operator*(const vec4& a, const vec4& b) {
	__m128 avec = _mm_loadu_ps(&a[0]);
	__m128 bvec =_mm_loadu_ps(&b[0]);
	__m128 out = _mm_mul_ps(avec, bvec);
	return vec4{ out.m128_f32[0], out.m128_f32[1], out.m128_f32[2], out.m128_f32[3] };
}


inline int Vec4ToPixel(const vec4& color) {
	__m128 x = _mm_loadu_ps(&color[0]);
	x = _mm_mul_ps(x, _mm_set1_ps(255.f));
	constexpr unsigned shuff = _MM_SHUFFLE(3, 0, 1, 2);
	x = _mm_shuffle_ps(x, x, shuff);
	__m128i y = _mm_cvtps_epi32(x);
	y = _mm_packs_epi32(y, y);
	y = _mm_packus_epi16(y, y);
	return _mm_cvtsi128_si32(y);
}

inline vec4 PixelToVec4(int pixelColor) {
	__m128i y = _mm_set_epi32(*(((unsigned char*)&pixelColor) + 3), *(((unsigned char*)&pixelColor) + 2), *(((unsigned char*)&pixelColor) + 1), *(((unsigned char*)&pixelColor)));
	__m128 x = _mm_cvtepi32_ps(y);
	x = _mm_div_ps(x, _mm_set1_ps(255.f));
	return *(vec4*)&x;
}


#endif



#endif // VEC4_HPP

