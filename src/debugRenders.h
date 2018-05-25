/*!
 * @file
 * @author Jackson McNeill
 *
 * Debug-grade renderers for debugging. 
 * DO NOT USE WHILE PERFORMANCE TESTING! Debug renders have a tendancy to cause bad and inconsistant performance.
 *
 * _PhysicsPosition functions do not account for rotation - they assume you will lock rotation during debugging.
 */
#pragma once

#include <string>
//Local includes
#include "line.h"
#include "shapes.h"


//forward declarations

struct SDL_Renderer;
class Camera;
class Vector2;
class Vector2;
class GE_Rectangle;
class GE_Color;







extern SDL_Renderer* GE_DEBUG_Renderer;
extern Camera* GE_DEBUG_Camera; 
void GE_DEBUG_PassRenderer(SDL_Renderer* yourRenderer, Camera* yourCamera);

void GE_DEBUG_TextAt(std::string text, Vector2 position);
void GE_DEBUG_TextAt(std::string text, Vector2r position);

void GE_DEBUG_TextAt_PhysicsPosition(std::string text, Vector2 position);

template<class XY>
void GE_DEBUG_TextAt_PhysicsPosition(std::string text, XY position)
{
	GE_DEBUG_TextAt_PhysicsPosition(text,{position.x,position.y});
}

std::string GE_DEBUG_VectorToString(Vector2 subject);
std::string GE_DEBUG_VectorToString(Vector2r subject);

void GE_DEBUG_Cursor_TextAtCursor();
Vector2 GE_DEBUG_QuickAddY(Vector2 positon, double addx);
Vector2r GE_DEBUG_QuickAddY(Vector2r positon, double addx);

void GE_DEBUG_DrawLine(Vector2 start, Vector2 end,GE_Color color);

void GE_DEBUG_DrawLine(Vector2 start, Vector2 end);

void GE_DEBUG_DrawLine(GE_Line shapeLine);

void GE_DEBUG_DrawLine_PhysicsPosition(Vector2 start, Vector2 end,GE_Color color);

void GE_DEBUG_DrawLine_PhysicsPosition(Vector2 start, Vector2 end);

void GE_DEBUG_DrawRect(GE_Rectangle rect,GE_Color clr);

void GE_DEBUG_DrawRect(GE_Rectangle rect);

void GE_DEBUG_DrawShape(GE_ShapePoints shape);

void GE_DEBUG_DrawRect_PhysicsPosition(GE_Rectangle rect, GE_Color clr);

void GE_DEBUG_DrawRect_PhysicsPosition(GE_Rectangle rect);
