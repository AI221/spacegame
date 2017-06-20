/*
Copyright 2017 Jackson Reed McNeill

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/





#include<SDL2/SDL.h>
#include<SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <math.h>
#include <vector>
#include <functional>
//Local includes
#include "vector2.h"
#include "debugUI.h"
#include "sprite.h"
#include "camera.h"
#include "physics.h"
#include "renderedPhysicsObject.h"


#define SPRITE_DIR "../sprites/"




//Definitions


#define debug true //wheather debug draws, menus, etc. is included. 




/*LONG-TERM TODO

*Lagged camera? ("chasecam") 
*Standard implementation for rotation matrix instead of spamming it everywhere?

*Possible "engine demo game" would be an asteroid field with constant collisions. Would demo the effeciency of the physics engine very well.


*/

//Options



//MATH FUNCTIONS

double distance(double x1, double y1, double x2, double y2) //tested & working
{
	return sqrt((pow(x2-x1,2))+(pow(y2-y1,2)));
}





Vector2r Vector2ToVector2r(Vector2 convert)
{
	Vector2r newV;
	newV.x = convert.x;
	newV.y = convert.y;
	
	return newV;
}


SDL_Renderer* myRenderer;

Camera camera;
int camFocusedObj = 1;

void render()
{
	camera.pos = allPhysicsObjects[camFocusedObj]->position;
	for (int i=1; i <= numPhysicsObjs; i++)
	{
		GE_BlitRenderedPhysicsObject(physicsObjects[i],&camera);
	}
}


Sprite* nothingHere;
Sprite* somethingHere;

void debug_render()
{ //pulled from my prototype
	GE_BlitSprite(somethingHere,{0,0,0},{10,10});
	camera.pos = allPhysicsObjects[camFocusedObj]->position;
	camera.pos.x -= 640/2;
	camera.pos.y -= 580/2;
	camera.pos.x = camera.pos.x/10;
	camera.pos.y = camera.pos.y/10;
	
	//camera.pos.x = 10;
	//camera.pos.y = 10;
	int camerax = (int) camera.pos.x;
	int cameray = (int) camera.pos.y;
	for (int i = 0; i < 70; i++) { //start from camera position, increase forward 
		for (int o = 0; o < 70; o++) {
			//std::cout << "i " << i <<" t " << i+camerax << std::endl;
			if ((i+camerax < 0) || (o+cameray < 0)) {} else {
				if ( sGrid[i+camerax][o+cameray] != 0)
				{
					GE_BlitSprite(somethingHere,{i*10,o*10,0},{0,0});
					//std::cout << "nothingHere; ";`
				} 
				else 
				{
					GE_BlitSprite(nothingHere,{i*10,o*10,0},{0,0});
					//std::cout << "somethingHere; ";
				}
			}
		}
	}


}


