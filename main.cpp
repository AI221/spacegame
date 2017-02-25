#include<SDL2/SDL.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <math.h>

//Definitions
#define MAX_PHYSICS_OBJECTS 256000000




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
			if (sGrid[posx][posy] == myNumber) //TODO: Check if necisary
			{
				sGrid[posx][posy] = 0;
			}
		}
	}
	//move ourselves forward
	//TODO: keep track of the last position, or maybe not because it could be reverse by minusing our position by our speed maybe?
	//first set velocity
	
	if (setNewVelocity) //TODO this doesn't seem to work
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





	grid.x = (int) position.x;
	grid.y = (int) position.y;

	//Calculate how the shape warps. The shape will stretch as you move faster to avoid clipping
	warpedShape.x = ((velocity.x/10)+1)*(grid.shapex/10);
	warpedShape.y = ((velocity.y/10)+1)*(grid.shapey/10);

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
	}
}

Sprite* nothingHere;
Sprite* somethingHere;

void debug_render()
{ //pulled from my prototype
	somethingHere->blit({0,0},{10,10});
	camera = physicsObjects[camFocusedObj]->myPhysicsObject->getPosition();
//	camera.x -= 640/2;
//	camera.y -= 580/2;
	camera.x = camera.x/10;
	camera.y = camera.y/10;
	int camerax = 0;//(int) camera.x;
	int cameray = 0;//(int) camera.y;
	for (int i = 0; i < 70; i++) { 
		for (int o = 0; o < 70; o++) {
			//std::cout << "i " << i <<" t " << i+camerax << std::endl;
			if ( sGrid[i+camerax][o+cameray] != 0)
			{
				somethingHere->blit({i*10,o*10},{0,0});
				//std::cout << "nothingHere; ";
			} 
			else 
			{
				nothingHere->blit({i*10,o*10},{0,0});
				//std::cout << "somethingHere; ";
			}
		}
	}


}
	
 	


int main()
{
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
	while (true)
	{
		if(SDL_PollEvent(&event))
		{
			if (event.type == SDL_KEYDOWN)
			{
				keysHeld[event.key.keysym.sym] = true;
			}
			if (event.type == SDL_KEYUP)
			{
				keysHeld[event.key.keysym.sym] = false;
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
			std::cout <<"x: " << physicsObjects[camFocusedObj]->myPhysicsObject->velocity.x << std::endl;
			std::cout <<"y: " << physicsObjects[camFocusedObj]->myPhysicsObject->velocity.y << std::endl;
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
		if(isDebugRender) //ineffecient but who the fuck cares, it's a goddamn debug render.
		{
			debug_render();
		}

		SDL_RenderPresent(myRenderer);
		SDL_Delay(16);
	}

	return 0;
}
