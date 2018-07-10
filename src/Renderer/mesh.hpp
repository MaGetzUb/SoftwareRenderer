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
#ifndef MESH_HPP
#define MESH_HPP

#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include "vertex.hpp"

class Mesh {

	std::vector<Vertex> mVertices; 
	std::vector<uvec3> mTriangles; 

	public:

		Mesh() {}
		Mesh(const Mesh& b) = delete;

		Mesh(Mesh&& b) {
			std::swap(mVertices, b.mVertices);
			std::swap(mTriangles, b.mTriangles);
		}

		Mesh& operator=(Mesh&& b) {
			std::swap(mVertices, b.mVertices);
			std::swap(mTriangles, b.mTriangles);
			return *this;
		}

		~Mesh() {}
		
		//Warning assumes that an .obj file contains only triangualted meshes
		bool load(const std::string& path);

		inline const std::vector<Vertex>& vertices() const { return mVertices; }

		inline const std::vector<uvec3> triangles() const { return mTriangles; }

		//Some functions, for convenience.

		inline const Vertex& vertex(unsigned index) const { return mVertices[index]; }

		inline unsigned vertexCount() const { return mVertices.size(); }

		inline const uvec3& triangle(unsigned index) const { return mTriangles[index]; }

		inline unsigned triangleCount() const { return mTriangles.size(); }
};

#endif //MESH_HPP

