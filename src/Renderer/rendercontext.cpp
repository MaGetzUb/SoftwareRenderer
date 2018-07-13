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



void RenderContext::drawMesh(const Mesh& mesh, const mat4& transform, const Texture& texture, const mat4& normalMatrix) {
	mTexture = &texture;

	for(auto& face: mesh.triangles()) {
		Vertex a, b, c;
		a = mesh.vertices()[face[0]];
		b = mesh.vertices()[face[1]];
		c = mesh.vertices()[face[2]];
		a.transform(transform, normalMatrix);
		b.transform(transform, normalMatrix);
		c.transform(transform, normalMatrix);

		fillTriangle(a, b, c);
	}

}

void RenderContext::drawMesh(const Mesh& mesh, const mat4& transform, const mat4& normalMatrix) {
	mTexture = nullptr;


	for(auto& face : mesh.triangles()) {
		Vertex a, b, c;
		a = mesh.vertices()[face[0]];
		b = mesh.vertices()[face[1]];
		c = mesh.vertices()[face[2]];
		a.transform(transform, normalMatrix);
		b.transform(transform, normalMatrix);
		c.transform(transform, normalMatrix);

		fillTriangle(a, b, c);
	}

}

void RenderContext::fillTriangle(const Vertex& a, const Vertex& b, const Vertex& c) {

	auto clipTriangle = [](const Vertex& a, const Vertex& b, const Vertex& c, std::vector<Vertex>& output) -> bool {

		std::vector<Vertex> tmp1{ a, b, c }, tmp2;
		auto ping = &tmp1, pong = &tmp2;

		for(int i = 0; i < 3; i++) {
			for(float j = -1.f; j <= 1.f; j += 2.f) {

				const Vertex* prev = &ping->back();
				float prevCmpVal = prev->xyzwComponent(i) * j;
				bool prevInside = prevCmpVal <= prev->w();

				for(int k = 0; k < ping->size(); k++) {
					const Vertex* curr = &ping->at(k);

					float currCmpVal = curr->xyzwComponent(i) * j;
					bool currInside = currCmpVal <= curr->w();

					if(currInside != prevInside) {
						float mixAmt = (prev->w() - prevCmpVal) / ((prev->w() - prevCmpVal) - (curr->w() - currCmpVal));
						pong->emplace_back(Vertex::Mix(*prev, *curr, mixAmt));
					}

					if(currInside) {
						pong->emplace_back(*curr);
					}

					prev = curr;
					prevCmpVal = currCmpVal;
					prevInside = currInside;

				 }
				ping->clear();
				if(pong->empty()) return false;
				std::swap(ping, pong);
			}
		}
		if(ping->empty()) return false;
		output = tmp1;
		return true;
	};
	
	auto fill = [this](const Vertex& a, const Vertex& b, const Vertex& c) {

		Vertex tra = a;
		Vertex trb = b;
		Vertex trc = c;
		


		tra.transform(mScreenSpaceTransform).perspectiveDivide();
		trb.transform(mScreenSpaceTransform).perspectiveDivide();
		trc.transform(mScreenSpaceTransform).perspectiveDivide();

		Vertex* minYV = &tra, *midYV = &trb, *maxYV = &trc;

		if(TriangleAreaDoubled(*minYV, *maxYV, *midYV) <= 0.0) return;

		if(maxYV->y() < midYV->y()) std::swap(maxYV, midYV);
		if(midYV->y() < minYV->y()) std::swap(midYV, minYV);
		if(maxYV->y() < midYV->y()) std::swap(maxYV, midYV);

		bool handedness = TriangleAreaDoubled(*minYV, *maxYV, *midYV) >= 0.0;

		scanTriangle(*minYV, *midYV, *maxYV, handedness);

		mDrawnTriangles++;

	};

	if(a.isInView() && b.isInView() && c.isInView()) {
		fill(a, b, c);
		return;
	}

	std::vector<Vertex> vertices{ a, b, c };

	if(clipTriangle(a, b, c, vertices)) {
		for(int i = 1; i < (int)vertices.size()-1; i++) {
			fill(vertices[0], vertices[i], vertices[i+1]);
		}
	}

}

