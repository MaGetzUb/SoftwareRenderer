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

#include "canvas.hpp"
#include "vertex.hpp"
#include "../Math/matrix.hpp"
#include "gradients.hpp"
#include "edge.hpp"


//Code based on TheBennybox' video tutorial series on software rendering
class RenderContext {
	
	Canvas* mCanvas = nullptr;

	mat4 mScreenSpaceTransform;

	public: 
		
		RenderContext() = default;
		RenderContext(Canvas& canvas): mCanvas(&canvas) {
			mScreenSpaceTransform = mat4::ScreenSpace((float)canvas.width() * .5f, (float)canvas.height() * .5f);
		}

		void setCanvas(Canvas& canvas) { 
			mCanvas = &canvas; 
			mScreenSpaceTransform = mat4::ScreenSpace((float)canvas.width() * .5f, (float)canvas.height() * .5f);
		}


		void resetCanvas() { mCanvas = nullptr; }

		void fillTriangle(const Vertex& a, const Vertex& b, const Vertex& c);

	private:


		void scanTriangle(const Vertex& a, const Vertex& b, const Vertex& c, bool handedness);

		void drawScanLine(const Gradients& gradients, Edge* a, Edge* b, int y);

		void scanEdge(const Gradients& gradients, Edge* a, Edge* b, bool handedness);

};


#endif 