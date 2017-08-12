/*!
 * @file
 * @author Jackson McNeill
 * Game-specific classes (not part of the core engine)
 * Also holds other things specific to this game.
 */

#include<SDL2/SDL.h>
#include<SDL2/SDL_ttf.h>

//Local includes
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

/*
class Subsystem
{
	public:
		virtual void Blit(Vector2r parrentPosition);
		virtual bool getIsOnline();
};

class SubThrusterForwardBack : Subsystem
{
	public:
		void Blit(Vector2r parrentPosition);
		bool getIsOnline();
	private:
		bool isOnline;
		int myCollisionRectangle;
		int level; //posibilites...

};

class SubLifeSupport : Subsystem
{
	public:
		void Blit(Vector2r parrentPosition);
		bool getIsOnline();
	private:
		bool isOnline;
		int myCollisionRectangle;
		
};

*/

int handleEvents(SDL_Event* event);

class Player : public GE_PhysicsObject
{
	public:
		Player(SDL_Renderer* renderer);
		bool C_Update();
		bool C_Collision(int victimID, int collisionRectangleID);
		//Subsystem iterableSubsystems[40];
		int numIterableSubsystems;

	
	private:
		GE_RenderedObject* renderObject;
		bool keysHeld[323] = {false}; 
		SDL_Renderer* renderer;


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
