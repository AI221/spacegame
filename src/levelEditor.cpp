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



class GE_PhysicsObjectHighlightBoxManager
{
	public:
		GE_PhysicsObjectHighlightBoxManager(SDL_Renderer* renderer, Camera* camera, GE_UI_LevelEditor2D* levelEditor, GE_PhysicsObject* object, unsigned int numBoxes, GE_UI_HighlightBoxStyle style);
		~GE_PhysicsObjectHighlightBoxManager();

		void giveEvent(Vector2 parrentPosition, SDL_Event event);
		void render(Vector2 parrentPosition);
	private:
		GE_PhysicsObject* object;
		unsigned int numBoxes;
		std::vector<std::unique_ptr<GE_UI_HighlightBox>> highlightBoxes;
};

GE_PhysicsObjectHighlightBoxManager::GE_PhysicsObjectHighlightBoxManager(SDL_Renderer* renderer, Camera* camera, GE_UI_LevelEditor2D* levelEditor, GE_PhysicsObject* object, unsigned int numBoxes, GE_UI_HighlightBoxStyle style)
{
	this->numBoxes = numBoxes;
	this->object = object;
	for (unsigned int i=0;i!=numBoxes;i++)
	{
		auto rectangle = dynamic_cast<GE_LevelEditorInterface*>(object)->getRectangle(i); 
		
		highlightBoxes.push_back(std::move(std::unique_ptr<GE_UI_HighlightBox>(new GE_UI_HighlightBox(renderer,camera,levelEditor,Vector2r{rectangle.x+object->position.x,rectangle.y+object->position.y,object->position.r},Vector2{rectangle.w,rectangle.h},style))));
	}
}
GE_PhysicsObjectHighlightBoxManager::~GE_PhysicsObjectHighlightBoxManager(){}

void GE_PhysicsObjectHighlightBoxManager::giveEvent(Vector2 parrentPosition, SDL_Event event)
{
	auto interf = dynamic_cast<GE_LevelEditorInterface*>(object);
	unsigned int i = 0;
	for (auto& highlightBox : highlightBoxes)
	{
		i++;
		highlightBox->giveEvent(parrentPosition, event);
		pthread_mutex_lock(&PhysicsEngineMutex);
		interf->setRectangle(i,highlightBox->getRectangle());
		pthread_mutex_unlock(&PhysicsEngineMutex);
	}

		
}
void GE_PhysicsObjectHighlightBoxManager::render(Vector2 parrentPosition)
{
	for (auto& highlightBox : highlightBoxes)
	{
		highlightBox->render(parrentPosition);
	}
}


