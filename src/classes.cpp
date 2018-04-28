#include "classes.h"

#include <SDL2/SDL.h>
#include <stdlib.h>     /* srand, rand */
#include <string>
#include <cmath>
#include <stack>

//Local includes
#include "vector2.h"
#include "gluePhysicsObject.h"
#include "minimap.h"
#include "inventory.h"
#include "raycast.h"

#include "GeneralEngineCPP.h"

//Debug includes
#include "debugRenders.h"

bool typeInGroup[TYPE_COUNT][GROUP_COUNT];



Subsystem::Subsystem(SDL_Renderer* renderer, std::string sprite, Vector2 size, GE_Rectangle animation, Vector2r relativePosition, int collisionRectangle, std::string name, Vector2* parrentGrid)
{
	printf("stt\n");
	this->renderer = renderer;
	renderObject = GE_CreateRenderedObject(renderer,sprite); //TODO
	this->spriteName = sprite;

	printf("Size: %f\n ",size.x);
	printf("anim %f\n",animation.w);

	renderObject->size = size;
	renderObject->animation = animation;

	this->relativePosition = relativePosition;

	this->collisionRectangle = collisionRectangle;
	
	this->health = 100;

	this->level = 0; //TODO

	this->parrentGrid = parrentGrid;

	this->position = {0,0,0};

	this->glueTarget = GE_addGlueSubject((&(renderObject->position)),&position,GE_PULL_ON_PHYSICS_TICK,sizeof(Vector2r));

	this->name = name;




	printf("a\n");
}
void Subsystem::CheckCollision(int checkCollisionRectangle)
{
	//printf("muh collision rect %d this one is %d\n",this->collisionRectangle, checkCollisionRectangle);
	if (checkCollisionRectangle == this->collisionRectangle)
	{
		//printf("yuppers\n");
		health -= 25; //TODO more dynamicness
		if (!GetIsOnline())
		{
			GE_ChangeRenderedObjectSprite(renderObject,(spriteName+"_broken"));
		}
		
			
	}
}
void Subsystem::Update(Vector2r parrentPosition)
{

	if (true)//GetIsOnline())
	{
		double halfrectw = parrentGrid->x/2;
		double halfrecth = parrentGrid->y/2;
		//printf("x %f\n",(halfrectw));
		Vector2r rotationMatrix = relativePosition;

		rotationMatrix.x = (rotationMatrix.x)+(renderObject->size.x/2)-halfrectw;
		rotationMatrix.y = (rotationMatrix.y)+(renderObject->size.y/2)-halfrecth;
		GE_Vector2RotationCCW(&rotationMatrix,parrentPosition.r);
		position = {rotationMatrix.x+parrentPosition.x,rotationMatrix.y+parrentPosition.y,parrentPosition.r}; //set our value because it will be updated at the proper time by the glue we added on initialization
	}
	else
	{
		//position = {-999999,-999999,-999999}; //TODO: Replace the image of us with a "broken part" image instead"

	}

}
bool Subsystem::GetIsOnline()
{
	return (health > 0);
}
int Subsystem::GetLevel()
{
	return level;
}
void Subsystem::SetLevel(int level)
{
	this->level = level;
}
		
Subsystem::~Subsystem()
{
	GE_ScheduleFreeRenderedObject(renderObject);
	GE_FreeGlueObject(glueTarget);
}




const std::string Player::name = "Player";

