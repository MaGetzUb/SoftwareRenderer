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

template <class T>
class tmat4;

template <class T>
class tmat3 : public tmat<T, 3, 3> {
	public:
		tmat3() :tmat<T, 3, 3>() {}

		tmat3(std::initializer_list<std::initializer_list<T>> init) :tmat<T, 3, 3>(init) {}

		tmat3(const tvec3<T>& a, const tvec3<T>& b, const tvec3<T>& c): 
			tmat3({a.x, a.y, a.z, b.x, b.y, b.z, c.x, c.y, c.z}) 
		{}

		tmat3(const tmat<T, 3, 3>& mat):tmat<T, 3, 3>(mat) {}

		static tmat3<T> Normal(const tmat4<T>& matB) {
			tmat3<T> mat;
			for(int j = 0; j < 3; j++) {
				for(int i = 0; i < 3; i++) {
					mat[i][j] =  matB[i][j];
				}
			}
			return mat;
		}

		static tmat3<T> Rotation(T angle, T x, T y, T z) {
			tmat3<T>  mat = tmat3<T>::Identity();

			T c = (T)Cos(angle);
			T s = (T)Sin(angle);

			mat[0][0] = x*x*((T)1 - c) + c;
			mat[1][0] = x*y*((T)1 - c) + z*s;
			mat[2][0] = x*z*((T)1 - c) - y*s;
			
			mat[0][1] = x*y*((T)1 - c) - z*s;
			mat[1][1] = y*y*((T)1 - c) + c;
			mat[2][1] = y*z*((T)1 - c) + x*s;

			mat[0][2] = x*z*((T)1 - c) + y*s;
			mat[1][2] = y*z*((T)1 - c) - x*s;
			mat[2][2] = z*z*((T)1 - c) + c;

			return mat;
		}

		static tmat3<T> Rotation(T angle, const tvec3<T>& dir) {
			return Rotation(angle, dir.x, dir.y, dir.z);
		}

		static tmat3<T> RotationZ(T ang) {
			tmat3<T> mat = Identity();
			mat[0][0] = Cos(ang); mat[1][0] = -Sin(ang);
			mat[0][1] = Sin(ang); mat[1][1] = Cos(ang);
			return mat;
		}

		static tmat3<T> AlignZ(const tvec3<T>& r, const tvec3<T>& u, const tvec3<T>& f) {
			tmat3<T> mat = Identity();
			mat[0][0] = r.x; mat[1][0] = r.y; mat[2][0] = r.z;
			mat[0][1] = u.x; mat[1][1] = u.y; mat[2][1] = u.z;
			mat[0][2] = f.x; mat[1][2] = f.y; mat[2][2] = f.z;
			return mat;
		}

		static tmat3<T> AlignZ(const tvec3<T>& f, const tvec3<T>& u) {

			tvec3<T> x, y;

			y = u - dot(f, u) * f;

			T ylen = length(y);
			if(ylen < std::numeric_limits<T>::epsilon()) {
				tvec3<T> k = tvec3<T>((T)1, (T)0, (T)0);
				y = k - f * dot(k, f);
				ylen = length(y);
				if(ylen < std::numeric_limits<T>::epsilon()) {
					k = tvec3<T>((T)0, (T)0, (T)1);
					y = k - f * dot(k, f);
					ylen = length(y);
				}
			}
			y = y / ylen;
			x = cross(y, f);

			return AlignZ(x, y, f).transposed();
		}

		static tmat3<T> Translate(T x, T y) {
			tmat3<T> mat = Identity();
			mat[2][0] = x;
			mat[2][1] = y;
			return mat;
		}

		static tmat3<T> Translate(const tvec2<T>& vec) {
			return Translate(vec.x, vec.y);
		}


		static tmat3<T> Scale(T x, T y) {
			tmat3<T> mat = Identity();
			mat[0][0] = x;
			mat[1][1] = y;
			return mat;
		}

		static tmat3<T> Scale(const tvec2<T>& vec) {
			return Scale(vec.x, vec.y);
		}

		tmat3<T>& translate(T x, T y) {
			self *= Translate(x, y);
			return *this;
		}

		tmat3<T>& scale(T x, T y) {
			self *= Scale(x, y);
			return *this;
		}

		tmat3<T>& rotate(T a) {
			self *= AlignZ(a);
			return *this;
		}

		static tmat3<T> Transpose(const tmat3& in) {
			tmat3<T> mat;
			for(int j = 0; j < 3; j++) {
				for(int i = 0; i < 3; i++) {
					mat[j][i] = in[i][j];
				}
			}
			return mat;
		}

