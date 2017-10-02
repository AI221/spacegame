#include "vector2.h"
#include "camera.h"
#include "UI.h"
#include "renderedObject.h"
#include "gluePhysicsObject.h"
#include "physics.h"
#include "classes.h"



#ifndef __GAME_RENDER_INCLUDED
#define __GAME_RENDER_INCLUDED

class GE_UI_GameRender: public GE_UI_TopLevelElement
{
	public:
		GE_UI_GameRender(SDL_Renderer* renderer, Vector2 position, Vector2 size, Player* focusObject);
		~GE_UI_GameRender();
		void giveEvent(Vector2 parrentPosition, SDL_Event event);
		void render(Vector2 parrentPosition);
		bool checkIfFocused(int mousex, int mousey);

		bool wantsEvents;

		Camera camera;
		GE_Rectangle camerasGrid;
	private:
		SDL_Renderer* renderer;
		Vector2 position;
		Vector2 size;
		GE_ThreadedEventStack* playerEventStack;

};
#endif //__GAME_RENDER_INCLUDED
