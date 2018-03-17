/*!
 * @file
 * @author Jackson McNeill
 *
 * A standard level editor
 */
#pragma once

#include <functional>

#include "UI.h"

#include "gameRender.h"
#include "camera.h"
#include "physics.h"
#include "gluePhysicsObject.h"

/*!
 * return a new physics object when given the position under the cursor
 */
typedef std::function<GE_PhysicsObject*(SDL_Renderer*,Vector2r)> GE_NewObject_t;


class GE_Stars;



struct GE_UI_LevelEditor2DStyle
{
	GE_UI_Style general;
	GE_UI_TextListStyle dropDown;
};

class GE_UI_LevelEditor2D : public GE_UI_TopLevelElement
{
	public:
		GE_UI_LevelEditor2D(SDL_Renderer* renderer, Vector2 position, Vector2 size,GE_UI_LevelEditor2DStyle);
		~GE_UI_LevelEditor2D();
		void render(Vector2 parrentPosition);
		void giveEvent(Vector2 parrentPosition, SDL_Event event);
		bool checkIfFocused(int mousex, int mousey);

		void getRealWorldCursorPosition(double* x, double* y);

		double levelScale;
	private:
		SDL_Renderer* renderer;
		GE_UI_Surface* mySurface;
		Vector2 position;
		Vector2 size;
		GE_UI_LevelEditor2DStyle style;
		GE_UI_GameRender* levelRenderer;
		Camera* camera;


		GE_PhysicsObject* focusedObject;
		bool hasFocusedObject;
		GE_GlueTarget* focusedObjectChangeGlue;
		Vector2 originalDistanceFromCenter;
		Vector2r tempvector;

		GE_UI_TextList* rightClickMenu;
		bool rightClickOpen;

		bool keysHeld[323];

		GE_Stars* stars[5];
		void setStarsScale(double scale);

		void updateMovingObjectPosition();
	

};

enum GE_LevelEditorObjectProperty_Resizability
{
	NONE,
	WHOLE_OBJECT,
	USER_DEFINED_SIZE_RECTANGLESALL_COLLISION_RECTANGLES,
};
struct GE_LevelEditorObjectProperties
{
	GE_LevelEditorObjectProperty_Resizability resizability;
};


/*!
 * Classes which register with the editor must inherit from this interface.
 */
class GE_LevelEditorInterface
{
	public:
		GE_LevelEditorObjectProperties properties;
		/*!
		 * Required for objects which have WHOLE_OBJECT as their resizability property.
		 */
		virtual void setWholeObjectSize(Vector2 size) {}; 
};
void internal_RegisterClassWithLevelEditor(std::string name, GE_NewObject_t allocationFunction, unsigned long long classID,GE_LevelEditorObjectProperties properties);


template <class type>
void GE_RegisterClassWithLevelEditor(unsigned long long classID)
{
	internal_RegisterClassWithLevelEditor(type::name, type::spawnFromLevelEditor,classID,type::properties);	

}
