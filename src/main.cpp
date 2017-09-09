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





#include "SDL.h"
#include "SDL_ttf.h"
#include <stdio.h>
#include <pthread.h>
#include <math.h>

#include <vector>
#include <functional>
#include <string>
#include <iostream>

//Local includes
#include "GeneralEngineCPP.h"
#include "vector2.h"
#include "camera.h"
#include "debugUI.h"
#include "sprite.h"
#include "physics.h"
#include "renderedObject.h"
#include "UI.h"
#include "gluePhysicsObject.h"
#include "gluePhysicsObjectInit.h"
#include "engine.h"

//Game-specific
#include "classes.h"

//tmp
#include "network.h"

//Config
#define BASE_DIR "../"


#define FONT_DIR BASE_DIR"fonts/"
#define SPRITE_DIR BASE_DIR"sprites/"

#define FREESANS_LOC FONT_DIR"FreeSans.ttf"
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
GE_Rectangle camerasGrid;
int camFocusedObj = 1;

void render()
{
	//printf("%d (differentiation) render\n",rand());
	//printf("~Trying lock render\n");
	pthread_mutex_lock(&RenderEngineMutex);
	//printf("~Lock render\n");
	

	GE_GlueRenderCallback(); //update all positions from the buffer

	camera.pos.x = (camera.pos.x+camerasGrid.w/2); //manipulate camera position now that it's updated
	camera.pos.y = (camera.pos.y+camerasGrid.h/2);
	#ifdef NO_CAMERA_ROTATE
		camera.pos.r = 0;
	#endif
	for (int i=0; i <= numRenderedObjects; i++)
	{
		if //TODO move to GE_BlitRenderedObject ? As of now, this does NOT ACCOUNT FOR ROTATION 
			/*(
				(renderedObjects[i]->position.x-renderedObjects[i]->grid.w <= camera.screenWidth*1.5) *1.5 is derrived from minusing half of camera.screenWidth to position.x. It is there because camera.pos.x is at the center and not at the top left, so minusing half the camera width accounts for this.
				&&(renderedObjects[i]->position.y-renderedObjects[i]->grid.h <= camera.screenHeight*1.5)
				&&(renderedObjects[i]->position.x+renderedObjects[i]->grid.w >= camera.pos.x-camera.screenWidth) //Same as the last thing, but because it's >= here instead of <= , it's the opposite of the action. 
				&&(renderedObjects[i]->position.y+renderedObjects[i]->grid.h >= camera.pos.y-camera.screenHeight)
			)*/
			(true)
		{
				
			if (!deadRenderedObjects[i])
			{
				GE_BlitRenderedObject(renderedObjects[i],&camera);
			}
			//else{printf("encounter dead render object\n");}
		}
		//else {printf("out of range\n");}
	}
	pthread_mutex_unlock(&RenderEngineMutex);
	//printf("Fin render\n");
}



// awful system that should probably be replaced but isn't that important so it probably never will
#define real
//#define regular
//#define game
//#define spritetest
//#define gluetest
//#define nettest

#ifdef real

