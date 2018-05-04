/*!
 * @file
 * @author Jackson McNeill
 *
 * A simple object-oriented UI system. This is intended to remain as minimal as possible, while providing necasary features for game HUDs and UIs. 
 * Many objects do not support rotation; some do (usually if used in HUDs). This helps to keep this system minimal.
 *
 * Copying any UI element results in undefined behavior. Relying on a GE_UI_Window without checking if it's been deleting results in undefined behavior. Top level elements might not be rendered every frame--optimization is allowed to be performed.
 */
#pragma once



#include <SDL2/SDL_ttf.h>
#include <string>
#include <functional>
#include <memory>
#include <optional>

#include "font.h"
#include "shapes.h"

class Vector2;
class IntVector2;


union SDL_Event;


//limits:
#define MAX_SURFACE_UI_ELEMENTS 256

struct GE_UI_FontStyle
{
	GE_Color color;
	GE_Font font;
};
struct GE_UI_ButtonStyle
{
	GE_Color background;
	GE_Color backgroundPressed;
	GE_Color text;
	GE_Color textPressed;
	Vector2 paddingSize;
	GE_Font font;
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

struct GE_UI_SpacerStyle
{
	GE_Color color;
	double size;
	double spaceLeftAtEachEdge;
	double extraSpaceBetween;
};
struct GE_UI_TextListStyle
{
	GE_UI_FontStyle font;
	unsigned int verticalSizeOfText;
	double textSpaceLeftAtEachEdge;
	GE_Color background;
	GE_Color highlight;
	GE_UI_SpacerStyle spacer;
	unsigned int spaceBetweenElements;
};
struct GE_UI_StringOrDivider
{
	std::string string;
	bool isDivider;
};

struct GE_UI_ClickedObject
{
	std::vector<unsigned int> trail;
	bool happened;
};
/*!
 * An element which is intended to be added to a GE_UI_TopLevelElement
 */
class GE_UI_Element
{
	public:
		virtual void render(Vector2 parrentPosition) = 0;
		virtual void giveEvent(Vector2 parrentPosition, SDL_Event event);
		virtual ~GE_UI_Element();
		bool wantsEvents;
};

/*!
 * An element which is intended to hold various GE_UI_Elements. This type of element must be able to check if it should steal focus. Similar to a window in an operating system.
 */
class GE_UI_TopLevelElement : public GE_UI_Element
{
	public:
		virtual void render(Vector2 parrentPosition) = 0;
		virtual void giveEvent(Vector2 parrentPosition, SDL_Event event) = 0;
		virtual bool checkIfFocused(int mousex, int mousey) = 0;
		virtual ~GE_UI_TopLevelElement();

		//bool wantsEvents;
};


GE_FORCE_INLINE bool checkIfFocused_ForBox(int mousex, int mousey, Vector2 position, Vector2 size) //inline convinience function for checking wheather the mouse clicked a box 
{
	if (mousex >= position.x && mousex <= position.x+size.x && mousey >= position.y && mousey <= position.y+size.y)
	{
		return true;
	}
	return false;
}

class GE_UI_Text : public GE_UI_Element
{
	public:
		GE_UI_Text(SDL_Renderer* renderer, Vector2 position, Vector2 size, std::string text, GE_Color color,GE_Font font);
		GE_UI_Text(SDL_Renderer* renderer, Vector2 position, Vector2 size, std::string text, GE_UI_FontStyle style);
		~GE_UI_Text();
		void render(Vector2 parrentPosition);
		void render();

		/*!
		 * Note that settings text above 2048 characters, while permitted, will not be optimized for not re-rendering text that is the same as the previous set.
		 */
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
		 * Align the text to the left
		 */
		void alignLeft();


		/*!
		 * Expand so that our boundries are equal to the ammount of text we're currently holding at this moment
		 */
		void expandToTextSize();
		void setCursor(int pos);
	//	bool wantsEvents;
		SDL_Rect Message_rect;


	private:
		GE_Font font;//Sans = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSans.ttf", 15);
		Vector2 position;
		Vector2 size;
		GE_Color color;
		SDL_Renderer* renderer;
		SDL_Texture* Message;
		char currentText[2048];
		int scrollPosition;
		int cursorPosition;

		bool doCenterX;
		bool doCenterY;
		bool doAlignLeft;

		const void _init(SDL_Renderer* renderer, Vector2 position, Vector2 size, std::string text, GE_Color color);
};
class GE_UI_TextInput : public GE_UI_Element
{
	public:
		GE_UI_TextInput(SDL_Renderer* renderer, Vector2 position, Vector2 size, GE_Color textColor, GE_Color color, GE_Font font);
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
		GE_UI_Button(SDL_Renderer* renderer, Vector2 position, Vector2 paddingSize, std::string text, GE_Color textColor, GE_Color color, GE_Color pressedColor, GE_Font font);
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
enum class GE_UI_PositioningRectangleModifiers 
{
	NONE,
	centerX,
	centerY,
	expandSizeToFit,
	SIZE,
};
class GE_UI_PositioningRectangle
{
	public:
		GE_UI_PositioningRectangle(Vector2 position, Vector2 size);
		
