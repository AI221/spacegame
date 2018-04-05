/*!
 * @file
 * @author Jackson McNeill
 * Game-specific classes (not part of the core engine)
 * Also holds other things specific to this game.
 */
#pragma once


#include "renderedObject.h"
#include "physics.h"
#include "threadedEventStack.h"
#include "serialize.h"
#include "serializeObject.h"
#include "levelEditor.h"

class Inventory;


//LIMITS:


#define MAX_SUBSYSTEMS 40

#include "UI.h" //TODO TEMP

enum TYPES
{
	TYPE_REGULAR, //starts at 0
	TYPE_PLAYER,
	TYPE_ENEMY,
	TYPE_DESTROYSUB, //bullets, etc.
	TYPE_SHIPWALL,
	TYPE_RESERVED,
	TYPE_COUNT // ammount of types that exist - do not use as a type
};

enum GROUPS
{
	GROUP_REGULAROBJECT,	
	GROUP_INTELIGENT, 
	GROUP_COUNT
};

extern bool typeInGroup[TYPE_COUNT][GROUP_COUNT];


void InitClasses();

//config

#define STD_BULLET_MASS 2



class Subsystem
{
	public:
		Subsystem(SDL_Renderer* renderer, std::string sprite, Vector2 size, GE_Rectangle animation, Vector2r relativePosition, int collisionRectangle, std::string name, Vector2* parrentGrid);
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

		std::string spriteName;
		Vector2r relativePosition;
		int collisionRectangle;
		int level; //posibilites...
		Vector2* parrentGrid;
		Vector2r position;
		GE_GlueTarget* glueTarget;

};



class Player : public GE_PhysicsObject, public GE_LevelEditorInterface
{
	public:
		Player(SDL_Renderer* renderer);
		~Player();
		void C_Destroyed() override;
		bool C_Update() override;
		bool C_Collision(GE_PhysicsObject* victim, int collisionRectangleID) override;
		bool GetIsOnline();
		Subsystem* iterableSubsystems[MAX_SUBSYSTEMS];
		const static int numIterableSubsystems = 8;


		GE_ThreadedEventStack threadedEventStack;

		Inventory* inventory;

		void serialize(char** buffer, size_t* bufferUsed,size_t* bufferSize) override;
		static Player* unserialize(char* buffer, size_t* bufferUnserialized, int version);
		static GE_PhysicsObject* spawnFromLevelEditor(SDL_Renderer* renderer, Vector2r position);
	
	private:
		GE_RenderedObject* renderObject;
		bool keysHeld[323] = {false}; 
		SDL_Renderer* renderer;
		int nextTickCanShoot;
		bool dampeners;

		GE_TrackedObject* tracker;
		




};

class Enemie : public GE_PhysicsObject, public GE_LevelEditorInterface
{
	public:
		Enemie(SDL_Renderer* renderer, Vector2r position, int level);
		~Enemie();
		void C_Destroyed() override;

		bool C_Update() override;
		bool C_Collision(GE_PhysicsObject* victim, int collisionRectangleID) override;

		void serialize(char** buffer, size_t* bufferUsed,size_t* bufferSize) override;
		static Enemie* unserialize(char* buffer, size_t* bufferUnserialized, int version);
		static GE_PhysicsObject* spawnFromLevelEditor(SDL_Renderer* renderer, Vector2r position);

		const static std::string name;
		constexpr const static GE_LevelEditorObjectProperties properties = GE_LevelEditorObjectProperties{0};
		
	private:
		GE_RenderedObject* renderObject;
		SDL_Renderer* renderer;
		int level;
		Vector2 lastFoundPlayer;
		bool foundPlayer;
		int lastTimeShotTick;

		GE_TrackedObject* tracker;

};


class BulletType : public GE_PhysicsObject, public GE_LevelEditorInterface
{
	public:
		BulletType(Vector2r position, Vector2r velocity,double mass);
		~BulletType();
		int level;
		bool C_Collision(GE_PhysicsObject* victim, int collisionRectangleID);
	private:
		GE_RenderedObject* renderObject;
		

};

class StdBullet : public BulletType //Hah
{
	public: 
		StdBullet(SDL_Renderer* renderer, Vector2r position, const char* spriteName);
		~StdBullet();
		void C_Destroyed();

	private:
		GE_RenderedObject* renderObject;
};


class Wall : public GE_PhysicsObject, public GE_LevelEditorInterface
{
	public:
		Wall(SDL_Renderer* renderer, Vector2r position, GE_Rectangle shape, double mass);
		~Wall();
		void C_Destroyed() override;


		void serialize(char** buffer, size_t* bufferUsed,size_t* bufferSize) override;
		static Wall* unserialize(char* buffer, size_t* bufferUnserialized, int version);
		static GE_PhysicsObject* spawnFromLevelEditor(SDL_Renderer* renderer, Vector2r position);

		const static std::string name;
		constexpr const static GE_LevelEditorObjectProperties properties = GE_LevelEditorObjectProperties{1};

		GE_Rectangle getRectangle(unsigned int id) override;
		void setRectangle(unsigned int id, GE_Rectangle rectangle) override;
		
	private:
		GE_RenderedObject* renderObject;
		GE_Rectangle shape;

		GE_TrackedObject* tracker;
};
