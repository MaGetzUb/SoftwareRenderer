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

//Matrix class, based on my other project 'FrostCore'. ~MaGetzUb
#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <initializer_list>
#include <ostream>
#include <cassert>
#include <cstring>
#include "math.hpp"

template <class T>
struct tvec2;

template <class T>
struct tvec4;

template <class T>
struct tvec3;

template <class T>
struct tquat;

template <class T, int N, int M>
class tmat;

template <class T, int N, int M>
class tmat_access {

	tmat<T, N, M> *mMatrix;
	const int mColumn;

	public:

		inline tmat_access(tmat<T, N, M> *mat, int col): 
			mMatrix(mat), 
			mColumn(col) 
		{}

		inline T& operator[](int row) {
			return mMatrix->ref(mColumn, row);
		}

		inline const T& operator[](int row) const {
			return mMatrix->ref(mColumn, row);
		}

};

template <class T, int N, int M>
class tmat_const_access {

	const tmat<T, N, M> *mMatrix;
	const int mColumn;

	public:
		inline tmat_const_access(const tmat<T, N, M> *mat, int col): 
			mMatrix(mat), 
			mColumn(col) 
		{}

		inline const T& operator[](int row) const {
			return mMatrix->ref(mColumn, row);
		}
};

template <class T, int N, int M>
static tmat<T, N, M> operator+(const tmat<T, N, M>& a, const tmat<T, N, M>& b);

template <class T, int N, int M>
static tmat<T, N, M> operator-(const tmat<T, N, M>& a, const tmat<T, N, M>& b);

//This matrix is configured to be row major by default.
//But it is accessible style [column][row]. So [x][y]
//Big thanks to Latexi95 https://github.com/Latexi95
template <class T, int N, int M>
class tmat {

	static_assert(std::is_arithmetic<T>::value, "Only arithmetic types allowed for tmat!");

	protected:
	T mData[N * M];
	public:
		template <class OT, int N1, int M1, int N2>
		friend tmat<OT, N2, M1> operator*(const tmat<OT, N1, M1>& a, const tmat<OT, N2, N1>& b);

		tmat() {
			for(int j = 0; j < M; j++)
				for(int i = 0; i < N; i++)
					mData[j*N+i] = (T)0;
		}
		tmat(std::initializer_list<std::initializer_list<T>> init) {
			assert(init.size() == M && !"Invalid matrix dimensions!");
			auto i = init.begin();
			for(int y = 0; y < M; y++) {
				assert(i->size() == N && "Invalid matrix dimensions!");
				auto vi = i->begin();
				for(int x = 0; x < N; x++) {
					*(mData + x * M + y) = *vi;
					vi++;
				}
				i++;
			}

		}

		~tmat() {}

		tmat(const T diag): tmat() {

			static_assert(N == M, "Cannot fill matrix diagonally because N is not same as M");
			for(int i = 0; i < M; i++)
				*(mData + i * N + i) = diag;

		}

		void fill(T* data) {
			FillMatrix(*this, data);
		}

		void clear() {
			for(int n = 0; n < N; n++)
				for(int m = 0; m < M; m++)
					*(mData + n * M + m) = (T)0;
		}

		inline tmat_access<T, N, M> operator[](int column) {
			return tmat_access<T, N, M>(this, column);
		}

		inline tmat_const_access<T, N, M> operator[](int column) const {
			return tmat_const_access<T, N, M>(this, column);
		}


		inline T& ref(int column, int row) {
			assert(column < N && "Invalid column!");
			assert(row < M && "Invalid row!");
			return *(mData + column * M + row);
		}

		inline const T& ref(int column, int row) const {
			assert(column < N && "Invalid column!");
			assert(row < M && "Invalid row!");
			return *(mData + column * M + row);
		}

		inline T at(int column, int row) const {
			return *(mData + column * M + row);
		}

		static tmat<T, N, M> Identity() {
			static_assert(N == M, "Identity matrix is only possible, when N == M");
			tmat<T, N, M> ret;

			for(int i = 0; i < N; i++)
				ret.ref(i, i) = (T)1;

			return ret;
		}


		inline tmat<T, N, M>& operator*=(const tmat<T, N, M>& matB) {
			*this = (*this * matB);
			return *this;
		}

