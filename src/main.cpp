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




#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
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
#include "sprite.h"
#include "physics.h"
#include "renderedObject.h"
#include "UI.h"
#include "minimap.h"
#include "gluePhysicsObject.h"
#include "gluePhysicsObjectInit.h"
#include "engine.h"
#include "stars.h"
#include "shapes.h"
#include "gameRender.h"

#ifdef GE_DEBUG
#include "debugRenders.h"
#endif

//Game-specific
#include "classes.h"

//tmp
#include "network.h"

//Config
#define BASE_DIR "../"


#define FONT_DIR BASE_DIR"fonts/"
#define SPRITE_DIR BASE_DIR"sprites/"

#define FREESANS_LOC FONT_DIR"FreeSans.ttf"


#ifdef PHYSICS_DEBUG_SLOWRENDERS
#define NO_CAMERA_ROTATE true
#endif






#include "inventory.h"


//#define debug //wheather debug draws, menus, etc. is included. 




/*LONG-TERM TODO

*Lagged camera? ("chasecam") 
*Standard implementation for rotation matrix instead of spamming it everywhere?

*Possible "engine demo game" would be an asteroid field with constant collisions. Would demo the effeciency of the physics engine very well.


*/











// awful system that should probably be replaced but isn't that important so it probably never will
#define real
//#define spritetest
//#define gluetest
//#define physicstest
//#define leveleditor
//#define nettest

#ifdef real


Player* player;

SDL_Renderer* renderer;
Camera camera;
GE_Rectangle camerasGrid;



TTF_Font* tinySans;
TTF_Font* bigSans;
TTF_Font* titleSans;

GE_UI_FontStyle fstyle = {{0x00,0x00,0x00,0xff},titleSans};
GE_UI_WindowTitleStyle windowTitleStyle = {fstyle,0,{GE_Color{0xff,0x00,0x00,0xff},GE_Color{0x66,0x66,0x66,0xff},{0xff,0xff,0xff,0xff},fstyle.color,{15,7},tinySans},2,GE_Color{0x66,0xff,0x33,0xff},25,true};
GE_UI_WindowStyle windowStyle = {windowTitleStyle, GE_Color{0x12,0x1f,0x1d,255},1,GE_Color{0x66,0xff,0x33,0xff}};
GE_UI_Style style = GE_UI_Style{fstyle,{GE_Color{0x00,0x33,0x00,0xff},tinySans},windowStyle};
GE_Color gameBackgroundColor = {0x00,0x00,0x00,255};




class MyOmniEventReciever : public GE_UI_OmniEventReciever
{
	public:
		MyOmniEventReciever(){}
		~MyOmniEventReciever(){}
		void giveEvent(SDL_Event event)
		{
			if (event.type == SDL_KEYUP)
			{
				if (event.key.keysym.sym == SDLK_t)
				{
					GE_UI_FontStyle fstyle = {{0x00,0x00,0x00,0xff},titleSans};
					GE_UI_WindowTitleStyle windowTitleStyle = {fstyle,0,{GE_Color{0xff,0x00,0x00,0xff},GE_Color{0x66,0x66,0x66,0xff},{0xff,0xff,0xff,0xff},fstyle.color,{15,7},titleSans},2,GE_Color{0x66,0xff,0x33,0xff},25,true};
					GE_UI_WindowStyle windowStyle = {windowTitleStyle, GE_Color{0x00,0x00,0xff,0xff},2,GE_Color{0x66,0xff,0x33,0xff}};
					GE_UI_Style style = GE_UI_Style{fstyle,{GE_Color{0x00,0x33,0x00,0xff},titleSans},windowStyle};


					GE_UI_Window* window = new GE_UI_Window(renderer,"INVENTORY",{250,250},{618,320},style);


					UI_InventoryView* inv = new UI_InventoryView(renderer, {0,0},{309,250},player->inventory,fstyle,{8,8},GE_Color{0xff,0xff,0xff,0x33},GE_Color{0x00,0x33,0x00,255},GE_Color{0xff,0x00,0x00,0xff});
					window->surface->addElement(inv);

					GE_UI_InsertTopLevelElement(window);

				}
			}



		}
	
};
class UI_HealthView : public GE_UI_Element
{
	public:

