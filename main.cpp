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

#define DEG_TO_RAD 57.2958

#define debug true //wheather debug draws, menus, etc. is included. 




/*LONG-TERM TODO

*Lagged camera? ("chasecam") 
*Standard implementation for rotation matrix instead of spamming it everywhere?

*Possible "engine demo game" would be an asteroid field with constant collisions. Would demo the effeciency of the physics engine very well.


*/

struct Vector2
{
	double x;
	double y;
};
struct Vector2r
{
	double x;
	double y;
	double r;
	void addRelativeVelocity(double inx, double iny,double posr)
	{
		double sin_rotation = std::sin(posr/DEG_TO_RAD); //posr must be used because this vector is a velocity vector, not a position vector
		double cos_rotation = std::cos(posr/DEG_TO_RAD);
		double new_x = (inx*cos_rotation)-(iny*sin_rotation);
		double new_y = (inx*sin_rotation)+(iny*cos_rotation);
		/*std::cout << "r " << posr << std::endl;
		std::cout << "cin " << sin_rotation << std::endl;
		std::cout << "cos " << cos_rotation << std::endl;
		std::cout << "new_x " << new_x << std::endl;
		std::cout << "new_y " << new_y << std::endl;*/
		x += new_x;
		y += new_y;
	}
		
};
struct GridInfo //info for the inaccurate physics pre-calculation
{
	int x;
	int y;
	int shapex;
	int shapey; 
};

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

int sGrid[2000][2000]; //TODO: Dynamically sized arrays for both of these

int collisionSpots[2000][1]; 
int nextCollisionSpot = 0;

class PhysicsObject
{
	public:
		Vector2r getPosition();
		Vector2r getVelocity();
		GridInfo getGridInfo();
		PhysicsObject(Vector2r newPosition, Vector2 shape);

		void setPosition(Vector2r newPosition);
		void setVelocity(Vector2r newVelocity);
		void addVelocity(Vector2r moreVelocity);
		void addRelativeVelocity(Vector2r moreVelocity);

		void setShape(Vector2 shape);

		void tickMyPhysics(); //important: this should NOT have collision detection. that would result on instances where things would collide even though the object they collide with would've moved next tick
		Vector2r position;
		Vector2r velocity; 

	private:

		Vector2r newPosition;
		bool setNewPosition = false;

		Vector2r newVelocity;
		bool setNewVelocity = false;

		GridInfo grid;
		Vector2 warpedShape;

		int myNumber;


};



PhysicsObject* allPhysicsObjects[MAX_PHYSICS_OBJECTS];
int nextPhysicsObject = 1; //TODO: Merge with the physicsObjects
//probably will change this to a dynamically sized array later



