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

#ifndef TEXTURE_HPP
#define TEXTURE_HPP 

#include "../Math/vec4.hpp"
#include "../Math/vec2.hpp"
#include <string>

class Texture { 
	
	vec4* mPixels = nullptr; //Store the pixel data as floats, so there's no need for byte -> float conversion.
	ivec2 mSize; 
	
	public:
		
		enum class Sampling {
			None,
			Linear,
			CubicHermite
		};

		enum class Wraping {
			Clamp,
			Repeat
		};

	
		Texture() = default; 
		
		Texture(const Texture& b) = delete; 
		
		Texture(Texture&& b) {
			std::swap(mPixels, b.mPixels);
			std::swap(mSize, b.mSize);
		}

		Texture(const std::string& path);

		~Texture() { delete [] mPixels; }
		
		Texture& operator=(Texture&& b) {
			std::swap(mPixels, b.mPixels);
			std::swap(mSize, b.mSize);
			return *this;
		}


		bool load(const std::string& path);
		
		int width() const { return mSize.x; }

		int height() const { return mSize.y; }

		inline vec4 sample(int x, int y) const { 
			assert(x >= 0); 
			assert(y >= 0);  
			return mPixels[(x % mSize.x) + (y % mSize.y) * mSize.x]; 
		}

		vec4 sample(float x, float y, Sampling sampling = Sampling::None, Wraping wraping = Wraping::Repeat) const;

		inline vec4 sample(const vec2& vec, Sampling sampling = Sampling::None, Wraping wraping = Wraping::Repeat) const { return sample(vec.x, vec.y, sampling, wraping); }
};


#endif //TEXTURE_HPP


