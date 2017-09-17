#include "SDL.h"
#include <unordered_map>

//Local includes
#include "UI.h"
#include "vector2.h"
#include "renderedObject.h"


#ifndef __MINIMAP_INCLUDED
#define __MINIMAP_INCLUDED

void GE_LinkMinimapToRenderedObject(GE_RenderedObject* subject, SDL_Color color);

void GE_FreeMinimapTarget(GE_RenderedObject* linkedRenderedObject);

class GE_UI_Minimap : public GE_UI_Element
{
	public:
		GE_UI_Minimap(SDL_Renderer* renderer, Vector2 position, Vector2 size, double scale, SDL_Color background, SDL_Color crosshair, Camera* camera);
		void render(Vector2 parrentPosition);
		bool wantsEvents;
	private:
		SDL_Renderer* renderer;
		Vector2 size;
		Vector2 position;
		double scale;
		SDL_Color background;
		SDL_Color crosshair;
		Camera* camera;
};

#endif // __MINIMAP_INCLUDED