#define bump() i++;
#define sizePlusDoubleSize(x,y)  {x*2,y*2}, {0,0,x,y}
#define positionDouble(x,y) {x*2,y*2,0}
//the above inserts TWO PARAMETERS
Player::Player(SDL_Renderer* renderer) : GE_PhysicsObject({0,0,0},{0,0,0},25)
{



	this->renderer = renderer;
	/*renderObject = GE_CreateRenderedObject(renderer,SPRITE_DIR"WholeShipv2"); //TODO
	renderObject->size = {98,102};
	renderObject->animation = {0,0,49,51};*/


	//grid = {0,0,98,102}; 


	//If I'd more time I'd make this... uh, better in every way.

	addCollisionRectangle(GE_Rectangler{18,0,0,10,102});
	addCollisionRectangle(GE_Rectangler{28,26,0,42,52}); 
	addCollisionRectangle(GE_Rectangler{0,30,0,4,38}); 
	addCollisionRectangle(GE_Rectangler{90,30,0,4,38}); 
	addCollisionRectangle(GE_Rectangler{70,0,0,10,102}); 
	addCollisionRectangle(GE_Rectangler{28,70,0,42,22}); 
	addCollisionRectangle(GE_Rectangler{28,4,0,42,22});  //~~
	addCollisionRectangle(GE_Rectangler{10,42,0,9,18});  
	addCollisionRectangle(GE_Rectangler{80,42,0,9,18});  

	callCallbackUpdate = true;

	//GE_addGlueSubject(&(renderObject->position),ID);
	printf("DONE\n");

	callCallbackAfterCollisionFunction = true;

	int i=0;


	iterableSubsystems[i] = new Subsystem(renderer,"playerThruster",sizePlusDoubleSize(5,51),positionDouble(9,0),0,"Left Thruster",&grid);
	bump();
	iterableSubsystems[i] = new Subsystem(renderer,"playerCoreReactor",sizePlusDoubleSize(21,26),positionDouble(14,13),1,"Core Reactor",&grid);
	bump();
	iterableSubsystems[i] = new Subsystem(renderer,"playerLShield",sizePlusDoubleSize(9,19),positionDouble(0,15),2,"Left Shield",&grid);
	bump();
	iterableSubsystems[i] = new Subsystem(renderer,"playerRShield",sizePlusDoubleSize(9,19),positionDouble(39,15),3,"Right Shield",&grid);
	bump();
	iterableSubsystems[i] = new Subsystem(renderer,"playerThruster",sizePlusDoubleSize(5,51),positionDouble(35,0),4,"Right Thruster",&grid);
	bump();
	iterableSubsystems[i] = new Subsystem(renderer,"playerLifeSupport",sizePlusDoubleSize(21,14),positionDouble(14,35),5,"Life Support",&grid);
	bump();
	iterableSubsystems[i] = new Subsystem(renderer,"playerShipHead",sizePlusDoubleSize(21,11),positionDouble(14,2),6,"Bridge",&grid);  //~~
	bump();
	iterableSubsystems[i] = new Subsystem(renderer,"playerLSmallTurret",sizePlusDoubleSize(4,9),positionDouble(5,21),7,"Left Turret",&grid); 
	bump();
	iterableSubsystems[i] = new Subsystem(renderer,"playerRSmallTurret",sizePlusDoubleSize(4,9),positionDouble(40,21),8,"Right Turret",&grid);
	bump();


	nextTickCanShoot = 0;




	iterableSubsystems[0]->SetLevel(4);

	this->threadedEventStack = GE_ThreadedEventStack();


	this->inventory = new Inventory(2000,this);



	this->inventory->storage.push_back(ItemStack{ITEM_NAMES::IRON,63});
	this->inventory->storage.push_back(ItemStack{ITEM_NAMES::IRON,63});
	this->inventory->storage.push_back(ItemStack{ITEM_NAMES::DUCT_TAPE,63});
	this->inventory->storage.push_back(ItemStack{ITEM_NAMES::IRON,63});

	type = TYPE_PLAYER;

	dampeners = false;

	C_Update();

	//tracker = GE_AddTrackedObject(type,this);
	
}
void Player::C_Destroyed()
{
	GE_RemoveTrackedObject(tracker);
	for (int i=0;i<=numIterableSubsystems;i++)
	{
		delete iterableSubsystems[i];
	}
}
Player::~Player()
{
	printf("Deleting the player!!!\n");
}
void Player::serialize(char** buffer, size_t* bufferUsed, size_t* bufferSize)
{
	GE_Serialize(&position,buffer,bufferUsed,bufferSize);
	GE_Serialize(&velocity,buffer,bufferUsed,bufferSize);
}
Player* Player::unserialize(char* buffer, size_t* bufferUnserialized,int version)
{
	Vector2r* _position = GE_Unserialize<Vector2r*>(buffer,bufferUnserialized,version);
	Vector2r* _velocity = GE_Unserialize<Vector2r*>(buffer,bufferUnserialized,version);

	auto newObj = new Player(GE_DEBUG_Renderer);
	newObj->position = *_position;
	newObj->velocity = *_velocity;
	delete _position;
	delete _velocity;
	return newObj;
}
GE_PhysicsObject* Player::spawnFromLevelEditor(SDL_Renderer* renderer, Vector2r position)
{
	auto myplayer = static_cast<GE_PhysicsObject*>(new Player(renderer));
	myplayer->position = position;
	return myplayer;
}
void Player::setPosition(Vector2r position)
{
	this->position = position;
	C_Update();
}