		UI_HealthView(SDL_Renderer* renderer,Vector2 position, Vector2 size, Camera* camera)
		{
			mySurface = new GE_UI_Surface(renderer,position,size,{0x00,0x33,0x00,255});

			numHealthTexts = 0;


			for (int i=0;i<player->numIterableSubsystems;i++)
			{
				GE_UI_Text* newText = new GE_UI_Text(renderer,{size.x/2,15*static_cast<double>(i)},{0,0},"This message should've been updated.", {0x66,0xFF,0x00,0xFF},tinySans);
				newText->centerX();
				newText->expandToTextSize();
				printf("add element %d\n",mySurface->addElement(newText));
				healthTexts[numHealthTexts] = newText;
				GE_LinkGlueToPhysicsObject(player,GE_addGlueSubject(&healthAmmount[numHealthTexts],&(player->iterableSubsystems[numHealthTexts]->health),GE_PULL_ON_PHYSICS_TICK,sizeof(double)) );
				numHealthTexts++;
			}
			speedText = new GE_UI_Text(renderer,{size.x/2,15*static_cast<double>(numHealthTexts+1)},{0,0},"This message should've been updated.", {0x66,0xFF,0x00,0xFF},tinySans);

			speedText->centerX();
			speedText->expandToTextSize();
			mySurface->addElement(speedText);

			playerSpeed = {0,0,0};
			GE_LinkVectorToPhysicsObjectVelocity(player,&playerSpeed);

			this->wantsEvents = false;
		}

		~UI_HealthView()
		{
			delete mySurface; //removes elements added
		}
		void render(Vector2 parrentPosition)
		{
			for (int i=0;i<numHealthTexts;i++)
			{
				sprintf(newStr, "%s: %.2f%%",player->iterableSubsystems[i]->name.c_str(),healthAmmount[i]); //name: healthpercentage%

				healthTexts[i]->setText(newStr);
			}
			sprintf (newStr, "Speed: %.2f km/s",(abs(playerSpeed.x)+abs(playerSpeed.y))*0.16667); //TODO this will need to be changed with physics timescales
			speedText->setText(newStr);


			mySurface->render({0,0});

		}
	private:
		int numHealthTexts;
		GE_UI_Surface* mySurface;
		GE_UI_Text* healthTexts[MAX_SUBSYSTEMS];
		double healthAmmount[MAX_SUBSYSTEMS];
		GE_UI_Text* speedText;
		Vector2r playerSpeed;

		char newStr[256] = {0}; //working space
};



