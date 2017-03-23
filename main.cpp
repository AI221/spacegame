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

//Definitions
#define MAX_PHYSICS_OBJECTS 256000000 //maximum ammount of physics objects in the game

#define debug true //wheather debug draws, menus, etc. is included. 




struct Vector2
{
	double x;
	double y;
};
struct GridInfo //info for the inaccurate physics pre-calculation
{
	int x;
	int y;
	int shapex;
	int shapey; 
};

int sGrid[2000][2000]; //TODO: Dynamically sized arrays for both of these
int dbGrid[2000][2000]; //TODO: Remove this!

int collisionSpots[2000][1]; 
int nextCollisionSpot = 0;

class PhysicsObject
{
	public:
		Vector2 getPosition();
		Vector2 getVelocity();
		GridInfo getGridInfo();
		PhysicsObject(Vector2 newPosition, Vector2 shape);
		void setPosition(Vector2 newPosition);
		void setVelocity(Vector2 newVelocity);
		void addVelocity(Vector2 moreVelocity);
		void setShape(Vector2 shape);

		void tickMyPhysics(); //important: this should NOT have collision detection. that would result on instances where things would collide even though the object they collide with would've moved next tick
		Vector2 position;
		Vector2 velocity; 

	private:

		Vector2 newPosition;
		bool setNewPosition = false;

		Vector2 newVelocity;
		bool setNewVelocity = false;

		GridInfo grid;
		Vector2 warpedShape;

		int myNumber;


};



PhysicsObject* allPhysicsObjects[MAX_PHYSICS_OBJECTS];
int nextPhysicsObject = 1; //TODO: Merge with the physicsObjects
//probably will change this to a dynamically sized array later



PhysicsObject::PhysicsObject(Vector2 newPosition, Vector2 shape)
{
	allPhysicsObjects[nextPhysicsObject] = this;
	myNumber = nextPhysicsObject;
	nextPhysicsObject++;

	position.x = newPosition.x;
	position.y = newPosition.y;
	velocity = {0,0};
	grid = {0,0,0,0};
	warpedShape = {0,0};

	

	//setPosition(newPosition); //commented this out b/c it waits till next physics tick to update position
	
	setShape(shape);
}
void PhysicsObject::setPosition(Vector2 newPosition)
{
	this->newPosition = newPosition;
	setNewPosition = true;
}
void PhysicsObject::setVelocity(Vector2 newVelocity)
{
	this->newVelocity = newVelocity;
	setNewVelocity = true;
}
void PhysicsObject::addVelocity(Vector2 moreVelocity)
{
	newVelocity.x = velocity.x+moreVelocity.x;
	newVelocity.y = velocity.y+moreVelocity.y;
	setNewVelocity = true;
}
void PhysicsObject::setShape(Vector2 shape)
{
	grid.shapex = shape.x;
	grid.shapey = shape.y;
}
void PhysicsObject::tickMyPhysics() //the fun part
{
	for (int x = 0; x < warpedShape.x; x++) 
	{
		for (int y = 0; y < warpedShape.y; y++) 
		{
			int posx = x+grid.x;
			int posy = y+grid.y;
			if (sGrid[posx][posy] == myNumber) //TODO: Check if necisary -- it might be okay to set other's grids to 0 because we'll detect collision and start an accurate simulation anyway
			{
				sGrid[posx][posy] = 0;
			}
			else if (sGrid[posx][posy] != 0)
			{
				std::cout << "osht we gots collision im: " << myNumber << " its: " << sGrid[posx][posy] << " differenciation: " << rand() << std::endl;
			}
		}
	}
	//move ourselves forward
	//TODO: keep track of the last position, or maybe not because it could be reverse by minusing our position by our speed maybe?
	//first set velocity
	
	if (setNewVelocity) 
	{
		velocity = newVelocity;
		setNewVelocity = false;
	}
	if (setNewPosition)
	{
		position = newPosition;
		setNewPosition = false;
	}

	position.x += velocity.x;
	position.y += velocity.y;
	//TODO multiple grids, but right now we can't have you go <0 (or 2000< but that'll be fixed too and doesnt happen as much)
	if (position.x < 0)
	{
		position.x = 0;
	}
	if (position.y < 0)
	{
		position.y = 0;
	}





	grid.x = (int) position.x/10;
	grid.y = (int) position.y/10;

	//Calculate how the shape warps. The shape will stretch as you move faster to avoid clipping
	warpedShape.x = ((std::abs(velocity.x)/10)+2)*(grid.shapex/10);
	warpedShape.y = ((std::abs(velocity.y)/10)+2)*(grid.shapey/10);

	for (int x = 0; x < warpedShape.x; x++) 
	{
		for (int y = 0; y < warpedShape.y; y++) 
		{
			int posx = x+grid.x;
			int posy = y+grid.y;
			if (sGrid[posx][posy] == 0) 
			{
				sGrid[posx][posy] = myNumber;
			}
			else
			{
				collisionSpots[nextCollisionSpot][0] = posx;
				collisionSpots[nextCollisionSpot][1] = posy;
				nextCollisionSpot++;
			}
		}
	}





}
Vector2 PhysicsObject::getPosition()
{
	return position;
}





