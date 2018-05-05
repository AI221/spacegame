/*!
 * @file
 * @author Jackson McNeill
 * A minimap that you can register RenderedObject's positions to, and assign a color. 
 */
#pragma once


class SDL_Renderer;

//Local includes
#include "UI.h"
#include "vector2.h"
#include "renderedObject.h"



/*!
 * Make an object show up on the minimap
 */
void GE_LinkMinimapToRenderedObject(GE_RenderedObject* subject, GE_Color color);

/*!
 * Deleted a minimap target -- may only be used in the render thread, or whilst the render thread is locked.
 */
void GE_FreeMinimapTarget(GE_RenderedObject* linkedRenderedObject);

/*!
 * Schedules a minimap target to be deleted. Safe to call from any thread!
 */
void GE_ScheduleFreeMinimapTarget(GE_RenderedObject* linkedRenderedObject);

class GE_UI_Minimap : public GE_UI_Element
{
	public:
		GE_UI_Minimap(SDL_Renderer* renderer, Vector2 position, Vector2 size, double scale, GE_Color background, GE_Color crosshair, Camera* camera);
		void render(Vector2 parrentPosition);
	private:
		SDL_Renderer* renderer;
		Vector2 size;
		Vector2 position;
		double scale;
		GE_UI_Rectangle background;
		GE_UI_Rectangle crosshair_x;
		GE_UI_Rectangle crosshair_y;
		Camera* camera;
};

