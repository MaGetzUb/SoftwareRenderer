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

#ifdef _MSC_VER
#pragma warning( disable : 4522 )
#endif


template <class T>
class alignas(16) tmat4: public tmat<T, 4, 4> {
	public:


		tmat4(std::initializer_list<std::initializer_list<T>> init): tmat<T, 4, 4>(init) {}

		tmat4(T diag = 1): tmat<T, 4, 4>() {
		    for(int j = 0; j < 4; j++)
			for(int i = 0; i < 4; i++)
				self[i][j] = (i == j)*diag;
		}

		tmat4(T a1, T a2, T a3, T a4,
			T b1, T b2, T b3, T b4,
			T c1, T c2, T c3, T c4,
			T d1, T d2, T d3, T d4):
		        tmat<T, 4, 4>()
		{
			self[0][0] = a1; self[1][0] = a2; self[2][0] = a3; self[3][0] = a4;
			self[0][1] = b1; self[1][1] = b2; self[2][1] = b3; self[3][1] = b4;
			self[0][2] = c1; self[1][2] = c2; self[2][2] = c3; self[3][2] = c4;
			self[0][3] = d1; self[1][3] = d2; self[2][3] = d3; self[3][3] = d4;
		}

		tmat4(const tvec4<T>& row1, const tvec4<T>& row2, const tvec4<T>& row3, const tvec4<T>& row4) {
		    self[0][0] = row1.x; self[1][0] = row1.y; self[2][0] = row1.z; self[3][0] = row1.w;
			self[0][1] = row2.x; self[1][1] = row2.y; self[2][1] = row2.z; self[3][1] = row2.w;
			self[0][2] = row3.x; self[1][2] = row3.y; self[2][2] = row3.z; self[3][2] = row3.w;
			self[0][3] = row4.x; self[1][3] = row4.y; self[2][3] = row4.z; self[3][3] = row4.w;
		}



		tmat4(const T* dat, bool rowmajor = true): tmat<T, 4, 4>() {
		        for(int j = 0; j < 4; j++) {
			        for(int i = 0; i < 4; i++) {
				        self[i][j] = dat[j*4+i];
				}
			}
			if(rowmajor == false)
			        transpose();
		}

		inline tmat4(const tmat<T, 4, 4>& mat): tmat<T, 4, 4>(mat) {}

		static inline tmat4<T> Scale(T x, T y, T z) {
			tmat4<T>  mat = tmat4<T>::Identity();
			mat[0][0] = x;
			mat[1][1] = y;
			mat[2][2] = z;
			return mat;
		}

		inline static tmat4<T> Scale(const tvec3<T>& scale) {
			return Scale(scale.x, scale.y, scale.z);
		}


		static inline tmat4<T> Identity() {
			return tmat<T, 4, 4>::Identity();
		}


		static inline tmat4<T> Frustum(T fleft, T fright, T fbottom, T ftop, T fnear, T ffar) {
			tmat4<T> mat = Identity();

			T xdist = fright - fleft;
			T ydist = ftop - fbottom;
			T zdist = ffar - fnear;

			mat[0][0] = ((T)2 * fnear) / xdist;
			mat[1][1] = ((T)2 * fnear) / ydist;

			mat[2][0] = (fright + fleft) / xdist;
			mat[2][1] = (ftop + fbottom) / ydist;
			mat[2][2] =-(ffar + fnear)   / zdist;
			mat[2][3] = (T)-1;

			mat[3][2] = -((T)2 * ffar * fnear) / zdist;

			mat[3][3] = (T)0;

			return mat;
		}



		static inline tmat4<T> Perspective(T aspect, T fov, T znear, T zfar) {
			T fright = znear * (T)tan(fov/(T)2);
			T fleft = -fright;

			T fbottom = fleft / aspect;
			T ftop = fright / aspect;

			return Frustum(fleft, fright, fbottom, ftop, znear, zfar);
		}