void ShootBullet(SDL_Renderer* renderer, GE_PhysicsObject* host, Vector2r addToVelocity, Vector2r addToPosition, bool isPlayer)
{
	addToPosition.x = addToPosition.x;// - host->grid.x/2;
	addToPosition.y = addToPosition.y;// - host->grid.x/2;

	GE_Vector2RotationCCW(&addToPosition,host->position.r);
	GE_Vector2RotationCCW(&addToVelocity,host->position.r);

	addToPosition.x = addToPosition.x;// + host->grid.x/2;
	addToPosition.y = addToPosition.y;// + host->grid.x/2;

	StdBullet* mybullet = new StdBullet(renderer,host->position+addToPosition,(isPlayer) ? ("stdBulletPlayer") : ("stdBulletEnemy"));

	addToVelocity.x += host->velocity.x; //avoid use of vector addition because we don't want the rotation to be added.
	addToVelocity.y += host->velocity.y;

	GE_AddVelocity(mybullet,addToVelocity);
}


class tester : public GE_PhysicsObject
{
	public:
		tester(SDL_Renderer* renderer,Vector2r position) : GE_PhysicsObject(position,{0,0,0},25)
		{
			renderObject = GE_CreateRenderedObject(renderer,"simple"); 
			renderObject->size = {8*2,9*2};
			renderObject->animation = {0,0,8,9};
			addCollisionRectangle(GE_Rectangler{0,0,0,8*2,9*2});

			GE_LinkVectorToPhysicsObjectPosition(this,&(renderObject->position)); 

			this->type = TYPE_RESERVED;

			tracker = GE_AddTrackedObject(type,this);


			velocity.x = 1;

		}
		void C_Destroyed()
		{
			GE_RemoveTrackedObject(tracker);
			GE_FreeRenderedObject(renderObject);
		}
		void serialize(char** buffer, size_t* bufferUsed, size_t* bufferSize)
		{
			GE_Serialize(&(this->position),buffer,bufferUsed,bufferSize);
		}
		static tester* unserialize(char* buffer, size_t* bufferUnserialized,int version)
		{
			Vector2r* _pos = GE_Unserialize<Vector2r*>(buffer,bufferUnserialized, version);	
			auto newclass = new tester(GE_DEBUG_Renderer,*_pos);
			delete _pos;
			return newclass;
		}

	private:
		GE_RenderedObject* renderObject;
		GE_TrackedObject* tracker;


};



const double turnSpeed= 0.0008726646;
const double moveSpeed = 0.25;
const double strafeSpeed = 0.05;

#include <iostream>
#include <fstream>

