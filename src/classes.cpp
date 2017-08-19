#include "classes.h"

#define SPRITE_DIR "../sprites/"


Player* targetPlayer;

Subsystem::Subsystem(SDL_Renderer* renderer, std::string sprite, Vector2 size, GE_Rectangle animation, Vector2r relativePosition, int collisionRectangle)
{
	printf("stt\n");
	this->renderer = renderer;
	renderObject = GE_CreateRenderedObject(renderer,SPRITE_DIR+sprite); //TODO
	printf("Size: %f\n ",size.x);
	printf("anim %f\n",animation.w);

	renderObject->size = size;
	renderObject->animation = animation;

	this->relativePosition = relativePosition;

	this->collisionRectangle = collisionRectangle;
	
	this->isOnline = true;

	this->level = 0; //TODO

	printf("a\n");
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
	//printf("x %f\n",relativePosition.x);
	Vector2r rotationMatrix = relativePosition;

	#define __X -51
	#define __Y -49

	rotationMatrix.x = (rotationMatrix.x+__X)+(renderObject->size.x/2);
	rotationMatrix.y = (rotationMatrix.y+__Y)+(renderObject->size.y/2);


	GE_Vector2RotationCCW(&rotationMatrix,parrentPosition.r);

	rotationMatrix.x = (rotationMatrix.x-__X)-(renderObject->size.x/2);
	rotationMatrix.y = (rotationMatrix.y-__Y)-(renderObject->size.y/2);

	//rotationMatrix.x = rotationMatrix.x+__X;
	//rotationMatrix.y = rotationMatrix.y+__Y;
//#define CAMERA_ROTATE
#ifdef CAMERA_ROTATE
	rotationMatrix = relativePosition;
#endif

	
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


	//TODO MOVE TO GAME ENGINE CODE IN GE_INIT
	srand(time(NULL));

	this->renderer = renderer;
	renderObject = GE_CreateRenderedObject(renderer,SPRITE_DIR"WholeShipv2.png"); //TODO
	renderObject->size = {0,0};//{98,102};
	renderObject->animation = {0,0,49,51};


	grid = {0,0,98,102}; 


	//If I'd more time I'd make this... uh, better in every way.

	collisionRectangles[numCollisionRectangles] = {18,0,10,102}; 
	numCollisionRectangles++;
	collisionRectangles[numCollisionRectangles] = {28,26,42,52}; 
	numCollisionRectangles++;
	collisionRectangles[numCollisionRectangles] = {0,30,4,38}; 
	numCollisionRectangles++;
	collisionRectangles[numCollisionRectangles] = {90,30,4,38}; 
	numCollisionRectangles++;
	collisionRectangles[numCollisionRectangles] = {70,0,10,102}; 
	numCollisionRectangles++;
	collisionRectangles[numCollisionRectangles] = {28,70,42,22}; 
	numCollisionRectangles++;
	collisionRectangles[numCollisionRectangles] = {28,4,42,22};  //~~
	numCollisionRectangles++;
	collisionRectangles[numCollisionRectangles] = {10,42,9,18};  
	numCollisionRectangles++;
	collisionRectangles[numCollisionRectangles] = {80,42,9,18};  
	numCollisionRectangles++;

	callCallbackUpdate = true;

	//GE_addGlueSubject(&(renderObject->position),ID);
	printf("DONE\n");

	callCallbackAfterCollisionFunction = true;


	numIterableSubsystems = 0;

	iterableSubsystems[numIterableSubsystems] = new Subsystem(renderer,"playerThruster.png",sizePlusDoubleSize(5,51),positionDouble(9,0),0);
	printf("??\n");
	bump();
	iterableSubsystems[numIterableSubsystems] = new Subsystem(renderer,"playerCoreReactor.png",sizePlusDoubleSize(21,26),positionDouble(14,13),1);
	bump();
	iterableSubsystems[numIterableSubsystems] = new Subsystem(renderer,"playerLShield.png",sizePlusDoubleSize(9,19),positionDouble(0,15),2);
	bump();
	iterableSubsystems[numIterableSubsystems] = new Subsystem(renderer,"playerRShield.png",sizePlusDoubleSize(9,19),positionDouble(39,15),3);
	bump();
	iterableSubsystems[numIterableSubsystems] = new Subsystem(renderer,"playerThruster.png",sizePlusDoubleSize(5,51),positionDouble(35,0),4);
	bump();
	iterableSubsystems[numIterableSubsystems] = new Subsystem(renderer,"playerLifeSupport.png",sizePlusDoubleSize(21,14),positionDouble(14,35),5);
	bump();
	iterableSubsystems[numIterableSubsystems] = new Subsystem(renderer,"playerShipHead.png",sizePlusDoubleSize(21,11),positionDouble(14,2),6);  //~~
	bump();
	iterableSubsystems[numIterableSubsystems] = new Subsystem(renderer,"playerLSmallTurret.png",sizePlusDoubleSize(4,9),positionDouble(5,21),7); 
	bump();
	iterableSubsystems[numIterableSubsystems] = new Subsystem(renderer,"playerRSmallTurret.png",sizePlusDoubleSize(4,9),positionDouble(40,21),8);
	bump();


	nextTickCanShoot = 0;



	targetPlayer = this;

}
//double fwdMove;
//#define unrealisticMove true



