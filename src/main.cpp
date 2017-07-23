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
#include <pthread.h>
#include <math.h>

#include <vector>
#include <functional>
#include <string>
#include <iostream>

//Local includes
#include "vector2.h"
#include "camera.h"
#include "debugUI.h"
#include "sprite.h"
#include "physics.h"
#include "renderedObject.h"
#include "UI.h"
#include "gluePhysicsObject.h"

//tmp
#include "network.h"

#define SPRITE_DIR "../sprites/"

//#define NO_CAMERA_ROTATE true


//Definitions


//#define debug //wheather debug draws, menus, etc. is included. 




/*LONG-TERM TODO

*Lagged camera? ("chasecam") 
*Standard implementation for rotation matrix instead of spamming it everywhere?

*Possible "engine demo game" would be an asteroid field with constant collisions. Would demo the effeciency of the physics engine very well.


*/








SDL_Renderer* myRenderer;

Camera camera;
PhysicsObject* camFocusedObj = 1;

void render()
{
	pthread_mutex_lock(&RenderEngineMutex);
	camera.pos = physicsObjects[camFocusedObj]->position;
	#ifdef NO_CAMERA_ROTATE
		camera.pos.r = 0;
	#endif
	for (int i=0; i <= numRenderedObjects; i++)
	{
		GE_BlitRenderedObject(renderedObjects[i],&camera);
	}
	pthread_mutex_unlock(&RenderEngineMutex);
}


//#define regular
//#define spritetest
#define gluetest
//#define nettest

#ifdef regular
int main()
{
	if (TTF_Init() < 0) 
	{
		//TODO: Handle error...
		std::cout << "!!!TFT_Init Error!" << std::endl;
		return 0;
	}

	SDL_Window* myWindow;
	SDL_Event event;


	camera.pos = {200,200,0};
	camera.screenWidth = 1080;
	camera.screenHeight = 720;

	
	myWindow = SDL_CreateWindow("Spacegame", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, camera.screenWidth, camera.screenHeight, 0);

	myRenderer = SDL_CreateRenderer(myWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);


	GE_LoadSpritesFromDir(myRenderer, SPRITE_DIR);


		
	int me;
	

	for (int i=0;i<20;i++)
	{
		GE_CreateRenderedObject(myRenderer,SPRITE_DIR"simple.bmp");	
		

		PhysicsObject* me = GE_CreatePhysicsObject({0,0,0},{0,0,0},{25,25});
		me->collisionRectangles[physicsObjects[me]->numCollisionRectangles] = {0,0,25,25};
		me->numCollisionRectangles++;
		GE_addGlueSubject(&renderedObjects[numRenderedObjects].position,me->ID);

	}

	//GE_CreateRenderedObject(myRenderer,SPRITE_DIR"shottyspaceship.bmp");	
	//TODO camFocusedObj = renderedObjects[numRenderedObjects]->physicsObject->ID;

	//TODO me = renderedObjects[numRenderedObjects]->physicsObject->ID;
	//physicsObjects[me]->collisionRectangles[physicsObjects[me]->numCollisionRectangles] = {0,0,25,25};
	//physicsObjects[me]->numCollisionRectangles++;

	



	
	
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
			Vector2r pos = physicsObjects[camFocusedObj]->position;
			for (int i=0;i < 20;i++)
			{
				GE_CreateRenderedObject(myRenderer,otherSprite,{pos.x,pos.y,0},{(double)(rand() % 11)/100,(double)(rand() % 11)/100,0},{25,25});	

				//renderedObjects[numRenderedObjects]->myPhysicsObject->setVelocity({(double)(rand() % 101)/100,(double)(rand() % 101)/100}); //random between 0-1 w/ 2 decimals

				int me = renderedObjects[numRenderedObjects]->physicsObject->ID;
				physicsObjects[me]->collisionRectangles[physicsObjects[me]->numCollisionRectangles] = {0,0,25,25};
				physicsObjects[me]->numCollisionRectangles++;
			}
		};
		spawnButton->C_Pressed = callback2;
		interface1_menu->addElement(spawnButton);


		

	#endif

	#ifdef physics_debug
		GE_DEBUG_PassRendererToPhysicsEngine(myRenderer,&camera);
	#endif

	GE_UI_TextInput* myTextIn = new GE_UI_TextInput(myRenderer,{0,0},{50,50},SDL_Color{255,255,255},SDL_Color{0,0,0});

	while (true)
	{
		if(SDL_PollEvent(&event) && (!myTextIn->isFocused))
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
			GE_AddRelativeVelocity(physicsObjects[camFocusedObj],{0,-0.5,0});
		}
		if(keysHeld[SDLK_s])
		{
			GE_AddRelativeVelocity(physicsObjects[camFocusedObj],{0,0.5,0});
		}
		if(keysHeld[SDLK_d])
		{
			GE_AddRelativeVelocity(physicsObjects[camFocusedObj],{0.5,0,0});
		}
		if(keysHeld[SDLK_a])
		{
			GE_AddRelativeVelocity(physicsObjects[camFocusedObj],{-0.5,0,0});
		}
		if(keysHeld[SDLK_q])
		{
			GE_AddRelativeVelocity(physicsObjects[camFocusedObj],{0,0,-0.25});
		}
		if(keysHeld[SDLK_e])
		{
			GE_AddRelativeVelocity(physicsObjects[camFocusedObj],{0,0,0.25});
		}
		if(keysHeld[SDLK_1])
		{
			camFocusedObj=15;
		}
		if(keysHeld[SDLK_0])
		{
			camFocusedObj=1;
		}
		if(keysHeld[SDLK_z])
		{
			std::cout <<"velx: " << physicsObjects[camFocusedObj]->velocity.x << std::endl;
			std::cout <<"vely: " << physicsObjects[camFocusedObj]->velocity.y << std::endl;
			std::cout <<"posx: " << physicsObjects[camFocusedObj]->position.x << std::endl;
			std::cout <<"posy: " << physicsObjects[camFocusedObj]->position.y << std::endl;
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
			physicsObjects[camFocusedObj]->velocity = Vector2r{0,0,0};
		}
		if (keysHeld[SDLK_ESCAPE])
		{
			break;
		}
		myTextIn->giveEvent({0,0},event);
			
		

						






		// GE_BlitSprite(bg,{0,0,0},{0,0}); TODO: update
		
		#ifndef physics_debug
			GE_TickPhysics(); //TODO: Seperate physics & renderer threads.
		#endif
		
		render();
		
		#ifdef physics_debug
			GE_TickPhysics();
		#endif

		#ifdef debug

				texttest->setText(std::to_string(physicsObjects[camFocusedObj]->position.r).c_str());
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

		//#define shittyunittest true
		#ifdef shittyunittest
			Vector2r myTest = {1,2,3};

			std::cout << myTest.x << " y " << myTest.y << " r " << myTest.r  << std::endl;

			myTest = myTest+Vector2r{1,1,1};
			
			std::cout << myTest.x << " y " << myTest.y << " r " << myTest.r  << std::endl;

		#endif

		myTextIn->render();
					
		SDL_RenderPresent(myRenderer);
		//SDL_Delay(16);
		//SDL_Delay(500);
	}
	TTF_Quit();

	
	delete myTextIn;

	//needed by shitty debug code
	GE_FreeSprite(nothingHere);
	GE_FreeSprite(somethingHere);
	//end needed


	SDL_DestroyRenderer(myRenderer);

	return 0;
}
#endif //regular
#ifdef spritetest

