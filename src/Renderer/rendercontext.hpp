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

#ifndef RENDERCONTEXT_HPP
#define RENDERCONTEXT_HPP

#include <vector>
#include "canvas.hpp"
#include "vertex.hpp"
#include "../Math/matrix.hpp"
#include "mesh.hpp"
#include "gradients.hpp"
#include "texture.hpp"
#include "edge.hpp"

//Code based on TheBennybox' video tutorial series on software rendering
class RenderContext {
	
	mat4 mScreenSpaceTransform;
	Canvas* mCanvas = nullptr;
	int mWidth, mHeight;
	const Texture* mTexture = nullptr;
	float* mDepthBuffer;

	bool mUseTexture = false;

	public: 
	
		inline void clearDepthBuffer() {
			__m128 fill = _mm_set_ps1(1.0f);
			for(int i = 0; i < mWidth*mHeight; i += 4) {
				_mm_store_ps((float*)mDepthBuffer + i, fill);
			}
		}

		RenderContext() = default;
		RenderContext(Canvas& canvas): 
			mCanvas(&canvas) 
		{
			mWidth = mCanvas->width();
			mHeight = mCanvas->height();
			mScreenSpaceTransform = mat4::ScreenSpace((float)canvas.width() * .5f, (float)canvas.height() * .5f);
			unsigned size = mWidth * mHeight * sizeof(float);
			size += (size % 16);
			mDepthBuffer = (float*)_aligned_malloc(size, 16);
		}

		void setCanvas(Canvas& canvas) { 
			mCanvas = &canvas; 
			mWidth = mCanvas->width();
			mHeight = mCanvas->height();
			mScreenSpaceTransform = mat4::ScreenSpace((float)canvas.width() * .5f, (float)canvas.height() * .5f);
			unsigned size = mWidth * mHeight * sizeof(float);
			size += (size % 16);
			mDepthBuffer = (float*)_aligned_realloc(mDepthBuffer, size, 16);
		}

		void drawMesh(const Mesh& mesh, const mat4& transform, const Texture& texture);


		inline void setTexture(const Texture& texture) { mTexture = &texture; }
		
		inline void resetTexture() { mTexture = nullptr; }
		
		inline void setTextureUsage(bool b) { mUseTexture = b; }

		inline bool isTexturing() const { return mUseTexture; }

		inline void resetCanvas() { mCanvas = nullptr; }

		void fillTriangle(const Vertex& a, const Vertex& b, const Vertex& c);

	private:


		void scanTriangle(const Vertex& a, const Vertex& b, const Vertex& c, bool handedness);

		void drawScanLine(const Gradients& gradients, Edge* a, Edge* b, int y);

		void scanEdge(const Gradients& gradients, Edge* a, Edge* b, bool handedness);

};


#endif //RENDERCONTEXT_HPP