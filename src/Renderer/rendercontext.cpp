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

float TriangleAreaDoubled(const Vertex& a, const Vertex& b, const Vertex& c) {

	float x1 = b.x() - a.x();
	float y1 = b.y() - a.y();
	float x2 = c.x() - a.x();
	float y2 = c.y() - a.y();

	return x1*y2 - x2*y1;
}



void RenderContext::fillTriangle(const Vertex& a, const Vertex& b, const Vertex& c) {
	Vertex tra = a;
	Vertex trb = b;
	Vertex trc = c;

	tra.transform(mScreenSpaceTransform).perspectiveDivide();
	trb.transform(mScreenSpaceTransform).perspectiveDivide();
	trc.transform(mScreenSpaceTransform).perspectiveDivide();

	Vertex* minYV = &tra,* midYV = &trb,* maxYV = &trc;


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
	
	vec4 color = a->color() + gradients.colorXStep() * offset;

	for(int x = xMin; x < xMax; x++) {
		mCanvas->set(x, y, color);
		color += gradients.colorXStep();
	}
}

void RenderContext::scanEdge(const Gradients& gradients, Edge* a, Edge* b, bool handedness) {

	Edge* left = a;
	Edge* right = b;
	if(handedness) std::swap(left, right);

	int yStart = (int)b->yStart();
	int yEnd = (int)b->yEnd();

	for(int j = yStart; j < yEnd; j++) {
		drawScanLine(gradients, left, right, j);
		left->step();
		right->step();
	}

}


