#include "UI.h"


/*
	The OO paradign makes a lot of sense for UI development. For this reason, I'm actually going to keep this OO. 

	It still isn't fun to develop. Procedural code is more elegent, easier to understand, and easier to program. 

*/

//TODO: memory management. yikes.

void GE_UI_Text::setText(const char* text)
{
	if (strlen(text) == 0) //this lil statement prevents everything from crashing and burning by preventing surfaceMessage from being NULL.
	{
		text = " ";
	}
	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, text, color);
	if (surfaceMessage == NULL) //extra protection
	{
		return;
	}
	Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

	Message_rect.w = surfaceMessage->w;
	Message_rect.h = surfaceMessage->h;

	SDL_FreeSurface(surfaceMessage);


}
void GE_UI_Text::setText(std::string text)
{
	setText(text.c_str());
}	
GE_UI_Text::GE_UI_Text(SDL_Renderer* renderer, Vector2 position, std::string text, SDL_Color color)
{
	Message_rect.x = position.x;
	Message_rect.y = position.y;
	this->renderer = renderer;
	setText(text.c_str());
	this->color = color;
	this->wantsEvents = false;
}
GE_UI_Text::~GE_UI_Text()
{
	
	//TTF_CloseFont(Sans); //this segfaults but shouldn't. perhaps the Sans is global to all GE_UI_Texts ? not super experinced with c++ oop
	SDL_DestroyTexture(Message);

}
void GE_UI_Text::render(Vector2 parrentPosition)
{
	SDL_Rect transformedRect = Message_rect;
	transformedRect.x += parrentPosition.x;
	transformedRect.y += parrentPosition.y;
	SDL_RenderCopy(renderer, Message, NULL,&transformedRect);	
}
void GE_UI_Text::render()
{
	render({0,0});
}
void GE_UI_Text::giveEvent(Vector2 parrentPosition, SDL_Event event) //shouldn't ever be ran; look into finding ways to avoid implementing this
{
}


GE_UI_TextInput::GE_UI_TextInput(SDL_Renderer* renderer, Vector2 position, Vector2 size, SDL_Color textColor, SDL_Color color)
{
	this->renderer = renderer;
	this->position = position;
	this->size = size;
	this->color = color;

	this->myText = new GE_UI_Text(renderer,position,"",textColor);
	this->text = "";
}
GE_UI_TextInput::~GE_UI_TextInput()
{
	delete myText;
}

void GE_UI_TextInput::render(Vector2 parrentPosition)
{
	SDL_Rect translatedRect;
	translatedRect.x = parrentPosition.x+position.x;
	translatedRect.y = parrentPosition.y+position.y;
	translatedRect.w = myText->Message_rect.w;
	translatedRect.h = myText->Message_rect.h;

	SDL_SetRenderDrawColor( renderer, color.r,color.g,color.b,color.a ); 

	//order is important. draw the background before the text.

	SDL_RenderFillRect(renderer, &translatedRect);
	myText->render(parrentPosition);

}
void GE_UI_TextInput::render()
{
	render({0,0});
}
void GE_UI_TextInput::setText(const char* text)
{
	myText->setText(text);
	this->text = text; //TODO works?
}
void GE_UI_TextInput::setText(std::string text)
{
	myText->setText(text);
	this->text = text;
}
void GE_UI_TextInput::giveEvent(Vector2 parrentPosition, SDL_Event event)
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
			if (actualPosition.x <= x && actualPosition.y <= y && actualPosition.x+size.x >= x && actualPosition.y+size.y >= y)
			{
				std::cout << "BINGO" << std::endl;
				if (event.type == SDL_MOUSEBUTTONDOWN)
				{
					isFocused = true;
					SDL_StartTextInput();
					
				}
				
			}
			else if (event.type == SDL_MOUSEBUTTONUP)
			{
				isFocused = false;
				SDL_StopTextInput();
			}
		}
	}	
	else if ((event.type == SDL_TEXTINPUT) && (isFocused) && (event.key.repeat == 0))
	{
		setText(text+event.text.text);	
	}
	else if ((event.type == SDL_KEYDOWN) && (isFocused) && (text.length() != 0) )
	{
		std::string tmptext = text;
		if (event.key.keysym.sym == SDLK_BACKSPACE)
		{
			tmptext.erase(text.length()-1);
		}
		else if (event.key.keysym.sym == SDLK_DELETE) 
		{
			tmptext.erase(1,1);
		}
		setText(tmptext);

	}
	else if (event.type == SDL_KEYUP && (isFocused))
	{
		if (event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_RETURN2 || event.key.keysym.sym == SDLK_KP_ENTER)
		{
			isFocused = false;
		}
	}
}
std::string GE_UI_TextInput::getText()
{
	return text;
}
const char* GE_UI_TextInput::getText_cstr()
{
	return text.c_str();
}