		inline tmat<T, N, M>& operator+=(const tmat<T, N, M>& matB) {
			*this = (*this + matB);
			return *this;
		}

		inline tmat<T, N, M>& operator-=(const tmat<T, N, M>& matB) {
			*this = (*this - matB);
			return *this;
		}

		inline void operator=(const tmat<T, N, M>& matB) {
			memcpy(mData, matB.mData, N*M*sizeof(T));
		}

		inline void operator=(T* mat) {
			memcpy(mData, mat, N*M*sizeof(T));
		}

		bool operator==(const tmat<T, N, M>& matB) {
			for(int n = 0; n < N; n++)
				for(int m = 0; m < M; m++)
					if(at(n, m) != matB.at(n, m))
						return false;
			return true;
		}


		inline T* data() { return mData; }

		inline const T* data() const { return mData; }

		tmat<T, N, M> operator-() {
			tmat<T, N, M> out;
			for(int n = 0; n < N; n++)
				for(int m = 0; m < M; m++)
					out[n][m] = -at(n, m);
			return out;
		}

};

template <class T, int N1, int M1, int N2>
static tmat<T, N2, M1> operator*(const tmat<T, N1, M1> &matA, const tmat<T, N2, N1> &matB) {

	tmat<T, N2, M1> result;

	for (int y = 0; y < M1; y++) {
		for (int x = 0; x < N2; x++) {
			T s = 0;
			for (int a = 0; a < N1; ++a) {
				s += matA.at(a, y) * matB.at(x, a);
			}
			result.ref(x, y) = s;
		}
	}

	return result;
}

template <class T, int N, int M>
std::ostream& operator<<(std::ostream& os, const tmat<T, N, M>& m) {

	for (int y = 0; y < M; ++y) {
		int x = 0;
		os << "[ ";
		for (; x < N - 1; ++x) {
			os << m.at(x, y) << " ";
		}
		os << m.at(x, y) << " ] \n";
	}
	return os;

}

template <class T, int N, int M>
static tmat<T, N, M> operator+(const tmat<T, N, M>& a, const tmat<T, N, M>& b) {
	tmat<T, N, M> out;
	for(int n = 0; n < N; n++)
		for(int m = 0; m < M; m++)
			out[n][m] = a[n][m] + b[n][m];
	return out;
}

template <class T, int N, int M>
static tmat<T, N, M> operator-(const tmat<T, N, M>& a, const tmat<T, N, M>& b) {
	tmat<T, N, M> out;
	for(int n = 0; n < N; n++)
		for(int m = 0; m < M; m++)
			out[n][m] = a[n][m] - b[n][m];
	return out;
}

template <class T, int N, int M>
static tmat<T, N, M> operator-(const tmat<T, N, M>& mat) {
	tmat<T, N, M> out;
	for(int n = 0; n < N; n++)
		for(int m = 0; m < M; m++)
			out[n][m] = -mat[n][m];
	return out;
}

template <class T, int N, int M>
void FillMatrix(tmat<T, N, M>& mat, T* data) {
	for(int y = 0; y < N; y++)
		for(int x = 0; x < M; x++)
			mat[x][y] = *(data+y*N+x);
}


#define self (*this)
#include "matrix3.inl"
#include "matrix4.inl"
#undef self


typedef tmat4<float> mat4;
typedef tmat3<float> mat3;

#ifdef USE_SIMD