char* TEMP;
size_t TEMPused;
size_t TEMPsize;
#include "FS.h"
bool Player::C_Update()
{
	//Update subsystem positions
		
	for (int i=0;i<=numIterableSubsystems;i++)
	{
		iterableSubsystems[i]->Update(position);
		if (!iterableSubsystems[i]->GetIsOnline())
		{
			collisionRectangles[i] = {0,0,0,0,0};
		}
	}
	//TODO temp
	//renderObject->position = this->position;


	if (GetIsOnline())
	{

		bool moveForward = false;
		bool moveBackward = false;
		bool moveLeft = false;
		bool moveRight = false;
		bool turnLeft = false;
		bool turnRight = false;


		//handle events
		SDL_Event event;

		threadedEventStack.PourInputStackToOutput();
		while (threadedEventStack.canGetEvent())
		{
			event = threadedEventStack.getEvent();
			if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym <= 323)
				{
					keysHeld[event.key.keysym.sym] = true;
				}
				if(event.key.keysym.sym == SDLK_SPACE)// && ticknum >= nextTickCanShoot )
				{
					//TODO at some point this should be based on a value made in an editor of some sort.
					if (iterableSubsystems[7]->GetIsOnline()) ShootBullet(renderer,this,{0,-10,0},{12-(grid.x/2),-60,0},true);
					if (iterableSubsystems[8]->GetIsOnline()) ShootBullet(renderer,this,{0,-10,0},{86-(grid.x/2),-60,0},true); //(49*2)-12=86 , because the other turret is at the opposite side of the ship
					nextTickCanShoot = ticknum +(30-(pow(iterableSubsystems[0]->GetLevel(),2)));
				}

				if (event.key.keysym.sym == SDLK_p)
				{
					printf(" kjfaskl jasdklfj sdkla;fjasdkl;ifjalsjk;\n");
					new Enemie(renderer, {position.x+100,position.y+100,0},1);
				}
				if (event.key.keysym.sym == SDLK_u)
				{

					GE_RenderedObject* ro = GE_CreateRenderedObject(this->renderer,"simple.bmp");	
					ro->size = {25,25};
					ro->animation = {0,0,8,9};
					

					GE_PhysicsObject* me = new GE_PhysicsObject(Vector2r{this->position.x+50,this->position.y+150,0},Vector2r{25,25,0},25);
					me->addCollisionRectangle(GE_Rectangler{0,0,0,25,25});
					me->callCallbackBeforeCollisionFunction = true;

					GE_LinkVectorToPhysicsObjectPosition(me,&(ro->position));

				}
				if (event.key.keysym.sym == SDLK_i)
				{
					new tester(renderer,position+Vector2r{50,150,0});
				}
				if (event.key.keysym.sym == SDLK_k)
				{
					//force push everything arround me
					for (auto obj : GE_GetObjectsInRadius({position.x,position.y},1000))
					{
						obj->velocity.x += 0.5;
					}
				}
				if (event.key.keysym.sym == SDLK_n) //tmp save
				{
					GE_FreeSerializeString(TEMP);
					TEMP = GE_SerializedTrackedObjects(&TEMPused,&TEMPsize);
					std::ofstream myfile;
					myfile.open ("/tmp/REMOVE-testsave2",std::ios::out|std::ios::binary|std::ios::ate);
					myfile.write(TEMP,TEMPused);
					myfile.close();
				}
				if (event.key.keysym.sym == SDLK_b) //tmp loadfilesave
				{

					std::streampos size;
					char * memblock;

					/*
					std::ifstream file ("/tmp/REMOVE-testsave2",std::ios::in|std::ios::binary|std::ios::ate);
					if (file.is_open())
					{
						size = file.tellg();
						memblock = new char [size];
						file.seekg (0, std::ios::beg);
						file.read (memblock, size);
						file.close();

						std::cout << "the entire file content is in memory" << std::endl;

						delete[] memblock;
					}
					else 
					{
						
						std::cout << "Unable to open file" << std::endl;		
						SDL_Delay(10000);
						
					}

					TEMP = memblock;
					*/
					TEMP = GE_GetCharArrayFromFileString(GE_ReadAllFromFile("/tmp/REMOVE-tempsave2"));
				}
				if (event.key.keysym.sym == SDLK_m) //tmp load
				{
					
					GE_UnserializeTrackedObjects(TEMP);
				}
				if (event.key.keysym.sym == SDLK_f)
				{
					dampeners = !dampeners;
					velocity = {0,0,0}; //TODO temp - fix once thruster placement is variable as well.
				}
			}
			if (event.key.keysym.sym == SDLK_j)
			{
				PhysicsDelaySeconds = 5;
			}
			if (event.key.keysym.sym == SDLK_l)
			{
				PhysicsDelaySeconds = 0.16667;
			}
			if (event.key.keysym.sym == SDLK_h)
			{
				PhysicsDelaySeconds = 60;
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
			moveForward = true;
		}
		if(keysHeld[SDLK_s])
		{
			moveBackward = true;
		}
		if(keysHeld[SDLK_d])
		{
			moveRight = true;
		}
		if(keysHeld[SDLK_a])
		{
			moveLeft = true;
		}
		if(keysHeld[SDLK_q])
		{
			turnLeft = true;
		}
		if(keysHeld[SDLK_e])
		{
			turnRight = true;
		}


		if(keysHeld[SDLK_x])
		{
			printf("posx %f\n",position.x);
			printf("posy %f\n",position.y);
			printf("r %f\n",position.r);
		}
		if (keysHeld[SDLK_n])
		{
			velocity = {-1,0,0};
		}



		if (dampeners)
		{
				
		}


		if(moveForward)
		{
			//account for damaged thrusters -- if one is offline, we're going to spin when we move.
			if (iterableSubsystems[0]->GetIsOnline()) GE_AddRelativeVelocity(this,{0,-moveSpeed,turnSpeed});
			if (iterableSubsystems[4]->GetIsOnline()) GE_AddRelativeVelocity(this,{0,-moveSpeed,-turnSpeed});
		}
		if (moveBackward)
		{
			if (iterableSubsystems[0]->GetIsOnline()) GE_AddRelativeVelocity(this,{0,moveSpeed,-turnSpeed});
			if (iterableSubsystems[4]->GetIsOnline()) GE_AddRelativeVelocity(this,{0,moveSpeed,turnSpeed});
		}
		if (moveLeft)
		{
			GE_AddRelativeVelocity(this,{-strafeSpeed,0,0});
		}
		if (moveRight)
		{
			GE_AddRelativeVelocity(this,{strafeSpeed,0,0});
		}
		if (turnLeft)
		{
			GE_AddRelativeVelocity(this,{0,0,-turnSpeed});
		}
		if (turnRight)
		{
			GE_AddRelativeVelocity(this,{0,0,turnSpeed});
		}




	}
	//GE_DEBUG_TextAt(std::to_string(position.r),Vector2{0,0});


	

	return false;
}
bool Player::C_Collision(GE_PhysicsObject* victim, int collisionRectangleID)
{
	//get physics object
	
	//is it a bullet?
	if (victim->type == TYPE_DESTROYSUB)
	{	
		//What subsytem did it hit?
		for (int i = 0;i<=numIterableSubsystems;i++)
		{
			iterableSubsystems[i]->CheckCollision(collisionRectangleID);
		}


	}


	return false;
}
bool Player::GetIsOnline()
{
	return (iterableSubsystems[1]->GetIsOnline() && iterableSubsystems[5]->GetIsOnline() && iterableSubsystems[6]->GetIsOnline());
}




