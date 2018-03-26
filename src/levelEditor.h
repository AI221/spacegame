/*!
 * @file
 * @author Jackson McNeill
 *
 * A standard level editor
 */
#pragma once

#include <functional>
#include <type_traits>
#include <cassert>

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

struct GE_LevelEditorObjectProperties
{
	unsigned int numResizableRectangles;
};


/*!
 * Classes which register with the editor must inherit from this interface.
 *
 * All static values must be implemented. Their presence in this interface is for human readability purposes only (as they are used by the class registration template)
 */
class GE_LevelEditorInterface
{
	public:
		/*!
		 * Returns one of the resizable rectangles.
		 */
		virtual GE_Rectangle getRelativeRectangle(unsigned int UNUSED(id)) 
		{
			assert(false);
		};

		/*!
		 * Sets one of the relative rectangles. Runs with the physics thread locked.
		 */
		virtual void setRelativeRectangle(unsigned int UNUSED(id), GE_Rectangle UNUSED(rectangle))
		{
			assert(false);
		};

		/*!
		 * Simply holds the human-readable name of this class
		 */
		const static std::string name;

		/*!
		 * The non-user-defined properties of this object
		 */
		constexpr const static GE_LevelEditorObjectProperties properties = {1};
		
		virtual ~GE_LevelEditorInterface(){};
};
void internal_RegisterClassWithLevelEditor(std::string name, GE_NewObject_t allocationFunction, unsigned long long classID,GE_LevelEditorObjectProperties properties);


template <class type>
void GE_RegisterClassWithLevelEditor(unsigned long long classID)
{
	static_assert(std::is_base_of<GE_LevelEditorInterface,type>::value,"GE_RegisterClassWithLevelEditor - Class must inherit from GE_LevelEditorInterface");
	internal_RegisterClassWithLevelEditor(type::name, type::spawnFromLevelEditor,classID,type::properties);	

}