GE_UI_HighlightBox::GE_UI_HighlightBox(SDL_Renderer* renderer,  Camera* camera, GE_UI_LevelEditor2D* levelEditor,Vector2r position, Vector2 size,GE_UI_HighlightBoxStyle style)
{
	this->position = position;
	this->size = size;
	this->style = style;
	this->camera = camera;
	this->levelEditor = levelEditor;

	this->draggableBoxes = std::unique_ptr<GE_RectangleShape>(new GE_RectangleShape(renderer,style.draggableBoxColor));
	this->rotatableBoxes = std::unique_ptr<GE_RectangleShape>(new GE_RectangleShape(renderer,style.rotatableBoxColor));

	this->box = std::unique_ptr<GE_HollowRectangleShape>(new GE_HollowRectangleShape(renderer,style.boxColor,style.lineThickness));

	isBeingDragged = false;
	expandByCenter = false;
	resizeRectangleByCenter = false;
}
GE_UI_HighlightBox::~GE_UI_HighlightBox()
{
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
		case middleTop:
			return middleBottom;
			break;
		case middleBottom:
			return middleTop;
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
	point.r = position.r;
		
	//if (box != hollowBox) point = point+(Vector2{style.draggableBoxDiameter,style.draggableBoxDiameter}/2);
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
	point = point+Vector2{topleftParrentPosition.x,topleftParrentPosition.y};

	if (addCamera)
	{
		point.x = point.x*(levelEditor->levelScale);
		point.y = point.y*(levelEditor->levelScale);
		point = GE_ApplyCameraOffset(&scaledcamera,point);
	}
	//if (box != hollowBox) point = point-(Vector2{style.draggableBoxDiameter,style.draggableBoxDiameter}/2);
	return point;
}
Vector2r GE_UI_HighlightBox::getTopLeftPosition(Vector2 parrentPosition)
{
	Vector2r pos = Vector2r{parrentPosition.x,parrentPosition.y,0};
	pos = pos+ position;


	Vector2r topleftPos = pos;
	Vector2 rotatedsize = size;
	GE_Vector2RotationCCW(&rotatedsize,pos.r);
	topleftPos.x -= rotatedsize.x/2;
	topleftPos.y -= rotatedsize.y/2;
	return topleftPos;
}
void GE_UI_HighlightBox::render(Vector2 parrentPosition)
{
	scaledcamera = *camera;
	scaledcamera.pos.x *= levelEditor->levelScale;
	scaledcamera.pos.y *= levelEditor->levelScale;

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
	}	
}
Vector2 resizeAnchoringAPoint(Vector2r anchor,internal_draggablebox_position movedPoint,Vector2 center,Vector2 initialSize, Vector2 newSize)
{
	Vector2 deltaSize = newSize-initialSize;

	Vector2 result = deltaSize/2;
	//adjust which points are anchored by adjusting how the position changes
	switch(movedPoint)
	{
		case middleTop:
		case middleLeft:
		case topLeft:
			result.x *= -1;
			result.y *= -1;
			break;
		case topRight:
			result.y *= -1;
			break;
		case bottomLeft:
			result.x *= -1;
			break;
		case bottomRight:
		default:
			break;
	}
	GE_Vector2RotationCCW(&result,anchor.r);

	return result;
}

