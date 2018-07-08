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

#ifndef MATH_HPP
#define MATH_HPP

#ifdef USE_SIMD
#include <immintrin.h>
#endif 

#include <algorithm>
#include <cmath>


constexpr double Pi = 3.1415926535897932384626433832795;

template <class T>
inline T Fract(T v) {
	return v - floor(v);
}

template <class T>
inline T QMod(T a, T b) {
	a /= b;
	return (a - floor(a)) * (b - (T)1);
}

template<class T>
inline T WrapAngle(T ang) {
	return QMod(ang, (T)360);
}

template <class T>
inline T Cos(T angle) {
	return cos(angle / (T)180 * (T)Pi);
}

template <class T>
inline T Sin(T angle) {
	return sin(angle / (T)180 * (T)Pi);
}

template <class T>
inline T Clamp(T v, T l, T u) {
	return std::max(l, std::min(v, u));
}

template <class T>
inline T Mix(T a, T b, T c) {
	return ((T)1 - c)*a + c*b;
}




#endif 