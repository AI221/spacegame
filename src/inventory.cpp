#include "inventory.h"
#include <cmath>
#include <unordered_set>

#include "UI.h"
#include "sprite.h"
#include "GeneralEngineCPP.h"
#include "physics.h"
#include "debugRenders.h"



Inventory* globalHeldItemInventory;
bool globalItemHeld = false;
GE_UI_Text* globalCountText;

const Vector2 SIZE_OF_ITEM = {INV_SQUARE_SIZE,INV_SQUARE_SIZE};
const double MAX_DISTANCE_FROM_INVENTORIES = 35000;

std::unordered_set<Inventory*> inventoriesInView;



void inventoryPhysicsCallback()
{
	for (Inventory* i :inventoriesInView)
	{
		i->handleSchedules();
	}
		
}
void initInventory(SDL_Renderer* renderer)
{
	GE_UI_SetCursorFollower(new UI_FloatingInventoryElement(renderer));

	GE_AddPhysicsDoneCallback(inventoryPhysicsCallback);

}

std::string itemHumanNames[] = {
	"Nothing",
	"Iron",
	"Duct Tape",
};
std::string itemSpriteNames[] = 
{
	"ERR",
	"inv_iron",
	"inv_duct-tape"
};

double itemMasses[] = { 
	0,
	80,
	10,
};

int getItemMass(ITEM_NAMES item)
{
	return itemMasses[static_cast<int>(item)];
}


Inventory::Inventory(int maxCapacityMass, GE_PhysicsObject* host)
{
	this->maxCapacityMass = maxCapacityMass;


	this->isHoldingItem = false;
	this->currentMass = 0;

	GE_LinkVectorToPhysicsObjectPosition(host,&(this->linkedPosition));
}
Inventory::~Inventory() 
{
}

int Inventory::add(ItemStack item)
{
	int newItemMass = getItemMass(item.item);
	if (newItemMass+currentMass > maxCapacityMass)
	{
		return 1;
	}
	storage.push_back(item);
	currentMass += newItemMass;
	return 0;
}
void Inventory::remove(int itemID)
{
	auto it = storage.begin()+itemID;
	currentMass -= getItemMass((*it).item);
	storage.erase(it);
}
int Inventory::pickup(int itemID)
{
	if (isHoldingItem || (itemID >= storage.size()))
	{
		return 1;
	}

	held = *(storage.begin()+itemID);
	isHoldingItem = true;
	remove(itemID);
	return 0;
}
int Inventory::transferFromPeer(Inventory* peer, int itemID)
{
	int err = add((*(peer->storage.begin()+itemID)));
	if (err == 0)
	{
		peer->remove(itemID);
		return 0;
	}
	return err;
}

int Inventory::transferHeldFromPeer(Inventory* peer)
{
	int err = add(peer->held);
	if (err ==0)
	{
		peer->isHoldingItem = false; //do not worry about clearing peer->held -- it will be overwritten the next time peer holds something
		return 0;
	}
	return err;
}
int Inventory::getCurrentMass()
{
	int mass = currentMass;

	if (isHoldingItem)
	{
		mass += getItemMass(held.item);
	}

	return mass;

}
Vector2r Inventory::getPosition()
{
	return linkedPosition;
}
struct SchedulePickup
{
	int itemID;
};
struct ScheduleTransfer
{
	Inventory* peer;
	int itemID;
};
struct ScheduleTransferHeld
{
	Inventory* peer;
};


union USchedule
{
	SchedulePickup schedulePickup;
	ScheduleTransfer scheduleTransfer;
	ScheduleTransferHeld scheduleTransferHeld;
};

enum schedType
{
	PICKUP,
	PEER,
	HELD,
};
struct Schedule
{
	USchedule data;
	schedType type;
};

void Inventory::handleSchedules()
{
	pthread_mutex_lock(&schedulemutex);
	while (!scheduleStack.empty())
	{
		Schedule i = scheduleStack.top();	
		scheduleStack.pop();

		if (i.type == PICKUP) 
		{
			pickup(i.data.schedulePickup.itemID);
		}
		else if (i.type == PEER)
		{
			transferFromPeer(i.data.scheduleTransfer.peer,i.data.scheduleTransfer.itemID);
		}
		else if (i.type == HELD)
		{
			transferHeldFromPeer(i.data.scheduleTransferHeld.peer);
		}
	}
	pthread_mutex_unlock(&schedulemutex);
}