int main()
{
	if (TTF_Init() < 0) 
	{
		//TODO: Handle error...
		std::cout << "!!!TFT_Init Error!" << std::endl;
		return 0;
	}
	for (int i = 0; i < 200; i++)
	{
		for( int o = 0; o < 200; o++)
		{
			sGrid[i][o] = 0;
		}
	}
	sGrid[10][10] = 1;

	SDL_Window* myWindow;
	SDL_Event event;


	camera.pos = {200,200,0};
	camera.screenWidth = 1080;
	camera.screenHeight = 720;

	
	myWindow = SDL_CreateWindow("Spacegame", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, camera.screenWidth, camera.screenHeight, 0);

	myRenderer = SDL_CreateRenderer(myWindow, -1, SDL_RENDERER_ACCELERATED);

	//debug 
	nothingHere = GE_CreateSprite(myRenderer,SPRITE_DIR"DEBUG_nothingHere.bmp",10,10);
	somethingHere = GE_CreateSprite(myRenderer, SPRITE_DIR"DEBUG_somethingHere.bmp",10,10);
	
	Sprite* mySprite = GE_CreateSprite(myRenderer, SPRITE_DIR"simple.bmp",25,25);

	Sprite* otherSprite = GE_CreateSprite(myRenderer, SPRITE_DIR"DEBUG_somethingHere.bmp",25,25);

	Sprite* shoddySpaceship = GE_CreateSprite(myRenderer, SPRITE_DIR"shottyspaceship.bmp",25,25);

	Sprite* bg = GE_CreateSprite(myRenderer,SPRITE_DIR"DEBUG_nothingHere.bmp",camera.screenWidth,camera.screenHeight);
		
	physicsObjects[numPhysicsObjs] = GE_CreateRenderedPhysicsObject(myRenderer,/*shoddySpaceship*/mySprite,{50,50,0},{0,0,0},{25,25});	
	camFocusedObj = physicsObjects[numPhysicsObjs]->physicsObject->ID;

	int me;
	me = physicsObjects[numPhysicsObjs]->physicsObject->ID;
	allPhysicsObjects[me]->collisionRectangles[allPhysicsObjects[me]->numCollisionRectangles] = {0,0,25,25};
	allPhysicsObjects[me]->numCollisionRectangles++;

	numPhysicsObjs++;
	physicsObjects[numPhysicsObjs] = GE_CreateRenderedPhysicsObject(myRenderer,mySprite,{200,200,0},{0,0,0},{25,25});	
	
	me = physicsObjects[numPhysicsObjs]->physicsObject->ID;
	allPhysicsObjects[me]->collisionRectangles[allPhysicsObjects[me]->numCollisionRectangles] = {0,0,25,25};
	allPhysicsObjects[me]->numCollisionRectangles++;

	numPhysicsObjs++;
	physicsObjects[numPhysicsObjs] =  GE_CreateRenderedPhysicsObject(myRenderer,otherSprite,{200,200,0},{0,0,0},{25,25});	
	
	me = physicsObjects[numPhysicsObjs]->physicsObject->ID;
	allPhysicsObjects[me]->collisionRectangles[allPhysicsObjects[me]->numCollisionRectangles] = {0,0,25,25};
	allPhysicsObjects[me]->numCollisionRectangles++;



	
	
	bool keysHeld[323] = {false}; 
	bool isDebugRender = false;
	#ifdef debug
		debugWindow* interface1_menu = new debugWindow(myRenderer,{100,100},{320,240});

		debugText* texttest = new debugText(myRenderer,{25,25},"texttext");
		interface1_menu->addElement(texttest);

		debugButton* cubeRenderButton = new debugButton(myRenderer,{0,50},{100,50},"cubeRenderer");

		auto callback = [&] () { isDebugRender = !isDebugRender; };
		cubeRenderButton->C_Pressed = callback;
		interface1_menu->addElement(cubeRenderButton);	
		
		debugButton* spawnButton = new debugButton(myRenderer,{175,50},{100,50},"spawn phys. obj.");

		auto callback2 = [&] () { 
			Vector2r pos = physicsObjects[camFocusedObj]->physicsObject->position;
			for (int i=0;i < 20;i++)
			{
				numPhysicsObjs++;
				physicsObjects[numPhysicsObjs] = GE_CreateRenderedPhysicsObject(myRenderer,otherSprite,{pos.x,pos.y,0},{(double)(rand() % 101)/100,(double)(rand() % 101)/100,0},{25,25});	
				//physicsObjects[numPhysicsObjs]->myPhysicsObject->setVelocity({(double)(rand() % 101)/100,(double)(rand() % 101)/100}); //random between 0-1 w/ 2 decimals
			}
		};
		spawnButton->C_Pressed = callback2;
		interface1_menu->addElement(spawnButton);


	#endif
	while (true)
	{
		if(SDL_PollEvent(&event))
		{
			if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym <= 323)
				{
					keysHeld[event.key.keysym.sym] = true;
				}
				#ifdef debug
					//std::cout << event.key.keysym.sym << std::endl;
					if (event.key.keysym.sym == SDLK_INSERT) //SDLK_SCROLLLOCK
					{
						interface1_menu->isOpen = true;
					}
				#endif
			}
			if (event.type == SDL_KEYUP)
			{
				if (event.key.keysym.sym <= 323)
				{
					keysHeld[event.key.keysym.sym] = false;
				}
			}
		}
		if(keysHeld[SDLK_w])
		{
			GE_AddRelativeVelocity(allPhysicsObjects[camFocusedObj],{0,-0.5,0});
		}
		if(keysHeld[SDLK_s])
		{
			GE_AddRelativeVelocity(allPhysicsObjects[camFocusedObj],{0,0.5,0});
		}
		if(keysHeld[SDLK_d])
		{
			GE_AddRelativeVelocity(allPhysicsObjects[camFocusedObj],{0.5,0,0});
		}
		if(keysHeld[SDLK_a])
		{
			GE_AddRelativeVelocity(allPhysicsObjects[camFocusedObj],{-0.5,0,0});
		}
		if(keysHeld[SDLK_q])
		{
			GE_AddRelativeVelocity(allPhysicsObjects[camFocusedObj],{0,0,-0.25});
		}
		if(keysHeld[SDLK_e])
		{
			GE_AddRelativeVelocity(allPhysicsObjects[camFocusedObj],{0,0,0.25});
		}
		if(keysHeld[SDLK_1])
		{
			camFocusedObj=2;
		}
		if(keysHeld[SDLK_0])
		{
			camFocusedObj=1;
		}
		if(keysHeld[SDLK_z])
		{
			std::cout <<"velx: " << allPhysicsObjects[camFocusedObj]->velocity.x << std::endl;
			std::cout <<"vely: " << allPhysicsObjects[camFocusedObj]->velocity.y << std::endl;
			std::cout <<"posx: " << allPhysicsObjects[camFocusedObj]->position.x << std::endl;
			std::cout <<"posy: " << allPhysicsObjects[camFocusedObj]->position.y << std::endl;
		}
		if(keysHeld[SDLK_x])
		{
			std::cout <<"2posx: " << camera.pos.x << std::endl;
			std::cout <<"2posy: " << camera.pos.y << std::endl;
		}
		if(keysHeld[SDLK_t])
		{
			isDebugRender = true;
		}
		if (keysHeld[SDLK_r])
		{
			isDebugRender = false;
		}
		if (keysHeld[SDLK_f])
		{
			allPhysicsObjects[camFocusedObj]->velocity = Vector2r{0,0,0};
		}
		if (keysHeld[SDLK_ESCAPE])
		{
			break;
		}
			
		

						






		GE_BlitSprite(bg,{0,0,0},{0,0});
		
		GE_TickPhysics(); //TODO: Seperate physics & renderer threads.
		render();
		#ifdef debug
				texttest->setText(std::to_string(allPhysicsObjects[camFocusedObj]->position.r).c_str());
				interface1_menu->giveEvent(event);
				if(isDebugRender) 
				{
					debug_render();
				}
				if (interface1_menu->isOpen)
				{
					interface1_menu->render();
				}
		#endif
					
		SDL_RenderPresent(myRenderer);
		SDL_Delay(16);
	}
	TTF_Quit();

	

	GE_FreeSprite(bg);
	GE_FreeSprite(nothingHere);
	GE_FreeSprite(somethingHere);
	GE_FreeSprite(shoddySpaceship);
	GE_FreeSprite(mySprite);
	GE_FreeSprite(otherSprite);

	return 0;
}