Enemie::Enemie(SDL_Renderer* renderer, Vector2r position, int level) : GE_PhysicsObject(position,{0,0,0},25)
{
	this->renderer = renderer;

	this->level = level;

	type = TYPE_ENEMY;
	
	renderObject = GE_CreateRenderedObject(renderer,"enemy"); 
	renderObject->size = {38,42};
	renderObject->animation = {0,0,19,21};

	GE_LinkVectorToPhysicsObjectPosition(this,&(renderObject->position)); 
	
	addCollisionRectangle(GE_Rectangler{0,0,0,38,42}); 

	
	callCallbackAfterCollisionFunction = true;
	callCallbackUpdate = true;
	lastTimeShotTick = ticknum;

	foundPlayer = false;

	tracker = GE_AddTrackedObject(type,this);
}
Enemie::~Enemie(){}
void Enemie::C_Destroyed()
{
	GE_ScheduleFreeMinimapTarget(renderObject);
	GE_RemoveTrackedObject(tracker);
	GE_ScheduleFreeRenderedObject(renderObject);
}
void Enemie::serialize(char** buffer, size_t* bufferUsed, size_t* bufferSize)
{
	GE_Serialize(&position,buffer,bufferUsed,bufferSize);
	GE_Serialize(&velocity,buffer,bufferUsed,bufferSize);
}
Enemie* Enemie::unserialize(char* buffer, size_t* bufferUnserialized,int version)
{
	printf("~~ENEMIE\n");
	Vector2r* _position = GE_Unserialize<Vector2r*>(buffer,bufferUnserialized,version);
	Vector2r* _velocity = GE_Unserialize<Vector2r*>(buffer,bufferUnserialized,version);

	printf("~~ENEMIE2\n");
	auto newObj = new Enemie(GE_DEBUG_Renderer,Vector2r{0,0,0},0);
	newObj->position = *_position;
	newObj->velocity = *_velocity;
	delete _position;
	delete _velocity;
	printf("~~ENEMIE3\n");
	return newObj;
}


