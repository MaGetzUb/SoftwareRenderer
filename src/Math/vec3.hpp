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

//Vector 3 class, based on my other project 'FrostCore'. ~MaGetzUb
#ifndef VEC3_HPP
#define VEC3_HPP

#include "math.hpp"
#include "matrix.hpp"


template<class T>
struct tquat;

template <class T>
struct tvec3;

template <class T>
struct tvec2;

template<class T>
inline static tvec3<T> operator*(T a, const tvec3<T>& b);

template<class T>
inline static tvec3<T> operator*(const tvec3<T>& a, T b);

template <class T>
struct tvec3 {

	static_assert(std::is_arithmetic<T>::value, "Only arithmetic types allowed for tvec3!");

	union {
		struct { T x, y, z; };
		struct { T r, g, b; };
		struct { T w, h, d; };
	};

	tvec3():
		x(0),
		y(0),
		z(0)
	{}

	tvec3(T v):
		x(v),
		y(v),
		z(v)
	{}

	tvec3(T av, T bv, T cv):
		x(av),
		y(bv),
		z(cv)
	{}

	tvec3(const tvec3<T>& r):
		x(r.x),
		y(r.y),
		z(r.z)
	{}

	tvec3(tvec3<T>&& r): 
		tvec3() 
	{
		std::swap(x, r.x);
		std::swap(y, r.y);
		std::swap(z, r.z);
	}

	template<class B>
	explicit tvec3(const tvec3<B>& b):
		x((T)b.x),
		y((T)b.y),
		z((T)b.z)
	{}

	~tvec3() {}

	T& operator[](int i) {
		return *((T*)&x+i);
	}

	const T& operator[](int i) const {
		return  *((T*)&x+i);
	}

	inline tvec3& operator=(const tvec3<T>& r) {
		x = r.x;
		y = r.y;
		z = r.z;
		return *this;
	}

	inline tvec3& operator+=(const tvec3<T>& r) {
		x += r.x;
		y += r.y;
		z += r.z;
		return *this;
	}

	inline tvec3& operator-=(const tvec3<T>& r) {
		x -= r.x;
		y -= r.y;
		z -= r.z;
		return *this;
	}

	inline tvec3& operator*=(T r) {
		x *= r;
		y *= r;
		z *= r;
		return *this;
	}

	inline tvec3& operator/=(T r) {
		x /= r;
		y /= r;
		z /= r;
		return *this;
	}

	bool operator==(const tvec3& r) {
		return ((x == r.x) && (y == r.y) && (z == r.z));
	}

	tvec3 operator-() {
		return tvec3<T>(-x, -y, -z);
	}

	inline T dot(const tvec3& r) const {
		return x*r.x + y*r.y + z*r.z;
	}

	inline tvec3 cross(const tvec3& r) const {
		return tvec3<T>{y*r.z - z*r.y, z*r.x - x*r.z, x*r.y - y*r.x};
	}

	/*inline tmat3<T> outer(const tvec3& b) const {
		return tmat3<T>({x*b.x, x*b.y, x*b.z},
				{y*b.x, y*b.y, y*b.z},
				{z*b.x, z*b.y, z*b.z});
	}*/

	inline tvec3 mix(const tvec3& r, T amt) const {
		return (*this)*((T)1 - amt) + r*amt;
	}

	inline T magnitude() const {
		return dot(*this);
	}

	inline T length() const {
		return sqrt(magnitude());
	}

	inline tvec3& normalize() {
		T mag = magnitude();
		//if(mag == (T)1) return *this;
		*this /= sqrt(mag);
		return *this;
	}

	inline void basis(tvec3<T>& t, tvec3<T>& bt) const {
#define SQRT1OVER3 0.57735026918962576450914878050196
		t = (x < (T)SQRT1OVER3) ? 
				tvec3<T>{(T)y, -(T)x, (T)0} : 
				tvec3<T>{(T)0, (T)z, -(T)y};
		t = reorthogonalize(*this, t);
		bt = (*this).cross(t);
		bt = reorthogonalize(*this, bt);
#undef SQRT1OVER3
	}

	inline tvec3 normalized() const {
		return tvec3(*this).normalize();
	}

	inline tvec3<T> rotated(const tquat<T>& q) const {
		return q.rotate(*this);
	}