static mat4 operator*(const mat4& a, const mat4& b) {
	mat4 out;
	__m128 row1 = _mm_loadu_ps(&a.data()[0]), row2 = _mm_loadu_ps(&a.data()[4]), row3 = _mm_loadu_ps(&a.data()[8]), row4 = _mm_loadu_ps(&a.data()[12]);
	__m128 col1, col2, col3, col4;
	__m128 row;

	col1 = _mm_set_ps1(b.data()[0]);
	col2 = _mm_set_ps1(b.data()[1]);
	col3 = _mm_set_ps1(b.data()[2]);
	col4 = _mm_set_ps1(b.data()[3]);
	row = _mm_add_ps(_mm_add_ps(_mm_mul_ps(row1, col1), _mm_mul_ps(row2, col2)), _mm_add_ps(_mm_mul_ps(row3, col3), _mm_mul_ps(row4, col4)));
	_mm_storeu_ps(&out.data()[0], row);

	col1 = _mm_set_ps1(b.data()[4]);
	col2 = _mm_set_ps1(b.data()[5]);
	col3 = _mm_set_ps1(b.data()[6]);
	col4 = _mm_set_ps1(b.data()[7]);
	row = _mm_add_ps(_mm_add_ps(_mm_mul_ps(row1, col1), _mm_mul_ps(row2, col2)), _mm_add_ps(_mm_mul_ps(row3, col3), _mm_mul_ps(row4, col4)));
	_mm_storeu_ps(&out.data()[4], row);

	col1 = _mm_set_ps1(b.data()[8]);
	col2 = _mm_set_ps1(b.data()[9]);
	col3 = _mm_set_ps1(b.data()[10]);
	col4 = _mm_set_ps1(b.data()[11]);
	row = _mm_add_ps(_mm_add_ps(_mm_mul_ps(row1, col1), _mm_mul_ps(row2, col2)), _mm_add_ps(_mm_mul_ps(row3, col3), _mm_mul_ps(row4, col4)));
	_mm_storeu_ps(&out.data()[8], row);

	col1 = _mm_set_ps1(b.data()[12]);
	col2 = _mm_set_ps1(b.data()[13]);
	col3 = _mm_set_ps1(b.data()[14]);
	col4 = _mm_set_ps1(b.data()[15]);
	row = _mm_add_ps(_mm_add_ps(_mm_mul_ps(row1, col1), _mm_mul_ps(row2, col2)), _mm_add_ps(_mm_mul_ps(row3, col3), _mm_mul_ps(row4, col4)));
	_mm_storeu_ps(&out.data()[12], row);

	return out;
}

static mat4 IsolateRotationAndScale(const mat4& mat) {
	mat4 out = mat;
	__m128 mul, row;
	mul = _mm_set_ps(1.f, 1.f, 1.f, 0.f);

	row = _mm_mul_ps(_mm_load_ps(&mat[0][0]), mul);
	_mm_store_ps(&out[0][0], row);
	row = _mm_mul_ps(_mm_load_ps(&mat[0][1]), mul);
	_mm_store_ps(&out[0][1], row);
	row = _mm_mul_ps(_mm_load_ps(&mat[0][2]), mul);
	_mm_store_ps(&out[0][2], row);
	row = _mm_mul_ps(_mm_load_ps(&mat[0][3]), mul);
	_mm_store_ps(&out[0][3], row);

	return out;
}


