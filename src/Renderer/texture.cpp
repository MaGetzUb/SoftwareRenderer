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

#include "texture.hpp"

#define STB_IMAGE_IMPLEMENTATION 
#include "../stb/stb_image.h" 



Texture::Texture(const std::string& path) {
	load(path);
}

bool Texture::load(const std::string& path) {
	
	int channels; 
	stbi_uc* pixels = stbi_load(path.c_str(), &mSize.x, &mSize.y, &channels, 4);
	if(!pixels)
		return false; 
	mPixels = new vec4[mSize.x * mSize.y];
	stbi_uc* ptr = pixels;
	for(int y = 0; y < mSize.y; y++)
	for(int x = 0; x < mSize.x; x++)
	{
		mPixels[x + y * mSize.x] = PixelToVec4(*(int*)ptr);
		ptr += 4;
	}
	stbi_image_free(pixels);
	return true;
}

vec4 Texture::sample(float x, float y, Sampling sampling, Wraping wraping) const {

	switch(wraping) {
		case Wraping::Repeat:
			x = QMod(x * mSize.x, (float)mSize.x);
			y = QMod(y * mSize.y, (float)mSize.y);
		break;
		case Wraping::Clamp:
			x = Clamp(x * mSize.x, 0.f, (float)mSize.x);
			y = Clamp(y * mSize.y, 0.f, (float)mSize.y);
		break;
	}
	switch(sampling) {
		case Sampling::None: return sample((int)x, (int)y); break;
		case Sampling::Linear: 
		case Sampling::CubicHermite:
		{
			float fracX = x - (int)x;
			float fracY = y - (int)y; 

			if(sampling == Sampling::CubicHermite) {
				fracX = fracX * fracX * (3.f - 2.f*fracX);
				fracY = fracY * fracY * (3.f - 2.f*fracX);
			}

			vec4 a = sample((int)x, (int)y);
			vec4 b = sample((int)x + 1, (int)y);
			vec4 c = sample((int)x, (int)y + 1);
			vec4 d = sample((int)x + 1, (int)y + 1);
			
			return a + fracX * (b - a) + fracY * (c - a) * (1.0f - fracX) + fracX * fracY * (d - b);
		}
	
	}

	return vec4{ 0.0f };

}
