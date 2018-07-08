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

//Quaternion class, based on my other project 'FrostCore'. ~MaGetzUb
//This is probably not useful in the Software Renderer project, but why get rid of it?
#ifndef QUAT_HPP
#define QUAT_HPP

#include "math.hpp"
#include "matrix.hpp"


template<class T>
class tmat4;

template<class T>
struct tvec3;

template<class T>
struct tvec4;

template <class T>
struct tquat {

	static_assert(std::is_arithmetic<T>::value, "Only arithmetic types allowed for tquat!");
	T w, x, y, z;


	tquat(): w(1), x(0), y(0), z(0) {}
	tquat(const tvec4<T>& vec):x(vec.x), y(vec.y), z(vec.z), w(vec.w) {}
	tquat(const tquat<T>& b):x(b.x), y(b.y), z(b.z), w(b.w) {}
	tquat(T angle, const tvec3<T>& axis): x(axis.x), y(axis.y), z(axis.z), w(Cos(angle/2)) {}
	tquat(T x, T y, T z, T w):  w(w), x(x), y(y),z(z) {}
	tquat(T x, T y, T z): tquat(EulerAngle(x, y, z)) {}

	tquat(const T* quat):x(quat[0]), y(quat[1]), z(quat[2]), w(quat[3]) {}

	static tquat AxisAngle(T ang, T tx, T ty, T tz) {

		T sinv = Sin(ang / (T)2);
		T cosv = Cos(ang / (T)2);

		tquat out(tx * sinv, ty * sinv, tz * sinv, cosv);

		return out;
	}

	T& operator[](int ind) {return *((&w) + ind);}
	T operator[](int ind) const {return *((&w) + ind);}

	static tquat AxisAngle(const T ang, const tvec3<T>& axis) {
		return AxisAngle(ang, axis.x, axis.y, axis.z);
	}

	tquat& rotate(const tvec3<T>& n1, const tvec3<T>& n2) {
		tvec3<T> n1 = n1.normalized();
		tvec3<T> n2 = n2.normalized();
		*this *= {cross(n1, n2), dot(n1, n2)};
		return *this;
	}

	static tquat EulerAngle(T yaw, T pitch, T roll) {

		T xcos = Cos(yaw/2), xsin = Sin(yaw/2);
		T ycos = Cos(pitch/2), ysin = Sin(pitch/2);
		T zcos = Cos(roll/2), zsin = Sin(roll/2);

		return tquat(xcos*ycos*zcos+xsin*ysin*zsin,
				  xsin*ycos*zcos-xcos*ysin*zsin,
				  xcos*ysin*zcos+xsin*ycos*zsin,
				  xcos*ycos*zsin-xsin*ysin*zcos);
	}

