#include "classes.h"

Player* targetPlayer;
Enemie* te;

Subsystem::Subsystem(SDL_Renderer* renderer, std::string sprite, Vector2 size, GE_Rectangle animation, Vector2r relativePosition, int collisionRectangle, std::string name, GE_Rectangle* parrentGrid)
{
	printf("stt\n");
	this->renderer = renderer;
	renderObject = GE_CreateRenderedObject(renderer,sprite); //TODO
	this->spriteName = sprite;
	pthread_mutex_lock(&RenderEngineMutex);

	renderObject->grid = *parrentGrid;
	printf("Size: %f\n ",size.x);
	printf("anim %f\n",animation.w);

	renderObject->size = size;
	renderObject->animation = animation;
	pthread_mutex_unlock(&RenderEngineMutex);

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
			GE_ChangeRenderedObjectSprite(renderObject,(spriteName.substr(0,spriteName.size()-4)+"_broken"));
		}
		
			
	}
}
void Subsystem::Update(Vector2r parrentPosition)
{

	if (true)//GetIsOnline())
	{
		double halfrectw = parrentGrid->w/2;
		double halfrecth = parrentGrid->h/2;
		//printf("x %f\n",(halfrectw));
		Vector2r rotationMatrix = relativePosition;

		rotationMatrix.x = (rotationMatrix.x-(halfrectw))+(renderObject->size.x/2);
		rotationMatrix.y = (rotationMatrix.y-(halfrecth))+(renderObject->size.y/2);


		GE_Vector2RotationCCW(&rotationMatrix,parrentPosition.r);

		rotationMatrix.x = (rotationMatrix.x+(halfrectw))-(renderObject->size.x/2);
		rotationMatrix.y = (rotationMatrix.y+(halfrecth))-(renderObject->size.y/2);

		//rotationMatrix.x = rotationMatrix.x+(halfrectw);
		//rotationMatrix.y = rotationMatrix.y+(halfrecth);

		
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
	delete glueTarget;
}





#define bump() numIterableSubsystems++;
#define sizePlusDoubleSize(x,y)  {x*2,y*2}, {0,0,x,y}
#define positionDouble(x,y) {x*2,y*2,0}
//the above inserts TWO PARAMETERS
TTF_Font* tSans;
TTF_Font* titleSans;
GE_UI_Text* txt;
Player::Player(SDL_Renderer* renderer) : GE_PhysicsObject({100,0,0},{0,0,0},GE_Rectangle{0,0,98,102},25)
{



	this->renderer = renderer;
	/*renderObject = GE_CreateRenderedObject(renderer,SPRITE_DIR"WholeShipv2"); //TODO
	renderObject->size = {98,102};
	renderObject->animation = {0,0,49,51};*/


	//grid = {0,0,98,102}; 


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

	iterableSubsystems[numIterableSubsystems] = new Subsystem(renderer,"playerThruster",sizePlusDoubleSize(5,51),positionDouble(9,0),0,"Left Thruster",&grid);
	bump();
	iterableSubsystems[numIterableSubsystems] = new Subsystem(renderer,"playerCoreReactor",sizePlusDoubleSize(21,26),positionDouble(14,13),1,"Core Reactor",&grid);
	bump();
	iterableSubsystems[numIterableSubsystems] = new Subsystem(renderer,"playerLShield",sizePlusDoubleSize(9,19),positionDouble(0,15),2,"Left Shield",&grid);
	bump();
	iterableSubsystems[numIterableSubsystems] = new Subsystem(renderer,"playerRShield",sizePlusDoubleSize(9,19),positionDouble(39,15),3,"Right Shield",&grid);
	bump();
	iterableSubsystems[numIterableSubsystems] = new Subsystem(renderer,"playerThruster",sizePlusDoubleSize(5,51),positionDouble(35,0),4,"Right Thruster",&grid);
	bump();
	iterableSubsystems[numIterableSubsystems] = new Subsystem(renderer,"playerLifeSupport",sizePlusDoubleSize(21,14),positionDouble(14,35),5,"Life Support",&grid);
	bump();
	iterableSubsystems[numIterableSubsystems] = new Subsystem(renderer,"playerShipHead",sizePlusDoubleSize(21,11),positionDouble(14,2),6,"Bridge",&grid);  //~~
	bump();
	iterableSubsystems[numIterableSubsystems] = new Subsystem(renderer,"playerLSmallTurret",sizePlusDoubleSize(4,9),positionDouble(5,21),7,"Left Turret",&grid); 
	bump();
	iterableSubsystems[numIterableSubsystems] = new Subsystem(renderer,"playerRSmallTurret",sizePlusDoubleSize(4,9),positionDouble(40,21),8,"Right Turret",&grid);
	bump();


	nextTickCanShoot = 0;



	targetPlayer = this;

	iterableSubsystems[0]->SetLevel(4);

	this->threadedEventStack = GE_ThreadedEventStack();


	this->inventory = new Inventory(2000,this);



	this->inventory->storage.push_back(ItemStack{ITEM_NAMES::IRON,63});
	this->inventory->storage.push_back(ItemStack{ITEM_NAMES::IRON,63});
	this->inventory->storage.push_back(ItemStack{ITEM_NAMES::DUCT_TAPE,63});
	this->inventory->storage.push_back(ItemStack{ITEM_NAMES::IRON,63});

#define BASE_DIR "../"


#define FONT_DIR BASE_DIR"fonts/"

#define FREESANS_LOC FONT_DIR"FreeSans.ttf"
tSans = TTF_OpenFont(FREESANS_LOC, 15);
if(!tSans) {
	printf("TTF_OpenFont: %s\n", TTF_GetError());
}
TTF_Font* bigSans =  TTF_OpenFont(FREESANS_LOC, 72);
if(!bigSans) {
	printf("TTF_OpenFont: %s\n", TTF_GetError());
}
TTF_SetFontStyle(bigSans,TTF_STYLE_ITALIC);
titleSans =  TTF_OpenFont(FREESANS_LOC, 18);
if(!titleSans) {
	printf("TTF_OpenFont: %s\n", TTF_GetError());
}
	txt = new GE_UI_Text(renderer,{35,35-15},{35,35},"placeholder",GE_Color{0xff,0x00,0x00,0xff},tSans);


}
Player::~Player()
{
	printf("Deleting the player!!!\n");
	for (int i=0;i<numIterableSubsystems;i++)
	{
		delete iterableSubsystems[numIterableSubsystems];
	}


}
//double fwdMove;
//#define unrealisticMove true



void ShootBullet(SDL_Renderer* renderer, GE_PhysicsObject* host, Vector2r addToVelocity, Vector2r addToPosition, bool isPlayer)
{
	addToPosition.x = addToPosition.x - host->grid.w/2;
	addToPosition.y = addToPosition.y - host->grid.h/2;

	GE_Vector2RotationCCW(&addToPosition,host->position.r);
	GE_Vector2RotationCCW(&addToVelocity,host->position.r);

	addToPosition.x = addToPosition.x + host->grid.w/2;
	addToPosition.y = addToPosition.y + host->grid.h/2;

	StdBullet* mybullet = new StdBullet(renderer,host->position+addToPosition,(isPlayer) ? ("stdBulletPlayer") : ("stdBulletEnemy"));
	GE_PhysicsObject* mybulletpo;
	GE_GetPhysicsObjectFromID(mybullet->ID,&mybulletpo);

	addToVelocity.x += host->velocity.x; //avoid use of vector addition because we don't want the rotation to be added.
	addToVelocity.y += host->velocity.y;

	GE_AddVelocity(mybullet,addToVelocity);
}


//#define unrealisticMove
//double fwdMove;
bool Player::C_Update()
{
	//Update subsystem positions
		
	for (int i=0;i<numIterableSubsystems;i++)
	{
		iterableSubsystems[i]->Update(position);
		if (!iterableSubsystems[i]->GetIsOnline())
		{
			collisionRectangles[i] = {0,0,0,0};
		}
	}
	//TODO temp
	//renderObject->position = this->position;


	if (GetIsOnline())
	{


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
				if(event.key.keysym.sym == SDLK_SPACE && ticknum >= nextTickCanShoot )
				{
					if (iterableSubsystems[7]->GetIsOnline()) ShootBullet(renderer,this,{0,-10,0},{12,-60,0},true);
					if (iterableSubsystems[8]->GetIsOnline()) ShootBullet(renderer,this,{0,-10,0},{86,-60,0},true); //(49*2)-12=86 , because the other turret is at the opposite side of the ship
					nextTickCanShoot = ticknum +(30-(pow(iterableSubsystems[0]->GetLevel(),2)));
				}

				if (event.key.keysym.sym == SDLK_p)
				{
					printf(" kjfaskl jasdklfj sdkla;fjasdkl;ifjalsjk;\n");
					Enemie* un = new Enemie(renderer, {position.x+100,position.y+100,0},1);
				}
				if (event.key.keysym.sym == SDLK_u)
				{

					GE_RenderedObject* ro = GE_CreateRenderedObject(this->renderer,"simple.bmp");	
					ro->size = {25,25};
					ro->animation = {0,0,8,9};
					

					GE_PhysicsObject* me = GE_CreatePhysicsObject({this->position.x+50,this->position.y+150,0},{0,0,0},{25,25},25);
					me->collisionRectangles[me->numCollisionRectangles] = {0,0,25,25};
					me->numCollisionRectangles++;
					me->callCallbackBeforeCollisionFunction = true;

					GE_LinkVectorToPhysicsObjectPosition(me,&(ro->position));

				}
if (event.key.keysym.sym == SDLK_t)
{
pthread_mutex_lock(&RenderEngineMutex);
				GE_UI_FontStyle fstyle = {{0x00,0x00,0x00,0xff},titleSans};
	GE_UI_WindowTitleStyle windowTitleStyle = {fstyle,0,{GE_Color{0xff,0x00,0x00,0xff},GE_Color{0x66,0x66,0x66,0xff},{0xff,0xff,0xff,0xff},fstyle.color,{15,7},tSans},2,GE_Color{0x66,0xff,0x33,0xff},25,true};
	GE_UI_WindowStyle windowStyle = {windowTitleStyle, GE_Color{0x00,0x00,0xff,0xff},2,GE_Color{0x66,0xff,0x33,0xff}};
	GE_UI_Style style = GE_UI_Style{fstyle,{GE_Color{0x00,0x33,0x00,0xff},tSans},windowStyle};


	GE_UI_Window* window = new GE_UI_Window(renderer,"INVENTORY",{250,250},{618,320},style);


	UI_InventoryView* inv = new UI_InventoryView(renderer, {0,0},{309,250},inventory,txt,{8,8},GE_Color{0xff,0xff,0xff,0x33},GE_Color{0x00,0x33,0x00,255},GE_Color{0xff,0x00,0x00,0xff});
	window->surface->addElement(inv);

	GE_UI_InsertTopLevelElement(window);
			pthread_mutex_unlock(&RenderEngineMutex);
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
			//GE_AddRelativeVelocity(cObj,{0,-0.5,0});
#ifdef unrealisticMove
			fwdMove = fwdMove-0.5;
#else
			//account for damaged thrusters -- if one is offline, we're going to spin when we move.
			if (iterableSubsystems[0]->GetIsOnline()) GE_AddRelativeVelocity(this,{0,-0.25,0.0008726646});
			if (iterableSubsystems[4]->GetIsOnline()) GE_AddRelativeVelocity(this,{0,-0.25,-0.0008726646});
#endif
		
		}
		if(keysHeld[SDLK_s])
		{
#ifdef unrealisticMove
			fwdMove = fwdMove+0.5;
#else
			if (iterableSubsystems[0]->GetIsOnline()) GE_AddRelativeVelocity(this,{0,0.25,-0.0008726646});
			if (iterableSubsystems[4]->GetIsOnline()) GE_AddRelativeVelocity(this,{0,0.25,0.0008726646});
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
			GE_AddRelativeVelocity(this,{0,0,-0.0008726646});
		}
		if(keysHeld[SDLK_e])
		{
			GE_AddRelativeVelocity(this,{0,0,0.0008726646});
		}


		if (keysHeld[SDLK_t])
		{
		}
	
		if(keysHeld[SDLK_x])
		{
			printf("posx %f\n",position.x);
			printf("posy %f\n",position.y);
			printf("r %f\n",position.r);
		}
		if(keysHeld[SDLK_b])
		{
			iterableSubsystems[0]->health -= 5.25;
		}
		//temp
		if(keysHeld[SDLK_f])
		{
			velocity = {0,0,0};
#ifdef unrealisticMove
			fwdMove = 0;
#endif
		} 
		if (keysHeld[SDLK_n])
		{
			velocity = {-1,0,0};
		}
		
		if (keysHeld[SDLK_ESCAPE])
		{
			//TODO GOD WHY
			iterableSubsystems[1]->health = 0;
		}
		if (keysHeld[SDLK_t]) te->velocity.r += 0.02;
		if (keysHeld[SDLK_y]) te->velocity.r -= 0.02;
		if (keysHeld[SDLK_r]) te->velocity.r =0;


#ifdef unrealisticMove
		velocity = {0,0,velocity.r};
		GE_AddRelativeVelocity(this,{0,fwdMove,0});
#endif



	}
	//GE_DEBUG_TextAt(std::to_string(position.r),Vector2{0,0});


	

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
		
		/*(if (true)
		{
			
			//It hit the core. Game over
			printf("TEMP GAMEOVER\n-----------\n");
			//TODO GOD WHY AGAIN
			exit(0);
		}*/

		for (int i = 0;i<numIterableSubsystems;i++)
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




Enemie::Enemie(SDL_Renderer* renderer, Vector2r position, int level) : GE_PhysicsObject(position,{0,0,0},GE_Rectangle{0,0,38,42},25)
{
	this->renderer = renderer;

	this->level = level;

	type = TYPE_ENEMY;
	
	renderObject = GE_CreateRenderedObject(renderer,"enemy"); 
	pthread_mutex_lock(&RenderEngineMutex);
	renderObjectID = numRenderedObjects;
	renderObject->size = {38,42};
	renderObject->animation = {0,0,19,21};
	GE_LinkMinimapToRenderedObject(renderObject,{0xA0,0x00,0x00,0xFF});


	pthread_mutex_unlock(&RenderEngineMutex);

	GE_LinkVectorToPhysicsObjectPosition(this,&(renderObject->position)); 
	
	collisionRectangles[numCollisionRectangles] = {0,0,38,42}; 
	numCollisionRectangles++;

	
	callCallbackAfterCollisionFunction = true;
	callCallbackUpdate = true;
	lastTimeShotTick = ticknum;

}
Enemie::~Enemie()
{
	pthread_mutex_lock(&RenderEngineMutex);
	GE_FreeMinimapTarget(renderObject);
	deadRenderedObjects[renderObjectID] = true;
	delete renderObject;
	pthread_mutex_unlock(&RenderEngineMutex);
}
bool Enemie::C_Update()
{
	te = this;


	//based on: https://gamedev.stackexchange.com/a/124803
	//printf("enemy up\n");
	//check if we're in the radius of our target player
	double rotaryDistance = GE_GetRotationalDistance(this->position,targetPlayer->position)-M_PI;
	double distanceToPlayer = GE_Distance(this->position.x, this->position.y, targetPlayer->position.x, targetPlayer->position.y);
	//printf("rdist %f\n",rotaryDistance);

	//printf("t %d l %d \n",ticknum,lastTimeShotTick);


	if (distanceToPlayer < 500)
	{
		foundPlayer=true;
		if (ticknum-lastTimeShotTick >= 60)
		{	
			//printf("cs\n");
			if (rotaryDistance < 0.3 && rotaryDistance > -0.3)
			{
				lastTimeShotTick = ticknum;
				//ShootBullet(renderer,this,{0,-3,0},{19,-9,0},false);
			}
		}
	}
	else if (distanceToPlayer > 2000)
	{
		foundPlayer = false;
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


		//printf("a %f b %f c %f\n",a,velocity.r,rotaryDistance);
		double timeToDestination = (std::abs(rotaryDistance)<0.04) ? 0 : std::abs(   ( -velocity.r+( ((rotaryDistance >= 0)? -1 : 1)*std::sqrt(std::abs( std::pow(velocity.r,2)+(4*0.5*acceleration_r*(rotaryDistance)) ))) )/(acceleration_r)   );
		//printf("time to dest %f",timeToDestination);
		//printf(" mid %f\n",std::pow(velocity.r,2)+(4*(0.5*acceleration_r)*(-rotaryDistance+position.r)));
		//printf("top %f\n",(-velocity.r+std::sqrt(std::pow(velocity.r,2)+(4*(0.5*acceleration_r)*(-rotaryDistance+position.r)))));
		//double timeToDestination =  (sqrt(std::abs((2*(rotaryDistance))/acceleration_r)))+( (velocity.r == 0.0)?(0.0):( std::abs(rotaryDistance/(velocity.r)) ) );
		double timeToStop = std::abs(velocity.r/acceleration_r); //the *2 is not part of the original equation, but was added to help soften the spring effect.
		//printf("time stop %f\n",timeToStop);
		if(timeToDestination<timeToStop)
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



BulletType::BulletType(Vector2r position, Vector2r velocity, GE_Rectangle grid, double mass) : GE_PhysicsObject(position,velocity,grid, mass)
{
	type = TYPE_DESTROYSUB;
}
bool BulletType::C_Collision(int victimID, int collisionRectangleID)
{
	//printf("Collision! I should be dead now!\n");
	return true; //Tell the physics engine to delete me.
}
BulletType::~BulletType()
{
/*	printf("!!!!!!!!!!!!!I WAS CALLED\n");

	pthread_mutex_lock(&RenderEngineMutex);
	deadRenderedObjects[renderObjectID] = true;
	delete renderObject;
	pthread_mutex_unlock(&RenderEngineMutex);*/
}


StdBullet::StdBullet(SDL_Renderer* renderer, Vector2r position, const char* spriteName) : BulletType(position,{0,0,0},GE_Rectangle{0,0,2,10},STD_BULLET_MASS)
{
	renderObject = GE_CreateRenderedObject(renderer,spriteName); 
	pthread_mutex_lock(&RenderEngineMutex);
	renderObjectID = numRenderedObjects;
	renderObject->size = {2,10};
	renderObject->animation = {0,0,1,5};
	pthread_mutex_unlock(&RenderEngineMutex);
	printf("fin ro\n");

	GE_LinkVectorToPhysicsObjectPosition(this,&(renderObject->position)); 
	
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

