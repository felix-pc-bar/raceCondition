#include <iostream>
#include <SDL.h>
#include <cstdint>
#include <vector>
#include <cmath>
#include <filesystem>
#include "render/CPURenderer.h"
#include "import3d.h"
#include "engconfig.h"

using std::endl, std::cout;

int main(int argc, char** args) {
	cout << std::filesystem::current_path() << endl;
	// Pointers to our window and surface
	SDL_Surface* winSurface = NULL;
	SDL_Window* window = NULL;
	SDL_Renderer* mainRenderer = NULL;

	int result;
	
	// Initialize SDL. SDL_Init will return -1 if it fails.
	result = SDL_Init(SDL_INIT_EVERYTHING);
	if (result < 0) 
	{
		cout << "Error initializing SDL: " << SDL_GetError() << endl;
		system("pause");
		// End the program
		return 1;
	}

	result = SDL_CreateWindowAndRenderer(screenwidth, screenheight, SDL_WINDOW_FULLSCREEN_DESKTOP, &window, &mainRenderer);
	if (result < 0)
	{
		cout << "Error creating window and renderer: " << SDL_GetError() << endl;
		return 1;
	}
	SDL_SetWindowTitle(window, "Provision");
	
	// Init
	// Lock the mouse to the window and hide the cursor
	SDL_ShowCursor(SDL_DISABLE);
	SDL_SetRelativeMouseMode(SDL_TRUE);

	CPURenderer vp(mainRenderer, screenwidth, screenheight); // Create viewport
	vp.Clear(0xFF5792FF); // sky blue

	Scene mainScene; //Create main scene
	currentScene = &mainScene; // Set the current scene to mainScene
	
	mainScene.cams.emplace_back(); // Add a camera to mainScene
	mainScene.currentCam = &mainScene.cams[0]; // Set mainScene's current camera to the camera we just created
	mainScene.cams[0].pos.z -= 2;
	//mainScene.cams[0].rot.pitch += pi / 2;
	mainScene.meshes.emplace_back(importObj("content/obj/raven.obj"));
	mainScene.meshes[0].materials.emplace_back(0.15f, 0.15f, 0.15f);
	mainScene.meshes.emplace_back(importObj("content/obj/fTerrain.obj"));
	mainScene.meshes[1].materials.emplace_back(1.0f, 0.7f, 0.5f);
	mainScene.meshes.emplace_back(importObj("content/obj/fCacti.obj"));
	mainScene.meshes[2].materials.emplace_back(0.25f, 0.95f, 0.3f);
	mainScene.meshes.emplace_back(importObj("content/obj/fRocks.obj"));
	mainScene.meshes[3].materials.emplace_back(0.33f, 0.33f, 0.33f);
	mainScene.meshes.emplace_back(importObj("content/obj/credit.obj"));
	mainScene.meshes[4].materials.emplace_back(0.8f, 0.8f, 0.5f);

	const Uint8* gk; 
	SDL_Event event;

	float freecamspeed;
	int frame = 0;

	Quaternion qDelta(pi / 200, { 0,1,0 });

	//mainScene.meshes[0].rotateQuat({ -pi / 4, -1, 0, 0 });
	mainScene.meshes[0].move({ -100,40,0 });


	Quaternion camLookOffset(0.2f, { 1, 0, 0 }); // no rotation, or maybe slight pitch down if needed
	mainScene.cams[0].quatIdentity = mainScene.meshes[0].quatIdentity * camLookOffset;

	float flightSpeed = 0.2f;
	float turnSpeed = 0.02f;
	float roll = 0.0f;

	bb3d testbb({ -10, -100, -10 }, { 10, 100, 10 });

	while (true)
	{
		mainScene.meshes[0].calcBaseVecs();
		Rotation3d& camRot = mainScene.cams[0].rot;
		// Handle inputs
		mainScene.cams[0].calcBaseVecs();
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				return 0;  
			}
			if (event.type == SDL_MOUSEMOTION)
			{
				//mainScene.meshes[0].rotateAxis((float)event.motion.xrel / 1000.0f, mainScene.meshes[0].up);
				//mainScene.meshes[0].rotateAxis((float)event.motion.yrel / 1000.0f, mainScene.meshes[0].right);
				//mainScene.cams[0].quatIdentity = mainScene.meshes[0].quatIdentity * camLookOffset;
			}
		}

		gk = SDL_GetKeyboardState(NULL); 
		//if (gk[SDL_SCANCODE_W]) { mainScene.meshes[0].move(mainScene.meshes[0].forward * 1.0f); }
		if (gk[SDL_SCANCODE_A]) 
		{
			if (roll <= 0.32f) { mainScene.meshes[0].rotateAxis(turnSpeed, { mainScene.meshes[0].forward }); roll += turnSpeed; }
		}
		if (gk[SDL_SCANCODE_D]) 
		{
			if (roll >= -0.32f) { mainScene.meshes[0].rotateAxis(-turnSpeed, { mainScene.meshes[0].forward }); roll -= turnSpeed; }
		}
		
		mainScene.meshes[0].rotateAxis(roll / -30.0f, { 0, 1, 0 }); 

		if (gk[SDL_SCANCODE_S]) {	mainScene.meshes[0].rotateAxis(-turnSpeed, {mainScene.meshes[0].right}); }
		if (gk[SDL_SCANCODE_W]) {	mainScene.meshes[0].rotateAxis(turnSpeed, {mainScene.meshes[0].right}); }
		if (gk[SDL_SCANCODE_LSHIFT]) { flightSpeed += 0.01f; }
		if (gk[SDL_SCANCODE_LCTRL] && flightSpeed > 0.01f) { flightSpeed -= 0.01f; }
	


		mainScene.cams[0].quatIdentity = mainScene.meshes[0].quatIdentity * camLookOffset;

		//if (gk[SDL_SCANCODE_A] && mainScene.meshes[0].quatIdentity) {}

		//flightSpeed = std::max(std::min(2.0l, (75.0f - mainScene.meshes[0].position.y) / 80.0l), 1.0l);
		mainScene.meshes[0].move(mainScene.meshes[0].forward * flightSpeed);
		
		Position3d camOffset(0, 4, -10);
		camOffset.rotateQuat(mainScene.meshes[0].quatIdentity);
		mainScene.cams[0].pos = mainScene.meshes[0].position + camOffset;

		vp.drawScene(*currentScene);
		vp.Present();
		frame++;
		vp.Clear(0xFF5792FF); // sky blue
	}

	system("pause");

	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}