	//Based on https://github.com/g-truc/glm/blob/be53cebcd3ec3366e30f520bfaa35991274fc390/glm/gtc/quaternion.inl#L650
	static tquat AlignZ(const tvec3<T>& forward, const tvec3<T>& up) {
		#if 0
		tmat3<T> m = tmat3<T>::AlignZ(forward, up);

		T fourXSquaredMinus1 = m[0][0] - m[1][1] - m[2][2];
		T fourYSquaredMinus1 = m[1][1] - m[0][0] - m[2][2];
		T fourZSquaredMinus1 = m[2][2] - m[0][0] - m[1][1];
		T fourWSquaredMinus1 = m[0][0] + m[1][1] + m[2][2];

		int biggestIndex = 0;
		T fourBiggestSquaredMinus1 = fourWSquaredMinus1;
		if(fourXSquaredMinus1 > fourBiggestSquaredMinus1) {
			fourBiggestSquaredMinus1 = fourXSquaredMinus1;
			biggestIndex = 1;
		}
		if(fourYSquaredMinus1 > fourBiggestSquaredMinus1) {
			fourBiggestSquaredMinus1 = fourYSquaredMinus1;
			biggestIndex = 2;
		}
		if(fourZSquaredMinus1 > fourBiggestSquaredMinus1) {
			fourBiggestSquaredMinus1 = fourZSquaredMinus1;
			biggestIndex = 3;
		}

		T biggestVal = sqrt(fourBiggestSquaredMinus1 + CastS(T, 1)) * CastS(T, 0.5);
		T mult = CastS(T, 0.25) / biggestVal;

		switch(biggestIndex) {
			case 0: return tquat<T>((m[1][2] - m[2][1]) * mult, (m[2][0] - m[0][2]) * mult, (m[0][1] - m[1][0]) * mult, biggestVal);
			case 1: return tquat<T>((m[2][0] + m[0][2]) * mult, biggestVal, (m[0][1] + m[1][0]) * mult, (m[1][2] - m[2][1]) * mult);
			case 2: return tquat<T>((m[1][2] + m[2][1]) * mult, (m[0][1] + m[1][0]) * mult, biggestVal, (m[2][0] - m[0][2]) * mult);
			case 3: return tquat<T>(biggestVal, (m[2][0] + m[0][2]) * mult, (m[1][2] + m[2][1]) * mult, (m[0][1] - m[1][0]) * mult);
		}

		FrostAssert(false, "Invalid output!");
		return tquat<T>(0, 0, 0, 1);
		#else

		tvec3<T> f = forward.normalized();
		tvec3<T> u = up.normalized();
		u = u - u.dot(f) * f;
		tvec3<T> r = cross(f, u);

		tquat<T> q;
		q.w = sqrt(1.0 + r.x + u.y + f.z) / 2.0;
		T w4 = (4.0 * q.w);
		q.x = (f.y - u.z) / w4;
		q.y = (r.z - f.x) / w4;
		q.z = (u.x - r.y) / w4;
		return q;
		#endif 
	}

	static tquat RotationX(T amt) {
		return AxisAngle(WrapAngle(amt), (T)1, (T)0, (T)0);
	}

	static tquat RotationY(T amt) {
		return AxisAngle(WrapAngle(amt), (T)0, (T)1, (T)0);
	}

	static tquat RotationZ(T amt) {
		return AxisAngle(WrapAngle(amt), (T)0, (T)0, (T)1);
	}

	static tquat RotateBetween(const tvec3<T>& from, const tvec3<T>& to) {
		tquat<T> out;
		tvec3<T> sum = from + to;
		sum.normalize();

		T rot = dot(from, sum);
		tvec3<T> plane = cross(from, sum);

		out.x = plane.x;
		out.y = plane.y;
		out.z = plane.z;
		out.w = rot;
		return out;
	}

	inline void set(T tx, T ty, T tz, T tw) {x = tx; y = ty; z = tz; w = tw;}

	inline tquat<T>& operator*=(const tquat& b) {
		*this = *this * b;
		return *this;
	}

	inline T angle() const {
		return ACos(w) * CastS(T, 2);
	}

	inline tvec3<T> axis() const {
		T tmp1 = CastS(T, 1) - w * w;
		if(tmp1 <= CastS(T, 0)) return tvec3<T>(0, 0, 1);
		T tmp2 = CastS(T, 1) / sqrt(tmp1);
		return tvec3<T>(x * tmp2, y * tmp2, z * tmp2);
	}

	inline tquat& normalize() {
		T len = sqrt(x*x + y*y + z*z + w*w);
		x /= len;
		y /= len;
		z /= len;
		w /= len;
		return *this;
	}

	inline tquat<T>& conjugate() {
		x= -x;
		y= -y;
		z= -z;
		return *this;
	}

	inline tquat<T> conjugated() {
		tquat<T> out = *this;
		out.conjugate();
		return out;
	}

	inline tquat<T>& invert() {
		(*this) = conjugated();
		T m = magnitude();
		x /= m;
		y /= m;
		z /= m;
		w /= m;
		return *this;
	}


	inline tquat<T>& rotate(T xang, T yang, T zang) {
		*this = *this * tquat<T>::EulerAngle(xang, yang, zang);
	}

	inline tquat<T>& rotate(T amt, T x, T y, T z) {
		*this = *this * tquat<T>::AxisAngle(amt, x, y, z);
		return *this;
	}

