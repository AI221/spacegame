#include "vector2.h"
#include<SDL2/SDL.h>
#include<SDL2/SDL_ttf.h>
#include <math.h>
#include <iostream>
#include <string>
#include <functional>

#ifndef __DEBUGUI_INCLUDED
#define __DEBUGUI_INCLUDED
class debugUIElement
{
	public:
	 /*	debugUIElement(SDL_Renderer* renderer, Vector2 position);
		Vector2 position;
		SDL_Renderer* renderer;*/
		virtual void render(Vector2 parrentPosition) = 0;
		virtual void giveEvent(Vector2 parrentPosition, SDL_Event event) = 0;
		bool wantsEvents;
};
class debugText : public debugUIElement
{
	public:
		debugText(SDL_Renderer* renderer, Vector2 position, std::string text);
		void render(Vector2 parrentPosition);
		void render();
		void setText(const char* text);
		void setText(std::string text);
		void giveEvent(Vector2 parrentPosition, SDL_Event event);
	//	bool wantsEvents;

	private:
		TTF_Font* Sans = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSans.ttf", 24);
		SDL_Color color = {0, 255, 0}; //GREEN TXT FOR HACKERS
		SDL_Rect Message_rect;
		SDL_Renderer* myRenderer;
		SDL_Texture* Message;
};
class debugButton : public debugUIElement
{
	public:
		debugButton(SDL_Renderer* myRenderer, Vector2 position, Vector2 size, std::string text);
		void render(Vector2 parrentPosition);
		void render();
		void giveEvent(Vector2 parrentPosition, SDL_Event event);
		std::function< void () > C_Pressed; //Callback	


		Vector2 position;
		SDL_Rect positionAndSize;
		bool pressed = false;
	private:
		debugText* myText;
		SDL_Renderer* myRenderer;
	};
class debugWindow
{
	public:
		debugWindow(SDL_Renderer* myRenderer, Vector2 position, Vector2 size);
		void render();
		int addElement(debugUIElement* element);
		debugUIElement* getElement(int elementID);
		void giveEvent(SDL_Event event);
		bool isOpen = false;

		Vector2 position;
		SDL_Rect positionAndSize;
		
	private:
		debugUIElement* elements[256];
		int nextUIElement = 0;
		SDL_Renderer* myRenderer;
		debugButton* xbutton;
		debugButton* dbutton;
		
};

#endif //__DEBUGUI_INCLUDED