		Vector2 getPosition();
		Vector2 getSize();

		void setPosition(Vector2 position);
		void setSize(Vector2 size);

		void setModifier(GE_UI_PositioningRectangleModifiers modifier, bool value);
		bool getModifierValue(GE_UI_PositioningRectangleModifiers modifier);

		Vector2 getOriginPosition();
		Vector2 getOriginSize();

	private:
		Vector2 originPosition;
		Vector2 originSize;

		bool modifiers[static_cast<int>(GE_UI_PositioningRectangleModifiers::SIZE)] = {0}; //initialize array to 0

};

/*!
 * An experimental UI implementation which makes use of a positioning rectangle, which effectively creates a standard method of positioning, centering, and sizing.
 */
class GE_Experimental_UI_Button : public GE_UI_Element
{
	public:
		GE_Experimental_UI_Button(SDL_Renderer* renderer, Vector2 position, Vector2 paddingSize, GE_Color color, GE_Color pressedColor, GE_Color highlightColor, GE_UI_Text* text);
		~GE_Experimental_UI_Button();
		void render(Vector2 parrentPosition);
		void render();
		void giveEvent(Vector2 parrentPosition, SDL_Event event);
		Vector2 getSize();
		bool getIsTriggered();
		void setIsTriggered(bool triggered);
		bool getIsHighlighted();

		std::unique_ptr<GE_UI_PositioningRectangle> positioningRectangle;
	private:
		SDL_Renderer* renderer;
		Vector2 position;
		Vector2 paddingSize;
		GE_UI_Text* text;
		
		bool pressed;
		bool triggered;
		bool highlighted;

		std::unique_ptr<GE_RectangleShape> normalRectangle;
		std::unique_ptr<GE_RectangleShape> pressedRectangle;
		std::unique_ptr<GE_RectangleShape> highlightRectangle;
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
	private:
		Vector2 position;
		Vector2 size;

		bool pollMouse;
};

class GE_UI_Window;

class GE_UI_Titlebar
{
	public:
		GE_UI_Titlebar(SDL_Renderer* renderer, std::string name, GE_UI_Window* parrent, GE_UI_WindowTitleStyle style);
		~GE_UI_Titlebar();
		void giveEvent(Vector2 parrentPosition, double parrentWidth, SDL_Event event, Vector2* windowPosition);
		void render(Vector2 parrentPosition, double parrentWidth);

		bool wantsEvents; //TODO temp
	private:
		GE_UI_Text* title;
		GE_RectangleShape* background;
		GE_UI_Button* XButton;
		GE_UI_WindowTitleStyle style;
		GE_UI_Window* parrent;

		bool dragging;
		Vector2 initialDragPosition;


};
	


/*!
 * Easy container which holds other elements. The first element added to this will be in the background, while the last will be in the foreground.
 */
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

		GE_UI_Surface* surface;


	private:
		GE_UI_Titlebar* titlebar;
		GE_RectangleShape* border;

		Vector2 position;
		Vector2 size;
		double titleBarHeight;
		double borderOffset;

};

/*!
 * like RectangleShape, but with a static position
 */
class GE_UI_Rectangle : public GE_UI_Element
{
	public:
		GE_UI_Rectangle(SDL_Renderer* renderer, Vector2 position, Vector2 size, GE_Color color);
		~GE_UI_Rectangle();
		void render(Vector2 parrentPosition);
	private:
		GE_RectangleShape* rectangleShape;
		Vector2 position;
		Vector2 size;


};

struct internal_return;
class GE_UI_TextList : public GE_UI_TopLevelElement
{
	public:
		GE_UI_TextList(SDL_Renderer* renderer, Vector2 position, Vector2 size, std::vector<GE_UI_StringOrDivider> elements, GE_UI_TextListStyle style);
		GE_UI_TextList(SDL_Renderer* renderer, Vector2 position, Vector2 size, std::vector<GE_UI_StringOrDivider> elements, std::vector<GE_UI_TextList*> dropRightMenus, std::vector<unsigned int> elementsWhichHostDropRightMenus, GE_UI_TextListStyle style);
		~GE_UI_TextList();
		void giveEvent(Vector2 parrentPosition, SDL_Event event);
		void render(Vector2 parrentPosition);
		bool checkIfFocused(int mousex, int mousey);
		GE_UI_ClickedObject getClicked();
		void setClicked(bool happened);
		double getPositionOfElement(unsigned int ID);