	inline tquat<T>& rotateX(T amt) {
		rotate(amt, (T)1, (T)0, (T)0);
		return *this;
	}

	inline tquat<T>& rotateY(T amt) {
		rotate(amt, (T)0, (T)1, (T)0);
		return *this;
	}

	inline tquat<T>& rotateZ(T amt) {
		rotate(amt, (T)0, (T)0, (T)1);
		return *this;
	}

	inline tquat normalized() const {
		tquat q = *this;
		q.normalize();
		return q;
	}

	inline tquat conjugated() const {
		tquat q = *this;
		q.conjugate();
		return q;
	}


	//const quat<T>& operator^(T amt) const {
	//	T a;
	//	vec3<T> n = vec3<T>();
	//
	//}

	tquat<T>& operator+=(const tquat& b) {
		*this = *this + b;
		return *this;
	}

	tquat<T>& operator-=(const tquat& b) {
		*this = *this - b;
		return *this;
	}


	tvec3<T> rotate(const tvec3<T>& vec) const {
		tquat<T> q = *this;
		tquat<T> q2(vec.x, vec.y, vec.z, (T)0);
		tquat<T> qInv = q.conjugated();
		q *= q2 * qInv;
		return tvec3<T>(q.x, q.y, q.z);
	}

	inline T yaw() const {
		T ysin = 2 * (x*z + w*y);
		T ycos = w*w - x*x - y*y + z*z;
		return  ATan2(ysin, ycos);
	}

	inline T pitch() const {
		T psin = -2 * (y*z - w*x);
		if(abs(psin) > 1)
			return psin < 0.0 ? -90.0 : 90.0;
		return ASin(psin);
	}

	inline T roll() const {
		T rsin = 2 * (x*y + w*z);
		T rcos = w*w + x*x - y*y - z*z;
		return ATan2(rsin, rcos);
	}

	T dot(const tquat<T>& b) const {
		return x*b.x + y*b.y + z*b.z + w * b.w;
	}

	T magnitude() const {
		return dot(*this);
	}

	#define USE_ROTATE_FOR_VECTOR 1

	inline tvec3<T> right() const {
		#if USE_ROTATE_FOR_VECTOR
		return rotate(tvec3<T>((T)1.0, (T)0.0, (T)0.0));
		#else 
		return tvec3<T>{ (T)1 - (T)2 * (y * y + z * z), (T)2 * (x * y + w * z), (T)2 * (x * z - w * y) }.normalize();
		#endif 
	}

	inline tvec3<T> left() const {
		#if USE_ROTATE_FOR_VECTOR
		return rotate(tvec3<T>((T)-1.0, (T)0.0, (T)0.0));
		#else 
		return tvec3<T>{ -((T)1 - (T)2 * (y * y + z * z)), -((T)2 * (x * y + w * z)), -((T)2 * (x * z - w * y)) }.normalize();
		#endif 
	}

	inline tvec3<T> up() const {
		#if USE_ROTATE_FOR_VECTOR
		return rotate(tvec3<T>((T)0.0, (T)1.0, (T)0.0));
		#else 
		return  tvec3<T>{ (T)2 * (x * y - w * z), (T)1 - (T)2 * (x * x + z * z), (T)2 * (y * z + w * x) }.normalize();
		#endif 
	}

	inline tvec3<T> down() const {
		#if USE_ROTATE_FOR_VECTOR
		return rotate(tvec3<T>((T)0.0, (T)-1.0, (T)0.0));
		#else 
		return  tvec3<T>{ -((T)2 * (x * y - w * z)), -((T)1 - (T)2 * (x * x + z * z)), -((T)2 * (y * z + w * x)) }.normalize();
		#endif 
	}

	inline tvec3<T> forward() const {
		#if USE_ROTATE_FOR_VECTOR
		return  rotate(tvec3<T>((T)0.0, (T)0.0, (T)1.0));
		#else 
		return tvec3<T>{ (T)2 * (x * z + w * y), (T)2 * (y * x - w * x), (T)1 - (T)2 * (x * x + y * y) }.normalize();
		#endif 
	}

