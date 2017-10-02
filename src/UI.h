/*!
 * @file
 * @author Jackson McNeill
 *
 * A simple object-oriented UI system. This is intended to remain as minimal as possible, while providing necasary features for game HUDs and UIs. 
 * Many objects do not support rotation; some do (usually if used in HUDs). This helps to keep this system minimal.
 */


#include "SDL.h"
#include "SDL_ttf.h"
#include <math.h>
#include <iostream>
#include <string>
#include <functional>
#include <cstring>
#include <cmath>
#include <list>


//Local includes

#include "vector2.h"
#include "shapes.h"

#ifndef __UI_INCLUDED
#define __UI_INCLUDED

//limits:
#define MAX_SURFACE_UI_ELEMENTS 256

struct GE_UI_FontStyle
{
	GE_Color color;
	TTF_Font* font;
};
struct GE_UI_ButtonStyle
{
	GE_Color background;
	GE_Color backgroundPressed;
	GE_Color text;
	GE_Color textPressed;
	Vector2 paddingSize;
	TTF_Font* font;
};

struct GE_UI_WindowTitleStyle
{

	GE_UI_FontStyle font;
	double textOffset;
	GE_UI_ButtonStyle XButton;
	double buttonDistanceFromRight;
	GE_Color background;
	double height;
	bool centerTitleText;

};


struct GE_UI_WindowStyle
{
	GE_UI_WindowTitleStyle titleStyle;
	GE_Color background;
	double borderSize;
	GE_Color borderColor;
};

struct GE_UI_Style
{
	GE_UI_FontStyle standardFont;
	GE_UI_FontStyle buttonFont;

	GE_UI_WindowStyle windowStyle;
};

class GE_UI_Element
{
	public:
		virtual void render(Vector2 parrentPosition) = 0;
		virtual void giveEvent(Vector2 parrentPosition, SDL_Event event);
		bool wantsEvents;
};
class GE_UI_TopLevelElement
{
	public:
		virtual void render(Vector2 parrentPosition) = 0;
		virtual void giveEvent(Vector2 parrentPosition, SDL_Event event) = 0;
		virtual bool checkIfFocused(int mousex, int mousey) = 0;

		bool wantsEvents;
};

class GE_UI_Text : public GE_UI_Element
{
	public:
		GE_UI_Text(SDL_Renderer* renderer, Vector2 position, Vector2 size, std::string text, GE_Color color,TTF_Font* font);
		GE_UI_Text(SDL_Renderer* renderer, Vector2 position, Vector2 size, std::string text, GE_UI_FontStyle style);
		~GE_UI_Text();
		void render(Vector2 parrentPosition);
		void render();
		void setText(const char* text);
		void setText(std::string text);
		void setSize(Vector2 newSize);
		Vector2 getSize();
		void setPosition(Vector2 newPosition);
	
		void centerX();
		void centerY();
		/*!
		 * Change positions, so that the center of the text becomes the what the position is now. 
		 */
		void center();

		/*!
		 * Expand so that our boundries are equal to the ammount of text we're currently holding at this moment
		 */
		void expandToTextSize();
		void setCursor(int pos);
	//	bool wantsEvents;
		SDL_Rect Message_rect;

	private:
		TTF_Font* font;//Sans = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSans.ttf", 15);
		Vector2 position;
		Vector2 size;
		GE_Color color;
		SDL_Renderer* renderer;
		SDL_Texture* Message;
		int scrollPosition;
		int cursorPosition;

		bool doCenterX;
		bool doCenterY;
};
class GE_UI_TextInput : public GE_UI_Element
{
	public:
		GE_UI_TextInput(SDL_Renderer* renderer, Vector2 position, Vector2 size, GE_Color textColor, GE_Color color, TTF_Font* font);
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
		GE_Color color;

		GE_UI_Text* myText;
		std::string text;
		
};
class GE_UI_Button : public GE_UI_Element
{
	public:
		GE_UI_Button(SDL_Renderer* renderer, Vector2 position, Vector2 paddingSize, std::string text, GE_Color textColor, GE_Color color, GE_Color pressedColor, TTF_Font* font);
		~GE_UI_Button();
		void render(Vector2 parrentPosition);
		void render();
		void giveEvent(Vector2 parrentPosition, SDL_Event event);
		Vector2 getSize();
		std::function< void () > C_Pressed; //Callback	


