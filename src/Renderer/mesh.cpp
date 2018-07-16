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
#include "mesh.hpp"

//My crappy .obj file loader -MaGetzUb
bool Mesh::load(const std::string & path) {
	std::ifstream file;
	file.open(path, std::ios::in);
	if(!file.is_open()) return false;

	std::string line;

	std::vector<vec3> positions; //Collect all the vertex positions 
	std::vector<vec2> texCoords; //and text coords first, before storing into member variables. 
	std::vector<vec3> normals; //Same with the normals

	float x, y, z;
	while(std::getline(file, line)) {
		std::stringstream lineStream(line);
		std::string cmd;
		lineStream >> cmd;
		if(cmd == "v") {
			lineStream >> x >> y >> z;
			positions.push_back({ x, y, z });
		} else if(cmd == "vt") {
			lineStream >> x >> y;
			texCoords.push_back({ x, y });
		} else if(cmd == "vn") {
			lineStream >> x >> y >> z;
			normals.push_back({ x, y, z });
		} else if(cmd == "f") {
			//We have to basically duplicate
			//vertices for many faces, especially
			//when texture coordinates are utilized

			int vids[256]{}, vtids[256]{}, vnids[256]{};
			unsigned faceIndices[256]{};
			char crap;

			int phase = 0;
			int vertsPerFace = 0;
			char peekChar;
			do {

				peekChar = lineStream.peek();
				if((peekChar >= '0' && peekChar <= '9') || peekChar == '.') {
					switch(phase) {
						case 0: lineStream >> vids[vertsPerFace]; break;
						case 1: lineStream >> vtids[vertsPerFace]; break;
						case 2: lineStream >> vnids[vertsPerFace]; break;
					}
				}

				if(peekChar == '/') { lineStream >> crap; phase++; }
				if(peekChar == ' ') { lineStream.ignore(); if((phase % 3) == 2) vertsPerFace++; phase = 0; continue; }

			} while(peekChar != EOF);


			for(int i = 0; i < vertsPerFace + 1; i++) {

				vids[i]--;
				if(texCoords.size()) vtids[i]--;
				if(normals.size()) vnids[i]--;

			}

			//A side note; this process can be probably be optimized with hashmap.
			for(int i = 0; i < vertsPerFace+1; i++) {

				auto vId = std::find_if(mVertices.begin(), mVertices.end(),
					[&](const Vertex& v) -> bool { //Lambda for finding certain vertex, if it doesn't found *

						return v.position().xyz() == positions[vids[i]] &&
							(!texCoords.empty() ? v.texCoord() == texCoords[vtids[i]] : true) &&
							(!normals.empty() ? v.normal() == normals[vnids[i]] : true);

					}
				);

				if(vId == mVertices.end()) {

					vec4 pos = vec4(positions[vids[i]]);
					faceIndices[i] = mVertices.size();
					mVertices.push_back(Vertex(pos)); // * add it to the vertex list **
					if(texCoords.size()) mVertices.back().setTexCoord(texCoords[vtids[i]]);
					if(normals.size()) mVertices.back().setNormal(normals[vnids[i]]);

				} else {

					faceIndices[i] = vId - mVertices.begin(); //** else, just calculate index and add it to face indices. 
				
				}

			}

			for(int k = 0; k < vertsPerFace-1; k++) {
				mTriangles.emplace_back(uvec3{ faceIndices[0], faceIndices[k + 1], faceIndices[k + 2] });
			}

		}
	}
	return true;
}