	inline tvec3<T> back() const {
		#if USE_ROTATE_FOR_VECTOR
		return  rotate(tvec3<T>((T)0.0, (T)0.0, (T)-1.0));
		#else 
		return tvec3<T>{ -((T)2 * (x * z + w * y)), -((T)2 * (y * x - w * x)), -((T)1 - (T)2 * (x * x + y * y)) }.normalize();
		#endif 
	}


	tquat<T> operator-() const {return tquat(-x, -y, -z, -w);}

	tvec4<T> vec4() const {
		return tvec4<T>(x, y, z, w);
	}

	tvec3<T> vec3() const {
		return tvec3<T>(x, y, z);
	}

	tmat4<T> mat4() const;

};



template <class T>
inline tquat<T> operator*(const tquat<T>& l, const tquat<T>& r) {
	return tquat<T>(
			l.w*r.x + l.x*r.w + l.y*r.z - l.z*r.y,
			l.w*r.y + l.y*r.w + l.z*r.x - l.x*r.z,
			l.w*r.z + l.z*r.w + l.x*r.y - l.y*r.x,
			l.w*r.w - l.x*r.x - l.y*r.y - l.z*r.z
		);
}


template <class T>
inline tquat<T> operator+(const tquat<T>& l, const tquat<T>& r) {
	return tquat<T>(l.x + r.x, l.y + r.y, l.z + r.z, l.w + r.w);
}

template <class T>
inline tquat<T> operator-(const tquat<T>& l, const tquat<T>& r) {
	return tquat<T>(l.x - r.x, l.y - r.y, l.z - r.z, l.w - r.w);
}


template <class T>
tquat<T> operator*(const tquat<T>& q, const tvec3<T>& v) {
	T w = -(q.x * v.x) - (q.y * v.y) - (q.z * v.z);
	T x =  (q.w * v.x) + (q.y * v.z) - (q.z * v.y);
	T y =  (q.w * v.y) + (q.z * v.x) - (q.x * v.z);
	T z =  (q.w * v.z) + (q.x * v.y) - (q.y * v.x);
	return tquat<T>(x, y, z, w);
}

template <class T>
inline static tquat<T> normalize(const tquat<T>& q) {
	return q.normalized();
}

template <class T>
static T dot(const tquat<T>& a, const tquat<T>& b) {
	return a.dot(b);
}

template<class T>
static tquat<T> Lerp(const tquat<T>& a, const tquat<T>& b, T amt) {
	tvec4<T> avec = a.vec4();
	tvec4<T> bvec = b.vec4();
	return tquat<T>(avec.mix(bvec, amt)).normalized();
}

template<class T>
static tquat<T> SLerp(const tquat<T>& a, const tquat<T>& b, T amt) {
	if(dot(a, b) < (T)0)
		{
		a.w=-a.w;
		a.x=-a.x;
		a.y=-a.y;
		a.z=-a.z;
	}
	T d = dot(a,b);
	if(d >= (T)1) {
		return a;
	}
	T theta = ACos(d);
	if(theta == (T)0) { return(a);}
	return a*(Sin(theta - amt*theta)/Sin(theta)) + b*(Sin(amt * theta)/Sin(theta));
}

template<class T>
tmat4<T> tquat<T>::mat4() const {
	return mat4::AlignZ(right(), up(), forward());
}

#define self (*this)
template<class T>
tquat<T> tmat4<T>::rotation() const {
	tquat<T> out;
	T w4 = (T)1.0 / ((T)4.0 * out.w);
	out.w = sqrt((T)1.0 + self[0][0] + self[1][1] + self[2][2]) * (T)0.5;
	out.x = (self[1][2] - self[2][1]) * w4;
	out.y = (self[2][0] - self[0][2]) * w4;
	out.z = (self[0][1] - self[1][0]) * w4;
	return out;
}

#undef self

template <class T>
std::ostream& operator << (std::ostream& o, tquat<T> quot) {
	o << quot.x <<", "<<quot.y << ", "<<quot.z << ", "<<quot.w;
	return o;
}


typedef tquat<float> quat;
typedef tquat<double> dquat;


#endif // QUAT

