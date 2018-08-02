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
#include <regex> //MEH

#include "Renderer/rendercontext.hpp"
#include "Renderer/mesh.hpp"
#include "starfield.hpp"
#include "Math/quat.hpp"


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

	int screenWidth = 640, screenHeight = 480;
	int canvasWidth = 512, canvasHeight = 384;

	std::fstream settings;
	settings.open("settings.ini", std::ios::in);
	if(!settings.is_open()) {
		settings.open("settings.ini", std::ios::out);
		settings << "Modify these values at your own risk." << std::endl;
		settings << "ScreenWidth = " << screenWidth << std::endl;
		settings << "ScreenHeight = " << screenHeight << std::endl;
		settings << "CanvasWidth = " << canvasWidth << std::endl;
		settings << "CanvasHeight = " << canvasHeight << std::endl;
		settings.close();
	} else {
		std::string line;
		std::string var, value;
		std::regex match("(\\w+)\\s*=(\\d+)\\s*");

		while(std::getline(settings, line)) {
			std::smatch subMatches;
			if(std::regex_match(line, subMatches, match)) {
				if(subMatches.size() == 3) {
					var = subMatches[1].str();
					value = subMatches[2].str();
				}
			}
			if(var == "ScreenWidth") screenWidth = stoi(value);
			if(var == "ScreenHeight") screenHeight = stoi(value);
			if(var == "CanvasWidth") canvasWidth = stoi(value);
			if(var == "CanvasHeight") canvasHeight = stoi(value);
		}
		settings.close();
	}


	Window window;
	window.initialize(screenWidth, screenHeight, "Software Renderer");
	Canvas canvas(window);
	canvas.resize(canvasWidth, canvasHeight);

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


	Texture texture1;
	texture1.load("res/texture1.png");


	Texture texture2;
	texture2.load("res/texture2.png");


	Mesh mesh1;
	mesh1.load("res/suzanne.obj");

	Mesh mesh2;
	mesh2.load("res/terrain.obj");

	Mesh mesh3;
	mesh3.load("res/cube.obj");

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

	float suzanneAngle = 0.f;

	rc.setSamplingMode(Texture::Sampling::CubicHermite);
	rc.setTextureWrapingMode(Texture::Wraping::Repeat);

	rc.enableLighting(true);
	rc.setAmbientColor({0.2f, 0.1f, 0.6f});
	rc.setAmbientIntensity(0.3f);

	rc.setSunColor({ 1.f, 0.6f, 0.2f });
	rc.setSunIntensity(4.f);

	rc.setSunPosition(vec3(16.f, 3.f, 8.f));

	while(!window.isClosed()) {
		window.pollEvents();


		rc.reset();
		canvas.clearCheckerboard(rc.checkerBoard(), rc.ambientColor()*rc.ambientIntensity());

		if(!rc.checkerBoard()) {
			rc.clearDepthBuffer();
		}
		#if TEST == STARFIELD 
		starfield.updateAndDraw(deltaTime);
		#endif 
		#if TEST == RENDERCONTEXT


		mat4 mat;

		if(inputs.isMouseHit(0)) ShowCursor(FALSE);
		if(inputs.isMouseUp(0)) ShowCursor(TRUE);
		if(inputs.isMouseDown(0)) {
			
			RECT rct;
			GetClientRect(window.handle(), &rct);

			POINT p;
			p.x = (rct.right - rct.left) / 2;
			p.y = (rct.bottom - rct.top) / 2;
			


			float mx = (p.x - inputs.mouseX());
			float my = (p.y - inputs.mouseY());

			window.setTitle("Coords: "+std::to_string(mx)+", "+std::to_string(my));

			cameraPitch += mx * deltaTime;
			cameraYaw += my * deltaTime;

			ClientToScreen(window.handle(), &p);
			SetCursorPos(p.x, p.y);
		}


		mat4 rotation = mat4::Rotation(cameraYaw, 1.0f, 0.0f, 0.0f)*mat4::Rotation(cameraPitch, 0.0f, 1.0f, 0.0f);

		vec3 dir = rotation.transposed() * vec3(0.f, 0.f, 1.f);
		vec3 right = cross(dir, vec3(0.f, 1.f, 0.f));
		right = reorthogonalize(right, dir);

		if(inputs.isKeyHit('C')) rc.setTextureWrapingMode(Texture::Wraping::Clamp);
		if(inputs.isKeyHit('R')) rc.setTextureWrapingMode(Texture::Wraping::Repeat);


		if(inputs.isKeyHit(0x31)) rc.setSamplingMode(Texture::Sampling::None);
		if(inputs.isKeyHit(0x32)) rc.setSamplingMode(Texture::Sampling::Linear);
		if(inputs.isKeyHit(0x33)) rc.setSamplingMode(Texture::Sampling::CubicHermite);

		if(inputs.isKeyDown('W')) cameraPosition += dir*5.f * deltaTime;
		if(inputs.isKeyDown('S')) cameraPosition -= dir*5.f * deltaTime;
		if(inputs.isKeyDown('D')) cameraPosition += right*5.f * deltaTime;
		if(inputs.isKeyDown('A')) cameraPosition -= right*5.f * deltaTime;

		/*
		if(inputs.isKeyHit(VK_SPACE)) {
			rc.testMipmap(!rc.isMipMapTesting());
		}

		if(rc.isMipMapTesting()) {
			rc.setMipMapLevel(rc.mipMapLevel() + (inputs.isKeyHit(VK_UP) - inputs.isKeyHit(VK_DOWN)));
		}*/

		mat4 viewProjection = mat4::Perspective(aRatio, 90.0f, .01f, 100.f) * rotation * mat4::Translate(cameraPosition);

		z -= (float)(inputs.isKeyDown(VK_UP) - inputs.isKeyDown(VK_DOWN)) * deltaTime;

		mat4 suzanneRotation = mat4::Rotation(QMod(suzanneAngle, 360.0f), 0.f, 1.f, 0.f);
		mat4 model = mat4::Translate(0.0f, 0.0f, -2.0f)  * suzanneRotation;
		mat = viewProjection * model;
		rc.drawMesh(mesh1, mat, texture1, suzanneRotation);

		suzanneAngle += deltaTime*20.f;

		model = mat4::Translate(0.0f, -4.0f, 0.0f);
		mat = viewProjection * model;
		rc.drawMesh(mesh2, mat, texture2);

		model = mat4::Translate(0.0f, -2.0f, -2.0f);
		mat = viewProjection * model;
		rc.drawMesh(mesh3, mat);
		#endif 


		#ifdef NDEBUG
		std::cout << inputs.mouseX() << ", " << inputs.mouseY() << '\r';
		#endif 
		canvas.swapBuffers();
		rc.advanceCheckerboard();


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
		deltaTime = (double)(tp - prevtime) / 1000.0;


		//Cap the FPS, so movement won't become too slow. 
		if(deltaTime < (1.0 / 120.0)) {
			Sleep((int)1000.0*(1.0 / 120.0));
		}

		//Calculate FPS
		prevtime = tp;
		frames++;
		fpsTime += (double)deltaTime;
		if(fpsTime > 1.0) {
			fps = frames; 
			frames = 0;
			fpsTime = 0;
		}

		//window.setTitle("Software Rendering | FPS: " + std::to_string(fps) + " | Triangles: "+std::to_string(rc.renderedTriangles()) /*+ (rc.isMipMapTesting() ? " | MipMap testing! " + std::to_string(rc.mipMapLevel()) : "")*/);


		inputs.update();
	}

	return 0;
}