class Sprite
{
	public:
		void blit(Vector2 position,Vector2 animation);
		void blit(Vector2 position);
		void setAnimation(int newAnimationNumber());
		Sprite(SDL_Renderer* renderer, const char* path, Vector2 size);

	private:
		SDL_Texture* myTexture;
		SDL_Renderer* myRenderer;
		SDL_Rect myPosition; //this object's x and y are NOT GAURUNTEED and should be assumed random.
		SDL_Rect myAnimation;
};

Sprite::Sprite(SDL_Renderer* renderer, const char* path, Vector2 size)
{
	myRenderer = renderer;
	SDL_Surface* LoadingSurface;
	LoadingSurface = SDL_LoadBMP(path);
	myTexture = SDL_CreateTextureFromSurface(myRenderer, LoadingSurface);
	SDL_FreeSurface(LoadingSurface);
	myPosition.w = size.x;
	myPosition.h = size.y;
	myAnimation.w = size.x;
	myAnimation.h = size.y;
	
}
void Sprite::blit(Vector2 position,Vector2 animation)	
{
	myPosition.x = position.x;
	myPosition.y = position.y;
	myAnimation.x = animation.x;
	myAnimation.y = animation.y;
	
	SDL_RenderCopy(myRenderer, myTexture, &myAnimation, &myPosition);
}
void Sprite::blit(Vector2 position)
{
	this->blit(position,{0,0});
}
class PositionedSprite //positioned sprites are more for interfaces
{
	public:
		void blit(); 
		void setPos(Vector2 position);
		void setAnimation(Vector2 animation);
		PositionedSprite(Vector2 position, Sprite* sprite);
	private:
		Vector2 position;
		Vector2 animation;
		Sprite* mySprite;
};
void PositionedSprite::setPos(Vector2 position)
{
	this->position = position;
}
void PositionedSprite::blit()
{
	mySprite->blit(position,animation);
}
PositionedSprite::PositionedSprite(Vector2 position, Sprite* sprite)
{
	this->position = position;
	mySprite = sprite;
}




class RenderedPhysicsObject 
{
	public:
		RenderedPhysicsObject(SDL_Renderer* renderer, Sprite* sprite, Vector2 *camera, Vector2 position);
		void tickMyPhysics();
		void render();
		void setVelocity(Vector2 velocity);
		PhysicsObject* myPhysicsObject;
	private:
		Sprite* sprite;
		int physicsID;
		SDL_Renderer* myRenderer;
		Vector2* camera;

		

};



RenderedPhysicsObject::RenderedPhysicsObject(SDL_Renderer* renderer, Sprite* sprite, Vector2 *camera, Vector2 position)
{
	Vector2 shape = {25,25}; //TODO
	myPhysicsObject = new PhysicsObject(position,shape);
	this->camera = camera;
	this->sprite = sprite;
}
void RenderedPhysicsObject::tickMyPhysics()
{
	myPhysicsObject->tickMyPhysics();
}
void RenderedPhysicsObject::setVelocity(Vector2 velocity)
{
	myPhysicsObject->setVelocity(velocity);
}
void RenderedPhysicsObject::render()
{
	Vector2 cammed_position = myPhysicsObject->getPosition();
	cammed_position.x -= camera->x;
	cammed_position.y -= camera->y;
	sprite->blit(cammed_position,{0,0});
}


SDL_Renderer* myRenderer;


RenderedPhysicsObject* physicsObjects[100] = {}; //TODO dimensions and what not
int numPhysicsObjs = 1; //Starts at 1 because sGrid uses 0 for 'nothing here'

Vector2 camera;
int camFocusedObj = 1;