class UI_WorldView : public GE_UI_Element //Includes stars and rendered objects but not HUD objects
{
	public:
		UI_WorldView(SDL_Renderer* renderer, Vector2 position, Vector2 size, Camera* camera)
		{
			this->camera = camera;

			//create a surface to put everything on
			mySurface = new GE_UI_Surface(renderer,position,size,gameBackgroundColor);

			//stars should be background elements so add them first
#define additionalStars 3
			double maxScreenSize = std::max(camera->screenWidth,camera->screenHeight)*additionalStars;
			std::vector<GE_Color> starColors = {{0xff,0xff,0xff,0xff},{0xfb,0xf3,0xf9,0xff},{0xba,0xd8,0xfc,0xff}};
			std::vector<int> starSizes = {2,2,2,2,2,2,1,1,3};

			mySurface->addElement( new GE_Stars(renderer, 550*additionalStars, maxScreenSize,maxScreenSize,{2,1,1,1},(0.833333333)/10,starColors,camera) );
			mySurface->addElement( new GE_Stars(renderer, 300*additionalStars, maxScreenSize,maxScreenSize,starSizes,0.833333333/5, starColors,camera) );
			mySurface->addElement( new GE_Stars(renderer, 100*additionalStars, maxScreenSize,maxScreenSize,starSizes,0.833333333/3,starColors,camera) ); 
			mySurface->addElement( new GE_Stars(renderer, 50*additionalStars, maxScreenSize,maxScreenSize,starSizes,0.833333333/2,starColors,camera) ); 
			mySurface->addElement( new GE_Stars(renderer, 20*additionalStars, maxScreenSize,maxScreenSize,starSizes,1.8, starColors,camera ) );


			//now add the game object renderer
			
			mySurface->addElement(new GE_UI_GameRender(renderer, position,size,camera));
		

			this->wantsEvents = true;
		}
		~UI_WorldView()
		{
			delete mySurface; //the surface will automatically free any elements added to it
		}
		void render(Vector2 parrentPosition)
		{
			mySurface->render(parrentPosition);	
		}
		void giveEvent(Vector2 parrentPosition, SDL_Event event)
		{
			player->threadedEventStack.giveEvent(event);
		}
	private:
		Camera* camera;
		GE_UI_Surface* mySurface;
};
class UI_GameView : public GE_UI_TopLevelElement //Includes a UI_WorldView and HUD elements
{
	public:
		UI_GameView(SDL_Renderer* renderer,Vector2 position,Vector2 size,Camera* camera)
		{
				this->position = position;
				this->size = size;

				mySurface = new GE_UI_Surface(renderer,position,size,{0x00,0x00,0x00,0x00}); //background color doesn't actually matter; the WorldView fills the surface
				//add the WorldView first to put it in the background
				worldView = new UI_WorldView(renderer,position,size,camera);
				mySurface->addElement(worldView);

				mySurface->addElement( new UI_HealthView(renderer,{static_cast<double>(camera->screenWidth)-size.x,static_cast<double>(camera->screenHeight)-size.y},{300,200},camera) );

				mySurface->addElement( new GE_UI_Minimap(renderer, {0,0},{150,150},0.02, {0x00,0x33,0x00,255},{0x33,0x99,0x00,0xFF}, camera) ); 

				this->wantsEvents = true;
		}
		~UI_GameView()
		{
			delete mySurface; //surfaces will automatically deleted elements added to them
		}
		void render(Vector2 parrentPosition)
		{
			mySurface->render(parrentPosition);
		}
		void giveEvent(Vector2 parrentPosition, SDL_Event event)
		{
			worldView->giveEvent(parrentPosition,event);
		}
		bool checkIfFocused(int mousex, int mousey)
		{
			return checkIfFocused_ForBox(mousex,mousey,position,size);
		}
	private:
		GE_UI_Surface* mySurface;
		UI_WorldView* worldView;
		Vector2 position;
		Vector2 size;
};