	inline tvec3<T>& rotate(const tquat<T>& q) {
		*this = q.rotate(*this);
		return *this;
	}


	inline tvec2<T> vec2() const {
		return {x, y};
	}

	inline tvec3<T>& rotate(T angle, const tvec3<T>& axis) {
		T sinHalfAng = Sin(angle);
		T cosHalfAng = Cos(angle);
		T rx = axis.x * sinHalfAng;
		T ry = axis.y * sinHalfAng;
		T rz = axis.z * sinHalfAng;
		T rw = cosHalfAng;
		tquat<T> rotQ(rx, ry, rz, rw);
		return rotate(rotQ);
	}


	inline tvec3<T> rotated(T angle, const tvec3<T>& axis) const {
		tvec3<T> n = (*this);
		n.rotate(angle, axis);
		return n;
	}

	inline tvec3<T> xzy() const {
		return tvec3(x, z, y);
	}

	inline tvec3<T> yxz() const {
		return tvec3(y, x, z);
	}

	inline tvec3<T> yzx() const {
		return tvec3(y, z, x);
	}

	inline tvec3<T> zyx() const {
		return tvec3(z, y, x);
	}

	inline tvec3<T> zxy() const {
		return tvec3(z, x, y);
	}

};


template <class T>
void tmat4<T>::transform(const tvec3<T>& vec) {
	transform(vec.x, vec.y, vec.z);
}

template <class T>
inline static T length(const tvec3<T>& v) {
	return v.length();
}

template <class T>
inline static T magnitude(const tvec3<T>& v) {
	return v.magnitude();
}

template<class T>
inline static T operator*(const tvec3<T>& a, const tvec3<T>& b) {
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

template <class T>
static inline tvec3<T> normalize(const tvec3<T>& vec) {
	return vec.normalized();
}

template <class T>
inline static tvec3<T> reorthogonalize(const tvec3<T>& a, const tvec3<T>& b) {
	return normalize(a - dot(a, b) * b);
}

template<class T>
inline static tvec3<T>& operator-(tvec3<T>& vec) {
	vec.x = -vec.x;
	vec.y = -vec.y;
	vec.z = -vec.z;
	return vec;
}

template<class T>
inline static bool operator==(const tvec3<T>& a, const tvec3<T>& b) {
	return (a.x == b.x && a.y == b.y && a.z == b.z);
}

template<class T>
inline static bool operator!=(const tvec3<T>& a, const tvec3<T>& b) {
	return !(a == b);
}

template<class T>
inline static tvec3<T> operator+(const tvec3<T>& a, const tvec3<T>& b) {
	return tvec3<T>(a.x+b.x, a.y+b.y, a.z+b.z);
}

template<class T>
inline static tvec3<T> operator-(const tvec3<T>& a, const tvec3<T>& b) {
	return tvec3<T>(a.x-b.x, a.y-b.y, a.z-b.z);
}

template<class T>
inline static tvec3<T> operator-(const tvec3<T>& v) {
	return tvec3<T>(-v.x, -v.y, -v.z);
}

template<class T>
inline static tvec3<T> operator/(const tvec3<T>& a, T b) {
	return tvec3<T>(a.x / b, a.y / b, a.z / b);
}

template<class T, class B>
inline static tvec3<T> operator/(const tvec3<T>& a, B b) {
	return tvec3<T>(a.x / (T)b, a.y / (T)b, a.z / (T)b);
}

template<class T>
inline static tvec3<T> operator*(const tvec3<T>& a, T b) {
	return tvec3<T>(a.x * b, a.y * b, a.z * b);
}

template<class T, class B>
inline static tvec3<T> operator*(const tvec3<T>& a, B b) {
	return tvec3<T>(a.x * (T)b, a.y * (T)b, a.z * (T)b);
}


//Hadamard product
template<class T>
inline static tvec3<T> operator*(const tvec3<T>& a, const tvec3<T>& b) {
	return tvec3<T>(a.x * b.x, a.y*b.y, a.z*b.z);
}


template<class T>
inline static tvec3<T> operator*(T a, const tvec3<T>& b) {
	return tvec3<T>(a * b.x, a * b.y, a * b.z);
}

template <class T>
static tvec3<T> operator*(const tmat4<T>& m, const tvec3<T>& v) {
	tvec3<T> o;
	o.x = v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0] + (T)1 * m[3][0];
	o.y = v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1] + (T)1 * m[3][1];
	o.z = v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2] + (T)1 * m[3][2];
	return o;
}
/*
template <class T>
static tvec3<T> operator*(const tmat3<T>& m, const tvec3<T>& v) {
	tvec3<T> o;
	o.x = v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0];
	o.y = v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1];
	o.z = v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2];
	return o;
}


*/



