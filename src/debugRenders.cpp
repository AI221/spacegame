#include "debugRenders.h"

#define BASE_DIR "../"


#define FONT_DIR BASE_DIR"fonts/"

#define FREESANS_LOC FONT_DIR"FreeSans.ttf"

SDL_Renderer* GE_DEBUG_Renderer;
Camera* GE_DEBUG_Camera; 
TTF_Font* tinySans;
bool isInit = false;
void init()
{
	tinySans = TTF_OpenFont(FREESANS_LOC, 15);
	if(!tinySans) {
		printf("TTF_OpenFont error (from debugRenders.cpp): %s\n", TTF_GetError());
		exit(100);
	}
}
#define initIfNeeded() if(!isInit) {isInit = true; init();} //glorious inline initialization

void GE_DEBUG_PassRenderer(SDL_Renderer* yourRenderer, Camera* yourCamera)
{
	initIfNeeded();
	GE_DEBUG_Renderer = yourRenderer;
	GE_DEBUG_Camera = yourCamera;
}
void GE_DEBUG_TextAt(std::string text, Vector2 position)
{
	initIfNeeded();
	auto mytext = GE_UI_Text(GE_DEBUG_Renderer, position, {0,0}, text, {0xff,0xff,0x99,0xFF},tinySans); //create a new text to render it once. debug-grade effeciency
	mytext.expandToTextSize();
	mytext.render({0,0});
}

void GE_DEBUG_TextAt(std::string text, Vector2r position)
{
	GE_DEBUG_TextAt(text,Vector2{position.x,position.y});
}

void GE_DEBUG_TextAt_PhysicsPosition(std::string text, Vector2 position)
{
	GE_DEBUG_TextAt(text,(position-Vector2{GE_DEBUG_Camera->pos.x-GE_DEBUG_Camera->screenWidth,GE_DEBUG_Camera->pos.y-GE_DEBUG_Camera->screenHeight}));
}
