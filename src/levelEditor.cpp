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

std::vector<std::string> classNames;
std::vector<GE_NewObject_t> classCreationFunctions;
std::vector<GE_LevelEditorObjectProperties> classProperties;
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
	hollowBox,
};

/*!
 * Gives a resizable, moveable, rotatable box. Positioned by CENTER!
 */
class GE_UI_HighlightBox : GE_UI_Element
{
	public:
		GE_UI_HighlightBox(SDL_Renderer* renderer, Camera* camera, GE_UI_LevelEditor2D* levelEditor, Vector2r position, Vector2 size,GE_UI_HighlightBoxStyle style);
		~GE_UI_HighlightBox();
		void render(Vector2 parrentPosition);
		void giveEvent(Vector2 parrentPosition, SDL_Event event);
		GE_UI_PositioningRectangle* positioningRectangle;



		Vector2 size;
	private:
		GE_RectangleShape* draggableBoxes;
		GE_RectangleShape* rotatableBoxes;
		GE_HollowRectangleShape* box;

		Vector2r position;
		Camera* camera;
		Camera scaledcamera;
		Vector2 scaledsize;
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

		
};


GE_UI_HighlightBox::GE_UI_HighlightBox(SDL_Renderer* renderer,  Camera* camera, GE_UI_LevelEditor2D* levelEditor,Vector2r position, Vector2 size,GE_UI_HighlightBoxStyle style)
{
	this->position = position;
	this->size = size;
	this->style = style;
	this->camera = camera;
	this->levelEditor = levelEditor;

	this->draggableBoxes = new GE_RectangleShape(renderer,style.draggableBoxColor);
	this->rotatableBoxes = new GE_RectangleShape(renderer,style.rotatableBoxColor);

	this->box = new GE_HollowRectangleShape(renderer,style.boxColor,style.lineThickness);

	isBeingDragged = false;
	expandByCenter = false;
	resizeRectangleByCenter = false;
}
GE_UI_HighlightBox::~GE_UI_HighlightBox()
{
	delete draggableBoxes;
	delete rotatableBoxes;
	delete box;
}
internal_draggablebox_position getOppositeCorner(internal_draggablebox_position corner)
{
	switch(corner)
	{
		case topLeft:
			return bottomRight;
			break;
		case topRight:
			return bottomLeft;
			break;
		case bottomRight:
			return topLeft;
			break;
		case bottomLeft:
			return topRight;
			break;
		case middleLeft:
			return middleRight;
			break;
		case middleRight:
			return middleLeft;
			break;
		default:
			return topLeft;
			break;
	}
}