		static inline tmat4<T> Ortho(T left, T right, T bottom, T top, T znear, T zfar) {

			tmat4<T> mat = Identity();
			mat[0][0] = (T)2 / (right - left);
			mat[1][1] = (T)2 / (top - bottom);
			mat[2][2] = - (T)2 / (zfar - znear);

			mat[3][0] = - (right + left) / (right - left);
			mat[3][1] = - (top + bottom) / (top - bottom);
			mat[3][2] = - (zfar + znear) / (zfar - znear);
			return mat;
		}

		static inline tmat4<T> ScreenSpace(T halfWidth, T halfHeight) {
			tmat4<T> mat((T)1);
			mat[0][0] = halfWidth; 
			mat[1][1] = -halfHeight;
			mat[3][0] = halfWidth - (T).5;
			mat[3][1] = halfHeight - (T).5;
			return mat;
		}

		static inline tmat4<T> Half() {
			tmat4<T> mat = tmat4<T>::Identity();

			mat[0][0] = (T)0.5;
			mat[1][1] = (T)0.5;
			mat[2][2] = (T)0.5;

			mat[3][0] = (T)0.5;
			mat[3][1] = (T)0.5;
			mat[3][2] = (T)0.5;

			return mat;
		}

		static inline tmat4<T> Translate(T x, T y, T z) {
			tmat4<T>  mat = tmat4<T>::Identity();
			mat[3][0] = x;
			mat[3][1] = y;
			mat[3][2] = z;
			return mat;
		}


		static inline tmat4<T> Translate(const tvec3<T>& trans) {
			tmat4<T>  mat = tmat4<T>::Identity();
			mat[3][0] = trans.x;
			mat[3][1] = trans.y;
			mat[3][2] = trans.z;
			return mat;
		}

