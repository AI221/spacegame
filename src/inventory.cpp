#include "inventory.h"

#define SPRITE_DIR "../sprites/"

std::string itemHumanNames[] = {
	"Nothing",
	"Iron",
};
std::string itemSpriteNames[] = 
{
	"Err",
	SPRITE_DIR"inv_iron.png"
};

double itemMasses[] = { 
	0,
	20,
};

int getItemMass(ITEM_NAMES item)
{
	return itemMasses[static_cast<int>(item)];
}


Inventory::Inventory(int maxCapacityMass) 
{
	this->maxCapacityMass = maxCapacityMass;


	this->isHoldingItem = false;
	this->currentMass = 0;
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
	if (isHoldingItem || itemID >= storage.size())
	{
		return 1;
	}

	held = *(storage.begin()+itemID);
	isHoldingItem = true;
	remove(itemID);
	return 0;
};
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

void renderInventoryBox(ItemStack* stack, Vector2 position, GE_UI_Text* ammountText)
{
	GE_BlitSprite( Sprites[GE_SpriteNameToID(itemSpriteNames[static_cast<int>(stack->item)]) ],Vector2r{position.x,position.y,0},Vector2{35,35},GE_Rectangle{0,0,35,35},GE_FLIP_NONE);

	ammountText->setText(std::to_string(stack->count));

	ammountText->render(position);
}
UI_InventoryView::UI_InventoryView(SDL_Renderer* renderer, Vector2 position, Vector2 size, Inventory* inventory, GE_UI_Text* countText, Vector2 paddingSize, GE_Color highlightColor)
{
	this->renderer = renderer;
	this->position = position;
	setSize(size);

	this->countText = countText;

	this->inventory = inventory;

	this->paddingSize = paddingSize;

	this->highlightRectangle = new GE_RectangleShape(renderer,highlightColor);


	this->wantsEvents = true;


	printf(" asdjfklasdfjn asdjkrh ajkldfh asjklfh asdjklbfhwants events %d\n",this->wantsEvents);
}
void UI_InventoryView::setSize(Vector2 size)
{
	ammountItemsPerRow = size.x/(INV_SQUARE_SIZE+paddingSize.x);
	this->size = size;
}
UI_InventoryView::~UI_InventoryView()
{
	delete highlightRectangle;
}
void UI_InventoryView::render(Vector2 parrentPosition)
{
	parrentPosition = parrentPosition+position;

	Vector2 additionalPosition = {-INV_SQUARE_SIZE,paddingSize.y};


	GE_DEBUG_TextAt(std::to_string(ammountItemsPerRow),parrentPosition);
	int currentItemsOnRow = -1;
	for (ItemStack item : inventory->storage)
	{
		currentItemsOnRow++;
		
		if (currentItemsOnRow >= ammountItemsPerRow+1)
		{
			currentItemsOnRow = 0;
			additionalPosition.x = paddingSize.x;
			additionalPosition.y += INV_SQUARE_SIZE+paddingSize.y;
		}
		else
		{
			additionalPosition.x += INV_SQUARE_SIZE+paddingSize.x;
		}



		
		renderInventoryBox(&item, parrentPosition+additionalPosition, countText);
		GE_DEBUG_TextAt(std::to_string(currentItemsOnRow),parrentPosition+additionalPosition);

	}

	Vector2 fullSize = Vector2{INV_SQUARE_SIZE,INV_SQUARE_SIZE}+(paddingSize);
	IntVector2 mouse = GE_UI_GetMousePosition();

	IntVector2 mousePos = mouse;

	mousePos = mousePos - parrentPosition;


				mousePos.x /= INV_SQUARE_SIZE+paddingSize.x;
				mousePos.y /= INV_SQUARE_SIZE+paddingSize.y;

				int item = (mousePos.y*static_cast<int>(ammountItemsPerRow) )+mousePos.x; 
				GE_DEBUG_TextAt("x "+std::to_string(mousePos.x)+" y "+std::to_string(mousePos.y)+" item "+std::to_string(item),Vector2{25,25});

				
	//    VVV: Check if the mouse is in the UI Element, then check if the mouse is hovering over padding, or over an actual item: only continue if it's an item
	if ( (mousePos >= IntVector2{0,0}) && (mousePos <= parrentPosition+size) && (wraparround_clamp(mousePos.x,fullSize.x) > paddingSize.x) && (wraparround_clamp(mousePos.y,fullSize.y) > paddingSize.y) )
	{
		mousePos.x = GE_ReduceAccuracy(mousePos.x,fullSize.x); //put x and y on a grid
		mousePos.y = GE_ReduceAccuracy(mousePos.y,fullSize.y);


		
		highlightRectangle->render(Vector2{static_cast<double>(mousePos.x)+paddingSize.x,static_cast<double>(mousePos.y)+paddingSize.y}+parrentPosition, {INV_SQUARE_SIZE,INV_SQUARE_SIZE});
		
	}

	GE_DEBUG_TextAt("end",parrentPosition+size);


	if (inventory->isHoldingItem)
	{
		renderInventoryBox(&inventory->held, Vector2{static_cast<double>(mouse.x),static_cast<double>(mouse.y)}, countText);
	}




}
void UI_InventoryView::giveEvent(Vector2 parrentPosition, SDL_Event event)
{
	if (event.type == SDL_MOUSEBUTTONDOWN)
	{
		GE_DEBUG_TextAt("SDFASFSDFASDFASDFFASDFSAFASDFASFDSAFAASDSAFDF",Vector2{0,0});
		if (inventory->isHoldingItem)
		{

		}
		else
		{
			Vector2 fullSize = Vector2{INV_SQUARE_SIZE,INV_SQUARE_SIZE}+(paddingSize);
			IntVector2 mousePos = GE_UI_GetMousePosition();

			mousePos = mousePos - parrentPosition;


			//    VVV: Check if the mouse is in the UI Element, then check if the mouse is hovering over padding, or over an actual item: only continue if it's an item
			if ( (mousePos >= IntVector2{0,0}) && (mousePos <= parrentPosition+size) && (wraparround_clamp(mousePos.x,fullSize.x) > paddingSize.x) && (wraparround_clamp(mousePos.y,fullSize.y) > paddingSize.y) )
			{
				mousePos.x /= INV_SQUARE_SIZE+paddingSize.x;
				mousePos.y /= INV_SQUARE_SIZE+paddingSize.y;

				int item = (mousePos.y*static_cast<int>(ammountItemsPerRow) )+mousePos.x; 

				printf("item %d\n",item);

				inventory->pickup(item);
			}
		}
	}





}

