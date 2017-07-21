#ifdef DEBUG_RENDERS
#include "debugRenders.h"

Sprite* nothingHere;
Sprite* somethingHere;
bool loadSprites = false;

void debug_render()
{ //pulled from my prototype
	
	//welcome to the oldest code in the engine

	if (!loadSprites)
	{
		loadSprites=true;
		nothingHere = GE_CreateSprite(myRenderer,SPRITE_DIR"DEBUG_nothingHere.bmp",10,10);
		somethingHere = GE_CreateSprite(myRenderer, SPRITE_DIR"DEBUG_somethingHere.bmp",10,10);
	}

	GE_BlitSprite(somethingHere,{0,0,0},{25,25},{10,10},FLIP_NONE);
	camera.pos = allPhysicsObjects[camFocusedObj]->position;
	camera.pos.x -= 640/2;
	camera.pos.y -= 580/2;
	camera.pos.x = camera.pos.x/10;
	camera.pos.y = camera.pos.y/10;
	
	//camera.pos.x = 10;
	//camera.pos.y = 10;
	int camerax = (int) camera.pos.x;
	int cameray = (int) camera.pos.y;
	for (int i = 0; i < 70; i++) { //start from camera position, increase forward 
		for (int o = 0; o < 70; o++) {
			//std::cout << "i " << i <<" t " << i+camerax << std::endl;
			if ((i+camerax < 0) || (o+cameray < 0)) {} else {
				if ( sGrid[i+camerax][o+cameray] != 0)
				{
					GE_BlitSprite(somethingHere,{i*10,o*10,0},{25,25},{0,0,8,9},FLIP_NONE);
					//std::cout << "nothingHere; ";`
				} 
				else 
				{
					GE_BlitSprite(nothingHere,{i*10,o*10,0},{25,25},{0,0,8,9},FLIP_NONE);
					//std::cout << "somethingHere; ";
				}
			}
		}
	}


}

#endif //DEBUG_RENDERS