void render()
{
	camera = physicsObjects[camFocusedObj]->myPhysicsObject->getPosition();
	camera.x -= 640/2;
	camera.y -= 580/2;
	for (int i=1; i <= numPhysicsObjs; i++)
	{
		physicsObjects[i]->render();
		//TODO: Physics tied to framerate. REEEEEEEEEEEEEEEEEEEEEEEEEEEEEE
		physicsObjects[i]->tickMyPhysics();
		//std::cout << "-------------------------------" << std::endl;
	}
}


Sprite* nothingHere;
Sprite* somethingHere;

void debug_render()
{ //pulled from my prototype
	somethingHere->blit({0,0},{10,10});
	camera = physicsObjects[camFocusedObj]->myPhysicsObject->getPosition();
	camera.x -= 640/2;
	camera.y -= 580/2;
	camera.x = camera.x/10;
	camera.y = camera.y/10;
	int camerax = (int) camera.x;
	int cameray = (int) camera.y;
	for (int i = 0; i < 70; i++) { 
		for (int o = 0; o < 70; o++) {
			//std::cout << "i " << i <<" t " << i+camerax << std::endl;
			if ((i+camerax < 0) || (o+cameray < 0)) {} else {
				if ( sGrid[i+camerax][o+cameray] != 0)
				{
					somethingHere->blit({i*10,o*10},{0,0});
					//std::cout << "nothingHere; ";`
				} 
				else 
				{
					nothingHere->blit({i*10,o*10},{0,0});
					//std::cout << "somethingHere; ";
				}
			}
		}
	}


}


