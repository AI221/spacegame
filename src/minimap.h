#include "UI.h"
#include "vector2.h"
#include "renderedObject.h"


#ifndef __MINIMAP_INCLUDED
#define __MINIMAP_INCLUDED

class GE_UI_Minimap : public GE_UI_Element
{
	public:
		GE_UI_Minimap(SDL_Renderer* renderer, Vector2 position, Vector2 size, SDL_Color background, SDL_Color crosshair);
		void render(Vector2 parrentPosition);
		void giveEvent(Vector2 parrentPosition, SDL_Event event);
		bool wantsEvents;
	private:
		SDL_Renderer* renderer;
		Vector2 size;
		Vector2 position;
		SDL_Color background;
		SDL_Color crosshair;
};

#endif // __MINIMAP_INCLUDED
