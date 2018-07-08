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

//Vector 2 class, based on my other project 'FrostCore'. ~MaGetzUb
#ifndef VEC2_HPP
#define VEC2_HPP

#include "matrix.hpp"
#include "math.hpp"

template <class T>
struct tvec2;

template <class T>
static T dot(const tvec2<T>& a, const tvec2<T>& b);

template <class T>
static tvec2<T> operator*(const tvec2<T>&, T);

template <class T>
static tvec2<T> operator*(T, const  tvec2<T>&);

template <class T>
struct tvec2 {

	static_assert(std::is_arithmetic<T>::value, "Only arithmetic types allowed for tvec2!");

	union {
		struct { T x, y; };
		struct { T u, v; };
		struct { T s, t; };
		struct { T w, h; };
	};

	tvec2():
		x(0),
		y(0)
	{}

	tvec2(T a, T b):
		x(a),
		y(b)
	{}

	tvec2(T v):
		x(v),
		y(v)
	{}

	tvec2(const tvec2<T>& r):
		x(r.x),
		y(r.y)
	{}

	tvec2(tvec2<T>&& r): tvec2() {
		std::swap(x, r.x);
		std::swap(y, r.y);
	}

	template<class B>
	explicit tvec2(const tvec2<B>& r):
		x((T)r.x),
		y((T)r.y)
	{}

	template<class B>
	tvec2(B av, B bv):
		x((T)av),
		y((T)bv)
	{}

	template<class B, class D>
	tvec2(B av, D bv):
		x((T)av),
		y((T)bv)
	{}

	~tvec2() {}

	T& operator[](int i) {
		return  *((T*)&x+i);
	}

	const T& operator[](int i) const {
		return  *((T*)&x+i);
	}

	template<class B>
	operator tvec2<B>() {
		return tvec2<B>((B)x, (B)y);
	}

	inline tvec2& operator=(const tvec2<T>& r) {
		x = r.x;
		y = r.y;
		return *this;
	}

	inline tvec2& operator+=(const tvec2<T>& r) {
		x += r.x;
		y += r.y;
		return *this;
	}

	inline tvec2& operator-=(const tvec2<T>& r) {
		x -= r.x;
		y -= r.y;
		return *this;
	}

	inline tvec2& operator*=(T r) {
		x *= r;
		y *= r;
		return *this;
	}

	inline tvec2& operator/=(T r) {
		x /= r;
		y /= r;
		return *this;
	}

	bool operator==(const tvec2<T>& r) {
		return (x == r.x && y == r.y);
	}

	bool operator!=(const tvec2<T>& r) {
		return !(*this == r);
	}

	inline tvec2 left() {
		return tvec2<T>(-y, x);
	}

	inline tvec2 right() {
		return tvec2<T>(y, -x);
	}

	inline tvec2 yx() {
		return tvec2<T>(y, x);
	}

	T dot(const tvec2& r) const {
		return x*r.x + y*r.y;
	}

	inline T magnitude() const {
		return dot(*this);
	}

	inline T length() const {
		return sqrt(magnitude());
	}

	inline tvec2& normalize() {
		*this /= length();
		return *this;
	}

	inline tvec2 normalized() const {
		return tvec2(*this).normalize();
	}

	inline tvec2& reflect(const tvec2& r) {
		tvec2& self = *this;
		self -= 2*dot(self, r)*r;
		return self;
	}

};

template <class T>
void tmat4<T>::transform(const tvec2<T>& vec) {
	transform(vec.x, vec.y);
}

template <class T>
inline static tvec2<T> normalize(const tvec2<T>& v) {
	return v.normalized();
}

template <class T>
inline static tvec2<T> reorthogonalize(const tvec2<T>& a, const tvec2<T>& b) {
	return normalize(a - dot(a, b) * b);
}


template <class T>
inline static T length(const tvec2<T>& v) {
	return v.length();
}

template<class T>
inline static T operator*(const tvec2<T>& a, const tvec2<T>& b) {
	return a.x*b.x + a.y*b.y;
}

template<class T>
inline static tvec2<T> operator-(const tvec2<T>& vec) {
	return vec2(-vec.x, -vec.y);
}

template<class T>
inline static bool operator==(const tvec2<T>& a, const tvec2<T>& b) {
	return (a.x == b.x && a.y == b.y);
}

template<class T>
inline static bool operator!=(const tvec2<T>& a, const tvec2<T>& b) {
	return !(a == b);
}

template<class T>
inline static tvec2<T> operator+(const tvec2<T>& a, const tvec2<T>& b) {
	return tvec2<T>(a.x + b.x, a.y + b.y);
}

template<class T>
inline static tvec2<T> operator-(const tvec2<T>& a, const tvec2<T>& b) {
	return tvec2<T>(a.x - b.x, a.y - b.y);
}

template<class T>
inline static tvec2<T> operator/(const tvec2<T>& a, T b) {
	return tvec2<T>(a.x / b, a.y / b);
}

template<class T>
inline static tvec2<T> operator*(const tvec2<T>& a, T b) {
	return tvec2<T>(a.x * b, a.y * b);
}

template<class T>
inline static tvec2<T> operator*(T a, const tvec2<T>& b) {
	return tvec2<T>(a * b.x, a * b.y);
}

template<class T>
inline static std::ostream& operator<<(std::ostream& o, const tvec2<T>& b) {
	o << "x: "<<b.x<<" y: "<<b.y ;
	return o;
}


template <class T>
static T dot(const tvec2<T>& a, const tvec2<T>& b) {
	return a.dot(b);
}

template<class T>
static tvec2<T> mix(const tvec2<T>& a, const tvec2<T>& b, T amt) {
	return a.mix(b, amt);
}


typedef tvec2<double> dvec2;
typedef tvec2<float> vec2;
typedef tvec2<int> ivec2;
typedef tvec2<unsigned> uvec2;


#endif // VEC2_HPP