#ifdef debug
	/* Debug code should be as isolated as possible except when pulling values from things. It should use its own UI library that shouldn't ever see updates unless really necessary */

	class debugUIElement
	{
		public:
		 /*	debugUIElement(SDL_Renderer* renderer, Vector2 position);
			Vector2 position;
			SDL_Renderer* renderer;*/
			virtual void render(Vector2 parrentPosition) = 0;
			virtual void giveEvent(Vector2 parrentPosition, SDL_Event event) = 0;
			bool wantsEvents;
	};



	
	class debugText : public debugUIElement
	{
		public:
			debugText(SDL_Renderer* renderer, Vector2 position, std::string text);
			void render(Vector2 parrentPosition);
			void render();
			void setText(const char* text);
			void giveEvent(Vector2 parrentPosition, SDL_Event event);
		//	bool wantsEvents;

		private:
			TTF_Font* Sans = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSans.ttf", 24);
			SDL_Color color = {0, 255, 0}; //GREEN TXT FOR HACKERS
			SDL_Rect Message_rect;
			SDL_Renderer* myRenderer;
			SDL_Texture* Message;
	};
	void debugText::setText(const char* text)
	{
		SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, text, color);
		Message = SDL_CreateTextureFromSurface(myRenderer, surfaceMessage);

		Message_rect.w = surfaceMessage->w;
		Message_rect.h = surfaceMessage->h;

		SDL_FreeSurface(surfaceMessage);


	}	
	debugText::debugText(SDL_Renderer* renderer, Vector2 position, std::string text)
	{
		Message_rect.x = position.x;
		Message_rect.y = position.y;
		this->myRenderer = renderer;
		setText(text.c_str());
		this->wantsEvents = false;
	}
	void debugText::render(Vector2 parrentPosition)
	{
		SDL_Rect transformedRect = Message_rect;
		transformedRect.x += parrentPosition.x;
		transformedRect.y += parrentPosition.y;
		SDL_RenderCopy(myRenderer, Message, NULL,&transformedRect);	
	}
	void debugText::render()
	{
		render({0,0});
	}
	void debugText::giveEvent(Vector2 parrentPosition, SDL_Event event) //shouldn't ever be ran; look into finding ways to avoid implementing this
	{
	}





	class debugButton : public debugUIElement
	{
		public:
			debugButton(SDL_Renderer* myRenderer, Vector2 position, Vector2 size, std::string text);
			void render(Vector2 parrentPosition);
			void render();
			void giveEvent(Vector2 parrentPosition, SDL_Event event);
			std::function< void () > C_Pressed; //Callback  


			Vector2 position;
			SDL_Rect positionAndSize;
			bool pressed = false;
		private:
			debugText* myText;
			SDL_Renderer* myRenderer;
	};
	debugButton::debugButton(SDL_Renderer* myRenderer, Vector2 position, Vector2 size, std::string text)
	{
		myText = new debugText(myRenderer,position,text);
		this->myRenderer = myRenderer;
		this->position = position;
		positionAndSize.x = position.x;
		positionAndSize.y = position.y;
		positionAndSize.w = size.x;
		positionAndSize.h = size.y;
		this->wantsEvents = true;
	}
	void debugButton::render(Vector2 parrentPosition)
	{
		SDL_Rect translatedRect = positionAndSize;
		translatedRect.x += parrentPosition.x;
		translatedRect.y += parrentPosition.y;

		if (pressed)
		{
			SDL_SetRenderDrawColor( myRenderer, 0x00, 0x00, 0x00, 0x00 );
		}
		else
		{
			SDL_SetRenderDrawColor( myRenderer, 0xFF, 0x00, 0x00, 0xFF ); 
		}
		SDL_RenderFillRect( myRenderer, &translatedRect ); 
		myText->render(parrentPosition);
	}
	void debugButton::render()
	{
		render({0,0});
	}
	void debugButton::giveEvent(Vector2 parrentPosition, SDL_Event event)
	{
		Vector2 actualPosition = position;
		actualPosition.x += parrentPosition.x;
		actualPosition.y += parrentPosition.y;

		if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP)
		{
			if (event.button.button == SDL_BUTTON_LEFT)
			{
				
				int x,y;
				SDL_GetMouseState(&x,&y);
				//std::cout << "x: " << x << " y: << " << y << " b: "<<actualPosition.x+positionAndSize.w << std::endl;
				if (actualPosition.x <= x && actualPosition.y <= y && actualPosition.x+positionAndSize.w >= x && actualPosition.y+positionAndSize.h >= y)
				{
					std::cout << "BINGO" << std::endl;
					if (event.type == SDL_MOUSEBUTTONDOWN)
					{
						pressed = true;
					}
					else
					{
						if ( C_Pressed)
						{
							C_Pressed();
						}
						pressed = false;
					}
					
				}
				else if (event.type == SDL_MOUSEBUTTONUP)
				{
					pressed = false;
				}
			}
		}		
	}




		




	class debugWindow
	{
		public:
			debugWindow(SDL_Renderer* myRenderer, Vector2 position, Vector2 size);
			void render();
			int addElement(debugUIElement* element);
			debugUIElement* getElement(int elementID);
			void giveEvent(SDL_Event event);
			bool isOpen = false;

			Vector2 position;
			SDL_Rect positionAndSize;
			
		private:
			debugUIElement* elements[256];
			int nextUIElement = 0;
			SDL_Renderer* myRenderer;
			debugButton* xbutton;
			debugButton* dbutton;
			
	};
	debugWindow::debugWindow(SDL_Renderer* myRenderer,Vector2 position, Vector2 size)
	{
		this->position = position;
		this->positionAndSize.x = position.x;
		this->positionAndSize.y = position.y;
		this->positionAndSize.w = size.x;
		this->positionAndSize.h = size.y;
		this->myRenderer = myRenderer;
		//add x button
		xbutton = new debugButton(myRenderer, {size.x-25,0},{25,25},"X");

		auto closeCallback = [&] () { isOpen = false; };
		xbutton->C_Pressed = closeCallback;

		addElement(xbutton);

		//add drag button
		dbutton = new debugButton(myRenderer, {0,0},{25,25},"D");
		addElement(dbutton);
	}
	int debugWindow::addElement(debugUIElement* element)
	{
		elements[nextUIElement] = element;
		nextUIElement++;
		return nextUIElement-1;
	}
	debugUIElement* debugWindow::getElement(int elementID)
	{
		return elements[elementID];
	}
	void debugWindow::render()
	{
		SDL_SetRenderDrawColor( myRenderer, 0xFF, 0xFF, 0xFF, 0xFF ); 
		SDL_RenderFillRect( myRenderer, &positionAndSize ); 
		for (int i=0;i<nextUIElement;i++)
		{
			elements[i]->render(position);
		}
	}
	void debugWindow::giveEvent(SDL_Event event)
	{
		for (int i=0;i<nextUIElement;i++)
		{
			if (elements[i]->wantsEvents)
			{
				elements[i]->giveEvent(position,event);
			}
		}
		if (dbutton->pressed) //improper use of buttons but whatever, it's a debug system
		{
			int x,y;
			SDL_GetMouseState(&x,&y);
			position.x = x;
			position.y = y;
			positionAndSize.x = x;
			positionAndSize.y = y;
		}

	}

		
			