bool canStop(double velocity, double distance, double acceleration)
{
	double timeToDestination = (std::abs(distance)<0.04) ? 0 : std::abs(   ( -velocity+( ((distance >= 0)? -1 : 1)*std::sqrt(std::abs( std::pow(velocity,2)+(4*0.5*acceleration*(distance)) ))) )/(acceleration)   );
	double timeToStop = std::abs(velocity/acceleration); 
	return timeToDestination>timeToStop;
}

bool Enemie::C_Update()
{

	GE_PhysicsObject* targetPlayer;
	bool foundTarget = false;
	std::vector<GE_PhysicsObject*> potentialBlock;
	
	//find stuff in our radius
	for (GE_PhysicsObject* obj : GE_GetObjectsInRadius({position.x,position.y},1000))
	{
		if (obj != this)
		{
			if (obj->type == TYPE_PLAYER)
			{
				targetPlayer = obj;
				foundTarget = true;
			}
			potentialBlock.insert(potentialBlock.end(), obj);
		}
	}
	if (foundTarget)
	{
		bool playerInSight = false;
		//we're only worried about our sight to the player.
		Vector2 endCast = {targetPlayer->position.x,targetPlayer->position.y}; 
		auto ret = GE_Raycast(Vector2{position.x,position.y},endCast,potentialBlock);

#ifdef PHYSICS_DEBUG_SLOWRENDERS
		GE_DEBUG_DrawLine_PhysicsPosition(Vector2{position.x,position.y},endCast,GE_Color{0xff,0x00,0x00,0x00});
		GE_DEBUG_DrawRect_PhysicsPosition({ret.position.x,ret.position.y,2,2},{0x00,0xff,0x00,0x00});
#endif

		//ret.victim->velocity.x += 0.1;
		if (ret.victim == targetPlayer)
		{
			lastFoundPlayer = ret.position;
			foundPlayer = true;
			playerInSight = true;
		}


		//based on: https://gamedev.stackexchange.com/a/124803
		double rotaryDistance = GE_GetRotationalDistance(this->position,lastFoundPlayer)-M_PI;
		double distanceToPlayer = GE_Distance(this->position.x, this->position.y, lastFoundPlayer.x, lastFoundPlayer.y);

		
		if (playerInSight)
		{
			GE_LinkMinimapToRenderedObject(renderObject,{0xA0,0x00,0x00,0xFF});
			if (ticknum-lastTimeShotTick >= 60)
			{	
				//printf("cs\n");
				if (rotaryDistance < 0.3 && rotaryDistance > -0.3)
				{
					lastTimeShotTick = ticknum;
					ShootBullet(renderer,this,{0,-7,0},{0,-grid.y/2,0},false);
				}
			}
		}
		else if (distanceToPlayer > 3000)
		{
			//foundPlayer = false;
			GE_ScheduleFreeMinimapTarget(renderObject);
		}
		if (foundPlayer)
		{
			
			double acceleration_r;
			if (rotaryDistance < 0 || (rotaryDistance > M_PI && rotaryDistance < TWO_PI ))
			{
				acceleration_r = 0.002;
			}
			else
			{
				acceleration_r = -0.02;
			}
			//double timeToDestination = (M_PI-rotaryDistance)/acceleration_r;


			//printf("time stop %f\n",timeToStop);
			if(!canStop(velocity.r,rotaryDistance,acceleration_r))
			{
				if (velocity.r > 0)
				{
					velocity.r -= std::abs(acceleration_r);
				}
				else
				{
					velocity.r += std::abs(acceleration_r);
				}
			}
			else
			{
				velocity.r += acceleration_r;
			}

			//accelerate towards the player

			if (rotaryDistance < 0.3 && rotaryDistance > -0.3 )
			{
				//GE_AddRelativeVelocity(this,{0,-0.1,0});
			}

		}
	}
	

	return false;
}






