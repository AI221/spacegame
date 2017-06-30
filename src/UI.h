#include "vector2.h"
#include<SDL2/SDL.h>
#include<SDL2/SDL_ttf.h>
#include <math.h>
#include <iostream>
#include <string>
#include <functional>

#ifndef __UI_INCLUDED
#define __UI_INCLUDED

//limits:
#define MAX_SURFACE_UI_ELEMENTS 256


class GE_UI_Element
{
	public:
	 /*	GE_UI_Element(SDL_Renderer* renderer, Vector2 position);
		Vector2 position;
		SDL_Renderer* renderer;*/
		virtual void render(Vector2 parrentPosition) = 0;
		virtual void giveEvent(Vector2 parrentPosition, SDL_Event event) = 0;
		bool wantsEvents;
};
class GE_UI_Text : public GE_UI_Element
{
	public:
		GE_UI_Text(SDL_Renderer* renderer, Vector2 position, std::string text, SDL_Color color);
		~GE_UI_Text();
		void render(Vector2 parrentPosition);
		void render();
		void setText(const char* text);
		void setText(std::string text);
		void giveEvent(Vector2 parrentPosition, SDL_Event event);
		SDL_Rect Message_rect;
	//	bool wantsEvents;

	private:
		TTF_Font* Sans = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSans.ttf", 24);
		SDL_Color color;
		SDL_Renderer* renderer;
		SDL_Texture* Message;
};
class GE_UI_TextInput : public GE_UI_Element
{
	public:
		GE_UI_TextInput(SDL_Renderer* renderer, Vector2 position, Vector2 size, SDL_Color textColor, SDL_Color color);
		~GE_UI_TextInput();
		void render(Vector2 parrentPosition);
		void render(); 
		void setText(const char* text);
		void setText(std::string text);
		void giveEvent(Vector2 parrentPosition, SDL_Event event);
		std::string getText();	
		const char* getText_cstr();
		
		bool isFocused;

	private:
		SDL_Renderer* renderer;
		Vector2 position;
		Vector2 size;
		SDL_Color color;

		GE_UI_Text* myText;
		std::string text;
		
};
class GE_UI_Button : public GE_UI_Element
{
	public:
		GE_UI_Button(SDL_Renderer* renderer, Vector2 position, Vector2 paddingSize, std::string text, SDL_Color textColor, SDL_Color color, SDL_Color pressedColor);
		~GE_UI_Button();
		void render(Vector2 parrentPosition);
		void render();
		void giveEvent(Vector2 parrentPosition, SDL_Event event);
		std::function< void () > C_Pressed; //Callback	


		Vector2 position;
		SDL_Rect positionAndSize;
		bool pressed = false;
	private:
		GE_UI_Text* myText;
		SDL_Renderer* renderer;
		Vector2 paddingSize;
		SDL_Color color;
		SDL_Color pressedColor;
	};
class GE_UI_Surface
{
	public:
		GE_UI_Surface(SDL_Renderer* renderer, Vector2 position, Vector2 size);
		~GE_UI_Surface();
		void render();
		int addElement(GE_UI_Element* element);
		GE_UI_Element* getElement(int elementID);
		void giveEvent(SDL_Event event);
		bool isOpen = false;

		Vector2 position;
		SDL_Rect positionAndSize;
		
	private:
		GE_UI_Element* elements[MAX_SURFACE_UI_ELEMENTS];
		int nextUIElement = 0;
		SDL_Renderer* renderer;
		
};

#endif //__UI_INCLUDED
