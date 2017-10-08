#include "UI.h"
#include "sprite.h"
#include "GeneralEngineCPP.h"

#ifndef __INVENTORY_ELEMENTS_INCLUDED
#define __INVENTORY_ELEMENTS_INCLUDED


#define INV_SQUARE_SIZE 35

#include "debugRenders.h"




enum class ITEM_NAMES
{
	NONE = 0,
	IRON =1,
};

extern std::string itemHumanNames[];
extern std::string itemSpriteNames[];
extern double itemMasses[];

struct ItemStack
{
	ITEM_NAMES item;
	unsigned int count;
};

void renderInventoryBox(ItemStack* stack, Vector2 position, GE_UI_Text* ammountText);

class Inventory
{
	public:
		Inventory(int maxCapacityMass);
		~Inventory();
		int add(ItemStack item);
		void remove(int itemID);
		int pickup(int itemID);
		int transferFromPeer(Inventory* peer, int itemID);
		int transferHeldFromPeer(Inventory* peer);

		int getCurrentMass();


		std::vector<ItemStack> storage;
		int maxHoldingMass;

		//holding items on the cursor 
		ItemStack held; 
		bool isHoldingItem;

		//okay to touch this value
		int maxCapacityMass;




	private:
		int currentMass;
};



class UI_InventoryView : public GE_UI_Element
{
	public:
		UI_InventoryView(SDL_Renderer* renderer, Vector2 position, Vector2 size, Inventory* inventory, GE_UI_Text* countText, Vector2 paddingSize, GE_Color highlightColor);
		~UI_InventoryView();
		void render(Vector2 parrentPosition);
		void giveEvent(Vector2 parrentPosition, SDL_Event event);
		void setSize(Vector2 size);
	private:
		SDL_Renderer* renderer;
		Vector2 position;
		Vector2 size;
		Vector2 paddingSize;
		Inventory* inventory;
		GE_UI_Text* countText;
		GE_RectangleShape* highlightRectangle;


		int ammountItemsPerRow;


};


#endif // __INVENTORY_ELEMENTS_INCLUDED
