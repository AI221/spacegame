#include "UI.h"
#include "sprite.h"
#include "GeneralEngineCPP.h"
#include "physics.h"

#include <vector>
#include <cmath>
#include <stack>
#include <unordered_set>

#ifndef __INVENTORY_ELEMENTS_INCLUDED
#define __INVENTORY_ELEMENTS_INCLUDED


#define INV_SQUARE_SIZE 35

#include "debugRenders.h"




enum class ITEM_NAMES
{
	NONE = 0,
	IRON =1,
	DUCT_TAPE=2,
};

extern std::string itemHumanNames[];
extern std::string itemSpriteNames[];
extern double itemMasses[];

struct ItemStack
{
	ITEM_NAMES item;
	unsigned int count;
};

void initInventory(SDL_Renderer* renderer);
void renderInventoryBox(ItemStack* stack, Vector2 position, GE_UI_Text* ammountText);

struct Schedule;
class Inventory
{
	public:
		Inventory(int maxCapacityMass, GE_PhysicsObject* host);
		~Inventory();
		int add(ItemStack item);
		void remove(int itemID);
		int pickup(int itemID);
		int transferFromPeer(Inventory* peer, int itemID);
		int transferHeldFromPeer(Inventory* peer);
		void handleSchedules();

		int getCurrentMass();
		Vector2r getPosition();

		void schedulePickup(int itemID);
		void scheduleTransferFromPeer(Inventory* peer, int itemID);
		void scheduleTransferHeldFromPeer(Inventory* peer);

		std::vector<ItemStack> storage;
		int maxHoldingMass;

		//holding items on the cursor 
		ItemStack held; 
		bool isHoldingItem;

		//okay to touch this value
		int maxCapacityMass;




	private:
		int currentMass;
		Vector2r linkedPosition;
		std::stack<Schedule> scheduleStack;
};


class UI_FloatingInventoryElement : public GE_UI_TopLevelElement
{	
	public:
		UI_FloatingInventoryElement(SDL_Renderer* renderer);
		void render(Vector2 parrentPosition);
		void giveEvent(Vector2 parrentPosition, SDL_Event event);
		bool checkIfFocused(int mousex, int mousey);
	private:
		SDL_Renderer* renderer;
};

class UI_InventoryView : public GE_UI_Element
{
	public:
		UI_InventoryView(SDL_Renderer* renderer, Vector2 position, Vector2 size, Inventory* inventory, GE_UI_Text* countText, Vector2 paddingSize, GE_Color highlightColor, GE_Color emptySlotColor, GE_Color borderColor);
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
		GE_RectangleShape* emptySlotRectangle;
		GE_RectangleShape* borderRectangle;


		int ammountItemsPerRow;
		int ammmountRows;


};






#endif // __INVENTORY_ELEMENTS_INCLUDED