void ShootBullet(SDL_Renderer* renderer, GE_PhysicsObject* host, Vector2r addToVelocity, Vector2r addToPosition)
{
	

	GE_Vector2RotationCCW(&addToPosition,host->position.r);
	GE_Vector2RotationCCW(&addToVelocity,host->position.r);
	StdBullet* mybullet = new StdBullet(renderer,host->position+addToPosition);
	GE_PhysicsObject* mybulletpo;
	GE_GetPhysicsObjectFromID(mybullet->ID,&mybulletpo);

	GE_AddVelocity(mybullet,addToVelocity);
}



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
			if(event.key.keysym.sym == SDLK_SPACE && ticknum >= nextTickCanShoot )
			{
				ShootBullet(renderer,this,{0,-10,0},{12,-30,0});
				nextTickCanShoot = ticknum +30;
			}

			if (event.key.keysym.sym == SDLK_p)
			{
				printf(" kjfaskl jasdklfj sdkla;fjasdkl;ifjalsjk;\n");
				Enemie* un = new Enemie(renderer, {position.x+100,position.y+100,0},1);
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
	if (keysHeld[SDLK_ESCAPE])
	{
		//TODO GOD WHY
		exit(0);
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
			printf("TEMP GAMEOVER\n-----------\n");
			//TODO GOD WHY AGAIN
			exit(0);
		}
	}


	return false;
}




Enemie::Enemie(SDL_Renderer* renderer, Vector2r position, int level) : GE_PhysicsObject(position,{0,0,0},{38,42})
{
	this->renderer = renderer;

	this->level = level;

	type = TYPE_ENEMY;
	
	renderObject = GE_CreateRenderedObject(renderer,SPRITE_DIR"enemy.png"); 
	renderObjectID = numRenderedObjects;
	renderObject->size = {38,42};
	renderObject->animation = {0,0,19,21};

	GE_addGlueSubject(&(renderObject->position),ID); 
	
	collisionRectangles[numCollisionRectangles] = {0,0,38,42}; 
	numCollisionRectangles++;

	
	callCallbackAfterCollisionFunction = true;
	callCallbackUpdate = true;

}
Enemie::~Enemie()
{
	pthread_mutex_lock(&RenderEngineMutex);
	deadRenderedObjects[renderObjectID] = true;
	delete renderObject;
	pthread_mutex_unlock(&RenderEngineMutex);
}
bool Enemie::C_Update()
{


	//based on: https://gamedev.stackexchange.com/a/124803
	//printf("enemy up\n");
	//check if we're in the radius of our target player
	if ( GE_Distance(this->position.x, this->position.y, targetPlayer->position.x, targetPlayer->position.y) < 1000)
	{
		//printf("can see\n");
		//turn toward the player
		

		double rotaryDistance = (this->position.r/DEG_TO_RAD)-(atan2(targetPlayer->position.x-this->position.x,targetPlayer->position.y-this->position.y));
		//printf("rdist %f\n",rotaryDistance);
		if (rotaryDistance < - 3.1415926 || (rotaryDistance > 0 && rotaryDistance < 3.1415926 ))
		{
			this->position.r = this->position.r + 5;
		}
		else	
		{
			this->position.r = this->position.r - 5;
		}
		if (ticknum % 60 == 0 )
		{
			//spawn bullet
			//ShootBullet(renderer,this,{0,-1,0},{0,-40,0});
		}
	}
	
	


	return false;
}
bool Enemie::C_Collision(int victimID, int collisionRectangleID)
{
	printf("enemy coll\n");
	//SDL_Delay(1600);
	//get physics object
	GE_PhysicsObject* victim;
	GE_GetPhysicsObjectFromID(victimID, &victim);
	
	//is it a bullet?
	if (victim->type == TYPE_DESTROYSUB)
	{
		//bye
		return true;
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

StdBullet::StdBullet(SDL_Renderer* renderer, Vector2r position) : GE_PhysicsObject(position,{0,0,0},{2,10})
{
	type = TYPE_DESTROYSUB;
	renderObject = GE_CreateRenderedObject(renderer,SPRITE_DIR"stdBulletPlayer.png"); 
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