void Inventory::schedulePickup(int itemID)
{
	pthread_mutex_lock(&schedulemutex);
	USchedule u;
	u.schedulePickup = {itemID};
	scheduleStack.push(Schedule{u,PICKUP});
	pthread_mutex_unlock(&schedulemutex);
}

void Inventory::scheduleTransferFromPeer(Inventory* peer, int itemID)
{
	pthread_mutex_lock(&schedulemutex);
	USchedule u;
	u.scheduleTransfer = {peer,itemID};
	scheduleStack.push(Schedule{u,PEER});
	pthread_mutex_unlock(&schedulemutex);
}
void Inventory::scheduleTransferHeldFromPeer(Inventory* peer)
{
	pthread_mutex_lock(&schedulemutex);
	USchedule u;
	u.scheduleTransferHeld = {peer};
	scheduleStack.push(Schedule{u,HELD});
	pthread_mutex_unlock(&schedulemutex);
}





void renderInventoryBox(ItemStack* stack, Vector2 position, GE_UI_Text* ammountText)
{
	GE_BlitSprite( Sprites[GE_SpriteNameToID(itemSpriteNames[static_cast<int>(stack->item)]) ],Vector2r{position.x,position.y,0},Vector2{35,35},GE_Rectangle{0,0,35,35},GE_FLIP_NONE);

	ammountText->setText(std::to_string(stack->count));

	ammountText->render(position);
}



UI_FloatingInventoryElement::UI_FloatingInventoryElement(SDL_Renderer* renderer)
{
}
void UI_FloatingInventoryElement::render(Vector2 parrentPosition)
{
	if (globalItemHeld)
	{
		renderInventoryBox(&globalHeldItemInventory->held, parrentPosition,globalCountText);
	}
}
void UI_FloatingInventoryElement::giveEvent(Vector2 parrentPosition, SDL_Event event)
{

}
bool UI_FloatingInventoryElement::checkIfFocused(int mousex, int mousey)
{
	return false;
}