		void setPosition(Vector2 position);
		Vector2 getSize();
	private:
		SDL_Renderer* renderer;
		Vector2 position;
		Vector2 size;
		GE_UI_TextListStyle style;
		GE_UI_Rectangle* background;
		std::vector<GE_UI_Text*> textObjects;
		std::vector<double> textObjectsPosition;
		std::vector<GE_UI_Rectangle*> dividers;

		GE_RectangleShape* highlight;
		unsigned int highlightedTextObject;
		bool hasHighlightedTextObject;
		
		bool hasDropRightMenus;
		std::vector<GE_UI_TextList*> dropRightMenus;
		std::vector<unsigned int> elementsWhichHostDropRightMenus;

		unsigned int dropRightMenuOpen;
		bool hasDropRightMenuOpen;

		GE_UI_ClickedObject currentClicked;
		void _construct_step_1(SDL_Renderer* renderer, Vector2 position, Vector2 size, std::vector<GE_UI_StringOrDivider> elements, GE_UI_TextListStyle style);
		internal_return checkIfCursorOnText(Vector2 parrentPosition, int x, int y);
};


struct GE_UI_IDStringList
{
	unsigned int ID;
	std::string string;
};
struct GE_UI_ListElement
{
	bool divider;
	unsigned int ID; //ignored if divider is true
};


class GE_Experimental_UI_TextList : GE_UI_Element
{
	public:
		GE_Experimental_UI_TextList(SDL_Renderer* renderer, Vector2 position, std::map<unsigned int, std::string> list, std::vector<GE_UI_ListElement> elements,bool listGoesDown, GE_UI_TextListStyle style);
		void render(Vector2 parrentPosition) override;
		void giveEvent(Vector2 parrentPosition, SDL_Event event) override;

		std::optional<unsigned int> getHighlighted();
		void setSelected(std::optional<unsigned int> selected);
		std::optional<unsigned int> getSelected();

	private:
		std::vector<std::unique_ptr<GE_UI_Rectangle>> dividers;
		std::vector<std::unique_ptr<GE_Experimental_UI_Button>> texts;
		std::vector<unsigned int> userIDs;
		std::unique_ptr<GE_UI_Rectangle> background;
		
		std::vector<GE_UI_ListElement> elements;

		double fontHeight;
		Vector2 size;
		Vector2 position;


		std::optional<unsigned int> highlightedText;
		std::optional<unsigned int> selectedText;

		

		GE_UI_TextListStyle style;
		Vector2 calcSize(std::map<unsigned int, std::string> list, bool listGoesDown );



};





/*!
 * This recieves all events all the time
 *
 * Typical use is to have one single one for your entire game and to never remove it until shutdown. But you could add and remove one every frame if you wanted to. Must be called from the renderer thread, or while the rendering thread is locked
 */
class GE_UI_OmniEventReciever
{
	public:
		virtual ~GE_UI_OmniEventReciever(){};
		virtual void giveEvent(SDL_Event event) = 0;
};




/*!
 * Inserts a top level element but does not focus it
 */
void GE_UI_InsertTopLevelElement(GE_UI_TopLevelElement* element);

/*!
 * Removes a top level element and does not free any memory
 */
void GE_UI_RemoveTopLevelElement(GE_UI_TopLevelElement* element);

/*
 * Inserts another omni event reciever
 * Memory managed automatically by the UI library. 
 */
void GE_UI_InsertOmniEventReciever(GE_UI_OmniEventReciever* element);

/*!
 * Removes an omni event reciever and deletes it
 */
void GE_UI_RemoveOmniEventReciever(GE_UI_OmniEventReciever* element);

/*
 * Set the top(focused) top level element (so, one GE_UI_TopLevelElement becomes focused)
 */
void GE_UI_SetTopElement(GE_UI_TopLevelElement* element);

/*!
 * Sets THE ONLY background element
 */
void GE_UI_SetBackgroundElement(GE_UI_TopLevelElement* element);

/*!
 * Returns the background element
 */
GE_UI_TopLevelElement* GE_UI_GetBackgroundElement();

/*!
 * Removes the background element and does not free any memory
 */
void GE_UI_RemoveBackgroundElement();

/*!
 * Sets THE ONE cursor follower
 */
void GE_UI_SetCursorFollower(GE_UI_TopLevelElement* element);

/*!
 * Gives events to the focused top level UI element, and may shift focus 
 */
bool GE_UI_PullEvents();

/*!
 * Renders all top level UI elements. Don't assume your element will be rendered every time, optimization is allowed to be performed to not render hidden elements
 */
void GE_UI_Render();


IntVector2 GE_UI_GetMousePosition();

struct MessageDialog_StringInputOptions
{
	bool allowCancel;
};
struct MessageDialog_StringInputReturn
{
	bool readyToRead;
	bool accepted;
	std::string output;
};
void MessageDialog_StringInput(SDL_Renderer* renderer, std::string name, std::string startingText,MessageDialog_StringInputOptions options, MessageDialog_StringInputReturn& returnVal, GE_UI_Style style);



void GE_ShutdownUI();