int main()
{
	if (TTF_Init() < 0) 
	{
		//TODO: Handle error...
		std::cout << "!!!TFT_Init Error!" << std::endl;
		return 0;
	}

	SDL_Window* myWindow;
	SDL_Event event;


	camera.pos = {200,200,0};
	camera.screenWidth = 1080;
	camera.screenHeight = 720;

	
	myWindow = SDL_CreateWindow("Spacegame", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, camera.screenWidth, camera.screenHeight, 0);

	myRenderer = SDL_CreateRenderer(myWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	GE_LoadSpritesFromDir(myRenderer, SPRITE_DIR);

	int spriteID;
	
	spriteID = GE_SpriteNameToID(SPRITE_DIR"simple-animation.bmp");
	int t = 0;
	while (true)
	{
			t++;
			if (t==2)
				t= 0;
			GE_BlitSprite(Sprites[spriteID],{0,0,0},{25,25},{t*8,0,8,9}, FLIP_NONE);
			SDL_RenderPresent(myRenderer);
			SDL_Delay(100);
	}

	return 0;
}

#endif //spritetest
#ifdef gluetest
int main()
{
	if (TTF_Init() < 0) 
	{
		//TODO: Handle error...
		std::cout << "!!!TFT_Init Error!" << std::endl;
		return 0;
	}

	SDL_Window* myWindow;
	SDL_Event event;


	camera.pos = {0,0,0};
	camera.screenWidth = 1080;
	camera.screenHeight = 720;

	
	myWindow = SDL_CreateWindow("Spacegame", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, camera.screenWidth, camera.screenHeight, 0);

	myRenderer = SDL_CreateRenderer(myWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	GE_LoadSpritesFromDir(myRenderer, SPRITE_DIR);
	
	RenderedObject* ro = GE_CreateRenderedObject(myRenderer, SPRITE_DIR"simple.bmp");
	ro->size = {25,25};
	ro->animation = {0,0,8,9};
	ro->position = {0,0,0};

	PhysicsObject* po = GE_CreatePhysicsObject({25,0,0},{1,1,0},{25,25});



	PhysicsObject* pot; 
	printf("idsys err %d\n",GE_GetPhysicsObjectFromID(po->ID,&pot)); //test of fakeID->ID system
	printf("id %d\n",pot->ID);
	pot->collisionRectangles[pot->numCollisionRectangles] = {0,0,25,25};
	pot->numCollisionRectangles++;
	GE_addGlueSubject(&ro->position,pot->ID);

	
	GE_PhysicsInit();
	GE_GlueInit();


	

	while (true)
	{
			GE_BlitRenderedObject(ro,&camera);
			SDL_RenderPresent(myRenderer);
			printf("am not kill \n");
	}
	GE_FreeAllSprites();
	GE_FreePhysicsObject(po);
	GE_FreeRenderedObject(ro);

	return 0;
}

#endif //gluetest
#ifdef nettest


int main(int argc, char *argv[]) 
{
	
	GE_NetworkSocket* mySocket = GE_CreateNetworkSocket();

	GE_FillNetworkSocket(mySocket,5668);
	if (argc == 2)
	{
		printf("Am client\n");
		printf(" cs %d\n",GE_ConnectServer(mySocket,"127.0.0.1"));

		GE_Write(mySocket, "hi",3);
		char buff[255] = "";

		GE_Read(mySocket, buff,sizeof(buff));

		printf("Response: %s\n",buff);
		
	}
	else
	{
		printf("Am server\n");
		GE_BindServer(mySocket);
		GE_NetworkSocket* theirSocket = GE_CreateNetworkSocket();
		printf(" cc %d\n",GE_ConnectClient(theirSocket,mySocket));

		char buff[255] = "";

		GE_Read(theirSocket, buff,sizeof(buff));
		printf("Response: %s\n",buff);
		
		GE_Write(theirSocket, "bye",4);

		GE_FreeNetworkSocket(theirSocket);

	}

	GE_FreeNetworkSocket(mySocket);

}

#endif
