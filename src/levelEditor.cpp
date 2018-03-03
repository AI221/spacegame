#include "levelEditor.h"

#include <pthread.h>
#include <set>
#include <map>
#include <algorithm>

//Local includes

#include "stars.h"
#include "serializeObject.h"
#include "FS.h"


//Debug includes

#include "debugRenders.h"


typedef unsigned long long class_id_t;

std::vector <std::string> classNames;
std::vector <GE_NewObject_t> classCreationFunctions;
//map our internal number to the real one
std::map<class_id_t,class_id_t> classRealTypeIDToLevelTypeID;



struct GE_UI_HighlightBoxStyle
{
	double lineThickness;
	double draggableBoxDiameter;
	double rotatableBoxDiameter;
	GE_Color boxColor;
	GE_Color draggableBoxColor;
	GE_Color rotatableBoxColor;
};

/*!
 * Gives a resizable, moveable, rotatable box. Positioned by CENTER!
 */
class GE_UI_HighlightBox : GE_UI_Element
{
	public:
		GE_UI_HighlightBox(SDL_Renderer* renderer, Vector2r position, Vector2 size,GE_UI_HighlightBoxStyle style);
		~GE_UI_HighlightBox();
		void render(Vector2 parrentPosition);
		void giveEvent(Vector2 parrentPosition, SDL_Event event);
		GE_UI_PositioningRectangle* positioningRectangle;
	private:
		GE_RectangleShape* draggableBoxes;
		GE_RectangleShape* rotatableBoxes;
		GE_HollowRectangleShape* box;

		Vector2r position;
		Vector2 size;

		GE_UI_HighlightBoxStyle style;

		Vector2r getDraggableBoxPosition(Vector2r topleftParrentPosition,unsigned int box);
};

GE_UI_HighlightBox::GE_UI_HighlightBox(SDL_Renderer* renderer, Vector2r position, Vector2 size,GE_UI_HighlightBoxStyle style)
{
	this->position = position;
	this->size = size;
	this->style = style;

	this->draggableBoxes = new GE_RectangleShape(renderer,style.draggableBoxColor);
	this->rotatableBoxes = new GE_RectangleShape(renderer,style.rotatableBoxColor);

	this->box = new GE_HollowRectangleShape(renderer,style.boxColor,style.lineThickness);
}
GE_UI_HighlightBox::~GE_UI_HighlightBox()
{
	delete draggableBoxes;
	delete rotatableBoxes;
	delete box;
}
Vector2r GE_UI_HighlightBox::getDraggableBoxPosition(Vector2r topleftParrentPosition,unsigned int box)
{
		
	switch(box)
	{
		case 0: //top left
			break;
		case 2: //bottom right
			topleftParrentPosition.y += size.y;
			[[fallthrough]];
		case 1: //top right
			topleftParrentPosition.x += size.x;
			break;
		case 3: //bottom left
			topleftParrentPosition.y += size.y;
			break;
		case 6: //middle bottom
			topleftParrentPosition.y += size.y;
		case 4: //middle top
			topleftParrentPosition.x += size.x/2;
			break;
		case 7: //left middle
			topleftParrentPosition.y += size.y/2;
			[[fallthrough]];
		case 5: //right middle
			topleftParrentPosition.x += size.x;
			break;
	}
	topleftParrentPosition.x -= style.draggableBoxDiameter/2;
	topleftParrentPosition.y -= style.draggableBoxDiameter/2;
	return topleftParrentPosition;
}
void GE_UI_HighlightBox::render(Vector2 parrentPosition)
{
	Vector2r pos = position+parrentPosition;

	Vector2r topleftPos = pos;
	topleftPos.x -= size.x/2;
	topleftPos.y -= size.y/2;

	box->render(topleftPos,size);

	//render draggable boxes at all 4 corners
	for (int i=0;i!=4;i++)
	{
		draggableBoxes->render(getDraggableBoxPosition(topleftPos,i),{style.draggableBoxDiameter,style.draggableBoxDiameter});
	}	
	

}
void GE_UI_HighlightBox::giveEvent(Vector2 parrentPosition, SDL_Event event)
{

}


const double rightClickMenuWidth = 200;
enum class rightClickOptions
{
	newObject,	
	save,
};