int main(int argc, char* argv[])
{
	camera.pos = {0,0,0};
	camera.screenWidth = 1080;
	camera.screenHeight = 720;

	printf("I'm alive maybe?\n");
	int ttferror = TTF_Init();
	if (ttferror < 0) 
	{
		//TODO: Handle error...
		printf("TTF_Init error %d\n",ttferror);
		return ttferror;
	}
	atexit(TTF_Quit); 
	//initialize some fonts we use
	tinySans = TTF_OpenFont(FREESANS_LOC, 15);
	if(!tinySans) {
		printf("TTF_OpenFont: %s\n", TTF_GetError());
		return 1;
	}
	bigSans =  TTF_OpenFont(FREESANS_LOC, 72);
	if(!bigSans) {
		printf("TTF_OpenFont: %s\n", TTF_GetError());
		return 1;
	}
	TTF_SetFontStyle(bigSans,TTF_STYLE_ITALIC);
	titleSans =  TTF_OpenFont(FREESANS_LOC, 18);
	if(!titleSans) {
		printf("TTF_OpenFont: %s\n", TTF_GetError());
		return 1;
	}
	TTF_SetFontStyle(titleSans,TTF_STYLE_NORMAL | TTF_STYLE_BOLD);
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		printf("Unable to initialize SDL: %s", SDL_GetError());
		return 1;
	}
	atexit(SDL_Quit);

	//Hints
	SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "0" );
	//Initialization
	SDL_Window* myWindow = SDL_CreateWindow("Spacegame", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, camera.screenWidth, camera.screenHeight, 0);
	renderer = SDL_CreateRenderer(myWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	//Initialize the engine
	pthread_mutex_lock(&PhysicsEngineMutex);

	int error = GE_Init(renderer);
	if (error != 0)
	{
		printf("Game engine initialization error: %d\n",error);
		return error;
	}
	GE_LoadSpritesFromDir(renderer, SPRITE_DIR);

	initInventory(renderer);

	//Let the renderer know approx what tick it is
	int rendererThreadsafeTicknum = 0;
	GE_GlueTarget* ticknumGlue = GE_addGlueSubject(&rendererThreadsafeTicknum,&ticknum, GE_PULL_ON_PHYSICS_TICK,sizeof(int));
	
	//initialize the player
	player = new Player(renderer);
	//link camera to the player
	GE_LinkVectorToPhysicsObjectPosition(player,&camera.pos);
	//link player bounding box to a variable we use to center the camera
	Vector2 playerGrid = {0,0};
	GE_LinkGlueToPhysicsObject(player,GE_addGlueSubject(&playerGrid,&player->grid,GE_PULL_ON_PHYSICS_TICK,sizeof(playerGrid)));

	GE_UI_Text* GameOver = new GE_UI_Text(renderer,{static_cast<double>(camera.screenWidth/2),static_cast<double>(camera.screenHeight/2)},{0,0},"GAME OVER!",{0xFF,0x00,0x00,0xFF},bigSans);
	GameOver->center();
	GameOver->expandToTextSize();

	//spawn some enemys why not
	Enemie*  lastenemy;
	for (int i=0;i<20;i++)
	{	
		double randomx = rand() % 5000 + 1;
		double randomy = rand() % 5000 + 1;
		
		//randomx = -50;
		//randomy = -50;


		lastenemy = new Enemie(renderer, {randomx-1500,randomy-1500,0},1);
	}

//#define tonsofbulletstest
#ifdef tonsofbulletstest
	for (int i =0;i<5000;i++)
	{
#define range 10000000
		new StdBullet(renderer,{random_range_double(range*-1,range),random_range_double(range*-1,range),2}, "stdBulletPlayer");
	}

#endif
	
#ifdef GE_DEBUG
		GE_DEBUG_PassRenderer(renderer,&camera);
#endif

	GE_UI_Window* window = new GE_UI_Window(renderer,"INVENTORY",{250,250},{618,320},style);
	UI_GameView* myGameView = new UI_GameView(renderer,{0,0},{static_cast<double>(camera.screenWidth),static_cast<double>(camera.screenHeight)},&camera);
	GE_UI_SetBackgroundElement(myGameView);
	GE_UI_InsertTopLevelElement(window);

	pthread_mutex_unlock(&PhysicsEngineMutex);


	auto inv_ = new Inventory(2500,lastenemy);
	inv_->storage.push_back(ItemStack{ITEM_NAMES::IRON,64});
	inv_->storage.push_back(ItemStack{ITEM_NAMES::IRON,63});
	inv_->storage.push_back(ItemStack{ITEM_NAMES::IRON,21});
	inv_->storage.push_back(ItemStack{ITEM_NAMES::IRON,272});
	inv_->storage.push_back(ItemStack{ITEM_NAMES::IRON,64});
	inv_->storage.push_back(ItemStack{ITEM_NAMES::IRON,63});
	inv_->storage.push_back(ItemStack{ITEM_NAMES::IRON,21});
	inv_->storage.push_back(ItemStack{ITEM_NAMES::IRON,272});
	inv_->storage.push_back(ItemStack{ITEM_NAMES::IRON,63});
	inv_->storage.push_back(ItemStack{ITEM_NAMES::IRON,21});
	inv_->storage.push_back(ItemStack{ITEM_NAMES::IRON,272});
	inv_->storage.push_back(ItemStack{ITEM_NAMES::IRON,63});
	inv_->storage.push_back(ItemStack{ITEM_NAMES::IRON,21});
	inv_->storage.push_back(ItemStack{ITEM_NAMES::IRON,272});
	inv_->storage.push_back(ItemStack{ITEM_NAMES::IRON,63});
	inv_->storage.push_back(ItemStack{ITEM_NAMES::IRON,21});
	inv_->storage.push_back(ItemStack{ITEM_NAMES::IRON,272});
	inv_->storage.push_back(ItemStack{ITEM_NAMES::IRON,63});
	inv_->storage.push_back(ItemStack{ITEM_NAMES::IRON,21});
	inv_->storage.push_back(ItemStack{ITEM_NAMES::IRON,272});
	inv_->storage.push_back(ItemStack{ITEM_NAMES::IRON,63});
	inv_->storage.push_back(ItemStack{ITEM_NAMES::IRON,21});
	inv_->storage.push_back(ItemStack{ITEM_NAMES::IRON,272});
	UI_InventoryView* inv = new UI_InventoryView(renderer, {0,0},{309,250},inv_,fstyle,{8,8},GE_Color{0xff,0xff,0xff,0x33},GE_Color{0x00,0x33,0x00,255},GE_Color{0xff,0x00,0x00,0xff});
	window->surface->addElement(inv);

	//Initialize our omni event handler
	MyOmniEventReciever* myOmniEventReciever = new MyOmniEventReciever();
	GE_UI_InsertOmniEventReciever(myOmniEventReciever);

	while (true)
	{
//#define tickperftest
#ifdef tickperftest
		if (rendererThreadsafeTicknum >= 30)
		{
			exit(0);
		}

#endif
		printf("render tick\n");

		GE_GlueRenderCallback(); //update all positions from the buffer

		//now that we updated the positions, center the camera by subtracting half the player bounding box
		camera.pos = camera.pos+(playerGrid/2);

#ifdef NO_CAMERA_ROTATE
		camera.pos.r = 0;
#endif

		if (GE_UI_PullEvents()) //Returns true if wants to quit
		{
			break; 
		}


		GE_UI_Render();

		if ((!player->GetIsOnline()) && static_cast<int>(floor(rendererThreadsafeTicknum / 5.0)) % 3) //Flash "Game over!" if the player is dead. Basically, this is (%number)/(dividedNumber), numerator being how often it's ON, denominator being how often it's OFF... except when it's not. I'll level with you: I figured out how to make timers based soley off of the tick number a long time ago. I no longer have a clue how this works. I adjusted it until I got the result I wanted. 
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


		SDL_RenderPresent(renderer); //Seems to be the VSyncer (expect ~16ms wait upon call)
	}
	

	printf("--BEGIN SHUTDOWN--\n");
	pthread_mutex_lock(&PhysicsEngineMutex);

	TTF_CloseFont(tinySans);
	TTF_CloseFont(bigSans);
	TTF_CloseFont(titleSans);
	printf("destroy sdl stuff\n");

	delete ticknumGlue;

	printf("plr\n");
	GE_FreePhysicsObject(player);
	printf("hud\n");

	delete myGameView;
	delete GameOver;
	delete myOmniEventReciever;

	printf("--Engine shutdown--\n");
	pthread_mutex_unlock(&PhysicsEngineMutex);
	GE_Shutdown();
	
	SDL_DestroyWindow(myWindow);
	SDL_DestroyRenderer(renderer);

	printf("Bye.\n");
	return 0;
}