const std::string Enemie::name = "Enemy";
bool Enemie::C_Collision(GE_PhysicsObject* victim, int collisionRectangleID)
{
	printf("enemy coll\n");
	//SDL_Delay(1600);
	//get physics object
	
	//is it a bullet?
	if (victim->type == TYPE_DESTROYSUB)
	{
		//bye
		return true;
	}
		

	return false;
}
GE_PhysicsObject* Enemie::spawnFromLevelEditor(SDL_Renderer* renderer, Vector2r position)
{
	return static_cast<GE_PhysicsObject*>(new Enemie(GE_DEBUG_Renderer,position,0));
}


BulletType::BulletType(Vector2r position, Vector2r velocity, double mass) : GE_PhysicsObject(position,velocity, mass)
{
	type = TYPE_DESTROYSUB;
}
bool BulletType::C_Collision(GE_PhysicsObject* victim, int collisionRectangleID)
{
	//printf("Collision! I should be dead now!\n");
	return true; //Tell the physics engine to delete me.
}
BulletType::~BulletType()
{
}


StdBullet::StdBullet(SDL_Renderer* renderer, Vector2r position, const char* spriteName) : BulletType(position,{0,0,0},STD_BULLET_MASS)
{
	renderObject = GE_CreateRenderedObject(renderer,spriteName); 
	renderObject->size = {2,10};
	renderObject->animation = {0,0,1,5};

	GE_LinkVectorToPhysicsObjectPosition(this,&(renderObject->position)); 
	
	addCollisionRectangle(GE_Rectangler{0,0,0,4,10}); 

	
	callCallbackAfterCollisionFunction = true;
}
void StdBullet::C_Destroyed()
{
	GE_ScheduleFreeRenderedObject(renderObject);
}
StdBullet::~StdBullet() {}


const std::string Wall::name = std::string("Wall");
Wall::Wall(SDL_Renderer* renderer, Vector2r position, GE_Rectangler shape, double mass) : GE_PhysicsObject(position,Vector2r{0,0,0},mass)
{
	this->shape = shape;
	this->addCollisionRectangle(shape);
	renderObject = GE_CreateRenderedObject(renderer,"gray");
	renderObject->size = {shape.w,shape.h};
	//GE_LinkGlueToPhysicsObject(this,GE_addGlueSubject(&renderObject->size.w,&collisionRectangle[0].w,&collisionRectangle[0].h
	renderObject->animation={0,0,1,1};

	GE_LinkVectorToPhysicsObjectPosition(this,&(renderObject->position)); 
	
	type = TYPE_SHIPWALL;



	tracker = GE_AddTrackedObject(type,this);
}