GE_UI_HighlightBox* highlightBox;
GE_UI_HighlightBoxStyle style_highlightBox = {2,4,4,{0x55,0x00,0x00,0xff},{0x00,0xff,0x00,0xff},{0x00,0xff,0x00,0xff}};
GE_UI_LevelEditor2D::GE_UI_LevelEditor2D(SDL_Renderer* renderer, Vector2 position, Vector2 size,GE_UI_LevelEditor2DStyle style)
{
	this->renderer = renderer;
	this->position = position;
	this->size = size;
	this->style = style;

	this->mySurface = new GE_UI_Surface(renderer,{0,0},size,GE_Color{0x00,0x00,0x00,0xff});
	

	this->camera = new Camera{Vector2r{0,0,0},static_cast<int>(size.x),static_cast<int>(size.y)};

	#define additionalStars 3
	double maxScreenSize = std::max(camera->screenWidth,camera->screenHeight)*additionalStars;
	std::vector<GE_Color> starColors = {{0xff,0xff,0xff,0xff},{0xfb,0xf3,0xf9,0xff},{0xba,0xd8,0xfc,0xff}};
	std::vector<int> starSizes = {2,2,2,2,2,2,1,1,3};

	stars[0] =  new GE_Stars(renderer, 550*additionalStars, maxScreenSize,maxScreenSize,{2,1,1,1},(0.833333333)/10,starColors,camera);
	stars[1] =  new GE_Stars(renderer, 300*additionalStars, maxScreenSize,maxScreenSize,starSizes,0.833333333/5, starColors,camera);
	stars[2] =  new GE_Stars(renderer, 100*additionalStars, maxScreenSize,maxScreenSize,starSizes,0.833333333/3,starColors,camera) ;
	stars[3] =  new GE_Stars(renderer, 50*additionalStars, maxScreenSize,maxScreenSize,starSizes,0.833333333/2,starColors,camera) ;

	for (int i=0;i!=4;i++)
	{
		mySurface->addElement(stars[i]);
	}


	std::vector<GE_UI_StringOrDivider> objectSubMenuNames;
	for (std::string name : classNames)
	{
		objectSubMenuNames.push_back({name,false});	
	}

	auto newObjectSubMenu = new GE_UI_TextList(renderer,{0,0},{rightClickMenuWidth,200},objectSubMenuNames,style.dropDown);


	//Construct right-click menu
	std::vector<GE_UI_StringOrDivider> t;
	t.push_back({"New Object>",false});
	t.push_back({"",true});
	t.push_back({"Save",false});
	t.push_back({"",true});

	this->rightClickOpen = false;
	rightClickMenu = new GE_UI_TextList(renderer,{0,0},{rightClickMenuWidth,200},t,{newObjectSubMenu},{0},style.dropDown);


	this->levelRenderer = new GE_UI_GameRender(renderer,position,size,this->camera,0.75);
	this->levelScale = 0.75;

	mySurface->addElement(levelRenderer);

	this->hasFocusedObject = false;

	GE_PhysicsEngine_TickingObjectsEnabled = false;
	GE_PhysicsEngine_CollisionsEnabled = false;


	memset(keysHeld,false,sizeof(keysHeld));

	highlightBox = new GE_UI_HighlightBox(renderer,{50,50,1},{25,25},style_highlightBox);



}
GE_UI_LevelEditor2D::~GE_UI_LevelEditor2D()
{
	delete camera;
	delete levelRenderer;
	delete mySurface;
	GE_PhysicsEngine_TickingObjectsEnabled = true;
	GE_PhysicsEngine_CollisionsEnabled = true;
}
void GE_UI_LevelEditor2D::render(Vector2 parrentPosition)
{
	if(keysHeld[SDL_SCANCODE_W])
	{
		camera->pos.y -= (1)/levelScale;
		updateMovingObjectPosition();
	}
	if(keysHeld[SDL_SCANCODE_S])
	{
		camera->pos.y += (1)/levelScale;
		updateMovingObjectPosition();
	}
	if(keysHeld[SDL_SCANCODE_A])
	{
		camera->pos.x -= (1)/levelScale;
		updateMovingObjectPosition();
	}
	if(keysHeld[SDL_SCANCODE_D])
	{
		camera->pos.x += (1)/levelScale;
		updateMovingObjectPosition();
	}
	if (keysHeld[SDL_SCANCODE_UP])
	{
		levelScale *= 1.1;
		levelRenderer->setScale(levelScale);
		updateMovingObjectPosition();
		//setStarsScale(levelScale);
	}
	if (keysHeld[SDL_SCANCODE_DOWN])
	{
		levelScale /= 1.1;
		levelRenderer->setScale(levelScale);
		updateMovingObjectPosition();
		//setStarsScale(levelScale);
	}
	mySurface->render(parrentPosition);

	if(rightClickOpen)
	{
		rightClickMenu->render(parrentPosition);
	}
	/*int screenX,screenY;
	SDL_GetMouseState(&screenX,&screenY);
	GE_DEBUG_DrawRect({static_cast<double>(screenX),static_cast<double>(screenY),2,2});
	SDL_ShowCursor(SDL_DISABLE);*/

	highlightBox->render(parrentPosition);

}
const double cameraSpeed = .1;
void GE_UI_LevelEditor2D::giveEvent(Vector2 parrentPosition, SDL_Event event)
{
	if(rightClickOpen)
	{
		rightClickMenu->giveEvent(parrentPosition,event);
		GE_UI_ClickedObject clicked = rightClickMenu->getClicked();
		if (clicked.happened)
		{
			rightClickMenu->setClicked(false);
			rightClickOpen = false;
			printf("clicked id ");
			for (auto id : clicked.trail)
			{
				printf("%d, ",id);
			}
			printf("\n");
			rightClickOpen = false;

			switch(clicked.trail[0])
			{
				case 0:
					classCreationFunctions[clicked.trail[1]](Vector2{tempvector.x,tempvector.y});
					break;
				case 1:
					printf("do a save\n");
					size_t TEMPused,TEMPsize;
					auto TEMP = GE_SerializedTrackedObjects(&TEMPused,&TEMPsize);
					GE_WriteToFile("/tmp/REMOVE-tempsave2",TEMP,TEMPused);
					free(TEMP);

			}
		}

	}
	double x,y;
	int screenX,screenY;
	
	getRealWorldCursorPosition(&x,&y);
	SDL_GetMouseState(&screenX,&screenY);


	
	if (event.type == SDL_KEYDOWN)
	{
		if (event.key.keysym.scancode <= 323)
		{
			keysHeld[event.key.keysym.scancode] = true;
		}
	}
	else if (event.type == SDL_KEYUP)
	{
		if (event.key.keysym.scancode <= 323)
		{
			keysHeld[event.key.keysym.scancode] = false;
		}
	}
	else if (event.type == SDL_MOUSEBUTTONUP || event.type == SDL_MOUSEBUTTONDOWN)
	{
		
		if (event.type == SDL_MOUSEBUTTONUP)
		{
			if (rightClickOpen)
			{
				if (!rightClickMenu->checkIfFocused(screenX,screenY))
				{
					rightClickOpen = false;
				}
			}
		}
		if (!rightClickOpen)
		{

			if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
			{
				//get objects in an area
				pthread_mutex_lock(&PhysicsEngineMutex);
				std::set<GE_PhysicsObject*> objects = GE_GetObjectsInRadius(Vector2{static_cast<double>(x),static_cast<double>(y)},500);
				pthread_mutex_unlock(&PhysicsEngineMutex);
				for( GE_PhysicsObject* obj : objects)
				{
					if (GE_IsPointInPhysicsObject(Vector2{static_cast<double>(x),static_cast<double>(y)},obj))
					{
						if(hasFocusedObject)
						{
							GE_FreeGlueObject(focusedObjectChangeGlue);	
						}
						hasFocusedObject = true;
						focusedObject = obj;
						tempvector = {static_cast<double>(x),static_cast<double>(y),0};
						pthread_mutex_lock(&PhysicsEngineMutex);
						originalDistanceFromCenter = {static_cast<double>(x)-focusedObject->position.x,static_cast<double>(y)-focusedObject->position.y};

						updateMovingObjectPosition();

						focusedObjectChangeGlue = GE_addGlueSubject(&(focusedObject->position),&tempvector,GE_PULL_ON_RENDER,sizeof(Vector2r));
						pthread_mutex_unlock(&PhysicsEngineMutex);
						break;
					}
				}
			}
			else if (event.type == SDL_MOUSEBUTTONUP && hasFocusedObject)
			{
				hasFocusedObject = false;
				GE_FreeGlueObject(focusedObjectChangeGlue);
			}
			if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_RIGHT)
			{
				//Update the position to be where the user right clicked to open the menu at
				updateMovingObjectPosition();
				rightClickOpen = true;
				rightClickMenu->setPosition({static_cast<double>(screenX),static_cast<double>(screenY)});
				GE_UI_SetTopElement(rightClickMenu);
			}
		}
	}
	else if(event.type == SDL_USEREVENT)
	{
		//TODO HACK
		GE_UI_RemoveTopLevelElement(rightClickMenu);
	}



	if (hasFocusedObject)
	{
		updateMovingObjectPosition();
	}
	
}