typedef tvec3<double> dvec3;
typedef tvec3<float> vec3;
typedef tvec3<int> ivec3;
typedef tvec3<unsigned> uvec3;


#ifdef USE_SIMD
//__m128 row1 = _mm_set_ps(m.data()[0], m.data()[1], m.data()[2], m.data()[3]);
//__m128 row2 = _mm_set_ps(m.data()[4], m.data()[5], m.data()[6], m.data()[7]);
//__m128 row3 = _mm_set_ps(m.data()[8], m.data()[9], m.data()[10], m.data()[11]);
//__m128 row4 = _mm_set_ps(m.data()[12], m.data()[13], m.data()[14], m.data()[15]);

/*
inline static vec3 operator*(const mat4& m, const vec3& vec) {
	#if 1

	// x*a + y*b + z*d + w*e
	__m128 x = _mm_set1_ps(vec.x);
	__m128 y = _mm_set1_ps(vec.y);
	__m128 z = _mm_set1_ps(vec.z);
	__m128 w = _mm_set1_ps(1.0f);

	__m128 col1 = _mm_set_ps(m[0][0], m[0][1], m[0][2], m[0][3]);
	__m128 col2 = _mm_set_ps(m[1][0], m[1][1], m[1][2], m[1][3]);
	__m128 col3 = _mm_set_ps(m[2][0], m[2][1], m[2][2], m[2][3]);
	__m128 col4 = _mm_set_ps(m[3][0], m[3][1], m[3][2], m[3][3]);
	__m128 ans = _mm_add_ps(_mm_add_ps(_mm_mul_ps(x, col1), _mm_mul_ps(y, col2)), _mm_add_ps(_mm_mul_ps(z, col3), _mm_mul_ps(w, col4)));
	vec3 out{ans.m128_f32[3], ans.m128_f32[2], ans.m128_f32[1]};
	#else
	vec3 out(vec.x*m[0][0] + vec.y*m[1][0] + vec.z*m[2][0] + m[3][0],
		 vec.x*m[0][1] + vec.y*m[1][1] + vec.z*m[2][1] + m[3][1],
		 vec.x*m[0][2] + vec.y*m[1][2] + vec.z*m[2][2] + m[3][2]);
	#endif
	return out;
}
*/

inline static vec3 operator*(const vec3& vec, const mat4& m) {
#if 1

	#if 1
	// x*a + y*b + z*d + w*e
	__m128 x = _mm_set1_ps(vec.x);
	__m128 y = _mm_set1_ps(vec.y);
	__m128 z = _mm_set1_ps(vec.z);
	__m128 w = _mm_set1_ps(1.0f);

	__m128 col1 = _mm_set_ps(m[0][0], m[1][0], m[2][0], m[3][0]);
	__m128 col2 = _mm_set_ps(m[0][1], m[1][1], m[2][1], m[3][1]);
	__m128 col3 = _mm_set_ps(m[0][2], m[1][2], m[2][2], m[3][2]);
	__m128 col4 = _mm_set_ps(m[0][3], m[1][3], m[2][3], m[3][3]);
	__m128 ans = _mm_add_ps(_mm_add_ps(_mm_mul_ps(x, col1), _mm_mul_ps(y, col2)), _mm_add_ps(_mm_mul_ps(z, col3), _mm_mul_ps(w, col4)));
	#else 

	__m128 col = _mm_set_ps(vec.x, vec.y, vec.z, 1.0f);

	__m128 row1 = _mm_load_ps(&m[0][0]);
	__m128 row2 = _mm_load_ps(&m[0][1]);
	__m128 row3 = _mm_load_ps(&m[0][2]);
	__m128 row4 = _mm_load_ps(&m[0][3]);

	__m128 ans = _mm_add_ps(_mm_add_ps(_mm_mul_ps(row1, col), _mm_mul_ps(row2, col)), _mm_add_ps(_mm_mul_ps(row3, col), _mm_mul_ps(row4, col)));

	#endif 
	vec3 out{((float*)&ans)[3], ((float*)&ans)[2], ((float*)&ans)[1]};
#else
	vec3 out(vec.x*m[0][0] + vec.y*m[1][0] + vec.z*m[2][0] + m[3][0],
		 vec.x*m[0][1] + vec.y*m[1][1] + vec.z*m[2][1] + m[3][1],
		 vec.x*m[0][2] + vec.y*m[1][2] + vec.z*m[2][2] + m[3][2]);
#endif
	return out;
}