GE_UI_Button::GE_UI_Button(SDL_Renderer* renderer, Vector2 position, Vector2 paddingSize, std::string text, SDL_Color textColor, SDL_Color color, SDL_Color pressedColor)
{
	myText = new GE_UI_Text(renderer,position,text,textColor);
	this->renderer = renderer;
	this->position = position;
	positionAndSize.x = position.x;
	positionAndSize.y = position.y;
	this->paddingSize = paddingSize;
	this->color = color;
	this->pressedColor = pressedColor;
	this->wantsEvents = true;
}
GE_UI_Button::~GE_UI_Button()
{
	delete myText;

}
void GE_UI_Button::render(Vector2 parrentPosition)
{
	SDL_Rect translatedRect = positionAndSize;
	translatedRect.x += parrentPosition.x;
	translatedRect.y += parrentPosition.y;
	translatedRect.w = myText->Message_rect.w+paddingSize.x; //add the size of the text plus our padding. good ol' violation of OO rules (direct access to data)
	translatedRect.h = myText->Message_rect.h+paddingSize.y;

	if (pressed)
	{
		//hey, yknow what would be funny? Let's give our API users a color struct... AND THEN MAKE THEM CONVERT IT BACK TO UINT8'S! 
		SDL_SetRenderDrawColor( renderer, pressedColor.r,pressedColor.g,pressedColor.b,pressedColor.a );
	}
	else
	{
		SDL_SetRenderDrawColor( renderer, color.r,color.g,color.b,color.a ); 
	}
	SDL_RenderFillRect( renderer, &translatedRect ); 
	myText->render(parrentPosition);
}
void GE_UI_Button::render()
{
	render({0,0});
}
void GE_UI_Button::giveEvent(Vector2 parrentPosition, SDL_Event event)
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
			if (actualPosition.x <= x && actualPosition.y <= y && actualPosition.x+myText->Message_rect.w+paddingSize.x >= x && actualPosition.y+myText->Message_rect.h+paddingSize.y >= y)
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




	




GE_UI_Surface::GE_UI_Surface(SDL_Renderer* renderer,Vector2 position, Vector2 size)
{
	this->position = position;
	this->positionAndSize.x = position.x;
	this->positionAndSize.y = position.y;
	this->positionAndSize.w = size.x;
	this->positionAndSize.h = size.y;
	this->renderer = renderer;
}
GE_UI_Surface::~GE_UI_Surface() 
{
	//do nothing. we didn't allocate the UI elements given to us, so we have no business deleting them.
}
int GE_UI_Surface::addElement(GE_UI_Element* element)
{
	elements[nextUIElement] = element;
	nextUIElement++;
	return nextUIElement-1;
}
GE_UI_Element* GE_UI_Surface::getElement(int elementID)
{
	return elements[elementID];
}
void GE_UI_Surface::render()
{
	SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF ); 
	SDL_RenderFillRect( renderer, &positionAndSize ); 
	for (int i=0;i<nextUIElement;i++)
	{
		elements[i]->render(position);
	}
}
void GE_UI_Surface::giveEvent(SDL_Event event)
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
}