		static inline tmat4<T> Rotation(T angle, T x, T y, T z) {
			tmat4<T>  mat = tmat4<T>::Identity();

			T c = Cos(angle);
			T s = Sin(angle);

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

		static tmat4<T> Transform(const tvec3<T>& mPosition, const tvec3<T>& scale, const tquat<T>& rotation) {
		        tmat4<T>  mat = tmat4<T>::Identity();
				/* Todo */
				return mat;
		}

		/**
		 * @brief AlignZ - builds a pointing matrix from 2 vectors
		 * @param u - up vector, it's pointing upwards from eg. camera
		 * @param f - forward vector, it's pointing forwards from eg. camera
		 * @return
		 */
		static tmat4<T> AlignZ(const tvec3<T>& u, const tvec3<T>& f) {
			tvec3<T> x, y;

			y = reorthogonalize(u, f);

			constexpr T eps = (std::numeric_limits<T>::epsilon() * (T)10);

			T ylen = length(y);
			if(ylen < eps) {
				tvec3<T> k = tvec3<T>((T)1, (T)0, (T)0);
				y = k - f * dot(k, f);
				ylen = length(y);
				if(ylen < eps) {
					k = tvec3<T>((T)0, (T)0, (T)1);
					y = k - f * dot(k, f);
					ylen = length(y);
				}
			}
			y = y / ylen;
			x = cross(y, f);

			return AlignZ(x, y, f);
		}

		/**
		 * @brief Rotation - builds a rotation matrix from 3 vectors
		 * @param r - right vector, it's the "x" -axis
		 * @param u - up vector, it's the "y" -axis
		 * @param f - forward vector it's the "z" -axis
		 * @return the rotation matrix
		 */
		static tmat4<T> AlignZ(const tvec3<T>& r, const tvec3<T>& u, const tvec3<T>& f) {
			tmat4<T> mat((T)1);
			mat[0][0] = r.x; mat[1][0] = r.y; mat[2][0] = r.z; //Row 0
			mat[0][1] = u.x; mat[1][1] = u.y; mat[2][1] = u.z; //Row 1
			mat[0][2] = f.x; mat[1][2] = f.y; mat[2][2] = f.z; //Row 2
			return mat;
		}

		static tmat4<T> LookAt(const tvec3<T>& from, const tvec3<T>& to, const tvec3<T>& up) {
			tvec3<T> dir = normalize(to - from);
			tmat4<T> out = AlignZ(dir, up);
			out[3][0] = from.x;
			out[3][1] = from.y;
			out[3][2] = from.z;
			return out;
		}


		static tmat4<T> Transform(const tvec3<T>& mPosition, const tvec3<T>& up, const tvec3<T>& forward) {
			up.normalize();
			forward.normalize();
			tvec3<T> right = cross(up, forward);
			tmat4<T> rot = AlignZ(right, up, forward);
			tmat4<T> pos = Translate(mPosition);
			return rot*pos;
		}

		static tmat4<T> Rotation(const tquat<T>& q) {
			#if 0
			return tmat4<T>((T)1 - (T)2 * (q.y * q.y + q.z * q.z), (T)2 * (q.x * q.y - q.z * q.w), (T)2 * (q.x * q.z + q.y * q.w), (T)0,
					(T)2 * (q.x * q.y + q.z * q.w), (T)1 - (T)2 * (q.x * q.x + q.z * q.z), (T)2 * (q.y * q.z - q.x * q.w), (T)0,
					(T)2 * (q.x * q.z - q.y * q.w), (T)2 * (q.y * q.z + q.x * q.w), (T)1 - (T)2 * (q.x * q.x + q.y * q.y), (T)0,
					(T)0, (T)0, (T)0, (T)1);
			#else
				return AlignZ(q.right(), q.up(), q.forward());
			#endif
		}



		static tmat4<T> Shadow(tvec3<T> ground, tvec3<T> lightPos) {
			tmat4<T> shadow((T)1);
			T dotP = tvec3<T>::dot(ground, lightPos);
			shadow[0][0] =    dotP - lightPos.x * ground.x;
			shadow[0][1] = - lightPos.x * ground.y;
			shadow[0][2] = - lightPos.x * ground.z;


			shadow[1][0] = - lightPos.y * ground.x;
			shadow[1][1] =   dotP - lightPos.y * ground.y;
			shadow[1][2] = - lightPos.y * ground.z;

			shadow[2][0] = - lightPos.z * ground.x;
			shadow[2][1] = - lightPos.z * ground.y;
			shadow[2][2] =   dotP - lightPos.z * ground.z;


			shadow[0][3] =  - lightPos.x * ground.x;
			shadow[1][3] =  - lightPos.y * ground.y;
			shadow[2][3] =    dotP - lightPos.z * ground.z;


			return shadow;
		}

		void transform(T& x, T& y, T& z, T& w) {
			T ox = self[0][0] * x + self[0][1] * y + self[0][2] * z + self[0][3] * w;
			T oy = self[1][0] * x + self[1][1] * y + self[1][2] * z + self[1][3] * w;
			T oz = self[2][0] * x + self[2][1] * y + self[2][2] * z + self[2][3] * w;
			T ow = self[3][0] * x + self[3][1] * y + self[3][2] * z + self[3][3] * w;
			x = ox; y = oy; z = oz; w = ow;
		}

		inline void transform(T& x, T& y, T& z) {
			T w;
			transform(x, y, z, w);
		}

		inline void transform(T& x, T& y) {
			T z, w;
			transform(x, y, z, w);
		}

		void transform(const tvec4<T>& vec);

		void transform(const tvec3<T>& vec);

		void transform(const tvec2<T>& vec);

		tvec3<T> translation() const;

		tvec3<T> scaling() const;

		tquat<T> rotation() const;

		inline void clear() {
		        tmat<T, 4, 4>self.clear();
		}

		inline tmat4<T>& operator=(tmat<T, 4, 4>& mb) {
			tmat<T, 4, 4>self = mb;
			return self;
		}

		inline tmat4<T>& operator*=(const tmat4<T>& mb) {
			self = self * mb;
			return self;
		}

		inline tmat4<T>& operator*=(const tmat<T, 4, 4>& mb) {
			self = self * tmat4<T>(mb);
			return self;
		}

		inline tmat4<T>& operator=(T* mb) {
			tmat<T, 4, 4>self = mb;
			return self;
		}

		inline tmat4<T> operator~() {
			return inversed();
		}

		inline tquat<T> quat() {
			tquat<T> q;
			q.w = sqrt(1.0 + self[0][0] + self[1][1] + self[2][2]) / 2.0;
			T w4 = (4.0 * q.w);
			q.x = (self[2][1] - self[1][2]) / w4;
			q.y = (self[0][2] - self[2][0]) / w4;
			q.z = (self[1][0] - self[0][1]) / w4;
			return q;
		}

		/*
		 ax, ay, az, aw
		 bx, by, bz, bw
		 cx, cy, cz, cw
		 dx, dy, dz, dw

		 ax, bx, cx, dx
		 ay, by, cy, dy
		 az, bz, cz, dz
		 aw, bw, cw, dw

		*/

		static tmat4<T> Transpose(const tmat4<T>& mat) {
			tmat4<T> out;
			for(int i = 0; i < 4; i++) {
				for(int j = 0; j < 4; j++) {
					out[i][j] = mat[j][i];
				}
			}
			return out;
		}


		inline tmat4<T> transposed() const {
			return Transpose(*this);
		}

		inline tmat4<T>& transpose() {
			self = self.transposed();
			return self;
		}


		inline tmat4<T>& setIdentity() {
			self = Identity();
			return self;
		}

		inline tmat4<T>& translate(T x, T y, T z) {
			self *= Translate(x, y, z);
			return self;
		}



		inline tmat4<T>& rotate(const tvec3<T>& r, const tvec3<T>& u, const tvec3<T>& f) {
			tmat4<T> mat = Identity();
			//This is transposed. That's why we aren't multiplying with AlignZ -function.
			mat[0][0] = r.x; mat[0][1] = r.y; mat[0][2] = r.z;
			mat[1][0] = u.x; mat[1][1] = u.y; mat[1][2] = u.z;
			mat[2][0] = f.x; mat[2][1] = f.y; mat[2][2] = f.z;
			self *= mat;
			return self;
		}

		inline tmat4<T>& rotate(const tvec3<T>& u, const tvec3<T>& f) {
			tvec3<T> x, y;

			y = reorthogonalize(u, f);

			constexpr T eps = (std::numeric_limits<T>::epsilon() * (T)10);

			T ylen = length(y);
			if(ylen < eps) {
				tvec3<T> k = tvec3<T>((T)1, (T)0, (T)0);
				y = k - f * dot(k, f);
				ylen = length(y);
				if(ylen < eps) {
					k = tvec3<T>((T)0, (T)0, (T)1);
					y = k - f * dot(k, f);
					ylen = length(y);
				}
			}
			y = y / ylen;
			x = cross(y, f);

			return rotate(x, y, f);
		}

		inline tmat4<T>& rotate(const tquat<T>& q) {
			return rotate(q.right(), q.up(), q.forward());
		}

		inline tmat4<T>& rotate(T angle, T x, T y, T z) {
		    WrapAngle<T>(angle);
			self *= Rotation(angle, x, y, z);
			return self;
		}

		inline tmat4<T>& scale(T x, T y, T z) {
			self *= Scale(x, y, z);
			return self;
		}

		inline tmat4<T>& operator=(const tmat4<T>& matB) {
		        memcpy(tmat<T, 4, 4>::data(), tmat<T, 4, 4>(matB).data(), 4*4*sizeof(T));
			return self;
		}

		static T Determinant(const tmat4<T>& mat) {
			return (T)(
				mat[0][0] * mat[1][1] * mat[2][2] * mat[3][3]-mat[0][0] * mat[1][1] * mat[3][2] * mat[2][3]+mat[0][0] * mat[2][1] * mat[3][2] * mat[1][3]-mat[0][0] * mat[2][1] * mat[1][2] * mat[3][3] +
				mat[0][0] * mat[3][1] * mat[1][2] * mat[2][3]-mat[0][0] * mat[3][1] * mat[2][2] * mat[1][3]-mat[1][0] * mat[2][1] * mat[3][2] * mat[0][3]+mat[1][0] * mat[2][1] * mat[0][2] * mat[3][3] -
				mat[1][0] * mat[3][1] * mat[0][2] * mat[2][3]+mat[1][0] * mat[3][1] * mat[2][2] * mat[0][3]-mat[1][0] * mat[0][1] * mat[2][2] * mat[3][3]+mat[1][0] * mat[0][1] * mat[3][2] * mat[2][3] +
				mat[2][0] * mat[3][1] * mat[0][2] * mat[1][3]-mat[2][0] * mat[3][1] * mat[1][2] * mat[0][3]+mat[2][0] * mat[0][1] * mat[1][2] * mat[3][3]-mat[2][0] * mat[0][1] * mat[3][2] * mat[1][3] +
				mat[2][0] * mat[1][1] * mat[3][2] * mat[0][3]-mat[2][0] * mat[1][1] * mat[0][2] * mat[3][3]-mat[3][0] * mat[0][1] * mat[1][2] * mat[2][3]+mat[3][0] * mat[0][1] * mat[2][2] * mat[1][3] -
				mat[3][0] * mat[1][1] * mat[2][2] * mat[0][3]+mat[3][0] * mat[1][1] * mat[0][2] * mat[2][3]-mat[3][0] * mat[2][1] * mat[0][2] * mat[1][3]+mat[3][0] * mat[2][1] * mat[1][2] * mat[0][3]
			);
		}

		static tmat4<T> Inverse(const tmat4<T>& mat) {
				T det = Determinant(mat);
				assert(det != (T)0 && "Determinant produced zero");

				T invDet = (T)1 / det;

				tmat4<T> out((T)0);
				out[0][0] = invDet  * (mat[1][1] * (mat[2][2] * mat[3][3] - mat[2][3] * mat[3][2]) + mat[1][2] * (mat[2][3] * mat[3][1] - mat[2][1] * mat[3][3]) + mat[1][3] * (mat[2][1] * mat[3][2] - mat[2][2] * mat[3][1]));
				out[1][0] = -invDet * (mat[1][0] * (mat[2][2] * mat[3][3] - mat[2][3] * mat[3][2]) + mat[1][2] * (mat[2][3] * mat[3][0] - mat[2][0] * mat[3][3]) + mat[1][3] * (mat[2][0] * mat[3][2] - mat[2][2] * mat[3][0]));
				out[2][0] = invDet  * (mat[1][0] * (mat[2][1] * mat[3][3] - mat[2][3] * mat[3][1]) + mat[1][1] * (mat[2][3] * mat[3][0] - mat[2][0] * mat[3][3]) + mat[1][3] * (mat[2][0] * mat[3][1] - mat[2][1] * mat[3][0]));
				out[3][0] = -invDet * (mat[1][0] * (mat[2][1] * mat[3][2] - mat[2][2] * mat[3][1]) + mat[1][1] * (mat[2][2] * mat[3][0] - mat[2][0] * mat[3][2]) + mat[1][2] * (mat[2][0] * mat[3][1] - mat[2][1] * mat[3][0]));

				out[0][1] = -invDet * (mat[0][1] * (mat[2][2] * mat[3][3] - mat[2][3] * mat[3][2]) + mat[0][2] * (mat[2][3] * mat[3][1] - mat[2][1] * mat[3][3]) + mat[0][3] * (mat[2][1] * mat[3][2] - mat[2][2] * mat[3][1]));
				out[1][1] = invDet  * (mat[0][0] * (mat[2][2] * mat[3][3] - mat[2][3] * mat[3][2]) + mat[0][2] * (mat[2][3] * mat[3][0] - mat[2][0] * mat[3][3]) + mat[0][3] * (mat[2][0] * mat[3][2] - mat[2][2] * mat[3][0]));
				out[2][1] = -invDet * (mat[0][0] * (mat[2][1] * mat[3][3] - mat[2][3] * mat[3][1]) + mat[0][1] * (mat[2][3] * mat[3][0] - mat[2][0] * mat[3][3]) + mat[0][3] * (mat[2][0] * mat[3][1] - mat[2][1] * mat[3][0]));
				out[3][1] = invDet  * (mat[0][0] * (mat[2][1] * mat[3][2] - mat[2][2] * mat[3][1]) + mat[0][1] * (mat[2][2] * mat[3][0] - mat[2][0] * mat[3][2]) + mat[0][2] * (mat[2][0] * mat[3][1] - mat[2][1] * mat[3][0]));

				out[0][2] = invDet  * (mat[0][1] * (mat[1][2] * mat[3][3] - mat[1][3] * mat[3][2]) + mat[0][2] * (mat[1][3] * mat[3][1] - mat[1][1] * mat[3][3]) + mat[0][3] * (mat[1][1] * mat[3][2] - mat[1][2] * mat[3][1]));
				out[1][2] = -invDet * (mat[0][0] * (mat[1][2] * mat[3][3] - mat[1][3] * mat[3][2]) + mat[0][2] * (mat[1][3] * mat[3][0] - mat[1][0] * mat[3][3]) + mat[0][3] * (mat[1][0] * mat[3][2] - mat[1][2] * mat[3][0]));
				out[2][2] = invDet  * (mat[0][0] * (mat[1][1] * mat[3][3] - mat[1][3] * mat[3][1]) + mat[0][1] * (mat[1][3] * mat[3][0] - mat[1][0] * mat[3][3]) + mat[0][3] * (mat[1][0] * mat[3][1] - mat[1][1] * mat[3][0]));
				out[3][2] = -invDet * (mat[0][0] * (mat[1][1] * mat[3][2] - mat[1][2] * mat[3][1]) + mat[0][1] * (mat[1][2] * mat[3][0] - mat[1][0] * mat[3][2]) + mat[0][2] * (mat[1][0] * mat[3][1] - mat[1][1] * mat[3][0]));

				out[0][3] = -invDet * (mat[0][1] * (mat[1][2] * mat[2][3] - mat[1][3] * mat[2][2]) + mat[0][2] * (mat[1][3] * mat[2][1] - mat[1][1] * mat[2][3]) + mat[0][3] * (mat[1][1] * mat[2][2] - mat[1][2] * mat[2][1]));
				out[1][3] = invDet  * (mat[0][0] * (mat[1][2] * mat[2][3] - mat[1][3] * mat[2][2]) + mat[0][2] * (mat[1][3] * mat[2][0] - mat[1][0] * mat[2][3]) + mat[0][3] * (mat[1][0] * mat[2][2] - mat[1][2] * mat[2][0]));
				out[2][3] = -invDet * (mat[0][0] * (mat[1][1] * mat[2][3] - mat[1][3] * mat[2][1]) + mat[0][1] * (mat[1][3] * mat[2][0] - mat[1][0] * mat[2][3]) + mat[0][3] * (mat[1][0] * mat[2][1] - mat[1][1] * mat[2][0]));
				out[3][3] = invDet  * (mat[0][0] * (mat[1][1] * mat[2][2] - mat[1][2] * mat[2][1]) + mat[0][1] * (mat[1][2] * mat[2][0] - mat[1][0] * mat[2][2]) + mat[0][2] * (mat[1][0] * mat[2][1] - mat[1][1] * mat[2][0]));

				return out;
		}

		inline T determinant() const {
			return Determinant(self);
		}


		inline tmat4& inverse() {
			self = Inverse(self);
			return self;
		}

		inline tmat4 inversed() const {
			return Inverse(self);
		}


};