int main(int argc, char* argv[])
{
	printf("I'm alive maybe?\n");
	int ttferror = TTF_Init();
	if (ttferror < 0) 
	{
		//TODO: Handle error...
		printf("TTF_Init error %d\n",ttferror);
		return ttferror;
	}
	//initialize some fonts we use
	TTF_Font* tinySans = TTF_OpenFont(FREESANS_LOC, 15);
	if(!tinySans) {
		printf("TTF_OpenFont: %s\n", TTF_GetError());
		return 1;
	}
	TTF_Font* bigSans =  TTF_OpenFont(FREESANS_LOC, 42);
	if(!bigSans) {
		printf("TTF_OpenFont: %s\n", TTF_GetError());
		return 1;
	}

	SDL_Window* myWindow;


	camera.pos = {0,0,0};
	camera.screenWidth = 1080;
	camera.screenHeight = 720;

	
	myWindow = SDL_CreateWindow("Spacegame", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, camera.screenWidth, camera.screenHeight, 0);

	myRenderer = SDL_CreateRenderer(myWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	//Initialize the engine
	
	pthread_mutex_lock(&PhysicsEngineMutex);
	int error = GE_Init(myRenderer);
	if (error != 0)
	{
		printf("Game engine initialization error: %d\n",error);
	}

	
	//Initialize stuff we need

	GE_LoadSpritesFromDir(myRenderer, SPRITE_DIR);


	//initialize the player
	Player* player = new Player(myRenderer);
	camFocusedObj = numFakePhysicsIDs;
	GE_LinkVectorToPhysicsObjectPosition(player,&(camera.pos));
	int focusGlueID = countGlueTargets;
	
	player->numGlueTargets++;
	player->glueTargets[player->numGlueTargets] = GE_addGlueSubject(&camerasGrid,&player->grid,GE_PULL_ON_PHYSICS_TICK,sizeof(GE_Rectangle));


//"locally scoped" definitions
#define HUD_SIZE_X 300
#define HUD_SIZE_Y 200
	GE_UI_Surface* myHUD = new GE_UI_Surface(myRenderer,{static_cast<double>(camera.screenWidth-HUD_SIZE_X),static_cast<double>(camera.screenHeight-HUD_SIZE_Y)},{HUD_SIZE_X,HUD_SIZE_Y},{0x00,0x33,0x00,255});

	int numHealthTexts = 0;
	GE_UI_Text* healthTexts[MAX_SUBSYSTEMS] = {};
	double healthAmmount[MAX_SUBSYSTEMS] = {};


	for (int i=0;i<player->numIterableSubsystems;i++)
	{
		GE_UI_Text* newText = new GE_UI_Text(myRenderer,{0,15*static_cast<double>(i)},{400,15},"Test Message Please Ignore", {0x66,0xFF,0x00,0xFF},tinySans);
		printf("add element %d\n",myHUD->addElement(newText));
		healthTexts[numHealthTexts] = newText;
		player->numGlueTargets++;
		player->glueTargets[player->numGlueTargets] = GE_addGlueSubject(&healthAmmount[numHealthTexts],&(player->iterableSubsystems[numHealthTexts]->health),GE_PULL_ON_PHYSICS_TICK,sizeof(double));
		numHealthTexts++;
	}


	GE_UI_Text* GameOver = new GE_UI_Text(myRenderer,{0,0},{400,80},"GAME OVER!",{0xFF,0x00,0x00,0xFF},bigSans);



	
	//spawn some enemys why not
	

	for (int i=0;i<20;i++)
	{	
		double randomx = rand() % 5000 + 1;
		double randomy = rand() % 5000 + 1;
		Enemie* un = new Enemie(myRenderer, {randomx-1500,randomy-1500,0},1);
	}



	
#ifdef PHYSICS_DEBUG_SLOWRENDERS
		GE_DEBUG_PassRendererToPhysicsEngine(myRenderer,&camera);
	#endif
	
	pthread_mutex_unlock(&PhysicsEngineMutex);



	char newStr[256] = {0};
	while (true)
	{
		//printf("Tryin to render here\n");
		//pthread_mutex_lock(&RenderEngineMutex);
		GE_BlitSprite(Sprites[GE_SpriteNameToID(SPRITE_DIR"color_black.bmp")],{0,0,0},{(double) camera.screenWidth,(double) camera.screenHeight},{0,0,25,25},GE_FLIP_NONE);		//TODO: Something less shitty
		//find the focus object's size
		

		render();

		


		for (int i=0;i<numHealthTexts;i++)
		{
			sprintf(newStr, "%s: %f%%",player->iterableSubsystems[i]->name.c_str(),healthAmmount[i]); //name: healthpercentage%

			healthTexts[i]->setText(newStr);
		}


		myHUD->render();

		if ((!player->GetIsOnline()) && ((int)floor(ticknum / 20.0) ) % 3) //Flash "Game over!" if the player is dead. Basically, this is 20x slower than flashing for 2 ticks on, 1 tick off.
		{
			GameOver->render({0,0});
		}


		#ifdef PHYSICS_DEBUG_SLOWRENDERS
		for (int i=0;i<numPhysicsTickPreCallbacks+1;i++)
		{
			C_PhysicsTickPreCallbacks[i]();
		}
		GE_TickPhysics();
		for (int i=0;i<numPhysicsTickDoneCallbacks+1;i++)
		{
			C_PhysicsTickDoneCallbacks[i]();
		}
		#endif



		SDL_RenderPresent(myRenderer); //Seems to be the VSyncer (expect ~16ms wait upon call)
	}

	TTF_CloseFont(tinySans);
	TTF_CloseFont(bigSans);

	SDL_DestroyWindow(myWindow);
	SDL_DestroyRenderer(myRenderer);
	delete player;
	delete myHUD;
	for (int i=0;i<numHealthTexts;i++)
	{
		delete healthTexts[i];
	}
	delete GameOver;

	GE_ShutdownPhysicsEngine();

	return 0;
}

#endif //real
#ifdef regular
int main(int argc, char* argv[])
{
	int ttferror = TTF_Init();
	if (ttferror < 0) 
	{
		//TODO: Handle error...
		printf("TTF_Init error %d\n",ttferror);
		return ttferror;
	}

	SDL_Window* myWindow;


	camera.pos = {0,0,0};
	camera.screenWidth = 1080;
	camera.screenHeight = 720;

	
	myWindow = SDL_CreateWindow("Spacegame", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, camera.screenWidth, camera.screenHeight, 0);

	myRenderer = SDL_CreateRenderer(myWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	//Initialize the engine
	int error = GE_Init(myRenderer);
	if (error != 0)
	{
		printf("Game engine initialization error: %d\n",error);
	}

	
	//Initialize stuff we need

	GE_LoadSpritesFromDir(myRenderer, SPRITE_DIR);
	
	GE_RenderedObject* ro = GE_CreateRenderedObject(myRenderer, SPRITE_DIR"enemy.png");
	ro->size = {50,50};
	ro->animation = {0,0,100,100};
	ro->position = {0,0,0};
	/*pthread_mutex_lock(&PhysicsEngineMutex);

	GE_PhysicsObject* po = GE_CreatePhysicsObject({25,0,0},{1,1,0},{25,25});


	


	GE_PhysicsObject* pot; 
	printf("idsys err %d\n",GE_GetPhysicsObjectFromID(po->ID,&pot)); //test of fakeID->ID system
	printf("id %d\n",pot->ID);
	pot->collisionRectangles[pot->numCollisionRectangles] = {0,0,25,25};
	pot->numCollisionRectangles++;

	pthread_mutex_unlock(&PhysicsEngineMutex);

	GE_addGlueSubject(&ro->position,pot->ID);	*/

	pthread_mutex_lock(&PhysicsEngineMutex);

	int tmpid = 0;
	GE_PhysicsObject* tmppo = 0;

	for (int i=0;i<20;i++)
	{
		GE_RenderedObject* ro = GE_CreateRenderedObject(myRenderer,SPRITE_DIR"simple.bmp");	
		ro->size = {25,25};
		ro->animation = {0,0,8,9};
		

		GE_PhysicsObject* me = GE_CreatePhysicsObject({20,static_cast<double>(i*50)+50,0},{0,0,0},{25,25});
		me->collisionRectangles[me->numCollisionRectangles] = {0,0,25,25};
		me->numCollisionRectangles++;
		GE_addGlueSubject(&(ro->position),me->ID);
		camFocusedObj = me->ID;

		//TODO tem,p
		tmpid = countGlueTargets;

		tmppo = me; 



	}
	GE_addGlueSubject(&(camera.pos),tmppo->ID);

	pthread_mutex_unlock(&PhysicsEngineMutex);

	//GE_CreateRenderedObject(myRenderer,SPRITE_DIR"shottyspaceship.bmp");	
	//TODO camFocusedObj = renderedObjects[numRenderedObjects]->physicsObject->ID;

	//TODO me = renderedObjects[numRenderedObjects]->physicsObject->ID;
	//physicsObjects[me]->collisionRectangles[physicsObjects[me]->numCollisionRectangles] = {0,0,25,25};
	//physicsObjects[me]->numCollisionRectangles++;

	



	
	
	#ifdef debug
		debugWindow* interface1_menu = new debugWindow(myRenderer,{100,100},{320,240});

		debugText* texttest = new debugText(myRenderer,{25,25},"texttext");
		interface1_menu->addElement(texttest);

		debugButton* cubeRenderButton = new debugButton(myRenderer,{0,50},{100,50},"cubeRenderer");

		auto callback = [&] () { isDebugRender = !isDebugRender; };
		cubeRenderButton->C_Pressed = callback;
		interface1_menu->addElement(cubeRenderButton);	
		
		debugButton* spawnButton = new debugButton(myRenderer,{175,50},{100,50},"spawn phys. obj.");



		auto callback2 = [&] () {  //TODO: fix the countless things wrong with this
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

	#ifdef PHYSICS_DEBUG_SLOWRENDERS
		GE_DEBUG_PassRendererToPhysicsEngine(myRenderer,&camera);
	#endif

	GE_UI_TextInput* myTextIn = new GE_UI_TextInput(myRenderer,{0,0},{250,25},SDL_Color{0,0,0},SDL_Color{255,255,255});




	bool keysHeld[323] = {false}; 
	bool isDebugRender = false;
	SDL_Event event;
	while (true)
	{
		while (SDL_PollEvent(&event) != 0)
		{
			if(!myTextIn->isFocused)
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
			//shitty method of doing this. all of this is fairly shitty.
			GE_PhysicsObject* cObj;
			GE_GetPhysicsObjectFromID(camFocusedObj,&cObj); //TODO: Error handling

			if(keysHeld[SDLK_w])
			{
				//GE_AddRelativeVelocity(cObj,{0,-0.5,0});
				GE_glueAddVelocity(tmpid,{0,-0.5,0},GE_ADD_TYPE_RELATIVE);
			}
			if(keysHeld[SDLK_s])
			{
				GE_AddRelativeVelocity(cObj,{0,0.5,0});
			}
			if(keysHeld[SDLK_d])
			{
				GE_AddRelativeVelocity(cObj,{0.5,0,0});
			}
			if(keysHeld[SDLK_a])
			{
				GE_AddRelativeVelocity(cObj,{-0.5,0,0});
			}
			if(keysHeld[SDLK_q])
			{
				GE_AddRelativeVelocity(cObj,{0,0,-0.25});
			}
			if(keysHeld[SDLK_e])
			{
				GE_AddRelativeVelocity(cObj,{0,0,0.25});
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
				std::cout <<"velx: " << cObj->velocity.x << std::endl;
				std::cout <<"vely: " << cObj->velocity.y << std::endl;
				std::cout <<"posx: " << cObj->position.x << std::endl;
				std::cout <<"posy: " << cObj->position.y << std::endl;
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
				cObj->velocity = Vector2r{0,0,0};
			}
			if (keysHeld[SDLK_ESCAPE])
			{
				break;
			}
			printf("Give event\n");
			myTextIn->giveEvent({0,0},event);
		}
				
			

							





		
		//pthread_mutex_lock(&RenderEngineMutex);
		
		GE_BlitSprite(Sprites[GE_SpriteNameToID(SPRITE_DIR"color_black.bmp")],{0,0,0},{(double) camera.screenWidth,(double) camera.screenHeight},{0,0,25,25},GE_FLIP_NONE);		//TODO: Something less shitty
		render();
		#ifdef PHYSICS_DEBUG_SLOWRENDERS
		for (int i=0;i<numPhysicsTickPreCallbacks+1;i++)
		{
			C_PhysicsTickPreCallbacks[i]();
		}
		GE_TickPhysics();
		for (int i=0;i<numPhysicsTickDoneCallbacks+1;i++)
		{
			C_PhysicsTickDoneCallbacks[i]();
		}
		#endif
		
		#ifdef debug

				texttest->setText(std::to_string(cObj->position.r).c_str());
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

		//pthread_mutex_unlock(&RenderEngineMutex);
		//SDL_Delay(16);
		//SDL_Delay(500);
	}
	TTF_Quit();

	
	delete myTextIn;


	SDL_DestroyRenderer(myRenderer);

	return 0;
}
#endif //regular
#ifdef game
int main(int argc, char* argv[])
{
	int ttferror = TTF_Init();
	if (ttferror < 0) 
	{
		//TODO: Handle error...
		printf("TTF_Init error %d\n",ttferror);
		return ttferror;
	}

	SDL_Window* myWindow;


	camera.pos = {0,0,0};
	camera.screenWidth = 1080;
	camera.screenHeight = 720;

	
	myWindow = SDL_CreateWindow("Spacegame", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, camera.screenWidth, camera.screenHeight, 0);

	myRenderer = SDL_CreateRenderer(myWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	//Initialize the engine
	int error = GE_Init(myRenderer);
	if (error != 0)
	{
		printf("Game engine initialization error: %d\n",error);
	}

	
	//Initialize stuff we need

	GE_LoadSpritesFromDir(myRenderer, SPRITE_DIR);
	
	GE_RenderedObject* ro = GE_CreateRenderedObject(myRenderer, SPRITE_DIR"enemy.png");
	ro->size = {50,50};
	ro->animation = {0,0,100,100};
	ro->position = {0,0,0};
	/*pthread_mutex_lock(&PhysicsEngineMutex);

	GE_PhysicsObject* po = GE_CreatePhysicsObject({25,0,0},{1,1,0},{25,25});


	


	GE_PhysicsObject* pot; 
	printf("idsys err %d\n",GE_GetPhysicsObjectFromID(po->ID,&pot)); //test of fakeID->ID system
	printf("id %d\n",pot->ID);
	pot->collisionRectangles[pot->numCollisionRectangles] = {0,0,25,25};
	pot->numCollisionRectangles++;

	pthread_mutex_unlock(&PhysicsEngineMutex);

	GE_addGlueSubject(&ro->position,pot->ID);	*/

	pthread_mutex_lock(&PhysicsEngineMutex);

	GE_RenderedObject* ro2 = GE_CreateRenderedObject(myRenderer,SPRITE_DIR"player.png");	
	ro2->size = {50,50};
	ro2->animation = {0,0,100,100};
	

	GE_PhysicsObject* me = GE_CreatePhysicsObject({1000,980,0},{0,0,0},{25,25});
	me->collisionRectangles[me->numCollisionRectangles] = {-1,0,25,25};
					me->collisionRectangles[me->numCollisionRectangles] = {17.5,17.5,17.5,1.75};
	me->numCollisionRectangles++;
	GE_addGlueSubject(&(ro2->position),me->ID);
	camFocusedObj = me->ID;
	{
		GE_RenderedObject* ro2 = GE_CreateRenderedObject(myRenderer,SPRITE_DIR"bullet.png");	
		ro2->size = {50,50};
		ro2->animation = {0,0,100,100};
		

		GE_PhysicsObject* me = GE_CreatePhysicsObject({950,1000,0},{0,0,0},{50,50});
		me->collisionRectangles[me->numCollisionRectangles] = {-1,0,25,25};
		me->numCollisionRectangles++;
		GE_addGlueSubject(&(ro2->position),me->ID);

	}
	{
		GE_RenderedObject* ro2 = GE_CreateRenderedObject(myRenderer,SPRITE_DIR"player.png");	
		ro2->size = {50,50};
		ro2->animation = {0,0,100,100};
		

		GE_PhysicsObject* me = GE_CreatePhysicsObject({1000,1000,0},{0,0,0},{25,25});
		me->collisionRectangles[me->numCollisionRectangles] = {-1,0,25,25};
						me->collisionRectangles[me->numCollisionRectangles] = {0,0,25,25};
		me->numCollisionRectangles++;
		GE_addGlueSubject(&(ro2->position),me->ID);

	}

	pthread_mutex_unlock(&PhysicsEngineMutex);

	//GE_CreateRenderedObject(myRenderer,SPRITE_DIR"shottyspaceship.bmp");	
	//TODO camFocusedObj = renderedObjects[numRenderedObjects]->physicsObject->ID;

	//TODO me = renderedObjects[numRenderedObjects]->physicsObject->ID;
	//physicsObjects[me]->collisionRectangles[physicsObjects[me]->numCollisionRectangles] = {0,0,25,25};
	//physicsObjects[me]->numCollisionRectangles++;

	



	
	
	#ifdef debug
		debugWindow* interface1_menu = new debugWindow(myRenderer,{100,100},{320,240});

		debugText* texttest = new debugText(myRenderer,{25,25},"texttext");
		interface1_menu->addElement(texttest);

		debugButton* cubeRenderButton = new debugButton(myRenderer,{0,50},{100,50},"cubeRenderer");

		auto callback = [&] () { isDebugRender = !isDebugRender; };
		cubeRenderButton->C_Pressed = callback;
		interface1_menu->addElement(cubeRenderButton);	
		
		debugButton* spawnButton = new debugButton(myRenderer,{175,50},{100,50},"spawn phys. obj.");



		auto callback2 = [&] () {  //TODO: fix the countless things wrong with this
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

	#ifdef PHYSICS_DEBUG_SLOWRENDERS
		GE_DEBUG_PassRendererToPhysicsEngine(myRenderer,&camera);
	#endif

	GE_UI_TextInput* myTextIn = new GE_UI_TextInput(myRenderer,{0,0},{50,50},SDL_Color{255,255,255},SDL_Color{0,0,0});


	bool keysHeld[323] = {false}; 
	bool isDebugRender = false;
	SDL_Event event;
	while (true)
	{
		//!!!!
		//TODO: PULL EVENT IN A LOOP
		//!!!!
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
				if (event.key.keysym.sym == SDLK_SPACE)
				{
					//insert a bullet
					pthread_mutex_lock(&PhysicsEngineMutex);

					GE_RenderedObject* ro2 = GE_CreateRenderedObject(myRenderer,SPRITE_DIR"bullet.png");	
					ro2->size = {50,50};
					ro2->animation = {0,0,100,100};
					

					//get our focused object
					
					GE_PhysicsObject* fcsObj;
					GE_GetPhysicsObjectFromID(camFocusedObj,&fcsObj);

					Vector2r addToPosition = {0,-50,0};
					Vector2r addToVelocity = {0,-0,0};
					

					GE_Vector2RotationCCW(&addToPosition,fcsObj->position.r);
					GE_Vector2RotationCCW(&addToVelocity,fcsObj->position.r);



					GE_PhysicsObject* me = GE_CreatePhysicsObject((addToPosition+Vector2r{fcsObj->position.x,fcsObj->position.y,fcsObj->position.r}),addToVelocity,{75,75});
					me->collisionRectangles[me->numCollisionRectangles] = {17.5,0,17.5,50};
					me->numCollisionRectangles++;
					GE_addGlueSubject(&(ro2->position),me->ID);

					


					///////////////////
					//
					GE_AddPhysicsObjectCollisionCallback(me,killObject,true);

					pthread_mutex_unlock(&PhysicsEngineMutex);

				}
			}
		}
		//shitty method of doing this. all of this is fairly shitty.
		GE_PhysicsObject* cObj;
		GE_GetPhysicsObjectFromID(camFocusedObj,&cObj); //TODO: Error handling

		if(keysHeld[SDLK_w])
		{
			GE_AddRelativeVelocity(cObj,{0,-0.5,0});
		}
		if(keysHeld[SDLK_s])
		{
			GE_AddRelativeVelocity(cObj,{0,0.5,0});
		}
		if(keysHeld[SDLK_d])
		{
			GE_AddRelativeVelocity(cObj,{0.5,0,0});
		}
		if(keysHeld[SDLK_a])
		{
			GE_AddRelativeVelocity(cObj,{-0.5,0,0});
		}
		if(keysHeld[SDLK_q])
		{
			GE_AddRelativeVelocity(cObj,{0,0,-0.25});
		}
		if(keysHeld[SDLK_e])
		{
			GE_AddRelativeVelocity(cObj,{0,0,0.25});
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
			std::cout <<"velx: " << cObj->velocity.x << std::endl;
			std::cout <<"vely: " << cObj->velocity.y << std::endl;
			std::cout <<"posx: " << cObj->position.x << std::endl;
			std::cout <<"posy: " << cObj->position.y << std::endl;
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
			cObj->velocity = Vector2r{0,0,0};
		}
		if (keysHeld[SDLK_ESCAPE])
		{
			break;
		}
		myTextIn->giveEvent({0,0},event);
			
		

						






		
		pthread_mutex_lock(&RenderEngineMutex);

		GE_BlitSprite(Sprites[GE_SpriteNameToID(SPRITE_DIR"color_black.bmp")],{0,0,0},{(double) camera.screenWidth,(double) camera.screenHeight},{0,0,25,25},GE_FLIP_NONE);		//TODO: Something less shitty
		render();

				
		
		

		#ifdef debug

				texttest->setText(std::to_string(cObj->position.r).c_str());
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


		myTextIn->render();
					
		SDL_RenderPresent(myRenderer);
		pthread_mutex_unlock(&RenderEngineMutex);
		
		
		
		#ifdef PHYSICS_DEBUG_SLOWRENDERS
		pthread_mutex_lock(&PhysicsEngineMutex);
		GE_TickPhysics();
		SDL_RenderPresent(myRenderer);
		pthread_mutex_unlock(&PhysicsEngineMutex);
		#endif

		//SDL_Delay(16);
		//SDL_Delay(500);
	}
	TTF_Quit();

	
	delete myTextIn;


	SDL_DestroyRenderer(myRenderer);

	return 0;
}

#endif
#ifdef spritetest

int main(int argc, char* argv[])
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
			GE_BlitSprite(Sprites[spriteID],{0,0,0},{25,25},{t*8,0,8,9}, GE_FLIP_NONE);
			SDL_RenderPresent(myRenderer);
			SDL_Delay(100);
	}

	return 0;
}

#endif //spritetest
#ifdef gluetest
int main(int argc, char* argv[])
{


	int ttferror = TTF_Init();
	if (ttferror < 0) 
	{
		//TODO: Handle error...
		printf("TTF_Init error %d\n",ttferror);
		return ttferror;
	}

	SDL_Window* myWindow;


	camera.pos = {0,0,0};
	camera.screenWidth = 1080;
	camera.screenHeight = 720;

	
	myWindow = SDL_CreateWindow("Spacegame", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, camera.screenWidth, camera.screenHeight, 0);

	myRenderer = SDL_CreateRenderer(myWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	//Initialize the engine
	int error = GE_Init(myRenderer);
	if (error != 0)
	{
		printf("Game engine initialization error: %d\n",error);
	}

	
	//Initialize stuff we need

	GE_LoadSpritesFromDir(myRenderer, SPRITE_DIR);

	printf("alsfd\n");
	
	GE_RenderedObject* ro = GE_CreateRenderedObject(myRenderer, SPRITE_DIR"simple.bmp");
	ro->size = {25,25};
	ro->animation = {0,0,8,9};
	ro->position = {0,0,0};

	printf("mutex lock\n");
	pthread_mutex_lock(&PhysicsEngineMutex);

	GE_PhysicsObject* po = GE_CreatePhysicsObject({25,0,0},{1,1,0},{25,25});


	


	GE_PhysicsObject* pot; 
	printf("idsys err %d\n",GE_GetPhysicsObjectFromID(po->ID,&pot)); //test of fakeID->ID system
	printf("id %d\n",pot->ID);
	pot->collisionRectangles[pot->numCollisionRectangles] = {0,0,25,25};
	pot->numCollisionRectangles++;

	pthread_mutex_unlock(&PhysicsEngineMutex);
	printf("mtx unlock\n");

	GE_addGlueSubject(&ro->position,pot->ID);	

	

	SDL_Event event;
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