Wall::~Wall()
{
}
void Wall::C_Destroyed()
{
	GE_RemoveTrackedObject(tracker);
	GE_FreeRenderedObject(renderObject);
}
void Wall::serialize(char** buffer, size_t* bufferUsed, size_t* bufferSize)
{
	GE_Serialize(&position,buffer,bufferUsed,bufferSize);
	GE_Serialize(&velocity,buffer,bufferUsed,bufferSize);
	GE_Serialize(&collisionRectangles[0],buffer,bufferUsed,bufferSize); //collision rectangle 0 = shape 
	GE_Serialize(mass,buffer,bufferUsed,bufferSize);
	printf("serialize a wall\n");

}
Wall* Wall::unserialize(char* buffer, size_t* bufferUnserialized,int version)
{
	printf("~~WALL\n");
	auto _position = GE_Unserialize<Vector2r*>(buffer,bufferUnserialized,version);
	auto _velocity = GE_Unserialize<Vector2r*>(buffer,bufferUnserialized,version);
	auto _shape = GE_Unserialize<GE_Rectangler*>(buffer,bufferUnserialized,version);
	auto mass = GE_Unserialize<double>(buffer,bufferUnserialized,version);
	
	printf("rectw %f\n",_shape->w);
	//SDL_Delay(10000);

	auto newObj = new Wall(GE_DEBUG_Renderer,*_position,*_shape,mass);
	newObj->velocity = *_velocity;
	delete _position;
	delete _velocity;
	delete _shape;
	return newObj;
}

GE_PhysicsObject* Wall::spawnFromLevelEditor(SDL_Renderer* renderer, Vector2r position)
{
	return static_cast<GE_PhysicsObject*>(new Wall(GE_DEBUG_Renderer,position,GE_Rectangler{0,0,0,100,10},100)); 
}
GE_Rectangler Wall::getRectangle(unsigned int rect)
{
	auto collisionRect = collisionRectangles[0];
	return collisionRect;
}
Vector2 Wall::getSize()
{
	auto rectangle = collisionRectangles[0];
	return {rectangle.w,rectangle.h};

}
void Wall::setSize(Vector2 size)
{
	collisionRectangles[0].w = size.x;
	collisionRectangles[0].h = size.y;
	pthread_mutex_lock(&RenderEngineMutex);
	renderObject->size = size;
	pthread_mutex_unlock(&RenderEngineMutex);
}
void Wall::setRectangle(unsigned int rect, GE_Rectangler value)
{
	collisionRectangles[0] = value;
	pthread_mutex_lock(&RenderEngineMutex);
	renderObject->size = {value.w,value.h};
	pthread_mutex_unlock(&RenderEngineMutex);
}
void InitClasses()
{
	typeInGroup[TYPE_PLAYER][GROUP_INTELIGENT] = true;//some players shouldn't be in this, but we'll overlook that
	typeInGroup[TYPE_REGULAR][GROUP_REGULAROBJECT] = true;

	GE_RegisterUnserializationFunction(TYPE_RESERVED,tester::unserialize);
	GE_RegisterUnserializationFunction(TYPE_PLAYER,Player::unserialize);
	GE_RegisterUnserializationFunction(TYPE_ENEMY,Enemie::unserialize);
	GE_RegisterUnserializationFunction(TYPE_SHIPWALL,Wall::unserialize);

	GE_RegisterClassWithLevelEditor<Wall>(TYPE_SHIPWALL);
	GE_RegisterClassWithLevelEditor<Enemie>(TYPE_ENEMY);
	GE_RegisterClassWithLevelEditor<Player>(TYPE_PLAYER);
}

