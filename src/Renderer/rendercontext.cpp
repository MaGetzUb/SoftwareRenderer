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

#include "rendercontext.hpp"
#include <iostream>
#include "edge.hpp"
#include "gradients.hpp"



void RenderContext::drawMesh(const Mesh& mesh, const mat4& transform, const Texture& texture) {
	mTexture = &texture;

	for(auto& face: mesh.triangles()) {
		Vertex a, b, c;
		a = mesh.vertices()[face[0]];
		b = mesh.vertices()[face[1]];
		c = mesh.vertices()[face[2]];
		a.transform(transform);
		b.transform(transform);
		c.transform(transform);

		fillTriangle(a, b, c);
	}

}

void RenderContext::fillTriangle(const Vertex& a, const Vertex& b, const Vertex& c) {
	Vertex tra = a;
	Vertex trb = b;
	Vertex trc = c;

	tra.transform(mScreenSpaceTransform).perspectiveDivide();
	trb.transform(mScreenSpaceTransform).perspectiveDivide();
	trc.transform(mScreenSpaceTransform).perspectiveDivide();

	Vertex* minYV = &tra,* midYV = &trb,* maxYV = &trc;

	if(TriangleAreaDoubled(*minYV, *maxYV, *midYV) <= 0.0) return;

	if(maxYV->y() < midYV->y()) std::swap(maxYV, midYV);
	if(midYV->y() < minYV->y()) std::swap(midYV, minYV);
	if(maxYV->y() < midYV->y()) std::swap(maxYV, midYV);

	bool handedness = TriangleAreaDoubled(*minYV, *maxYV, *midYV) >= 0.0;

	scanTriangle(*minYV, *midYV, *maxYV, handedness);

}

void RenderContext::scanTriangle(const Vertex& a, const Vertex& b, const Vertex& c, bool handedness) {
	
	Gradients gradients(a, b, c);
	Edge topBottom(gradients, a, c, 0);
	Edge topMiddle(gradients, a, b, 0);
	Edge middleBottom(gradients, b, c, 1);

	scanEdge(gradients, &topBottom, &topMiddle, handedness);
	scanEdge(gradients, &topBottom, &middleBottom, handedness);
		
}

void RenderContext::drawScanLine(const Gradients& gradients, Edge* a, Edge* b, int y) {
	int xMin = (int)ceilf(a->x());
	int xMax = (int)ceilf(b->x());

	float offset = xMin - a->x();
	float depth = a->depth() + gradients.depthXStep() * offset;
	float zDivisor = a->zDivisor() + gradients.zDivisorXStep() * offset;

	vec4 color = a->color() + gradients.colorXStep() * offset;
	vec2 texCoord = a->texCoord() + gradients.texCoordXStep() * offset;
	
	for(int x = xMin; x < xMax; x++) {
		//mCanvas->set(x, y, color);
		float z = 1.0f / zDivisor;
		float& db = mDepthBuffer[x + y * mWidth];
		if(depth < db) {
			mCanvas->set(x, y, mTexture->sample(texCoord * z, Texture::Sampling::Linear)*(color*z));
			db = depth;
		}
		color += gradients.colorXStep();
		texCoord += gradients.texCoordXStep();
		zDivisor += gradients.zDivisorXStep();
		depth += gradients.depthXStep();
	}
}

void RenderContext::scanEdge(const Gradients& gradients, Edge* a, Edge* b, bool handedness) {

	Edge* left = a;
	Edge* right = b;
	if(handedness) std::swap(left, right);

	int yStart = (int)b->yStart();
	int yEnd = (int)b->yEnd();

	if(mTexture)
	for(int j = yStart; j < yEnd; j++) {
		drawScanLine(gradients, left, right, j);
		left->step();
		right->step();
	}

}