#endif
	
	


int main()
{
	if (TTF_Init() < 0) 
	{
	// Handle error...
		std::cout << "!!!TFT_Init Error!" << std::endl;
	}
	for (int i = 0; i < 200; i++)
	{
		for( int o = 0; o < 200; o++)
		{
			sGrid[i][o] = 0;
			dbGrid[i][o] = 0;
		}
	}
	physicsObjects[0] = NULL;


	sGrid[10][10] = 1; //TODO: Remove this!

	SDL_Window* myWindow;
	SDL_Event event;


	camera = {200,0};
	camFocusedObj = 1;

	
	myWindow = SDL_CreateWindow("Spacegame",
	SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 580, 0);

	myRenderer = SDL_CreateRenderer(myWindow, -1, SDL_RENDERER_ACCELERATED);

	//debug bullshit
	nothingHere = new Sprite(myRenderer,"DEBUG_nothingHere.bmp",{10,10});
	somethingHere = new Sprite(myRenderer,"DEBUG_somethingHere.bmp",{10,10});
	
	Sprite* mySprite = new Sprite(myRenderer,"simple.bmp",{25,25});

	Sprite* otherSprite = new Sprite(myRenderer,"DEBUG_somethingHere.bmp",{25,25});

	Sprite* shoddySpaceship = new Sprite(myRenderer,"shottyspaceship.bmp",{25,25});

	Sprite* bg = new Sprite(myRenderer,"DEBUG_nothingHere.bmp",{640,580});
		
	physicsObjects[numPhysicsObjs] = new RenderedPhysicsObject(myRenderer,shoddySpaceship,&camera,{50,50});	

	numPhysicsObjs++;
	physicsObjects[numPhysicsObjs] = new RenderedPhysicsObject(myRenderer,mySprite,&camera,{200,200});	

	numPhysicsObjs++;
	physicsObjects[numPhysicsObjs] = new RenderedPhysicsObject(myRenderer,otherSprite,&camera,{200,200});	
	physicsObjects[numPhysicsObjs]->myPhysicsObject->setVelocity({0,1});


	
	
	bool keysHeld[323] = {false}; 
	bool isDebugRender = false;
	#ifdef debug
		debugText* texttest = new debugText(myRenderer,{25,25},"texttext");
		debugButton* testbutton = new debugButton(myRenderer,{50,50},{100,25},"texttext");
		debugWindow* interface1_menu = new debugWindow(myRenderer,{100,100},{320,240});
		interface1_menu->addElement(texttest);
		interface1_menu->addElement(testbutton);
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
					std::cout << event.key.keysym.sym << std::endl;
					if (event.key.keysym.sym == SDLK_SCROLLLOCK)
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
			physicsObjects[camFocusedObj]->myPhysicsObject->addVelocity({0,-0.5});
		}
		if(keysHeld[SDLK_s])
		{
			physicsObjects[camFocusedObj]->myPhysicsObject->addVelocity({0,0.5});
		}
		if(keysHeld[SDLK_d])
		{
			physicsObjects[camFocusedObj]->myPhysicsObject->addVelocity({0.5,0});
		}
		if(keysHeld[SDLK_a])
		{
			physicsObjects[camFocusedObj]->myPhysicsObject->addVelocity({-0.5,0});
		}
		if(keysHeld[SDLK_1])
		{
			camFocusedObj=2;
		}
		if(keysHeld[SDLK_0])
		{
			camFocusedObj=1;
		}
		if(keysHeld[SDLK_q])
		{
			std::cout <<"velx: " << physicsObjects[camFocusedObj]->myPhysicsObject->velocity.x << std::endl;
			std::cout <<"vely: " << physicsObjects[camFocusedObj]->myPhysicsObject->velocity.y << std::endl;
			std::cout <<"posx: " << physicsObjects[camFocusedObj]->myPhysicsObject->position.x << std::endl;
			std::cout <<"posy: " << physicsObjects[camFocusedObj]->myPhysicsObject->position.y << std::endl;
		}
		if(keysHeld[SDLK_e])
		{
			isDebugRender = true;
		}
		if (keysHeld[SDLK_r])
		{
			isDebugRender = false;
		}
		

						






		bg->blit({0,0},{0,0});
		
		render();
		#ifdef debug
				interface1_menu->giveEvent(event);
				if(isDebugRender) //ineffecient but who the fuck cares, it's a goddamn debug render.
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

	return 0;
}
