#include "debugRenders.h"

#include<SDL2/SDL.h>
#include<SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <math.h>
#include <vector>

#include "camera.h"
#include "UI.h"

#define BASE_DIR "../"


#define FONT_DIR BASE_DIR"fonts/"

#define FREESANS_LOC FONT_DIR"gnuFreeFonts/FreeSans.ttf"

SDL_Renderer* GE_DEBUG_Renderer;//memory leak - wontfix
Camera* GE_DEBUG_Camera; //memory leak - wontfix
TTF_Font* debugSans; //memory leak - wontfix
bool isInit = false;
void init()
{
	debugSans = TTF_OpenFont(FREESANS_LOC, 15);
	if(!debugSans) {
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
	auto mytext = GE_UI_Text(GE_DEBUG_Renderer, position, {0,0}, text, {0xff,0xff,0x99,0xFF},debugSans); //create a new text to render it once. debug-grade effeciency
	mytext.expandToTextSize();
	mytext.render({0,0});
}

void GE_DEBUG_TextAt(std::string text, Vector2r position)
{
	GE_DEBUG_TextAt(text,Vector2{position.x,position.y});
}

void GE_DEBUG_TextAt_PhysicsPosition(std::string text, Vector2 position)
{
	GE_DEBUG_TextAt(text,(position-Vector2{GE_DEBUG_Camera->pos.x-(GE_DEBUG_Camera->screenWidth/2),GE_DEBUG_Camera->pos.y-(GE_DEBUG_Camera->screenHeight/2)}));
}

void GE_DEBUG_Cursor_TextAtCursor()
{
	int x,y;
	SDL_GetMouseState(&x,&y);
	GE_DEBUG_TextAt("  x "+std::to_string(x)+" y "+std::to_string(y),Vector2{static_cast<double>(x),static_cast<double>(y)});
}
	

Vector2 GE_DEBUG_QuickAddY(Vector2 positon, double addx)
{
	positon.y += addx;
	return positon;
}
Vector2r GE_DEBUG_QuickAddY(Vector2r positon, double addx)
{
	positon.y += addx;
	return positon;
}


std::string GE_DEBUG_VectorToString(Vector2 subject)
{
	return "X: "+std::to_string(subject.x) + " Y: "+std::to_string(subject.x);
}

std::string GE_DEBUG_VectorToString(Vector2r subject)
{
	return "X: "+std::to_string(subject.x) + " Y: "+std::to_string(subject.x) + " R: "+std::to_string(subject.r);
}



void GE_DEBUG_DrawLine(Vector2 start, Vector2 end)
{
	SDL_SetRenderDrawColor(GE_DEBUG_Renderer,0xff,0xff,0x99,SDL_ALPHA_OPAQUE);
	SDL_RenderDrawLine(GE_DEBUG_Renderer,start.x,start.y,end.x,end.y);
}
void GE_DEBUG_DrawLine(GE_Line shapeLine)
{
	if (shapeLine.mIsInf)
	{
		GE_DEBUG_DrawLine({shapeLine.b,0},{shapeLine.b,1000});
	}
	else 
	{
		GE_DEBUG_DrawLine({0,shapeLine.b},{1000,(shapeLine.m*1000)+shapeLine.b});
	}
}

void GE_DEBUG_DrawRect(GE_Rectangle rect,GE_Color clr)
{
	SDL_SetRenderDrawColor(GE_DEBUG_Renderer,clr.r,clr.g,clr.b,clr.a);
	auto sdlr = SDL_Rect{static_cast<int>(rect.x),static_cast<int>(rect.y),static_cast<int>(rect.w),static_cast<int>(rect.h)};
	SDL_RenderDrawRect(GE_DEBUG_Renderer,&sdlr);
}
void GE_DEBUG_DrawRect(GE_Rectangle rect)
{
	GE_DEBUG_DrawRect(rect,{0xff,0xff,0x99,SDL_ALPHA_OPAQUE});
}
void GE_DEBUG_DrawShape(GE_ShapeLines shape)
{
	auto lastSpot = shape.begin();
	for (auto spot = shape.begin()+1;spot!= shape.end();spot++)
	{
		GE_DEBUG_DrawLine(*lastSpot,*spot);
		lastSpot = spot;
	}
}