void RenderContext::scanTriangle(const Vertex& a, const Vertex& b, const Vertex& c, bool handedness) {
	
	Gradients gradients(mPerspectiveCorrected, a, b, c);
	Edge topBottom(gradients, a, c, 0);
	Edge topMiddle(gradients, a, b, 0);
	Edge middleBottom(gradients, b, c, 1);

	scanEdge(gradients, &topBottom, &topMiddle, handedness);
	scanEdge(gradients, &topBottom, &middleBottom, handedness);
		
}

void RenderContext::drawScanLineTextured(const Gradients& gradients, Edge* a, Edge* b, int y) {
	int xMin = (int)ceilf(a->x());
	int xMax = (int)ceilf(b->x());

	float offset = xMin - a->x();
	float depth = a->depth() + gradients.depthXStep() * offset;
	float zDivisor = a->zDivisor() + gradients.zDivisorXStep() * offset;

	vec4 color = a->color() + gradients.colorXStep() * offset;
	//vec2 texCoord = a->texCoord() + gradients.texCoordXStep() * offset;
	vec3 normal = a->normal() + gradients.normalXStep() * offset;


	float xDist = b->x() - a->x();
	vec2 texCoordStep = (b->texCoord() - a->texCoord()) / xDist;
	vec2 texCoord = a->texCoord() + texCoordStep * offset;

	vec3 sunPos = vec3(mSunPosition).normalized();
	float mipLevels = (float)mTexture->mipLevels();

	for(int x = xMin; x < xMax; x++) {
		//mCanvas->set(x, y, color);

		if((x & 1) ^ (y & 1) ^ (mCheckerBoard&1)) {
			float z = 1.0f / zDivisor;
			float& db = mDepthBuffer[x + y * mWidth];
			if(depth < db) {
				vec4 sun = mEnableLighting ? mix(mAmbientColor, mSunColor, std::min(1.f, std::max(mAmbientIntensity, dot(normal*z, sunPos)*mSunIntensity))) : 1.f;
				float zd = (1.0f - (depth * zDivisor)) - 0.15f;
				int mipLevel = (int)(std::min(1.f, std::max(0.f, zd*zd)) * mipLevels);
				mCanvas->set(x, y, mTexture->sample(texCoord * z, mipLevel, Texture::Sampling::Linear) * (color * z) * sun);
				db = depth;
			}
		}
		color += gradients.colorXStep();
		texCoord += texCoordStep;
		zDivisor += gradients.zDivisorXStep();
		depth += gradients.depthXStep();
		normal += gradients.normalXStep();
	}
}

void RenderContext::drawScanLine(const Gradients& gradients, Edge* a, Edge* b, int y) {
	int xMin = (int)ceilf(a->x());
	int xMax = (int)ceilf(b->x());

	float offset = xMin - a->x();
	float depth = a->depth() + gradients.depthXStep() * offset;
	float zDivisor = a->zDivisor() + gradients.zDivisorXStep() * offset;



	vec4 color = a->color() + gradients.colorXStep() * offset;
	vec3 normal = a->normal() + gradients.normalXStep() * offset;

	vec3 sunPos = vec3(mSunPosition).normalized();

	for(int x = xMin; x < xMax; x++) {
		
		if((x & 1) ^ (y & 1) ^ (mCheckerBoard & 1)) {
			float z = 1.0f / zDivisor;
			float& db = mDepthBuffer[x + y * mWidth];
			if(depth < db) {
				vec4 sun = mEnableLighting ? mix(mAmbientColor, mSunColor, std::min(1.f, std::max(mAmbientIntensity, dot(normal*z, sunPos)*mSunIntensity))) : 1.f;
				mCanvas->set(x, y, (color * z) * sun);
				db = depth;
			}
		}
		color += gradients.colorXStep();
		zDivisor += gradients.zDivisorXStep();
		depth += gradients.depthXStep();
		normal += gradients.normalXStep();
	}
}


void RenderContext::scanEdge(const Gradients& gradients, Edge* a, Edge* b, bool handedness) {

	Edge* left = a;
	Edge* right = b;
	if(handedness) std::swap(left, right);

	int yStart = (int)b->yStart();
	int yEnd = (int)b->yEnd();

	if(mTexture) {
		for(int j = yStart; j < yEnd; j++) {
			drawScanLineTextured(gradients, left, right, j);
			left->step();
			right->step();
		}
	} else {
		for(int j = yStart; j < yEnd; j++) {
			drawScanLine(gradients, left, right, j);
			left->step();
			right->step();
		}
	}
}