PhysicsObject::PhysicsObject(Vector2r newPosition, Vector2 shape)
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
void PhysicsObject::setPosition(Vector2r newPosition)
{
	this->newPosition = newPosition;
	setNewPosition = true;
}
void PhysicsObject::setVelocity(Vector2r newVelocity)
{
	this->newVelocity = newVelocity;
	setNewVelocity = true;
}
void PhysicsObject::addVelocity(Vector2r moreVelocity)
{
	newVelocity.x = velocity.x+moreVelocity.x;
	newVelocity.y = velocity.y+moreVelocity.y;
	newVelocity.r = velocity.r+moreVelocity.r;
	setNewVelocity = true;
}
#define TEN_DIVIDED_BY_NINE 1.1111111111111111
void PhysicsObject::addRelativeVelocity(Vector2r moreVelocity)
{
	/*newVelocity.y = velocity.x+(moreVelocity.y*(((100-(position.r*TEN_DIVIDED_BY_NINE)))/100));
	newVelocity.x = velocity.y+(moreVelocity.y*(((position.r*TEN_DIVIDED_BY_NINE))/100));*/
	newVelocity.r = velocity.r+moreVelocity.r;
	newVelocity.addRelativeVelocity(moreVelocity.x,moreVelocity.y,this->position.r);
	setNewVelocity = true;
}
Vector2r PhysicsObject::getVelocity()
{
	return velocity;
}
Vector2r PhysicsObject::getPosition()
{
	return position;
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
				//std::cout << "osht we gots collision im: " << myNumber << " its: " << sGrid[posx][posy] << " differenciation: " << rand() << std::endl;
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
	position.r += velocity.r;

	//prevent rotation from being >360
	position.r -= floor(position.r/360)*360;


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
	warpedShape.x = ((abs(velocity.x)/10)+2)*(grid.shapex/10);
	warpedShape.y = ((abs(velocity.y)/10)+2)*(grid.shapey/10);

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





class Sprite
{
	public:
		void blit(Vector2r position,Vector2 animation);
		void blit(Vector2 position,Vector2 animation, double rotation);
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
void Sprite::blit(Vector2r position,Vector2 animation)	
{
	blit({position.x,position.y},animation,position.r);
}
void Sprite::blit(Vector2 position,Vector2 animation,double rotation)	
{
	myPosition.x = position.x;
	myPosition.y = position.y;
	myAnimation.x = animation.x;
	myAnimation.y = animation.y;
	
	SDL_RenderCopyEx(myRenderer, myTexture, &myAnimation, &myPosition,rotation,NULL,SDL_FLIP_NONE);
}
void Sprite::blit(Vector2 position, Vector2 animation)
{
	blit(position,animation,0);
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
		RenderedPhysicsObject(SDL_Renderer* renderer, Sprite* sprite, Vector2r *camera, Vector2r position);
		void tickMyPhysics();
		void render();
		PhysicsObject* myPhysicsObject;
	private:
		Sprite* sprite;
		int physicsID;
		SDL_Renderer* myRenderer;
		Vector2r* camera;

		

};



RenderedPhysicsObject::RenderedPhysicsObject(SDL_Renderer* renderer, Sprite* sprite, Vector2r *camera, Vector2r position)
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
void RenderedPhysicsObject::render()
{
	Vector2r position = myPhysicsObject->getPosition();
	Vector2r cammed_position = position; 
	
	cammed_position.x -= camera->x;
	cammed_position.y -= camera->y;
	//cammed_position.r -= camera->r;

	std::cout << "1| " << camera->x << std::endl;
	std::cout << "2| " << camera->y << std::endl;

	double dist = distance(camera->x*-1,camera->y*-1,position.x,position.y);

	std::cout << "dist" << dist << std::endl;

//	std::co

	std::cout << "2 "<<(camera->x) << std::endl;
	std::cout << "1 "<<(camera->y) << std::endl;

	double angle = (atan(abs(position.y-camera->x-320)/abs(position.x-camera->y-290)))*DEG_TO_RAD;
	cammed_position.r = angle;

	std::cout << "angle" << angle << std::endl;

	std::cout << "abc" << (double) asin((position.x+camera->x)) << std::endl;
	
	double sin_angle = sin((camera->r/DEG_TO_RAD));
	double cos_angle = cos((camera->r/DEG_TO_RAD));

	cammed_position.x = (position.x-camera->x-320)*cos_angle - (position.y-camera->y-290)*sin_angle;
	cammed_position.y = (position.x-camera->x-320)*sin_angle + (position.y-camera->y-290)*cos_angle;

	cammed_position.x += 320;
	cammed_position.y += 290;

/*	cammed_position.x -= camera->x-320;
cammed_position.y -= camera->y-290;*/




	
	
	sprite->blit(cammed_position,{0,0});
}


SDL_Renderer* myRenderer;


RenderedPhysicsObject* physicsObjects[1000] = {}; //TODO dimensions and what not
int numPhysicsObjs = 1; //Starts at 1 because sGrid uses 0 for 'nothing here'

Vector2r camera;
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
	somethingHere->blit({0,0,0},{10,10});
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
					somethingHere->blit({i*10,o*10,0},{0,0});
					//std::cout << "nothingHere; ";`
				} 
				else 
				{
					nothingHere->blit({i*10,o*10,0},{0,0});
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
			void setText(std::string text);
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
	void debugText::setText(std::string text)
	{
		setText(text.c_str());
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
					else if (pressed)
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
		if (!isOpen)
		{
			return;
		}
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
		//TODO: Handle error...
		std::cout << "!!!TFT_Init Error!" << std::endl;
	}
	for (int i = 0; i < 200; i++)
	{
		for( int o = 0; o < 200; o++)
		{
			sGrid[i][o] = 0;
		}
	}
	physicsObjects[0] = NULL;

	SDL_Window* myWindow;
	SDL_Event event;


	camera = {200,0,0};
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
		
	physicsObjects[numPhysicsObjs] = new RenderedPhysicsObject(myRenderer,shoddySpaceship,&camera,{50,50,0});	

	numPhysicsObjs++;
	physicsObjects[numPhysicsObjs] = new RenderedPhysicsObject(myRenderer,mySprite,&camera,{200,200,0});	

	numPhysicsObjs++;
	physicsObjects[numPhysicsObjs] = new RenderedPhysicsObject(myRenderer,otherSprite,&camera,{200,200,0});	
	physicsObjects[numPhysicsObjs]->myPhysicsObject->setVelocity({0,1});


	
	
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
			Vector2r pos = physicsObjects[camFocusedObj]->myPhysicsObject->getPosition();
			for (int i=0;i < 20;i++)
			{
				numPhysicsObjs++;
				physicsObjects[numPhysicsObjs] = new RenderedPhysicsObject(myRenderer,otherSprite,&camera,{pos.x,pos.y,0});	
				physicsObjects[numPhysicsObjs]->myPhysicsObject->setVelocity({(double)(rand() % 101)/100,(double)(rand() % 11)/10}); //random between 0-1 w/ 2 decimals
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
			physicsObjects[camFocusedObj]->myPhysicsObject->addRelativeVelocity({0,-0.5});
		}
		if(keysHeld[SDLK_s])
		{
			physicsObjects[camFocusedObj]->myPhysicsObject->addRelativeVelocity({0,0.5});
		}
		if(keysHeld[SDLK_d])
		{
			physicsObjects[camFocusedObj]->myPhysicsObject->addRelativeVelocity({0.5,0});
		}
		if(keysHeld[SDLK_a])
		{
			physicsObjects[camFocusedObj]->myPhysicsObject->addRelativeVelocity({-0.5,0});
		}
		if(keysHeld[SDLK_q])
		{
			physicsObjects[camFocusedObj]->myPhysicsObject->addVelocity({0,0,-0.25});
		}
		if(keysHeld[SDLK_e])
		{
			physicsObjects[camFocusedObj]->myPhysicsObject->addVelocity({0,0,0.25});
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
			std::cout <<"velx: " << physicsObjects[camFocusedObj]->myPhysicsObject->velocity.x << std::endl;
			std::cout <<"vely: " << physicsObjects[camFocusedObj]->myPhysicsObject->velocity.y << std::endl;
			std::cout <<"posx: " << physicsObjects[camFocusedObj]->myPhysicsObject->position.x << std::endl;
			std::cout <<"posy: " << physicsObjects[camFocusedObj]->myPhysicsObject->position.y << std::endl;
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
			physicsObjects[camFocusedObj]->myPhysicsObject->setVelocity({0,0,0});
		}
		if (keysHeld[SDLK_ESCAPE])
		{
			break;
		}
			
		

						






		bg->blit({0,0,0},{0,0});
		
		render();
		#ifdef debug
				texttest->setText(std::to_string(physicsObjects[camFocusedObj]->myPhysicsObject->position.r).c_str());
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
