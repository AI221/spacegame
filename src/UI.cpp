#include "UI.h"



void GE_UI_Element::giveEvent(Vector2 parrentPosition, SDL_Event event) {} //defined so that not all derivatives must define it



/*
	The OO paradign makes a lot of sense for UI development. For this reason, I'm actually going to keep this OO. 
*/

//TODO: memory management. yikes.

void GE_UI_Text::setText(const char* text)
{
	if (strlen(text) == 0) //this lil statement prevents everything from crashing and burning by preventing surfaceMessage from being NULL.
	{
		text = " ";
	}
	SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, text, color);
	if (surfaceMessage == NULL) //extra protection
	{
		return; //TODO CRASH NOTE: IF, DURING CONSTRUCTION, MESSAGE IS FAILED TO BE SET, THE NEXT RENDER() WILL CAUSE A CRASH.
	}
	if (Message != NULL)
	{
		SDL_DestroyTexture(Message); //Destroy old texture before putting in a new one
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
void GE_UI_Text::setSize(Vector2 newSize)
{
	this->size = newSize;
}	
void GE_UI_Text::setPosition(Vector2 newPosition)
{
	this->position = newPosition;
}
void GE_UI_Text::centerX()
{
	doCenterX = true;
}
void GE_UI_Text::centerY()
{
	doCenterY = true;
}
void GE_UI_Text::center()
{
	centerX();
	centerY();
}
void GE_UI_Text::expandToTextSize()
{
	this->size.x = this->Message_rect.w;
	this->size.y = this->Message_rect.h;
}
GE_UI_Text::GE_UI_Text(SDL_Renderer* renderer, Vector2 position, Vector2 size, std::string text, SDL_Color color, TTF_Font* font)
{
	this->font = font;



	this->position = position;
	this->size = size;

	this->renderer = renderer;

	this->color = color;
	Message = NULL; //Initialized Message to NULL so that setText knows it shouldn't be freed ( because it doesn't yet exist)
	setText(text); //Fills Message variable belonging to this class
	this->wantsEvents = false;

	this->doCenterX = false;
	this->doCenterY = false;

	//this->scrollPosition;
	//this->cursorPosition;
}
GE_UI_Text::~GE_UI_Text()
{
	
	//TTF_CloseFont(Sans); //this segfaults but shouldn't. perhaps the Sans is global to all GE_UI_Texts ?
	SDL_DestroyTexture(Message);

}
void GE_UI_Text::render(Vector2 parrentPosition)
{
	SDL_Rect transformedRect,animationRect;
	transformedRect.x = parrentPosition.x+position.x;
	transformedRect.y = parrentPosition.y+position.y;
	transformedRect.w = std::min(size.x,(double) Message_rect.w);
	transformedRect.h = std::min(size.y,(double) Message_rect.h);
	/*transformedRect.w = size.x;
	transformedRect.h = size.y;*/

	animationRect.x = 0;
	animationRect.y = 0;
	animationRect.w = size.x;
	animationRect.h = size.y;


	if (doCenterX)
	{
		transformedRect.x -= (this->Message_rect.w/2);
	}
	if (doCenterY)
	{
		transformedRect.y -= (this->Message_rect.h/2);
	}

	SDL_RenderCopy(renderer, Message,&animationRect,&transformedRect);	
}
void GE_UI_Text::render()
{
	render({0,0});
}
GE_UI_TextInput::GE_UI_TextInput(SDL_Renderer* renderer, Vector2 position, Vector2 size, SDL_Color textColor, SDL_Color color, TTF_Font* font)
{
	this->renderer = renderer;
	this->position = position;
	this->size = size;
	this->color = color;

	this->myText = new GE_UI_Text(renderer,position,size,"",textColor,font);
	this->text = "";
	this->isFocused = false; //First catch by cppcheck!
}
GE_UI_TextInput::~GE_UI_TextInput()
{
	delete myText;
}

void GE_UI_TextInput::render(Vector2 parrentPosition)
{
	SDL_Rect translatedRect;
	translatedRect.x = position.x;
	translatedRect.y = position.y;
	translatedRect.w = size.x;//myText->Message_rect.w;
	translatedRect.h = size.y;//myText->Message_rect.h;

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
	else if ((event.type == SDL_TEXTINPUT) && (isFocused) )
	{
		printf("TEXT INPUT: %s\n",event.text.text);
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
	//printf("textediting: %d\n",SDL_TEXTEDITING);
}
std::string GE_UI_TextInput::getText()
{
	return text;
}
const char* GE_UI_TextInput::getText_cstr()
{
	return text.c_str();
}


GE_UI_Button::GE_UI_Button(SDL_Renderer* renderer, Vector2 position, Vector2 paddingSize, std::string text, SDL_Color textColor, SDL_Color color, SDL_Color pressedColor, TTF_Font* font)
{
	myText = new GE_UI_Text(renderer,position,{0,0},text,textColor,font);
	myText->setSize({static_cast<double>(myText->Message_rect.w), static_cast<double>(myText->Message_rect.h)});
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
		//hey, yknow what would be funny? Let's give our API users a color struct... and then make them convert it back to uint8'S! 
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



GE_UI_ProgressBar::GE_UI_ProgressBar(SDL_Renderer* renderer, Vector2 position, Vector2 size, GE_Color color, GE_Color background, bool showProgressNumber)
{
	this->renderer = renderer;
	this->position = position;
	this->size = size;
	this->showProgressNumber = showProgressNumber;
	this->background = new GE_RectangleShape(renderer,background);
	this->bar = new GE_RectangleShape(renderer,color);

	setProgress(0);
	this->wantsEvents = false;
	

}
GE_UI_ProgressBar::~GE_UI_ProgressBar()
{
	delete background;
	delete bar;
}
void GE_UI_ProgressBar::render(Vector2 parrentPosition)
{
	render({parrentPosition.x,parrentPosition.y,0});
}
void GE_UI_ProgressBar::render(Vector2r parrentPosition)
{
	background->render(parrentPosition+position,size);
	Vector2 barSize = size;
	barSize.x *= progress;


	//Probably a more effecient way of doing this, but this works.

	if (parrentPosition.r != 0.0) //small micro-optmization, we don't need need to calculate all of this if we have no rotation because result.x and result.y will be 0
	{
		Vector2 bigTopLeft = size/2;

		GE_Vector2Rotation(&bigTopLeft,parrentPosition.r);
		
		bigTopLeft = bigTopLeft - (size/2);


		Vector2 smallTopLeft = barSize/2;

		GE_Vector2Rotation(&smallTopLeft,parrentPosition.r);
		
		smallTopLeft = smallTopLeft - (barSize/2);

		Vector2 result = (bigTopLeft-smallTopLeft);
		result.x = std::abs(result.x);

		parrentPosition.x += result.x; //basically, take the diffence in the resulting top-lefts of the rectangles and subtract that from the small one, aligning it to the big one's top left
		parrentPosition.y -= result.y;
	}

	//printf("res %f,%f\n",result.x,result.y);




	bar->render(parrentPosition+position,barSize);
}
void GE_UI_ProgressBar::setProgress(double progress)
{
	this->progress = std::max(std::min(progress,1.0),0.0);
}	
double GE_UI_ProgressBar::getProgress()
{
	return progress;
}


GE_UI_DraggableProgressBar::GE_UI_DraggableProgressBar(SDL_Renderer* renderer, Vector2 position, Vector2 size, GE_Color color, GE_Color background, bool showProgressNumber) : GE_UI_ProgressBar(renderer,position,size,color,background,showProgressNumber)
{
	this->position = position;
	this->size = size;

	this->pollMouse = false;
	this->wantsEvents = true; //Re-set this to true (after been set to false by ProgressBar's constructor
} 
GE_UI_DraggableProgressBar::~GE_UI_DraggableProgressBar()
{
}
void GE_UI_DraggableProgressBar::giveEvent(Vector2 parrentPosition, SDL_Event event)
{
	Vector2 actualPosition = position;
	actualPosition.x += parrentPosition.x;
	actualPosition.y += parrentPosition.y;

	if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
	{
		int x,y;
		SDL_GetMouseState(&x,&y);
		if (x >= actualPosition.x && y>= actualPosition.y && x<= size.x+actualPosition.x && y <= size.y+actualPosition.y)
		{
			printf("pullmouse2\n");
			pollMouse = true;
		}
	}
	else if (event.type == SDL_MOUSEBUTTONUP)
	{
		pollMouse = false;
	}
	if (pollMouse)
	{
		int x,y;
		SDL_GetMouseState(&x,&y);
		setProgress((x-actualPosition.x)/size.x);
	}

}
	

GE_UI_Surface::GE_UI_Surface(SDL_Renderer* renderer,Vector2 position, Vector2 size, SDL_Color backgroundColor)
{
	this->position = position;
	this->positionAndSize.x = position.x;
	this->positionAndSize.y = position.y;
	this->positionAndSize.w = size.x;
	this->positionAndSize.h = size.y;
	this->renderer = renderer;
	this->backgroundColor = backgroundColor;
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
	SDL_SetRenderDrawColor( renderer,backgroundColor.r,backgroundColor.g,backgroundColor.b,backgroundColor.a ); 
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