#endif //real































#ifdef spritetest

int main(int argc, char* argv[])
{
	Camera camera;
	SDL_Renderer* renderer;
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

	renderer = SDL_CreateRenderer(myWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	//
	int error = GE_Init(renderer);
	//
	GE_LoadSpritesFromDir(renderer, SPRITE_DIR);

	int spriteID;
	
	spriteID = GE_SpriteNameToID(SPRITE_DIR"simple-animation.bmp");
	int t = 0;
	while (true)
	{
			t++;
			if (t==2)
				t= 0;
			GE_BlitSprite(Sprites[spriteID],{0,0,0},{25,25},{static_cast<double>(t*8),0,8,9}, GE_FLIP_NONE);
			SDL_RenderPresent(renderer);
			SDL_Delay(100);
	}

	return 0;
}

#endif //spritetest
#ifdef gluetest
int main(int argc, char* argv[])
{

	Camera camera;
	SDL_Renderer* renderer;

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

	renderer = SDL_CreateRenderer(myWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	//Initialize the engine
	int error = GE_Init(renderer);
	if (error != 0)
	{
		printf("Game engine initialization error: %d\n",error);
	}

	
	//Initialize stuff we need

	GE_LoadSpritesFromDir(renderer, SPRITE_DIR);

	printf("alsfd\n");
	
	GE_RenderedObject* ro = GE_CreateRenderedObject(renderer, SPRITE_DIR"simple.bmp");
	ro->size = {25,25};
	ro->animation = {0,0,8,9};
	ro->position = {0,0,0};

	printf("mutex lock\n");
	pthread_mutex_lock(&PhysicsEngineMutex);

	GE_PhysicsObject* po = new GE_PhysicsObject({25,0,0},{1,1,0},{25,25},5);


	


	po->addCollisionRectangle(GE_Rectangle{0,0,25,25});

	po->velocity = {0,1,0};

	pthread_mutex_unlock(&PhysicsEngineMutex);
	Vector2r test;
	GE_LinkVectorToPhysicsObjectPosition(po,&(ro->position));
	int physicsTick;
	GE_GlueTarget* tickglue  = GE_addGlueSubject(&physicsTick,&ticknum,GE_PULL_ON_RENDER,sizeof(physicsTick));
	printf("mtx unlock\n");


	

	SDL_Event event;
	while (true)
	{
		printf("tick %d",physicsTick);
			if (physicsTick == 25)
			{
				GE_FreeGlueObject(tickglue);
				physicsTick = 26;
			}
				
			GE_GlueRenderCallback(); //update all positions from the buffer
			GE_BlitRenderedObject(ro,&camera,1);
			SDL_RenderPresent(renderer);
			printf("am not kill \n");
	}
	GE_FreeAllSprites();
	GE_FreePhysicsObject(po);
	GE_FreeRenderedObject(ro);

	return 0;
}

#endif //gluetest
#ifdef physicstest
int main(int argc, char* argv[])
{

	Camera camera;
	SDL_Renderer* renderer;

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

	renderer = SDL_CreateRenderer(myWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	//Initialize the engine
	int error = GE_Init(renderer);
	if (error != 0)
	{
		printf("Game engine initialization error: %d\n",error);
	}

	
	//Initialize stuff we need

	GE_LoadSpritesFromDir(renderer, SPRITE_DIR);

	printf("alsfd\n");
	
	GE_RenderedObject* ro = GE_CreateRenderedObject(renderer, SPRITE_DIR"simple.bmp");
	ro->size = {25,25};
	ro->animation = {0,0,8,9};
	ro->position = {0,0,0};
	
	GE_RenderedObject* ro2 = GE_CreateRenderedObject(renderer, SPRITE_DIR"simple.bmp");

	ro2->size = {25,25};
	ro2->animation = {0,0,8,9};
	ro2->position = {0,0,0};


	printf("mutex lock\n");
	pthread_mutex_lock(&PhysicsEngineMutex);

	GE_PhysicsObject* po = new GE_PhysicsObject({0,0,0},{1,1,0},5);

	po->addCollisionRectangle(GE_Rectangle{0,0,25,25});

	po->velocity = {0,-0.59,0};



	GE_PhysicsObject* po2 = new GE_PhysicsObject({0.0,-50,1},{0,0,0},5);

	po2->addCollisionRectangle(GE_Rectangle{0,0,25,25});

	printf("should be 25:%f\n",po2->grid.x);

	pthread_mutex_unlock(&PhysicsEngineMutex);
	GE_LinkVectorToPhysicsObjectPosition(po,&(ro->position));
	GE_LinkVectorToPhysicsObjectPosition(po2,&(ro2->position));
	int physicsTick;
	GE_GlueTarget* tickglue  = GE_addGlueSubject(&physicsTick,&ticknum,GE_PULL_ON_RENDER,sizeof(physicsTick));
	printf("mtx unlock\n");


	
	GE_RectangleShape* background = new GE_RectangleShape(renderer, GE_Color{0x00,0x00,0x00,0xff});

	
	PhysicsDelaySeconds = 3;
	SDL_Event event;
	while (true)
	{
		background->render({0,0,0},{static_cast<double>(camera.screenWidth),static_cast<double>(camera.screenHeight)});
		GE_GlueRenderCallback(); //update all positions from the buffer
		GE_BlitRenderedObject(ro,&camera,1);
		GE_BlitRenderedObject(ro2,&camera,1);
		SDL_RenderPresent(renderer);
		printf("am not kill \n");
	}
	GE_FreeAllSprites();
	GE_FreePhysicsObject(po);
	GE_FreeRenderedObject(ro);

	return 0;
}



#endif //physicstest
#ifdef leveleditor

SDL_Renderer* renderer;
Camera camera;

TTF_Font* tinySans;
TTF_Font* bigSans;
TTF_Font* titleSans;



class MyOmniEventReciever : public GE_UI_OmniEventReciever
{
	public:
		MyOmniEventReciever(){}
		~MyOmniEventReciever(){}
		void giveEvent(SDL_Event event)
		{
		}
	
};


int main(int argc, char *argv[]) 
{

	printf("I'm alive maybe?\n");
	int ttferror = TTF_Init();
	if (ttferror < 0) 
	{
		//TODO: Handle error...
		printf("TTF_Init error %d\n",ttferror);
		return ttferror;
	}
	atexit(TTF_Quit); 
	//initialize some fonts we use
	tinySans = TTF_OpenFont(FREESANS_LOC, 15);
	if(!tinySans) {
		printf("TTF_OpenFont: %s\n", TTF_GetError());
		return 1;
	}
	bigSans =  TTF_OpenFont(FREESANS_LOC, 72);
	if(!bigSans) {
		printf("TTF_OpenFont: %s\n", TTF_GetError());
		return 1;
	}
	TTF_SetFontStyle(bigSans,TTF_STYLE_ITALIC);
	titleSans =  TTF_OpenFont(FREESANS_LOC, 18);
	if(!titleSans) {
		printf("TTF_OpenFont: %s\n", TTF_GetError());
		return 1;
	}
	TTF_SetFontStyle(titleSans,TTF_STYLE_NORMAL | TTF_STYLE_BOLD);


	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		printf("Unable to initialize SDL: %s", SDL_GetError());
		return 1;
	}
	atexit(SDL_Quit);

	//Hints
	SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "0" );

	camera.pos = {0,0,0};
	camera.screenWidth = 1080;
	camera.screenHeight = 720;
	
	SDL_Window* myWindow = SDL_CreateWindow("Spacegame", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, camera.screenWidth, camera.screenHeight, 0);
	renderer = SDL_CreateRenderer(myWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	//Initialize the engine
	
	pthread_mutex_lock(&PhysicsEngineMutex);
	int error = GE_Init(renderer);
	if (error != 0)
	{
		printf("Game engine initialization error: %d\n",error);
		return error;
	}

	GE_LoadSpritesFromDir(renderer, SPRITE_DIR);

	pthread_mutex_unlock(&PhysicsEngineMutex);

	GE_UI_GameRender* gameRender = new GE_UI_GameRender(renderer, {0,0},{camera.screenHeight,camera.screenWidth},player);

	GE_UI_SetBackgroundElement(gameRender);


	printf("--BEGIN SHUTDOWN--\n");
	pthread_mutex_lock(&PhysicsEngineMutex);

	TTF_CloseFont(tinySans);
	TTF_CloseFont(bigSans);
	TTF_CloseFont(titleSans);
	printf("destroy sdl stuff\n");

	GE_FreeStars(stars1);
	GE_FreeStars(stars2);
	GE_FreeStars(stars3);
	GE_FreeStars(stars4);
	GE_FreeStars(stars5);

	delete myOmniEventReciever;

	printf("--Engine shutdown--\n");
	pthread_mutex_unlock(&PhysicsEngineMutex);
	GE_Shutdown();
	
	SDL_DestroyWindow(myWindow);
	SDL_DestroyRenderer(renderer);

	printf("Bye.\n");


}




#endif
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