bool GE_UI_LevelEditor2D::checkIfFocused(int mousex, int mousey)
{
	return checkIfFocused_ForBox(mousex,mousey,position,size);
}

void GE_UI_LevelEditor2D::setStarsScale(double scale)
{
	stars[0]->setScale((0.833333333/10)/scale);
	stars[1]->setScale((0.833333333/5)/scale);
	stars[2]->setScale((0.833333333/3)/scale);
	stars[3]->setScale((0.833333333/2)/scale);
}
void GE_UI_LevelEditor2D::updateMovingObjectPosition()
{
	double x,y;
	getRealWorldCursorPosition(&x,&y);
	tempvector =  Vector2r{x-originalDistanceFromCenter.x,y-originalDistanceFromCenter.y,0};
}
void GE_UI_LevelEditor2D::getRealWorldCursorPosition(double* x, double* y)
{
	int mx,my;
	SDL_GetMouseState(&mx,&my);
	*x = static_cast<double>(mx);
	*y = static_cast<double>(my);

	//account for display centering
	*x-=(size.x/2);
	*y-=(size.y/2);
	//account for camera position

	*x /= levelScale;
	*y /= levelScale;

	*x+=camera->pos.x;
	*y+=camera->pos.y;


//	printf("x %f y %f\n",*x,*y);

}

void GE_RegisterClassWithLevelEditor(std::string name, GE_NewObject_t allocationFunction, unsigned long long classID)
{
	classNames.push_back(name);
	classCreationFunctions.push_back(allocationFunction);
	classRealTypeIDToLevelTypeID.insert(std::make_pair(classID,classNames.size()));	
}