Vector2r GE_UI_HighlightBox::getDraggableBoxPosition(Vector2r topleftParrentPosition,internal_draggablebox_position box, bool addCamera)
{
	Vector2r point;
	switch(box)
	{
		case topLeft: 
			point = {0,0,0};
			break;
		case topRight: 
			point = {size.x,0,0};
			break;
		case bottomRight: 
			point = {size.x,size.y,0};
			break;
		case bottomLeft: 
			point = {0,size.y,0};
			break;
		case middleTop: 
			point = {size.x/2,0,0};
			break;
		case middleRight: 
			point = {size.x,size.y/2,0};
			break;
		case middleBottom:
			point = {size.x/2,size.y,0};
			break;
		case middleLeft: 
			point = {0,size.y/2,0};
			break;
		case hollowBox:
			point = {size.x/2,size.y/2,0};
			break;
	}
	Vector2 scaledsize = size;//*levelEditor->levelScale;

	point.r = position.r;
		
	//point = point-(Vector2{scaledsize.x,scaledsize.y}/2);
	if(box != hollowBox)
	{
		point = point-Vector2{style.draggableBoxDiameter/2,style.draggableBoxDiameter/2};
	}
	GE_Vector2RotationCCW(&point);
	if(box != hollowBox)
	{
		point = point+Vector2{style.draggableBoxDiameter/2,style.draggableBoxDiameter/2};
	}
	point = point+(Vector2{scaledsize.x,scaledsize.y}/2)+Vector2{topleftParrentPosition.x,topleftParrentPosition.y}-(Vector2{style.draggableBoxDiameter,style.draggableBoxDiameter}/2);

	if (addCamera)
	{
		point = point*(levelEditor->levelScale);
		point = GE_ApplyCameraOffset(&scaledcamera,point);

		//printf("pt %s\n",GE_DEBUG_VectorToString(point).c_str());
	}
	return point;
}
Vector2r GE_UI_HighlightBox::getTopLeftPosition(Vector2 parrentPosition)
{
	Vector2r pos = Vector2r{parrentPosition.x,parrentPosition.y,0};
	pos = pos+ position;

	Vector2r topleftPos = pos;
	topleftPos.x -= scaledsize.x/2;
	topleftPos.y -= scaledsize.y/2;
	return topleftPos;
}
void GE_UI_HighlightBox::render(Vector2 parrentPosition)
{
	scaledcamera = *camera;
	scaledcamera.pos.x *= levelEditor->levelScale;
	scaledcamera.pos.y *= levelEditor->levelScale;
	scaledsize = size*levelEditor->levelScale;

	//position.r += 0.01;
	

	


	Vector2r topleftPos = getTopLeftPosition(parrentPosition);
	
	Vector2r boxpos = getDraggableBoxPosition(topleftPos,hollowBox,true);

	//boxpos.r -= scaledcamera.pos.r;
	boxpos.r *= -1;
	//printf("bpos %s",GE_DEBUG_VectorToString(boxpos).c_str());

	box->render(boxpos,size*levelEditor->levelScale);

	//render draggable boxes at all 4 corners
	for (int i=0;i!=9;i++)
	{
		draggableBoxes->render(getDraggableBoxPosition(topleftPos,static_cast<internal_draggablebox_position>(i),true),{style.draggableBoxDiameter,style.draggableBoxDiameter});
		auto tmp = getDraggableBoxPosition(topleftPos,static_cast<internal_draggablebox_position>(i),true);
	GE_DEBUG_DrawRect_PhysicsPosition({tmp.x,tmp.y,4,4});
	}	

	GE_DEBUG_DrawRect_PhysicsPosition({topleftPos.x,topleftPos.y,4,4});
	

}
Vector2 resizeAnchoringAPoint(Vector2r anchor,Vector2 center,Vector2 initialSize, Vector2 newSize)
{
	Vector2 deltaSize = newSize-initialSize;

	Vector2 result = deltaSize/2;

	GE_Vector2RotationCCW(&result,anchor.r);

	return result;
}

