#include "classes.h"

#define SPRITE_DIR "../sprites/"


Subsystem::Subsystem(SDL_Renderer* renderer, std::string sprite, Vector2 size, GE_Rectangle animation, Vector2r relativePosition, int collisionRectangle)
{
	this->renderer = renderer;
	renderObject = GE_CreateRenderedObject(renderer,SPRITE_DIR+sprite); //TODO
	printf("Size: %f\n ",size.x);
	printf("anim %f\n",animation.w);

	renderObject->size = size;
	renderObject->animation = animation;

	this->relativePosition = relativePosition;

	this->collisionRectangle = collisionRectangle;
}
void Subsystem::CheckCollision(int collisionRectangle)
{
	if (collisionRectangle == this->collisionRectangle)
	{
		isOnline = false;
	}
}
void Subsystem::Update(Vector2r parrentPosition)
{
	printf("x %f\n",relativePosition.x);
	Vector2r rotationMatrix = relativePosition;

	#define __X 51
	#define __Y 49

	rotationMatrix.x = rotationMatrix.x-((__X)-renderObject->size.x/2);
	rotationMatrix.y = rotationMatrix.y-((__Y)-renderObject->size.x/2);


	GE_Vector2RotationCCW(&rotationMatrix,parrentPosition.r);

	rotationMatrix.x = (rotationMatrix.x - (renderObject->size.x/2))+51;
	rotationMatrix.y = (rotationMatrix.y - (renderObject->size.y/2))+49;

	renderObject->position = {rotationMatrix.x+parrentPosition.x,rotationMatrix.y+parrentPosition.y,parrentPosition.r};

	//renderObject->size = {3,3};
}
bool Subsystem::GetIsOnline()
{
	return isOnline;
}
	
		




int handleEvents(SDL_Event* event)
{
	int succ = SDL_PollEvent(event);
	//TODO: Do various handlings
	return succ;
}


#define bump() numIterableSubsystems++;
#define sizePlusDoubleSize(x,y)  {x*2,y*2}, {0,0,x,y}
#define positionDouble(x,y) {x*2,y*2,0}
//the above inserts TWO PARAMETERS

Player::Player(SDL_Renderer* renderer) : GE_PhysicsObject({100,0,0},{0,0,0},{0,0})
{
	this->renderer = renderer;
	renderObject = GE_CreateRenderedObject(renderer,SPRITE_DIR"WholeShipv2.png"); //TODO
	renderObject->size = {0,0};//{98,102};
	renderObject->animation = {0,0,49,51};


	grid = {0,0,300,300}; //TODO THIS NEEDS TO BE PROPERLY ADJUSTED!

	collisionRectangles[numCollisionRectangles] = {18,0,10,102}; //TODO
	numCollisionRectangles++;
	collisionRectangles[numCollisionRectangles] = {28,26,42,52}; //TODO
	numCollisionRectangles++;
	collisionRectangles[numCollisionRectangles] = {0,0,0,0}; //TODO
	numCollisionRectangles++;

	callCallbackUpdate = true;

	//GE_addGlueSubject(&(renderObject->position),ID);
	printf("DONE\n");

	callCallbackAfterCollisionFunction = true;

	iterableSubsystems[numIterableSubsystems] = new Subsystem(renderer,"playerThruster.png",sizePlusDoubleSize(5,51),positionDouble(9,0),0);
	bump();
	iterableSubsystems[numIterableSubsystems] = new Subsystem(renderer,"playerCoreReactor.png",sizePlusDoubleSize(21,26),positionDouble(14,13),0);
	bump();
	
	iterableSubsystems[numIterableSubsystems] = new Subsystem(renderer,"playerLShield.png",sizePlusDoubleSize(9,19),positionDouble(1,15),0);
	bump();
	
	
	iterableSubsystems[numIterableSubsystems] = new Subsystem(renderer,"playerRShield.png",sizePlusDoubleSize(9,19),{31,2},0);
	bump();

}
//double fwdMove;
//#define unrealisticMove true
bool Player::C_Update()
{
	//Update subsystem positions
		
	for (int i=0;i<numIterableSubsystems;i++)
	{
		iterableSubsystems[i]->Update(position);
	}
	//TODO temp
	renderObject->position = this->position;


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
		//GE_AddRelativeVelocity(cObj,{0,-0.5,0});
#ifdef unrealisticMove
		fwdMove = fwdMove-0.5;
#else
		GE_AddRelativeVelocity(this,{0,-0.5,0});
#endif
	
	}
	if(keysHeld[SDLK_s])
	{
#ifdef unrealisticMove
		fwdMove = fwdMove+0.5;
#else
		GE_AddRelativeVelocity(this,{0,0.5,0});
#endif

	}
	if(keysHeld[SDLK_d])
	{
		GE_AddRelativeVelocity(this,{0.05,0,0});
	}
	if(keysHeld[SDLK_a])
	{
		GE_AddRelativeVelocity(this,{-0.05,0,0});
	}
	if(keysHeld[SDLK_q])
	{
		GE_AddRelativeVelocity(this,{0,0,-0.05});
	}
	if(keysHeld[SDLK_e])
	{
		GE_AddRelativeVelocity(this,{0,0,0.05});
	}
	//temp
	if(keysHeld[SDLK_f])
	{
		velocity = {0,0,0};
#ifdef unrealisticMove
		fwdMove = 0;
#endif
	}

#ifdef unrealisticMove
	velocity = {0,0,velocity.r};
	GE_AddRelativeVelocity(this,{0,fwdMove,0});
#endif


	

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
	renderObject->size = {2,10};
	renderObject->animation = {0,0,1,5};

	GE_addGlueSubject(&(renderObject->position),ID); //TODO delete on death. might cause performance issues with the bullet...
	
	collisionRectangles[numCollisionRectangles] = {0,0,4,10}; 
	numCollisionRectangles++;

	
	callCallbackAfterCollisionFunction = true;
	printf("I AM A BULLET WHO IS FAKEID #%d\n",ID);
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