UI_InventoryView::UI_InventoryView(SDL_Renderer* renderer, Vector2 position, Vector2 size, Inventory* inventory, GE_UI_FontStyle countTextStyle, Vector2 paddingSize, GE_Color highlightColor, GE_Color emptySlotColor, GE_Color borderColor)
{
	this->renderer = renderer;
	this->position = position;

	this->countTextStyle = countTextStyle;

	this->inventory = inventory;

	this->paddingSize = paddingSize;

	setSize(size); //must go after padding size set

	this->highlightRectangle = new GE_RectangleShape(renderer,highlightColor);
	this->emptySlotRectangle = new GE_RectangleShape(renderer, emptySlotColor);
	this->borderRectangle = new GE_RectangleShape(renderer, borderColor);


	this->wantsEvents = true;


	inventoriesInView.insert(inventory);


}
void UI_InventoryView::setSize(Vector2 size)
{
	ammountItemsPerRow = ((size.x-paddingSize.x)/(INV_SQUARE_SIZE+paddingSize.x));
	ammmountRows = ((size.y-paddingSize.y)/(INV_SQUARE_SIZE+paddingSize.y))-1;

	int totalTexts = ammmountRows*ammountItemsPerRow;
	for (int i=0;i<totalTexts;i++)
	{
		inventoryAmmountTexts.insert(inventoryAmmountTexts.end(),new GE_UI_Text(renderer,{0,0},{50,100},"",countTextStyle));
	}
	this->size = size;
}
UI_InventoryView::~UI_InventoryView()
{
	inventoriesInView.erase(inventory);
	delete highlightRectangle;
	delete emptySlotRectangle;
	delete borderRectangle;

	for (GE_UI_Text* i : inventoryAmmountTexts)
	{
		delete i;
	}
}
void UI_InventoryView::render(Vector2 parrentPosition)
{
	parrentPosition = parrentPosition+position;

	//render border first (so that it is overwritten)
	borderRectangle->render(parrentPosition,size);

	Vector2 additionalPosition = {-INV_SQUARE_SIZE,paddingSize.y};


	//GE_DEBUG_TextAt(std::to_string(ammountItemsPerRow),parrentPosition);
	int currentItemsOnRow = -1;
	int countRows = 0;
	auto currentInvNumberText = inventoryAmmountTexts.begin();

	auto it = inventory->storage.begin();
	auto end = inventory->storage.end();


	while (true) 
	{
		currentItemsOnRow++;
				
		if (currentItemsOnRow >= ammountItemsPerRow)
		{
			currentItemsOnRow = 0;
			additionalPosition.x = paddingSize.x;
			additionalPosition.y += INV_SQUARE_SIZE+paddingSize.y;
			countRows++;
			if (countRows > ammmountRows)
			{
				break;
			}

		}
		else
		{
			additionalPosition.x += INV_SQUARE_SIZE+paddingSize.x;
		}

		emptySlotRectangle->render(parrentPosition+additionalPosition,SIZE_OF_ITEM);
		if (it != end)
		{
			renderInventoryBox(&(*it), parrentPosition+additionalPosition, (*currentInvNumberText));
			//GE_DEBUG_TextAt(std::to_string(currentItemsOnRow),parrentPosition+additionalPosition);
			it++;
			currentInvNumberText++;
		}
	}

	Vector2 fullSize = Vector2{INV_SQUARE_SIZE,INV_SQUARE_SIZE}+(paddingSize);
	IntVector2 mouse = GE_UI_GetMousePosition();

	IntVector2 mousePos = mouse;

	mousePos = mousePos - parrentPosition;



	
		/*		mousePos.x /= INV_SQUARE_SIZE+paddingSize.x;
				mousePos.y /= INV_SQUARE_SIZE+paddingSize.y;

				int item = (mousePos.y*static_cast<int>(ammountItemsPerRow) )+mousePos.x; 
				GE_DEBUG_TextAt("x "+std::to_string(mousePos.x)+" y "+std::to_string(mousePos.y)+" item "+std::to_string(item),Vector2{25,25});*/
			

				
	//    VVV: Check if the mouse is in the UI Element, then check if the mouse is hovering over padding, or over an actual item: only continue if it's an item
	if ( (mousePos >= IntVector2{0,0}) && (mousePos <= size) && (wraparround_clamp(mousePos.x,fullSize.x) > paddingSize.x) && (wraparround_clamp(mousePos.y,fullSize.y) > paddingSize.y) )
	{
		mousePos.x = GE_ReduceAccuracy(mousePos.x,fullSize.x); //put x and y on a grid
		mousePos.y = GE_ReduceAccuracy(mousePos.y,fullSize.y);


		
		highlightRectangle->render(Vector2{static_cast<double>(mousePos.x)+paddingSize.x,static_cast<double>(mousePos.y)+paddingSize.y}+parrentPosition, {INV_SQUARE_SIZE,INV_SQUARE_SIZE});
		
	}

	
	//GE_DEBUG_TextAt("end "+std::to_string(ammountItemsPerRow),parrentPosition+size);
	

	if(globalItemHeld)
	{
		GE_DEBUG_TextAt(std::to_string(GE_Distance(globalHeldItemInventory->getPosition(),inventory->getPosition())),parrentPosition);
				}



}
void UI_InventoryView::giveEvent(Vector2 parrentPosition, SDL_Event event)
{
	parrentPosition = parrentPosition+position;
	if (event.type == SDL_MOUSEBUTTONDOWN)
	{
		Vector2 fullSize = Vector2{INV_SQUARE_SIZE,INV_SQUARE_SIZE}+(paddingSize);
		IntVector2 mousePos = GE_UI_GetMousePosition();

		mousePos = mousePos - parrentPosition;

		//    VVV: Check if the mouse is in the UI Element, then check if the mouse is hovering over padding, or over an actual item: only continue if it's an item
	
		if ( (mousePos >= IntVector2{0,0}) && (mousePos <= size) && (wraparround_clamp(mousePos.x,fullSize.x) > paddingSize.x) && (wraparround_clamp(mousePos.y,fullSize.y) > paddingSize.y) )
		{
			mousePos.x /= INV_SQUARE_SIZE+paddingSize.x; //Do note this calculation is technically slightly off--it doesn't incorporate the start padding x and y from item 0--but because it is only wrong while hovering over padding, it never gives us the wrong value here.
			mousePos.y /= INV_SQUARE_SIZE+paddingSize.y;

			if (globalItemHeld && GE_Distance(globalHeldItemInventory->getPosition(),inventory->getPosition()) < MAX_DISTANCE_FROM_INVENTORIES  )
			{
				inventory->scheduleTransferHeldFromPeer(globalHeldItemInventory);
				globalItemHeld = false;
			}
			else
			{

				int item = (mousePos.y*static_cast<int>(ammountItemsPerRow) )+mousePos.x; 


				if ( inventory->pickup(item) == 0)
				{
					globalHeldItemInventory = inventory;
					globalItemHeld = true;
					globalCountText = inventoryAmmountTexts.at(item);
				}
			}
		}
	}
}