void GE_UI_HighlightBox::giveEvent(Vector2 parrentPosition, SDL_Event event)
{
	scaledcamera = *camera;
	scaledcamera.pos.x *= levelEditor->levelScale;
	scaledcamera.pos.y *= levelEditor->levelScale;
	scaledsize = size*levelEditor->levelScale;


	Vector2r topleftPos = getTopLeftPosition(parrentPosition);
	Vector2 mousepos;
	levelEditor->getRealWorldCursorPosition(&mousepos.x,&mousepos.y);
	
	if(event.type == SDL_MOUSEBUTTONDOWN)
	{
		for(int i=0;i!=8;i++)
		{
			Vector2r pos = getDraggableBoxPosition(topleftPos,static_cast<internal_draggablebox_position>(i),false);
			GE_DEBUG_DrawRect_PhysicsPosition({pos.x,pos.y,2,2},{0xff,0x00,0xff});
			if (checkIfFocused_ForBox(mousepos.x,mousepos.y,{pos.x,pos.y},Vector2{style.draggableBoxDiameter,style.draggableBoxDiameter}))
			{
				isBeingDragged = true;
				boxDragged = static_cast<internal_draggablebox_position>(i);
				printf("i %d\n",i);

				levelEditor->getRealWorldCursorPosition(&initialMousePosition.x,&initialMousePosition.y);
				


				initialSize = size;
				initialPosition=position;
				break;
			}
		}
	}
	else if (event.type == SDL_MOUSEBUTTONUP)
	{
		isBeingDragged = false;
	}
	if (isBeingDragged && (event.type == SDL_MOUSEMOTION || event.type == SDL_USEREVENT))
	{
		if (static_cast<int>(boxDragged) <= 3 || boxDragged == middleLeft || boxDragged == middleRight)
		{
			Vector2r start_rot = getDraggableBoxPosition(topleftPos,getOppositeCorner(boxDragged),false)/levelEditor->levelScale;
			Vector2 start = start_rot;
			Vector2 end = mousepos;
				
			Vector2 sizeAfterResizingBasedOnCursor= GE_PointsToRectangle(start,end,position.r);
			sizeAfterResizingBasedOnCursor.x = std::abs(sizeAfterResizingBasedOnCursor.x);
			sizeAfterResizingBasedOnCursor.y = std::abs(sizeAfterResizingBasedOnCursor.y);
			if (!resizeRectangleByCenter)
			{
				Vector2 movement = (resizeAnchoringAPoint(start_rot,position,size,sizeAfterResizingBasedOnCursor));
				switch(boxDragged)
				{
					case topLeft:
					
						position = position-movement;
						break;
					case bottomRight:
						position = position+movement;
						break;
					case topRight:
						position.x += movement.x;
						position.y -= movement.y;
						break;
					case bottomLeft:
						position.x -= movement.x;
						position.y += movement.y;
						break;
					default:
						break;
				}

			}


			

			size = sizeAfterResizingBasedOnCursor;

			

		}
	}
	else if (event.type == SDL_KEYDOWN)
	{
		if (event.key.keysym.sym == SDLK_LSHIFT || event.key.keysym.sym == SDLK_RSHIFT)
		{
			resizeRectangleByCenter = true;
		}
	}
	else if (event.type == SDL_KEYUP)
	{
		if (event.key.keysym.sym == SDLK_LSHIFT || event.key.keysym.sym == SDLK_RSHIFT)
		{
			resizeRectangleByCenter = false;
		}
	}

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


	this->levelRenderer = new GE_UI_GameRender(renderer,position,size,this->camera,1);
	this->levelScale = 1;

	mySurface->addElement(levelRenderer);

	this->hasFocusedObject = false;

	GE_PhysicsEngine_TickingObjectsEnabled = false;
	GE_PhysicsEngine_CollisionsEnabled = false;


	memset(keysHeld,false,sizeof(keysHeld));

	highlightBox = new GE_UI_HighlightBox(renderer,camera,this,{100,100,0},{25,25},style_highlightBox);
	

	GE_DEBUG_PassRenderer(renderer,camera);



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
	if(keysHeld[SDL_SCANCODE_Q])
	{
		camera->pos.r += (0.01);
	}
	if(keysHeld[SDL_SCANCODE_E])
	{
		camera->pos.r -= (0.01);
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
					pthread_mutex_lock(&PhysicsEngineMutex);
					classCreationFunctions[clicked.trail[1]](renderer,Vector2r{tempvector.x,tempvector.y,camera->pos.r});
					pthread_mutex_unlock(&PhysicsEngineMutex);
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
						originalDistanceFromCenter = {static_cast<double>(x)-focusedObject->position.x,static_cast<double>(y)-focusedObject->position.y};

						updateMovingObjectPosition();

						focusedObjectChangeGlue = GE_addGlueSubject(&(focusedObject->position),&tempvector,GE_PULL_ON_RENDER,sizeof(Vector2r));

						auto objLevelEditorInterface = dynamic_cast<GE_LevelEditorInterface*>(obj);
						auto rect = objLevelEditorInterface->getRelativeRectangle(0);
						printf("!! w %f\n",rect.w);
						rect.x += obj->position.x;
						rect.y += obj->position.y;

						highlightBox->size.x = rect.w;
						highlightBox->size.y = rect.h;


						

						break;
					}
				}
				pthread_mutex_unlock(&PhysicsEngineMutex);
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
	
	highlightBox->giveEvent(parrentPosition,event);
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


	GE_Vector2RotationCCW(x,y,camera->pos.r);

	*x+=camera->pos.x;
	*y+=camera->pos.y;

	//GE_DEBUG_DrawRect_PhysicsPosition({*x,*y,5,5});


//	printf("x %f y %f\n",*x,*y);

}

void internal_RegisterClassWithLevelEditor(std::string name, GE_NewObject_t allocationFunction, unsigned long long classID,GE_LevelEditorObjectProperties properties)
{
	classNames.push_back(name);
	classCreationFunctions.push_back(allocationFunction);
	classRealTypeIDToLevelTypeID.insert(std::make_pair(classID,classNames.size()));	
	classProperties.push_back(properties);
}