		Vector2 position;
		SDL_Rect positionAndSize;
		bool pressed = false;
	private:
		GE_UI_Text* myText;
		SDL_Renderer* renderer;
		Vector2 paddingSize;
		GE_Color color;
		GE_Color pressedColor;
};

class GE_UI_ProgressBar : public GE_UI_Element
{
	public:
		GE_UI_ProgressBar(SDL_Renderer* renderer, Vector2 position, Vector2 size, GE_Color color, GE_Color background, bool showProgressNumber);
		~GE_UI_ProgressBar();
		void render(Vector2 parrentPosition);
		void render(Vector2r parrentPosition); //due to its use in HUDs, this supports rotation
		void setProgress(double progress);
		double getProgress();
		
		bool wantsEvents;
	private:
		SDL_Renderer* renderer;
		Vector2 position;
		Vector2 size;
		GE_RectangleShape* background;
		GE_RectangleShape* bar;
		bool showProgressNumber;
		GE_UI_Text* text;

		double progress;
};

class GE_UI_DraggableProgressBar : public GE_UI_ProgressBar
{
	public:
		GE_UI_DraggableProgressBar(SDL_Renderer* renderer, Vector2 position, Vector2 size, GE_Color color, GE_Color background, bool showProgressNumber);
		~GE_UI_DraggableProgressBar();
		void giveEvent(Vector2 parrentPosition, SDL_Event event);

		bool wantsEvents;
	private:
		Vector2 position;
		Vector2 size;

		bool pollMouse;
};

class GE_UI_Titlebar
{
	public:
		GE_UI_Titlebar(SDL_Renderer* renderer, std::string name, GE_UI_WindowTitleStyle style);
		~GE_UI_Titlebar();
		void giveEvent(Vector2 parrentPosition, double parrentWidth, SDL_Event event, Vector2* windowPosition);
		void render(Vector2 parrentPosition, double parrentWidth);

		bool wantsEvents;
	private:
		GE_UI_Text* title;
		GE_RectangleShape* background;
		GE_UI_Button* XButton;
		GE_UI_WindowTitleStyle style;

		bool dragging;
		Vector2 initialDragPosition;


};
	


class GE_UI_Surface : public GE_UI_Element
{
	public:
		GE_UI_Surface(SDL_Renderer* renderer, Vector2 position, Vector2 size, GE_Color backgroundColor);
		~GE_UI_Surface();
		void render(Vector2 parrentPosition);
		int addElement(GE_UI_Element* element);
		GE_UI_Element* getElement(int elementID);
		void giveEvent(Vector2 position, SDL_Event event);
		bool checkIfFocused(int mousex, int mousey);


		bool isOpen = false;

		Vector2 position;
		Vector2 size;
		
	private:
		GE_UI_Element* elements[MAX_SURFACE_UI_ELEMENTS];
		int nextUIElement = 0;
		SDL_Renderer* renderer;
		GE_RectangleShape* backgroundRect;
		
};


class GE_UI_Window: public GE_UI_TopLevelElement
{
	public: 
		GE_UI_Window(SDL_Renderer* renderer, std::string name, Vector2 position, Vector2 surfaceSize, GE_UI_Style style);
		~GE_UI_Window();
		void giveEvent(Vector2 parrentPosition, SDL_Event event);
		void render(Vector2 parrentPosition);
		bool checkIfFocused(int mousex, int mousey);


		bool wantsEvents;


	private:
		GE_UI_Titlebar* titlebar;
		GE_UI_Surface* surface;
		GE_RectangleShape* border;

		Vector2 position;
		Vector2 size;
		double titleBarHeight;
		double borderOffset;

};


void GE_UI_InsertTopLevelElement(GE_UI_TopLevelElement* element);
void GE_UI_RemoveTopLevelElement(GE_UI_TopLevelElement* element);
void GE_UI_SetTopElement(GE_UI_TopLevelElement* element);
void GE_UI_SetBackgroundElement(GE_UI_TopLevelElement* element);

void GE_UI_PullEvents();
void GE_UI_Render();





#endif //__UI_INCLUDED