inline static vec3 operator+(const vec3& a, const vec3& b) {
	__m128 avec = _mm_set_ps(a.x, a.y, a.z, 0.0f);
	__m128 bvec = _mm_set_ps(b.x, b.y, b.z, 0.0f);
	__m128 out = _mm_add_ps(avec, bvec);
	return vec3{((float*)&out)[3], ((float*)&out)[2], ((float*)&out)[1]};
}

inline static vec3 operator-(const vec3& a, const vec3& b) {
	__m128 avec = _mm_set_ps(a.x, a.y, a.z, 0.0f);
	__m128 bvec = _mm_set_ps(b.x, b.y, b.z, 0.0f);
	__m128 out = _mm_sub_ps(avec, bvec);
	return vec3{((float*)&out)[3], ((float*)&out)[2], ((float*)&out)[1]};
}

inline static vec3 operator/(const vec3& a, float b) {
	__m128 avec = _mm_set_ps(a.x, a.y, a.z, 0.0f);
	__m128 bvec = _mm_set_ps(b, b, b, 0.0f);
	__m128 out = _mm_div_ps(avec, bvec);
	return vec3{((float*)&out)[3], ((float*)&out)[2], ((float*)&out)[1]};
}

inline static vec3 operator*(const vec3& a, float b) {
	__m128 avec = _mm_set_ps(a.x, a.y, a.z, 0.0f);
	__m128 bvec = _mm_set_ps(b, b, b, 0.0f);
	__m128 out = _mm_mul_ps(avec, bvec);
	return vec3{((float*)&out)[3], ((float*)&out)[2], ((float*)&out)[1]};
}

//Does the hadamard product
inline static vec3 operator*(const vec3& a, const vec3& b) {
	__m128 avec = _mm_set_ps(a.x, a.y, a.z, 0.0f);
	__m128 bvec = _mm_set_ps(b.x, b.y, b.z, 0.0f);
	__m128 out = _mm_mul_ps(avec, bvec);
	return vec3{((float*)&out)[3], ((float*)&out)[2], ((float*)&out)[1]};
}


inline static vec3 operator*(float a, const vec3& b) {
	__m128 avec = _mm_set_ps(a, a, a, 0.0f);
	__m128 bvec = _mm_set_ps(b.x, b.y, b.z, 0.0f);
	__m128 out = _mm_mul_ps(avec, bvec);
	return vec3{((float*)&out)[3], ((float*)&out)[2], ((float*)&out)[1]};
}
#endif

#define self (*this)
template<class T>
tvec3<T> tmat4<T>::translation() const {
	return tvec3<T>{self[3][0], self[3][1], self[3][2]};
}

template<class T>
tvec3<T> tmat4<T>::scaling() const {
	return tvec3<T>{self[0][0], self[1][1], self[2][2]};
}
#undef self

template<class T>
inline static std::ostream& operator<<(std::ostream& o, const tvec3<T>& b) {
	o << "x: "<<b.x<<" y: "<<b.y<<" z: "<<b.z;
	return o;
}


template <class T>
static tvec3<T> cross(const tvec3<T>& a, const tvec3<T>& b) {
	return tvec3<T>(a).cross(b);
}

template <class T>
static T dot(const tvec3<T>& a, const tvec3<T>& b) {
	return a.dot(b);
}
/*
template <class T>
static tmat3<T> outer(const tvec3<T>& a, const tvec3<T>& b) {
	return a.outer(b);
}
*/
template<class T>
static tvec3<T> Mix(const tvec3<T>& a, const tvec3<T>& b, T amt) {
	return a.mix(b, amt);
}


#endif // VEC3_HPP

