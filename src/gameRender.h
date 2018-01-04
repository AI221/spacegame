/*!
 *
 * @file
 * @author Jackson McNeill
 *
 * A UI element which renders all renderedObjects
 */

#include "vector2.h"
#include "camera.h"
#include "UI.h"
#include "renderedObject.h"


#include <pthread.h>



#ifndef __GAME_RENDER_INCLUDED
#define __GAME_RENDER_INCLUDED

class GE_UI_GameRender: public GE_UI_TopLevelElement
{
	public:
		GE_UI_GameRender(SDL_Renderer* renderer, Vector2 position, Vector2 size, Camera* camera);
		~GE_UI_GameRender();
		void giveEvent(Vector2 parrentPosition, SDL_Event event);
		void render(Vector2 parrentPosition);
		bool checkIfFocused(int mousex, int mousey);

		bool wantsEvents;
	private:
		SDL_Renderer* renderer;
		Vector2 position;
		Vector2 size;
		Camera* camera;

};
#endif //__GAME_RENDER_INCLUDED