//Based on https://github.com/niswegmann/small-matrix-inverse/blob/master/invert4x4_sse.h
template<>
inline mat4 tmat4<float>::Inverse(const mat4& mat) {
	tmat4<float> out;
	__m128 minor0, minor1, minor2, minor3;
	__m128 row0, row1, row2, row3;
	__m128 det, tmp1;

	tmp1 = _mm_loadh_pi(_mm_loadl_pi(tmp1, (__m64*)(mat.data())), (__m64*)(mat.data() + 4));
	row1 = _mm_loadh_pi(_mm_loadl_pi(row1, (__m64*)(mat.data() + 8)), (__m64*)(mat.data() + 12));

	row0 = _mm_shuffle_ps(tmp1, row1, 0x88);
	row1 = _mm_shuffle_ps(row1, tmp1, 0xDD);

	tmp1 = _mm_loadh_pi(_mm_loadl_pi(tmp1, (__m64*)(mat.data() + 2)), (__m64*)(mat.data() + 6));
	row3 = _mm_loadh_pi(_mm_loadl_pi(row3, (__m64*)(mat.data() + 10)), (__m64*)(mat.data() + 14));

	row2 = _mm_shuffle_ps(tmp1, row3, 0x88);
	row3 = _mm_shuffle_ps(row3, tmp1, 0xDD);

	tmp1 = _mm_mul_ps(row2, row3);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);

	minor0 = _mm_mul_ps(row1, tmp1);
	minor1 = _mm_mul_ps(row0, tmp1);

	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);

	minor0 = _mm_sub_ps(_mm_mul_ps(row1, tmp1), minor0);
	minor1 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor1);
	minor1 = _mm_shuffle_ps(minor1, minor1, 0x4E);

	tmp1 = _mm_mul_ps(row1, row2);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);

	minor0 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor0);
	minor3 = _mm_mul_ps(row0, tmp1);

	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);

	minor0 = _mm_sub_ps(minor0, _mm_mul_ps(row3, tmp1));
	minor3 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor3);
	minor3 = _mm_shuffle_ps(minor3, minor3, 0x4E);

	tmp1 = _mm_mul_ps(_mm_shuffle_ps(row1, row1, 0x4E), row3);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
	row2 = _mm_shuffle_ps(row2, row2, 0x4E);

	minor0 = _mm_add_ps(_mm_mul_ps(row2, tmp1), minor0);
	minor2 = _mm_mul_ps(row0, tmp1);

	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);

	minor0 = _mm_sub_ps(minor0, _mm_mul_ps(row2, tmp1));
	minor2 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor2);
	minor2 = _mm_shuffle_ps(minor2, minor2, 0x4E);

	tmp1 = _mm_mul_ps(row0, row1);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);

	minor2 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor2);
	minor3 = _mm_sub_ps(_mm_mul_ps(row2, tmp1), minor3);

	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);

	minor2 = _mm_sub_ps(_mm_mul_ps(row3, tmp1), minor2);
	minor3 = _mm_sub_ps(minor3, _mm_mul_ps(row2, tmp1));

	tmp1 = _mm_mul_ps(row0, row3);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);

	minor1 = _mm_sub_ps(minor1, _mm_mul_ps(row2, tmp1));
	minor2 = _mm_add_ps(_mm_mul_ps(row1, tmp1), minor2);

	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);

	minor1 = _mm_add_ps(_mm_mul_ps(row2, tmp1), minor1);
	minor2 = _mm_sub_ps(minor2, _mm_mul_ps(row1, tmp1));

	tmp1 = _mm_mul_ps(row0, row2);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);

	minor1 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor1);
	minor3 = _mm_sub_ps(minor3, _mm_mul_ps(row1, tmp1));

	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);

	minor1 = _mm_sub_ps(minor1, _mm_mul_ps(row3, tmp1));
	minor3 = _mm_add_ps(_mm_mul_ps(row1, tmp1), minor3);

	det = _mm_mul_ps(row0, minor0);
	det = _mm_add_ps(_mm_shuffle_ps(det, det, 0x4E), det);
	det = _mm_add_ss(_mm_shuffle_ps(det, det, 0xB1), det);

	tmp1 = _mm_rcp_ss(det);

	det = _mm_sub_ss(_mm_add_ss(tmp1, tmp1), _mm_mul_ss(det, _mm_mul_ss(tmp1, tmp1)));
	det = _mm_shuffle_ps(det, det, 0x00);

	minor0 = _mm_mul_ps(det, minor0);
	_mm_storel_pi((__m64*)(out.data()), minor0);
	_mm_storeh_pi((__m64*)(out.data() + 2), minor0);

	minor1 = _mm_mul_ps(det, minor1);
	_mm_storel_pi((__m64*)(out.data() + 4), minor1);
	_mm_storeh_pi((__m64*)(out.data() + 6), minor1);

	minor2 = _mm_mul_ps(det, minor2);
	_mm_storel_pi((__m64*)(out.data() + 8), minor2);
	_mm_storeh_pi((__m64*)(out.data() + 10), minor2);

	minor3 = _mm_mul_ps(det, minor3);
	_mm_storel_pi((__m64*)(out.data() + 12), minor3);
	_mm_storeh_pi((__m64*)(out.data() + 14), minor3);

	return out;
}

#else
	static mat4 operator*(const mat4& a, const mat4& b) {
		mat4 out(0.0f);
		for(int y = 0; y < 4; y++) {
			for(int x = 0; x < 4; x++) {
				float o = 0;
				for(int c = 0; c < 4; c++) {
					 o += a[c][y] * b[x][c];
				}
				out[x][y] = o;
			}
		}
		return out;
	}
#endif



#endif // MATRIX_HPP
