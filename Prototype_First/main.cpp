#include<SDL2/SDL.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <math.h>





#define SHAPE_SIZE 20

int main(int argc, char *argv[])
{
	SDL_Window* Main_Window;
	SDL_Renderer* Main_Renderer;
	SDL_Surface* Loading_Surf;
	SDL_Texture* Background_Tx;
	SDL_Texture* BlueShapes;
	SDL_Texture* DEBUG_nothingHere;
	SDL_Texture* DEBUG_somethingHere;

	/* Rectangles for drawing which will specify source (inside the texture)
	and target (on the screen) for rendering our textures. */
	SDL_Rect SrcR;
	SDL_Rect DestR;
	SDL_Rect DestR1;

	SrcR.x = 0;
	SrcR.y = 0;
	SrcR.w = SHAPE_SIZE;
	SrcR.h = SHAPE_SIZE;

	DestR.x = 0;//640 / 2 - SHAPE_SIZE / 2;
	DestR.y = 0;//580 / 2 - SHAPE_SIZE / 2;
	DestR.w = SHAPE_SIZE;
	DestR.h = SHAPE_SIZE;

	DestR1.x = 150;//640 / 2 - SHAPE_SIZE / 2;
	DestR1.y = 150;//580 / 2 - SHAPE_SIZE / 2;
	DestR1.w = SHAPE_SIZE;
	DestR1.h = SHAPE_SIZE;

	/* Before we can render anything, we need a window and a renderer */
	Main_Window = SDL_CreateWindow("SDL_RenderCopy Example",
	SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 580, 0);
	Main_Renderer = SDL_CreateRenderer(Main_Window, -1, SDL_RENDERER_ACCELERATED);

	/* The loading of the background texture. Since SDL_LoadBMP() returns
	a surface, we convert it to a texture afterwards for fast accelerated
	blitting. */
	Loading_Surf = SDL_LoadBMP("simple.bmp");
	Background_Tx = SDL_CreateTextureFromSurface(Main_Renderer, Loading_Surf);
	SDL_FreeSurface(Loading_Surf); /* we got the texture now -> free surface */

	/* Load an additional texture */
	Loading_Surf = SDL_LoadBMP("simple.bmp");
	BlueShapes = SDL_CreateTextureFromSurface(Main_Renderer, Loading_Surf);
	SDL_FreeSurface(Loading_Surf);

	Loading_Surf = SDL_LoadBMP("DEBUG_nothingHere.bmp");
	DEBUG_nothingHere = SDL_CreateTextureFromSurface(Main_Renderer, Loading_Surf);
	SDL_FreeSurface(Loading_Surf);

	Loading_Surf = SDL_LoadBMP("DEBUG_somethingHere.bmp");
	DEBUG_somethingHere = SDL_CreateTextureFromSurface(Main_Renderer, Loading_Surf);
	SDL_FreeSurface(Loading_Surf);


	SDL_Event event;
	double speed_y = 0;
	double speed_x = 0;

	bool sGrid[2000][2000]; //simplified grid , a simplified, inaccurate but precise version of where everything is. Helps calculate wheather any physics collision calculations need to be done. table is x:y. real version should be an int table which has the objectID
	for ( int i; i < 2000; i++) {
		for ( int o; o < 2000; o++) {
			sGrid[i][o] = false;
		}
	}
	//sGrid[1][1] = true;



	bool debugDisplaySGrid = false;
	int sLastPosX=0;
	int sLastPosY=0;
	int sLastSizeX = 0;
	int sLastSizeY = 0;
	int sLastPosX1=0;
	int sLastPosY1=0;
	int sLastSizeX1 = 0;
	int sLastSizeY1 = 0;
	int physicsDecreaseTick = 1; //clipping rate, basically (because physics ticks is tied to framerate xD)
	while (true) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
					case SDLK_w:
						speed_y=speed_y-0.1*physicsDecreaseTick;
						break;
					case SDLK_s:
						speed_y=speed_y+0.1*physicsDecreaseTick;
						break;
					case SDLK_a:
						speed_x=speed_x-0.1*physicsDecreaseTick;
						break;
					case SDLK_d:
						speed_x=speed_x+0.1*physicsDecreaseTick;
						break;
					case SDLK_t:
						debugDisplaySGrid = true;//!debugDisplaySGrid;
						//std::cout << "yup" << std::endl;
						break;
					case SDLK_y:
						debugDisplaySGrid = false;
						break;
				}
			}
		}
		//we know the shape is a square
		//first remove old griod
		//std::cout << "sizex " << sLastSizeX << std::endl;
		for (int i = 0; i < sLastSizeX; i++) {
			for (int o = 0; o < sLastSizeY; o++) {
				sGrid[i+sLastPosX][o+sLastPosY] = false;
				//std::cout << "x: " << i+sLastPosX << " | Y: " << o+sLastPosY << std::endl;
			}
		}
		for (int i = 0; i < sLastSizeX1; i++) {
			for (int o = 0; o < sLastSizeY1; o++) {
				sGrid[i+sLastPosX1][o+sLastPosY1] = false;
			}
		}
		//set some values
		int cPosX = (int) DestR.x/10; //will round down. 
		int cPosY = (int) DestR.y/10;
		int cPosX1 = (int) DestR1.x/10; //will round down. 
		int cPosY1 = (int) DestR1.y/10;

		int cSizeX = (SHAPE_SIZE/10)+((speed_x/9)+0.5); //need to find a good value, maybe some log or roots
		int cSizeY = (SHAPE_SIZE/10)+((speed_y/9)+0.5); //divided by 60 and times'd by 2 simplified

		//make new one, check for collisions
		bool didCollide = false;
		for (int i = 0; i < cSizeX ; i++) {
			for (int o = 0; o < cSizeY; o++) {
				//std::cout << i+cPosX << " x" << std::endl;
				//std::cout << o+cPosY << " y" << std::endl;

				if (sGrid[i+cPosX][o+cPosY] == true)
				{
					didCollide = true;
				}
				sGrid[i+cPosX][o+cPosY] = true;
			}
		}
		bool didCollide1 = false;
		for (int i = 0; i < SHAPE_SIZE/10 ; i++) {
			for (int o = 0; o < SHAPE_SIZE/10; o++) {
				//std::cout << i+cPosX << " x" << std::endl;
				//std::cout << o+cPosY << " y" << std::endl;

				if (sGrid[i+cPosX1][o+cPosY1] == true)
				{
					didCollide1 = true;
				}
				sGrid[i+cPosX1][o+cPosY1] = true;
			}
		}
		if (didCollide)
		{
			std::cout << "didCollide" << std::endl;
		}
		if (didCollide1)
		{
			std::cout << "DIDCOLLIDE" << std::endl;
		}
		//set old values
		sLastPosX = cPosX;
		sLastPosY = cPosY;
		sLastPosX1 = cPosX1;
		sLastPosY1 = cPosY1;

		//std::cout << "SIex " << cPosX << std::endl;
		sLastSizeX = cSizeX;
		sLastSizeY = cSizeY;//SHAPE_SIZE/10;
		sLastSizeX1 = SHAPE_SIZE/10; //prototype excuse
		sLastSizeY1 = SHAPE_SIZE/10;

		
		DestR.y = DestR.y+speed_y;
		DestR.x = DestR.x+speed_x;
		if (!debugDisplaySGrid)
		{
			SDL_RenderCopy(Main_Renderer, Background_Tx, NULL, NULL);
			SDL_RenderCopy(Main_Renderer, BlueShapes, NULL, &DestR);
			SDL_RenderCopy(Main_Renderer, BlueShapes, NULL, &DestR1);
			SDL_RenderPresent(Main_Renderer);
			SDL_Delay(16*physicsDecreaseTick);
			for (int i = 0; i < 10; i++)
			{
		//		std::cout << i;
			}
		} else {
			//this render  your cpu ;)
			SDL_RenderCopy(Main_Renderer, Background_Tx, NULL, NULL);
			/*SDL_Rect iDestR; //inaccurate destR
			iDestR.x = (double) cPosX*10;
			iDestR.y = (double) cPosY*10;
			iDestR.w = SHAPE_SIZE;
			iDestR.h = SHAPE_SIZE;
			SDL_RenderCopy(Main_Renderer, BlueShapes, NULL, &iDestR);*/

	/*for ( int r; r < 1000; r++) {
		for ( int t; t < 1000; t++) {
			sGrid[r][t] = false;
		}
	}*/

			SDL_Rect iDestR; //inaccurate destR
			for (int i = 0; i < 70; i++) { //ur screen cant even fit 50 of these
				//std::cout << "muh" << i << std::endl;
				for (int o = 0; o < 70; o++) {
					//std::cout << "o" << i << std::endl;
					if ( sGrid[i][o])
					{
						iDestR.x = (double) i*10;
						iDestR.y = (double) o*10;
						iDestR.w = SHAPE_SIZE;
						iDestR.h = SHAPE_SIZE;
						SDL_RenderCopy(Main_Renderer, DEBUG_somethingHere, NULL, &iDestR);
						//std::cout << "nothingHere; ";
					} else {
						iDestR.x = (double) i*10;
						iDestR.y = (double) o*10;
						iDestR.w = SHAPE_SIZE;
						iDestR.h = SHAPE_SIZE;
						SDL_RenderCopy(Main_Renderer, DEBUG_nothingHere, NULL, &iDestR);
						//std::cout << "somethingHere; ";
					}
				}
			}

			SDL_RenderPresent(Main_Renderer);
			SDL_Delay(16*physicsDecreaseTick);
		}
	}



	/* now onto the fun part.
	This will render a rotating selection of the blue shapes
	in the middle of the screen */
	int i;
	int n;
	for (i = 0; i < 2; ++i) {
		for(n = 0; n < 1000; ++n) {
			SrcR.x = SHAPE_SIZE * (n % 2);
			if (n > 1) {
				SrcR.y = SHAPE_SIZE;
			} else {
				SrcR.y = 0;
			}


			/* render background, whereas NULL for source and destination
			rectangles just means "use the default" */

			/* render the current animation step of our shape */
		}
	}



	/* The renderer works pretty much like a big canvas:
	when you RenderCopy() you are adding paint, each time adding it
	on top.
	You can change how it blends with the stuff that
	the new data goes over.
	When your 'picture' is complete, you show it
	by using SDL_RenderPresent(). */

	/* SDL 1.2 hint: If you're stuck on the whole renderer idea coming
	from 1.2 surfaces and blitting, think of the renderer as your
	main surface, and SDL_RenderCopy() as the blit function to that main
	surface, with SDL_RenderPresent() as the old SDL_Flip() function.*/

	SDL_DestroyTexture(BlueShapes);
	SDL_DestroyTexture(Background_Tx);
	SDL_DestroyRenderer(Main_Renderer);
	SDL_DestroyWindow(Main_Window);
	SDL_Quit();


	return 0;
}

