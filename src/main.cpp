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

#include "System/window.hpp"
#include "Renderer/canvas.hpp"
#include "System/inputmanager.hpp"
#include "System/timer.hpp"
#include <string>
#include <iostream>

#include "Renderer/rendercontext.hpp"
#include "starfield.hpp"

#define NONE 0
#define STARFIELD 1
#define RENDERCONTEXT 2

#define TEST RENDERCONTEXT

#ifdef NDEBUG 
int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow
)
#else
int main() 
#endif 
{

	Window window; 
	window.initialize(800, 600, "Software Renderer");
	Canvas canvas(window);
	canvas.resize(800, 600);

	InputManager inputs; 
	window.setMessageCallback(inputs.callbackProcessor());


	#if TEST == STARFIELD 
	Starfield starfield(canvas);
	#endif 
	#if TEST == RENDERCONTEXT
	RenderContext rc(canvas);
	#endif 


	auto prevtime = Timer();
	float deltaTime = 0.0f; 

	float ang = 0.0f;
	std::string input;
	int eraseTime = Timer();
	int rot = 0;

	Vertex vertices[3] = { {-1.0f, -1.0f}, {0.0f, 1.0f}, {1.0f, -1.0f} };
	vertices[0].setColor({ 1.0f, 0.0f, 0.0f, 1.0 });
	vertices[1].setColor({ 0.0f, 1.0f, 0.0f, 1.0 });
	vertices[2].setColor({ 0.0f, 0.0f, 1.0f, 1.0 });

	int frames = 0, fps = 0;
	float fpsTime = 0.0f;

	while(!window.isClosed()) {
		window.pollEvents();
		
		canvas.clear(0, 0, 0);

		#if TEST == STARFIELD 
		starfield.updateAndDraw(deltaTime);
		#endif 
		#if TEST == RENDERCONTEXT


		mat4 mat;
		mat = mat4::Perspective(4.0f/3.0f, 90.0f, .01f, 10.f) * mat4::Translate(0.0f, 0.0f, -2.0f) * mat4::Rotation(ang, 0.0f, 1.0f, 0.0f);

		ang += (float)(inputs.isKeyDown(VK_RIGHT) - inputs.isKeyDown(VK_LEFT)) * (deltaTime / 1000.0f) * 60.0f;
		rc.fillTriangle(vertices[0].transformed(mat), vertices[1].transformed(mat), vertices[2].transformed(mat));

		#endif 


		#ifdef NDEBUG
		std::cout << inputs.mouseX() << ", " << inputs.mouseY() << '\r';
		#endif 
		canvas.swapBuffers();

		RECT rct = { 0, 0, 800, 300 };


		#if 0
		unsigned int ch;
		if(inputs.isTextInput(ch)) {
			if(isprint(ch)) {
				input += ((char)ch);
			}
		}
		if((inputs.isKeyHit(8) || (inputs.isKeyDown(8) && eraseTime > 1000)) && !input.empty()) {
			eraseTime = Timer()/10 - eraseTime;
			input.pop_back();
		}

		rct.top += 20;
		DrawTextA(window.dc(), input.c_str(), -1, &rct, 0);
		#endif 
		
		auto tp = Timer();
		deltaTime = tp - prevtime;
		prevtime = tp;
		frames++;
		fpsTime += (float)deltaTime / 1000.0f;
		if(fpsTime > 1.0f) {
			fps = frames; 
			frames = 0;
			fpsTime = 0;
		}

		window.setTitle("Software Rendering | FPS: " + std::to_string(fps));


		inputs.update();
	}

	return 0;
}