		tmat3& operator=(const tmat3& matB) {
		        memcpy(tmat<T, 3, 3>::data(), matB.data(), 3*3*sizeof(T));
			return *this;
		}


		static tmat3<T> Identity() {
			return tmat<T, 3, 3>::Identity();
		}

		void identity() {
			self = Identity();
		}

		void transpose() {
			self = Transpose(self);
		}

		void inverse() {
			self = Inverse(self);
		}

		tmat3 transposed() const {
			return Transpose(self);
		}

		tmat3 inversed() const {
			return Inverse(self);
		}

		tmat3 operator-() {
			return tmat<T, 3, 3>::operator-();
		}

#if 1

static T Determinant(const tmat3<T>& m) {
	return  m[0][0] * m[1][1] * m[2][2] -
		m[0][0] * m[2][1] * m[1][2] +
		m[1][0] * m[2][1] * m[0][2] -
		m[1][0] * m[0][1] * m[2][2] +
		m[2][0] * m[0][1] * m[1][2] -
		m[2][0] * m[1][1] * m[0][2];
}

static tmat3<T> Inverse(const tmat3<T>& m) {
	tmat3<T> b;
	T d = Determinant(m);
	assert(d != 0 && "Determinant produces 0!");

	T dinv = (T)1 / d;

	b[0][0] = dinv * (m[1][1] * m[2][2] - m[2][1] * m[1][2]);
	b[1][0] =-dinv * (m[1][0] * m[2][2] - m[2][0] * m[1][2]);
	b[2][0] = dinv * (m[1][0] * m[2][1] - m[2][0] * m[1][1]);
	b[0][1] =-dinv * (m[0][1] * m[2][2] - m[2][1] * m[0][2]);
	b[1][1] = dinv * (m[0][0] * m[2][2] - m[2][0] * m[0][2]);
	b[2][1] =-dinv * (m[0][0] * m[2][1] - m[2][0] * m[0][1]);
	b[0][2] = dinv * (m[0][1] * m[1][2] - m[1][1] * m[0][2]);
	b[1][2] =-dinv * (m[0][0] * m[1][2] - m[1][0] * m[0][2]);
	b[2][2] = dinv * (m[0][0] * m[1][1] - m[1][0] * m[0][1]);

	return b;
}

#else
#define a1(m) m[0][0] //Row 1 x
#define a2(m) m[1][0] //Row 1 y
#define a3(m) m[2][0] //Row 1 z
#define b1(m) m[0][1] //Row 2 x
#define b2(m) m[1][1] //Row 2 y
#define b3(m) m[2][1] //Row 2 z
#define c1(m) m[0][2] //Row 3 x
#define c2(m) m[1][2] //Row 3 y
#define c3(m) m[2][2] //Row 3 z

static T Determinant(const tmat3<T>& m) {
	return  a1(m)*b2(m)*c3(m) - 
			a1(m)*b3(m)*c2(m) +
			a2(m)*b3(m)*c1(m) - 
			a2(m)*b1(m)*c3(m) +
			a3(m)*b1(m)*c2(m) - 
			a3(m)*b2(m)*c1(m);
}

static tmat3<T> Inverse(const tmat3<T> &m) {
	tmat3<T> b;
	T d = Determinant(m);
	FrostAssert(d != 0, "Determinant produces 0!");

	T dinv = (T)1 / d;

	a1(b) = dinv  * (b2(m) * c3(m) - b3(m) * c2(m));
	a2(b) = -dinv * (a2(m) * c3(m) - a3(m) * c2(m));
	a3(b) = dinv  * (a2(m) * b3(m) - a3(m) * b2(m));
	c3(b) = dinv *  (a1(m) * b2(m) - a2(m) * b1(m));
	b1(b) = -dinv * (b1(m) * c3(m) - b3(m) * c1(m));
	b2(b) = dinv  * (a1(m) * c3(m) - a3(m) * c1(m));
	b3(b) = -dinv * (a1(m) * b3(m) - a3(m) * b1(m));
	c1(b) = dinv  * (b1(m) * c2(m) - b2(m) * c1(m));
	c2(b) = -dinv * (a1(m) * c2(m) - a2(m) * c1(m));

	return b;
}


#undef a1
#undef a2
#undef a3
#undef b1
#undef b2
#undef b3
#undef c1
#undef c2
#undef c3
#endif 

};

template <class T>
static tmat3<T> operator-(const tmat3<T>& mat) {
	return -(tmat<T, 3, 3>)mat;
}

