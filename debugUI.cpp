#include "debugUI.h"
	/* Debug code should be as isolated as possible except when pulling values from things. It should use its own UI library that shouldn't ever see updates unless really necessary */
void debugText::setText(const char* text)
{
	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, text, color);
	Message = SDL_CreateTextureFromSurface(myRenderer, surfaceMessage);

	Message_rect.w = surfaceMessage->w;
	Message_rect.h = surfaceMessage->h;

	SDL_FreeSurface(surfaceMessage);


}
void debugText::setText(std::string text)
{
	setText(text.c_str());
}	
debugText::debugText(SDL_Renderer* renderer, Vector2 position, std::string text)
{
	Message_rect.x = position.x;
	Message_rect.y = position.y;
	this->myRenderer = renderer;
	setText(text.c_str());
	this->wantsEvents = false;
}
void debugText::render(Vector2 parrentPosition)
{
	SDL_Rect transformedRect = Message_rect;
	transformedRect.x += parrentPosition.x;
	transformedRect.y += parrentPosition.y;
	SDL_RenderCopy(myRenderer, Message, NULL,&transformedRect);	
}
void debugText::render()
{
	render({0,0});
}
void debugText::giveEvent(Vector2 parrentPosition, SDL_Event event) //shouldn't ever be ran; look into finding ways to avoid implementing this
{
}





debugButton::debugButton(SDL_Renderer* myRenderer, Vector2 position, Vector2 size, std::string text)
{
	myText = new debugText(myRenderer,position,text);
	this->myRenderer = myRenderer;
	this->position = position;
	positionAndSize.x = position.x;
	positionAndSize.y = position.y;
	positionAndSize.w = size.x;
	positionAndSize.h = size.y;
	this->wantsEvents = true;
}
void debugButton::render(Vector2 parrentPosition)
{
	SDL_Rect translatedRect = positionAndSize;
	translatedRect.x += parrentPosition.x;
	translatedRect.y += parrentPosition.y;

	if (pressed)
	{
		SDL_SetRenderDrawColor( myRenderer, 0x00, 0x00, 0x00, 0x00 );
	}
	else
	{
		SDL_SetRenderDrawColor( myRenderer, 0xFF, 0x00, 0x00, 0xFF ); 
	}
	SDL_RenderFillRect( myRenderer, &translatedRect ); 
	myText->render(parrentPosition);
}
void debugButton::render()
{
	render({0,0});
}
void debugButton::giveEvent(Vector2 parrentPosition, SDL_Event event)
{
	
	Vector2 actualPosition = position;
	actualPosition.x += parrentPosition.x;
	actualPosition.y += parrentPosition.y;

	if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP)
	{
		if (event.button.button == SDL_BUTTON_LEFT)
		{
			
			int x,y;
			SDL_GetMouseState(&x,&y);
			//std::cout << "x: " << x << " y: << " << y << " b: "<<actualPosition.x+positionAndSize.w << std::endl;
			if (actualPosition.x <= x && actualPosition.y <= y && actualPosition.x+positionAndSize.w >= x && actualPosition.y+positionAndSize.h >= y)
			{
				std::cout << "BINGO" << std::endl;
				if (event.type == SDL_MOUSEBUTTONDOWN)
				{
					pressed = true;
				}
				else if (pressed)
				{
					if ( C_Pressed)
					{
						C_Pressed();
					}
					pressed = false;
				}
				
			}
			else if (event.type == SDL_MOUSEBUTTONUP)
			{
				pressed = false;
			}
		}
	}		
}




	




debugWindow::debugWindow(SDL_Renderer* myRenderer,Vector2 position, Vector2 size)
{
	this->position = position;
	this->positionAndSize.x = position.x;
	this->positionAndSize.y = position.y;
	this->positionAndSize.w = size.x;
	this->positionAndSize.h = size.y;
	this->myRenderer = myRenderer;
	//add x button
	xbutton = new debugButton(myRenderer, {size.x-25,0},{25,25},"X");

	auto closeCallback = [&] () { isOpen = false; };
	xbutton->C_Pressed = closeCallback;

	addElement(xbutton);

	//add drag button
	dbutton = new debugButton(myRenderer, {0,0},{25,25},"D");
	addElement(dbutton);
}
int debugWindow::addElement(debugUIElement* element)
{
	elements[nextUIElement] = element;
	nextUIElement++;
	return nextUIElement-1;
}
debugUIElement* debugWindow::getElement(int elementID)
{
	return elements[elementID];
}
void debugWindow::render()
{
	SDL_SetRenderDrawColor( myRenderer, 0xFF, 0xFF, 0xFF, 0xFF ); 
	SDL_RenderFillRect( myRenderer, &positionAndSize ); 
	for (int i=0;i<nextUIElement;i++)
	{
		elements[i]->render(position);
	}
}
void debugWindow::giveEvent(SDL_Event event)
{
	if (!isOpen)
	{
		return;
	}
	for (int i=0;i<nextUIElement;i++)
	{
		if (elements[i]->wantsEvents)
		{
			elements[i]->giveEvent(position,event);
		}
	}
	if (dbutton->pressed) //improper use of buttons but whatever, it's a debug system
	{
		int x,y;
		SDL_GetMouseState(&x,&y);
		position.x = x;
		position.y = y;
		positionAndSize.x = x;
		positionAndSize.y = y;
	}

}
