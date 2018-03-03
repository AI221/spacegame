/*!
 *
 * @file
 * @author Jackson McNeill
 *
 * A UI element which renders all renderedObjects
 */
#pragma once

#include "vector2.h"
#include "camera.h"
#include "UI.h"






class GE_UI_GameRender: public GE_UI_TopLevelElement
{
	public:
		GE_UI_GameRender(SDL_Renderer* renderer, Vector2 position, Vector2 size, Camera* camera, double scale);
		~GE_UI_GameRender();
		void giveEvent(Vector2 parrentPosition, SDL_Event event);
		void render(Vector2 parrentPosition);
		bool checkIfFocused(int mousex, int mousey);
		void setScale(double scale);

		bool wantsEvents;
	private:
		SDL_Renderer* renderer;
		Vector2 position;
		Vector2 size;
		Camera* camera;
		double scale;

};
