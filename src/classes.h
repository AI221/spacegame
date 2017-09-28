/*!
 * @file
 * @author Jackson McNeill
 * Game-specific classes (not part of the core engine)
 * Also holds other things specific to this game.
 */

#include "SDL.h"
#include "SDL_ttf.h"
#include <string>
#include <math.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <cmath>

//Local includes
#include "vector2.h"
#include "physics.h"
#include "renderedObject.h"
#include "gluePhysicsObject.h"
#include "minimap.h"


#

//LIMITS:


#define MAX_SUBSYSTEMS 40

#include "UI.h" //TODO TEMP
#ifndef __CLASSES_INCLUDED
#define __CLASSES_INCLUDED

enum TYPES
{
	TYPE_REGULAR, //starts at 0
	TYPE_PLAYER,
	TYPE_ENEMY,
	TYPE_DESTROYSUB, //bullets, etc.
};


extern GE_UI_DraggableProgressBar* progress;


//config

#define STD_BULLET_MASS 2

class Subsystem
{
	public:
		Subsystem(SDL_Renderer* renderer, std::string sprite, Vector2 size, GE_Rectangle animation, Vector2r relativePosition, int collisionRectangle, std::string name, GE_Rectangle* parrentGrid);
		~Subsystem();
		void CheckCollision(int checkCollisionRectangle);
		void Update(Vector2r parrentPosition);
		bool GetIsOnline();
		int GetLevel();
		void SetLevel(int level);

		double health; //this is public so that is can be glued for UI updating
		std::string name;
	private:
		SDL_Renderer* renderer; 
		GE_RenderedObject* renderObject;
		Vector2r relativePosition;
		int collisionRectangle;
		int level; //posibilites...
		GE_Rectangle* parrentGrid;
		Vector2r position;
		GE_GlueTarget* glueTarget;

};


int handleEvents(SDL_Event* event);

class Player : public GE_PhysicsObject
{
	public:
		Player(SDL_Renderer* renderer);
		~Player();
		bool C_Update();
		bool C_Collision(int victimID, int collisionRectangleID);
		bool GetIsOnline();
		Subsystem* iterableSubsystems[MAX_SUBSYSTEMS];
		int numIterableSubsystems;

	
	private:
		GE_RenderedObject* renderObject;
		bool keysHeld[323] = {false}; 
		SDL_Renderer* renderer;
		int nextTickCanShoot;


};

class Enemie : public GE_PhysicsObject
{
	public:
		Enemie(SDL_Renderer* renderer, Vector2r position, int level);
		~Enemie();
		bool C_Update();
		bool C_Collision(int victimID, int collisionRectangleID);
	private:
		int renderObjectID;
		GE_RenderedObject* renderObject;
		SDL_Renderer* renderer;
		int level;
		bool foundPlayer;

};


class BulletType : public GE_PhysicsObject
{
	public:
		BulletType(Vector2r position, Vector2r velocity, GE_Rectangle grid, double mass);
		~BulletType();
		int level;
		bool C_Collision(int victimID, int collisionRectangleID);
	private:
		int renderObjectID;
		GE_RenderedObject* renderObject;
		

};

class StdBullet : public BulletType //Hah
{
	public: 
		StdBullet(SDL_Renderer* renderer, Vector2r position, const char* spriteName);
		~StdBullet();

	private:
		int renderObjectID;
		GE_RenderedObject* renderObject;
};


#endif //__CLASSES_INCLUDED
