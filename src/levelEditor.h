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
#include <memory>
#include <optional>

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
class GE_UI_LevelEditor2D;


GE_FORCE_INLINE bool single_run_if_object_at(Vector2 position, std::function<void(GE_PhysicsObject* obj)> function)
{
	std::set<GE_PhysicsObject*> objects = GE_GetObjectsInRadius(position,500);
	for( GE_PhysicsObject* obj : objects)
	{
		if (GE_IsPointInPhysicsObject(position,obj))
		{
			function(obj);
			return true;
		}
	}
	return false;
}



struct GE_UI_HighlightBoxStyle
{
	double lineThickness;
	double draggableBoxDiameter;
	double rotatableBoxDiameter;
	GE_Color boxColor;
	GE_Color draggableBoxColor;
	GE_Color rotatableBoxColor;
};
enum internal_draggablebox_position
{
	topLeft,
	topRight,
	bottomRight,
	bottomLeft,
	middleTop,
	middleRight,
	middleBottom,
	middleLeft,
	center,
	rotater,
	hollowBox,
};


/*!
 * Gives a resizable, moveable, rotatable box. Positioned by CENTER!
 */
class GE_UI_HighlightBox : GE_UI_Element
{
	public:
		GE_UI_HighlightBox(SDL_Renderer* renderer, Camera* camera, GE_UI_LevelEditor2D* levelEditor, Vector2r hostPosition, GE_Rectangler rectangle,bool resizeEnabled,GE_UI_HighlightBoxStyle style);
		~GE_UI_HighlightBox();
		void render(Vector2 parrentPosition);
		void giveEvent(Vector2 parrentPosition, SDL_Event event);
		GE_UI_PositioningRectangle* positioningRectangle;
		GE_Rectangler getRectangle();



		Vector2 size;
	private:
		bool resizeEnabled; 
		std::unique_ptr<GE_RectangleShape> draggableBoxes;
		std::unique_ptr<GE_RectangleShape> rotatableBoxes;
		std::unique_ptr<GE_HollowRectangleShape> box;

		Vector2r originPosition;

		Vector2r position;
		Camera* camera;
		Camera scaledcamera;
		GE_UI_LevelEditor2D* levelEditor;

		GE_UI_HighlightBoxStyle style;

		bool isBeingDragged; 
		internal_draggablebox_position boxDragged;
		bool resizeRectangleByCenter;


		Vector2 initialMousePosition;
		Vector2 initialSize;
		Vector2r initialPosition;
		bool expandByCenter; //basically, if true, when the corners are dragged both sides will expand equally. probably set when SHIFT is held down.

		Vector2r getDraggableBoxPosition(Vector2r topleftParrentPosition,internal_draggablebox_position box, bool addCamera);
		Vector2r getTopLeftPosition(Vector2 parrentPosition);

		void renderDraggableBox(unsigned int box, Vector2r parrentPosition);

		
};


struct GE_UI_LevelEditor2DStyle
{
	GE_UI_Style general;
	GE_UI_TextListStyle dropDown;
};


class GE_PhysicsObjectHighlightBoxManager;
class GE_PhysicsObjectResizeManager;
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
		Vector2r cursorVector;

		GE_UI_TextList* rightClickMenu_normal;
		GE_UI_TextList* rightClickMenu_object;
		enum {
			NONE,
			NORMAL,
			OBJECT
		} rightClickOpen;
		GE_PhysicsObject* rightClickMenuSelectedObject;

		bool keysHeld[323];

		GE_Stars* stars[5];
		void setStarsScale(double scale);

		void updateMovingObjectPosition();

		std::optional<std::unique_ptr<GE_PhysicsObjectHighlightBoxManager>> highlightBoxManager;

		std::optional<std::unique_ptr<GE_PhysicsObjectResizeManager>> resizeManager;

		void closeRightClickMenu();
		void closeObjectManipulators();
	

};


struct GE_LevelEditorObjectProperties
{
	unsigned int numResizableRectangles;
	bool resizeEnabled;
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
		 * Returns one of the resizable rectangles, in absolute cordinates.
		 */
		virtual GE_Rectangler getRectangle(unsigned int UNUSED(id)) 
		{
			assert(false);
		};

		/*!
		 * Sets one of the rectangles. Runs with the physics thread locked. Given in absolute cordinates.
		 */
		virtual void setRectangle(unsigned int UNUSED(id), GE_Rectangler UNUSED(rectangle))
		{
			assert(false);
		};
		
		/*!
		 * This should be implemented if you set the "resizeEnabled" property is set
		 */
		virtual void setSize(Vector2 size)
		{
			assert(false);
		};

		/*!
		 * This should be implemented if you set the "resizeEnabled" property is set
		 */
		virtual Vector2 getSize()
		{
			assert(false);
			return {0,0};
		};

		/*!
		 * Contains a default implementation -- suitable for most cases
		 */
		virtual void setPosition(Vector2r position)
		{
			dynamic_cast<GE_PhysicsObject*>(this)->position = position;
		}

		/*!
		 * Contains a default implementation -- suitable for most cases
		 */
		virtual Vector2r getPosition()
		{
			return dynamic_cast<GE_PhysicsObject*>(this)->position;
		}

		/*!
		 * Simply holds the human-readable name of this class
		 */
		const static std::string name;

		/*!
		 * The properties of this object
		 */
		constexpr const static GE_LevelEditorObjectProperties properties = {0,false,};
		
		virtual ~GE_LevelEditorInterface(){};
};
void internal_RegisterClassWithLevelEditor(std::string name, GE_NewObject_t allocationFunction, unsigned long long classID,GE_LevelEditorObjectProperties properties);


template <class type>
void GE_RegisterClassWithLevelEditor(unsigned long long classID)
{
	static_assert(std::is_base_of<GE_LevelEditorInterface,type>::value,"GE_RegisterClassWithLevelEditor - Class must inherit from GE_LevelEditorInterface");
	internal_RegisterClassWithLevelEditor(type::name, type::spawnFromLevelEditor,classID,type::properties);	

}