void GE_UI_HighlightBox::giveEvent(Vector2 parrentPosition, SDL_Event event)
{

	//Perhaps I should give up on UI code. lol.
	


	scaledcamera = *camera;
	scaledcamera.pos.x *= levelEditor->levelScale;
	scaledcamera.pos.y *= levelEditor->levelScale;


	Vector2r topleftPos = getTopLeftPosition(parrentPosition);
	Vector2 mousepos;
	levelEditor->getRealWorldCursorPosition(&mousepos.x,&mousepos.y);
	GE_DEBUG_TextAt(GE_DEBUG_VectorToString(mousepos),Vector2{0,0});
	
	if(event.type == SDL_MOUSEBUTTONDOWN)
	{
		for(int i=0;i!=9;i++)
		{
			Vector2r pos = getDraggableBoxPosition(topleftPos,static_cast<internal_draggablebox_position>(i),false);
			if (GE_Distance(mousepos,pos) <= (style.draggableBoxDiameter*2)/levelEditor->levelScale) //check if the cursor is in a radius. this makes it much easier to grab the rectangles.
			{
				isBeingDragged = true;
				boxDragged = static_cast<internal_draggablebox_position>(i);

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
	if (isBeingDragged)
	{
		if (static_cast<int>(boxDragged) <= 7) //handle moveable corners
		{
			//figure out the new rectangle size by moving a point
			Vector2r start_rot = getDraggableBoxPosition(topleftPos,getOppositeCorner(boxDragged),false);
			Vector2 start = start_rot;
			Vector2 end = mousepos;
				
			//find rect size
			Vector2 sizeAfterResizingBasedOnCursor= GE_PointsToRectangle(start,end,position.r);


			//correct for negatives
			sizeAfterResizingBasedOnCursor.x = std::abs(sizeAfterResizingBasedOnCursor.x);
			sizeAfterResizingBasedOnCursor.y = std::abs(sizeAfterResizingBasedOnCursor.y);
			//if they drag a middle one, then they're only resizing in 1 direction
			switch(boxDragged)
			{
				case middleRight:
					[[fallthrough]];
				case middleLeft:
					sizeAfterResizingBasedOnCursor.y = size.y;
					
					break;
				case middleBottom:
					[[fallthrough]];
				case middleTop:
					sizeAfterResizingBasedOnCursor.x = size.x;
					break;
				default:
					break;
			}


			if (!resizeRectangleByCenter)
			{
				position = position+(resizeAnchoringAPoint(start_rot,boxDragged,position,size,sizeAfterResizingBasedOnCursor));
			}



			

			size = sizeAfterResizingBasedOnCursor;

			

		}
		else if(boxDragged == hollowBox)
		{
			position.x = mousepos.x;
			position.y = mousepos.y;
		}
	}
	


	//shift behaviour
	if (event.type == SDL_KEYDOWN)
	{
		if (event.key.keysym.sym == SDLK_LSHIFT || event.key.keysym.sym == SDLK_RSHIFT)
		{
			printf("!\n");
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
GE_Rectangle GE_UI_HighlightBox::getRectangle()
{
	return GE_Rectangle{position.x,position.y,size.x,size.y};
}

const double rightClickMenuWidth = 200;
enum class rightClickOptions
{
	newObject,	
	save,
};

GE_UI_HighlightBox* highlightBox;
GE_UI_HighlightBoxStyle style_highlightBox = {2,8,4,{0x55,0x00,0x00,0xff},{0x00,0xff,0x00,0xff},{0x00,0xff,0x00,0xff}};
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
	printf("Delete level editor\n");
	delete camera;
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

	
	//give updates to the highlight box - hack
	
	{
		SDL_Event hack;
		hack.type = SDL_USEREVENT;
		highlightBox->giveEvent(parrentPosition,hack);
	}

	//highlightBox->render(parrentPosition);
	if(highlightBoxManager.has_value())
	{
		(*highlightBoxManager)->render(parrentPosition);
	}

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
							//GE_FreeGlueObject(focusedObjectChangeGlue);	
						}
						/*
						hasFocusedObject = true;
						focusedObject = obj;
						tempvector = {static_cast<double>(x),static_cast<double>(y),0};
						originalDistanceFromCenter = {static_cast<double>(x)-focusedObject->position.x,static_cast<double>(y)-focusedObject->position.y};

						updateMovingObjectPosition();

						//focusedObjectChangeGlue = GE_addGlueSubject(&(focusedObject->position),&tempvector,GE_PULL_ON_RENDER,sizeof(Vector2r));

						*/
						auto objLevelEditorInterface = dynamic_cast<GE_LevelEditorInterface*>(obj);
						/*
						auto rect = objLevelEditorInterface->getRectangle(0);
						printf("!! w %f\n",rect.w);
						rect.x += obj->position.x;
						rect.y += obj->position.y;

						highlightBox->size.x = rect.w;
						highlightBox->size.y = rect.h;
						*/

						highlightBoxManager = std::unique_ptr<GE_PhysicsObjectHighlightBoxManager>(new GE_PhysicsObjectHighlightBoxManager(renderer,camera,this,obj,classProperties[classRealTypeIDToLevelTypeID[obj->type]].numResizableRectangles,style_highlightBox));
						printf("lid %llu\n",classRealTypeIDToLevelTypeID[obj->type]);

						

						break;
					}
				}
				pthread_mutex_unlock(&PhysicsEngineMutex);
			}
			else if (event.type == SDL_MOUSEBUTTONUP && hasFocusedObject)
			{
				hasFocusedObject = false;
				//GE_FreeGlueObject(focusedObjectChangeGlue);
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
		{
			GE_UI_RemoveTopLevelElement(rightClickMenu);
		}
	}



	if (hasFocusedObject)
	{
		updateMovingObjectPosition();
	}
	
	//highlightBox->giveEvent(parrentPosition,event);
	if(highlightBoxManager.has_value())
	{
		(*highlightBoxManager)->giveEvent(parrentPosition,event);
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


	GE_Vector2RotationCCW(x,y,camera->pos.r);

	*x+=camera->pos.x;
	*y+=camera->pos.y;

	//GE_DEBUG_DrawRect_PhysicsPosition({*x,*y,5,5});


//	printf("x %f y %f\n",*x,*y);

}

void internal_RegisterClassWithLevelEditor(std::string name, GE_NewObject_t allocationFunction, unsigned long long classID,GE_LevelEditorObjectProperties properties)
{
	printf("rid %llu num %d\n",classID,properties.numResizableRectangles);
	classRealTypeIDToLevelTypeID.insert(std::make_pair(classID,classNames.size()));	
	classNames.push_back(name);
	classCreationFunctions.push_back(allocationFunction);
	classProperties.push_back(properties);
}
