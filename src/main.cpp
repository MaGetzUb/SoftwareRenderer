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
#include "Renderer/mesh.hpp"
#include "starfield.hpp"

#define NONE 0
#define STARFIELD 1
#define RENDERCONTEXT 2

#define TEST RENDERCONTEXT

#ifdef NDEBUG 
int CALLBACK WinMain(
	HINSTANCE /*hInstance*/,
	HINSTANCE /*hPrevInstance*/,
	LPSTR     /*lpCmdLine*/,
	int       /*nCmdShow*/
)
#else
int main()
#endif 
{

	Window window;
	window.initialize(640, 480, "Software Renderer");
	Canvas canvas(window);
	canvas.resize(640, 480);

	float aRatio = 4.0f / 3.0f;



	InputManager inputs;

	inputs.setCustomMessageCallback([&aRatio](Window& window, UINT msg, WPARAM wparam, LPARAM lparam)->LRESULT {
		switch(msg) {
			case WM_SIZE: {
				float w = GET_X_LPARAM(lparam);
				float h = GET_Y_LPARAM(lparam);
				aRatio = w / h;
			} break;
		}
		return window.defaultWindowProc(msg, wparam, lparam);
	});
	
	window.setMessageCallback(inputs.callbackProcessor());


	#if TEST == STARFIELD 
	Starfield starfield(canvas);
	#endif 
	#if TEST == RENDERCONTEXT
	RenderContext rc(canvas);
	


	Texture texture;
	texture.load("res/texture2.png");

	Mesh mesh;
	mesh.load("res/terrain.obj");

	rc.setTexture(texture);
	
	#endif 

	auto prevtime = Timer();
	double deltaTime = 0.0f; 

	float ang = 0.0f;

	#ifdef TEXT_INPUT_TEST
	std::string input;
	auto eraseTime = Timer();
	#endif 

	//TL---TR
	//|     |
	//|     |
	//BL---BR

	Vertex vertices[] = { 
		{-1.0f, -1.0f}, //Bottom left  
		{-1.0f,  1.0f}, //Top    left  
		{ 1.0f,  1.0f}, //Top    right 
		{ 1.0f, -1.0f}  //Bottom right 
	};
	vertices[0].setColor({ 1.0f, 0.0f, 0.0f, 1.0 });
	vertices[1].setColor({ 1.0f, 1.0f, 0.0f, 1.0 });
	vertices[2].setColor({ 0.0f, 0.0f, 1.0f, 1.0 });
	vertices[3].setColor({ 0.0f, 1.0f, 1.0f, 1.0 });

	vertices[0].setTexCoord({ 0.0f, 1.0f });
	vertices[1].setTexCoord({ 0.0f, 0.0f });
	vertices[2].setTexCoord({ 1.0f, 0.0f });
	vertices[3].setTexCoord({ 1.0f, 1.0f });

	int frames = 0, fps = 0;
	float z = -2.0f;
	double fpsTime = 0.0f;

	float cameraYaw = 0.0f; 
	float cameraPitch = 0.0f;
	vec3 cameraPosition; 

	while(!window.isClosed()) {
		window.pollEvents();
		rc.clearDepthBuffer();
		canvas.clear(0, 0, 0);

		#if TEST == STARFIELD 
		starfield.updateAndDraw(deltaTime);
		#endif 
		#if TEST == RENDERCONTEXT


		mat4 mat;

		if(inputs.isMouseDown(0)) {
			//ShowCursor(FALSE);
			RECT rct;
			GetWindowRect(window.handle(), &rct);
			
			int centerX = rct.left + (rct.right - rct.left) / 2;
			int centerY = rct.top + (rct.bottom - rct.top) / 2;

			float mx = inputs.mouseMoveX();
			float my = inputs.mouseMoveY();

			cameraPitch += mx * deltaTime / 1000.0f;;
			cameraYaw += my * deltaTime / 1000.0f;;
			
			SetCursorPos(centerX, centerY);
		}

		//cameraPitch += (float)(inputs.isKeyDown(VK_LEFT) - inputs.isKeyDown(VK_RIGHT)) * 100.f * deltaTime / 1000.0f;
		//cameraYaw += (float)(inputs.isKeyDown(VK_UP) - inputs.isKeyDown(VK_DOWN)) * 100.f * deltaTime / 1000.0f;
		mat4 rotation = mat4::Rotation(cameraYaw, 1.0f, 0.0f, 0.0f)*mat4::Rotation(cameraPitch, 0.0f, 1.0f, 0.0f);

		if(inputs.isKeyDown('W')) {
			cameraPosition += rotation.transposed() * vec3(0.f, 0.f, 5.f) * (deltaTime / 1000.f);
		}

		if(inputs.isKeyDown('S')) {
			cameraPosition -= rotation.transposed() * vec3(0.f, 0.f, 5.f) * (deltaTime / 1000.f);
		}

		mat4 model = mat4::Translate(0.0f, -4.0f, 0.0f);

		mat4 view = rotation * mat4::Translate(cameraPosition);

		mat = mat4::Perspective(aRatio, 90.0f, .01f, 100.f) * view * model;
		z -= (float)(inputs.isKeyDown(VK_UP) - inputs.isKeyDown(VK_DOWN)) * (deltaTime/1000.f);

		rc.drawMesh(mesh, mat, texture);


		#endif 


		#ifdef NDEBUG
		std::cout << inputs.mouseX() << ", " << inputs.mouseY() << '\r';
		#endif 
		canvas.swapBuffers();



		#ifdef TEXT_INPUT_TEST
		RECT rct = { 0, 0, 800, 300 };
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
		deltaTime = (double)(tp - prevtime);
		prevtime = tp;
		frames++;
		fpsTime += (double)deltaTime / 1000.0;
		if(fpsTime > 1.0) {
			fps = frames; 
			frames = 0;
			fpsTime = 0;
		}

		window.setTitle("Software Rendering | FPS: " + std::to_string(fps));


		inputs.update();
	}

	return 0;
}




