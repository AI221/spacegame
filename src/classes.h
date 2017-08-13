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

//Local includes
#include "vector2.h"
#include "physics.h"
#include "renderedObject.h"
#include "gluePhysicsObject.h"


#ifndef __CLASSES_INCLUDED
#define __CLASSES_INCLUDED

enum TYPES
{
	TYPE_REGULAR, //starts at 0
	TYPE_PLAYER,
	TYPE_ENEMY,
	TYPE_DESTROYSUB, //bullets, etc.
};

class Subsystem
{
	public:
		Subsystem(SDL_Renderer* renderer, std::string sprite, Vector2 size, GE_Rectangle animation, Vector2r relativePosition, int collisionRectangle);
		void CheckCollision(int collisionRectangle);
		void Update(Vector2r parrentPosition);
		bool GetIsOnline();
	private:
		SDL_Renderer* renderer; 
		GE_RenderedObject* renderObject;
		Vector2r relativePosition;
		bool isOnline;
		int collisionRectangle;
		int level; //posibilites...

};


int handleEvents(SDL_Event* event);

class Player : public GE_PhysicsObject
{
	public:
		Player(SDL_Renderer* renderer);
		bool C_Update();
		bool C_Collision(int victimID, int collisionRectangleID);
		Subsystem* iterableSubsystems[40];
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

};


class BulletType : virtual public GE_PhysicsObject
{
	public:
		BulletType(Vector2r position, Vector2r velocity, GE_Rectangle grid);
		~BulletType();
		int level;
	private:
		GE_RenderedObject* renderObject;
		

};

class StdBullet : public GE_PhysicsObject//virtual public BulletType //Hah
{
	public: 
		StdBullet(SDL_Renderer* renderer, Vector2r position);
		~StdBullet();
		bool C_Collision(int victimID, int collisionRectangleID);

	private:
		int renderObjectID;
		GE_RenderedObject* renderObject;
};


#endif //__CLASSES_INCLUDED
