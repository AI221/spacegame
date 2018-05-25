/*!
 * @file
 * @author Jackson McNeill
 * Performs raycasting
 */
#pragma once

#include "line.h"
#include <vector>
#include "physics.h"

struct Vector2;

struct ObjectRaycastReturn
{
	GE_PhysicsObject* victim;
	Vector2 position;
};

/*!
 * Raycasts using a line and shapes and returns the collision point. 
 */
std::optional<Vector2> GE_Raycast(Vector2 start, Vector2 end, GE_ShapePointsVector obstacles);
/*!
 * Raycasts using a line and an non-order-specific vector list of physics objects and returns the physics object hit and the spot hit
 */
ObjectRaycastReturn GE_Raycast(Vector2 start, Vector2 end, std::vector<GE_PhysicsObject*> objects);


//GE_PhysicsObject* GE_Raycast(Vector2 start, Vector2 end, std::vector<GE_PhysicsObject*> objects);

