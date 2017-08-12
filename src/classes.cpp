#include "classes.h"

#define SPRITE_DIR "../sprites/"

int handleEvents(SDL_Event* event)
{
	int succ = SDL_PollEvent(event);
	//TODO: Do various handlings
	return succ;
}


Player::Player(SDL_Renderer* renderer) : GE_PhysicsObject({0,0,0},{0,0,0},{0,0})
{
	this->renderer = renderer;
	renderObject = GE_CreateRenderedObject(renderer,SPRITE_DIR"coreReactor.png"); //TODO
	renderObject->size = {17,24};
	renderObject->animation = {0,0,17,27};


	grid = {0,0,300,300}; //TODO THIS NEEDS TO BE PROPERLY ADJUSTED!

	collisionRectangles[numCollisionRectangles] = {0,0,17,27}; //TODO
	numCollisionRectangles++;

	callCallbackUpdate = true;

	GE_addGlueSubject(&(renderObject->position),ID);
	printf("DONE\n");

	callCallbackAfterCollisionFunction = true;
}
bool Player::C_Update()
{
	//handle events
	SDL_Event event;
	while (handleEvents(&event) != 0)
	{
		if (event.type == SDL_KEYDOWN)
		{
			if (event.key.keysym.sym <= 323)
			{
				keysHeld[event.key.keysym.sym] = true;
			}
		}
		if (event.type == SDL_KEYUP)
		{
			if (event.key.keysym.sym <= 323)
			{
				keysHeld[event.key.keysym.sym] = false;
			}
		}
		if(event.key.keysym.sym == SDLK_SPACE)
		{
			Vector2r addToPosition = {0,-50,0};
			Vector2r addToVelocity = {0,-1,0};
			

			GE_Vector2RotationCCW(&addToPosition,position.r);
			GE_Vector2RotationCCW(&addToVelocity,position.r);
			StdBullet* mybullet = new StdBullet(renderer,position+addToPosition);
			GE_PhysicsObject* mybulletpo;
			GE_GetPhysicsObjectFromID(mybullet->ID,&mybulletpo);

			GE_AddVelocity(mybullet,addToVelocity);
			
		}
	
	}
	if(keysHeld[SDLK_w])
	{
		//GE_AddRelativeVelocity(cObj,{0,-0.5,0});
		GE_AddRelativeVelocity(this,{0,-0.5,0});
	}
	if(keysHeld[SDLK_s])
	{
		GE_AddRelativeVelocity(this,{0,0.5,0});
	}
	if(keysHeld[SDLK_d])
	{
		GE_AddRelativeVelocity(this,{0.5,0,0});
	}
	if(keysHeld[SDLK_a])
	{
		GE_AddRelativeVelocity(this,{-0.5,0,0});
	}
	if(keysHeld[SDLK_q])
	{
		GE_AddRelativeVelocity(this,{0,0,-0.25});
	}
	if(keysHeld[SDLK_e])
	{
		GE_AddRelativeVelocity(this,{0,0,0.25});
	}
	//temp
	if(keysHeld[SDLK_f])
	{
		velocity = {0,0,0};
	}



	

	return false;
}
bool Player::C_Collision(int victimID, int collisionRectangleID)
{
	//get physics object
	GE_PhysicsObject* victim;
	GE_GetPhysicsObjectFromID(victimID, &victim);
	
	//is it a bullet?
	if (victim->type == TYPE_DESTROYSUB)
	{	
		//What subsytem did it hit?
		//TODO TEMP
		
		if (true)
		{
			//It hit the core. Game over
			printf("TEMP GAMEOVER\n");
		}
	}


	return true;
	return false;
}

BulletType::BulletType(Vector2r position, Vector2r velocity, GE_Rectangle grid) : GE_PhysicsObject(position,velocity,grid)
{
	type = TYPE_DESTROYSUB;
}
BulletType::~BulletType()
{
}

StdBullet::StdBullet(SDL_Renderer* renderer, Vector2r position) : GE_PhysicsObject(position,{0,0,0},{25,25})//, BulletType(position,{0,0,0},{25,25}) //TODO proper sgrid adjust
{
	type = TYPE_DESTROYSUB;
	renderObject = GE_CreateRenderedObject(renderer,SPRITE_DIR"stdBulletPlayer.png"); //TODO
	renderObjectID = numRenderedObjects;
	renderObject->size = {4,10};
	renderObject->animation = {0,0,2,5};

	GE_addGlueSubject(&(renderObject->position),ID); //TODO delete on death. might cause performance issues with the bullet...
	
	collisionRectangles[numCollisionRectangles] = {0,0,4,10}; 
	numCollisionRectangles++;

	
	callCallbackAfterCollisionFunction = true;
}
StdBullet::~StdBullet()
{
	printf("!!!!!!!!!!!!!I WAS CALLED\n");

	pthread_mutex_lock(&RenderEngineMutex);
	deadRenderedObjects[renderObjectID] = true;
	delete renderObject;
	pthread_mutex_unlock(&RenderEngineMutex);
}
bool StdBullet::C_Collision(int victimID, int collisionRectangleID)
{
	//printf("Collision! I should be dead now!\n");
	return true; //Tell the physics engine to delete me.
}
