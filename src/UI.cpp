#include "UI.h"

#include <math.h>
#include <iostream>
#include <string>
#include <functional>
#include <cstring>
#include <cmath>
#include <list>
#include <algorithm>


//Local includes

#include "vector2.h"
#include "shapes.h"
#include "GeneralEngineCPP.h"


void GE_UI_Element::giveEvent(Vector2 UNUSED(parrentPosition), SDL_Event UNUSED(event)) {} //defined so that not all derivatives must define it
GE_UI_Element::~GE_UI_Element() {}

GE_UI_TopLevelElement::~GE_UI_TopLevelElement() 
{
	GE_UI_RemoveTopLevelElement(this);
}




//TODO: memory management. yikes.

void GE_UI_Text::setText(const char* text)
{
	size_t textSize = strlen(text);
	if (textSize == 0) //this lil statement prevents everything from crashing and burning by preventing surfaceMessage from being NULL.
	{
		text = " ";
	}
	if (strcmp(text,currentText)== 0 ) //don't re-render text we've already rendered. cannot cause segfault for surfaceMessage being unitialized -- text will be set to " " if blank, and current texted is initialzed as blank.
	{
	//	printf("%s and %s are the same\n",text,currentText);
		return;
	}
	//	printf("%s and %s are different\n",text,currentText);
	size_t end = std::min(sizeof(currentText),textSize);
	strncpy(currentText,text,end);
	currentText[end] = '\0';

	SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, text, static_cast<SDL_Color>(color));
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
Vector2 GE_UI_Text::getSize()
{
	return this->size;
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
void GE_UI_Text::alignLeft()
{
	doAlignLeft = true;
}
void GE_UI_Text::expandToTextSize()
{
	this->size.x = this->Message_rect.w;
	this->size.y = this->Message_rect.h;
}

void GE_UI_Text::_init(SDL_Renderer* renderer, Vector2 position, Vector2 size, std::string text, GE_Color color, TTF_Font* font)
{
	//sanity checks because you don't want errors on font rendering(hard to trace)
	
	if (font == NULL)
	{
		printf("[Fatal] 0001: GE_UI_Text -- Your TTF_Font* was NULL.\n");
		exit(EXIT_FAILURE);
	}
	



	this->font = font;



	this->position = position;
	this->size = size;

	this->renderer = renderer;

	this->color = color;
	Message = NULL; //Initialized Message to NULL so that setText knows it shouldn't be freed ( because it doesn't yet exist)
	currentText[0] = '\0'; //initialize currentText to be blank
	setText(text); //Fills Message variable belonging to this class

	this->doCenterX = false;
	this->doCenterY = false;
	this->doAlignLeft = false;

	//this->scrollPosition;
	//this->cursorPosition;
	
	
	this->wantsEvents = false;
}

GE_UI_Text::GE_UI_Text(SDL_Renderer* renderer, Vector2 position, Vector2 size, std::string text, GE_Color color, TTF_Font* font)
{
	_init(renderer,position,size,text,color,font);
}
GE_UI_Text::GE_UI_Text(SDL_Renderer* renderer, Vector2 position, Vector2 size, std::string text, GE_UI_FontStyle style)
{
	_init(renderer, position, size, text, style.color,style.font);
}
GE_UI_Text::~GE_UI_Text()
{
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
	if (doAlignLeft)
	{
		transformedRect.x -=(this->Message_rect.w);
	}

	SDL_RenderCopy(renderer, Message,&animationRect,&transformedRect);	
}
void GE_UI_Text::render()
{
	render({0,0});
}
GE_UI_TextInput::GE_UI_TextInput(SDL_Renderer* renderer, Vector2 position, Vector2 size, GE_Color textColor, GE_Color color, TTF_Font* font)
{
	this->renderer = renderer;
	this->position = position;
	this->size = size;
	this->color = color;

	this->myText = new GE_UI_Text(renderer,position,size,"",textColor,font);
	this->text = "";
	this->isFocused = false; //First catch by cppcheck!

	this->wantsEvents = true;
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


GE_UI_Button::GE_UI_Button(SDL_Renderer* renderer, Vector2 position, Vector2 paddingSize, std::string text, GE_Color textColor, GE_Color color, GE_Color pressedColor, TTF_Font* font)
{
	myText = new GE_UI_Text(renderer,position+paddingSize/2,{0,0},text,textColor,font);
	myText->expandToTextSize();
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
	translatedRect.w = myText->Message_rect.w+paddingSize.x; //add the size of the text plus our padding. 
	translatedRect.h = myText->Message_rect.h+paddingSize.y;

	if (pressed)
	{
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
Vector2 GE_UI_Button::getSize()
{
	return {myText->Message_rect.w+paddingSize.x,myText->Message_rect.y+paddingSize.y};
}



GE_UI_PositioningRectangle::GE_UI_PositioningRectangle(Vector2 position, Vector2 size)
{
	this->originPosition = position;
	this->originSize = size;
}

Vector2 GE_UI_PositioningRectangle::getPosition()
{
	Vector2 returnPosition = originPosition;
	if (getModifierValue(GE_UI_PositioningRectangleModifiers::centerX))
	{
		returnPosition.x -= originSize.x/2;
	}
	if (getModifierValue(GE_UI_PositioningRectangleModifiers::centerY))
	{
		returnPosition.y -= originSize.y/2;
	}


	return returnPosition;
}
Vector2 GE_UI_PositioningRectangle::getSize()
{
	Vector2 returnSize = originSize;

	return returnSize;
}

void GE_UI_PositioningRectangle::setPosition(Vector2 position)
{
	originPosition = position;
}
void GE_UI_PositioningRectangle::setSize(Vector2 size)
{
	/*if (getModifierValue(GE_UI_PositioningRectangleModifiers::expandSizeToFit))
	{
		printf("[Warning] 0002 - GE_UI_PositioningRectangle::setSize cannot be called when GE_UI_PositioningRectangleModifiers::expandSizeToFit is true\n");
		return;
	}*/ //TODO: Implement a way for the owner object to change the size that the public cannot touch
	originSize = size;

}

void GE_UI_PositioningRectangle::setModifier(GE_UI_PositioningRectangleModifiers modifier, bool value)
{
	modifiers[static_cast<int>(modifier)] = value;
}
bool GE_UI_PositioningRectangle::getModifierValue(GE_UI_PositioningRectangleModifiers modifier)
{
	return modifiers[static_cast<int>(modifier)];
}
Vector2 GE_UI_PositioningRectangle::getOriginPosition()
{
	return originPosition;
}
Vector2 GE_UI_PositioningRectangle::getOriginSize()
{
	return originSize;
}

GE_Experimental_UI_Button::GE_Experimental_UI_Button(SDL_Renderer* renderer, Vector2 position, Vector2 paddingSize, GE_Color color, GE_Color pressedColor, GE_UI_Text* text)
{
	this->text = text;
	this->renderer = renderer;
	this->paddingSize = paddingSize;
	this->color = color;
	this->pressedColor = pressedColor;

	positioningRectangle = new GE_UI_PositioningRectangle(position,paddingSize);
	//positioningRectangle->setModifier(GE_UI_PositioningRectangleModifiers::expandSizeToFit,true);
	normalRectangle = new GE_RectangleShape(renderer,color);
	pressedRectangle = new GE_RectangleShape(renderer,pressedColor);
	this->pressed = false;
	this->triggered = false;

	this->wantsEvents = true;
}
GE_Experimental_UI_Button::~GE_Experimental_UI_Button()
{
	delete text;

	delete positioningRectangle;
	delete normalRectangle;
	delete pressedRectangle;
	

}
void GE_Experimental_UI_Button::render(Vector2 parrentPosition)
{
	if (positioningRectangle->getModifierValue(GE_UI_PositioningRectangleModifiers::expandSizeToFit))
	{
		//adjust our size in case the text size changed
		positioningRectangle->setSize({text->Message_rect.w+paddingSize.x,text->Message_rect.h+paddingSize.y});
	}
	if (pressed)
	{
		pressedRectangle->render(parrentPosition+positioningRectangle->getPosition(),positioningRectangle->getSize());
	}
	else
	{
		normalRectangle->render(parrentPosition+positioningRectangle->getPosition(),positioningRectangle->getSize());
	}
	text->render(parrentPosition);
}
void GE_Experimental_UI_Button::render()
{
	render({0,0});
}
void GE_Experimental_UI_Button::giveEvent(Vector2 parrentPosition, SDL_Event event)
{
	Vector2 actualPosition = positioningRectangle->getPosition();
	actualPosition.x += parrentPosition.x;
	actualPosition.y += parrentPosition.y;
	if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP)
	{
		if (event.button.button == SDL_BUTTON_LEFT)
		{
			int x,y;
			SDL_GetMouseState(&x,&y);
			if (actualPosition.x <= x && actualPosition.y <= y && positioningRectangle->getSize().x+positioningRectangle->getPosition().x >= x && positioningRectangle->getSize().y +positioningRectangle->getPosition().y>= y)
			{
				std::cout << "BINGO" << std::endl;
				if (event.type == SDL_MOUSEBUTTONDOWN)
				{
					pressed = true;
				}
				else if (pressed && event.type == SDL_MOUSEBUTTONUP)
				{
					triggered = true;
					pressed = false;
				}
				
			}
			else if (event.type == SDL_MOUSEBUTTONUP) //release the button but do not trigger it if they release the mouse outside the button box
			{
				pressed = false;
			}
		}
	}		
}
bool GE_Experimental_UI_Button::getIsTriggered()
{
	return triggered;
}
void GE_Experimental_UI_Button::setIsTriggered(bool triggered)
{
	this->triggered = triggered;
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

Vector2 GE_AlignChildRectToParrent(double parrentRotation, Vector2 parrentSize, double childRotation, Vector2 childSize)
{
	//find their top lefts
	
	//set the values equal to the center
	Vector2 parrentTopLeft = parrentSize/2;
	Vector2 childTopLeft = childSize/2;

	//apply rotation to positions
	GE_Vector2Rotation(&parrentTopLeft,parrentRotation);
	GE_Vector2Rotation(&childTopLeft,childRotation);

	//subtract the center to get the top left
	
	parrentTopLeft = parrentTopLeft-parrentSize/2;
	childTopLeft = childTopLeft-childSize/2;


	//find the difference between the top lefts
	
	Vector2 difference = childTopLeft-parrentTopLeft;


	return difference;


}
void GE_UI_ProgressBar::render(Vector2r parrentPosition)
{
	background->render(parrentPosition+position,size);
	Vector2 barSize = size;
	barSize.x *= progress;


	//Probably a more effecient way of doing this, but this works.

	if (parrentPosition.r != 0.0) //small micro-optmization, we don't need need to calculate all of this if we have no rotation because result.x and result.y will be 0
	{

		parrentPosition = parrentPosition+GE_AlignChildRectToParrent(parrentPosition.r,size,parrentPosition.r,barSize);
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
	

	


GE_UI_Titlebar::GE_UI_Titlebar(SDL_Renderer* renderer, std::string name, GE_UI_Window* parrent, GE_UI_WindowTitleStyle style)
{
	this->title = new GE_UI_Text(renderer,{style.textOffset,style.height/2},{0,style.height},name,style.font.color,style.font.font);
	this->title->expandToTextSize();
	this->title->centerY();
	if (style.centerTitleText)
	{
		this->title->centerX();
	}

	this->background = new GE_RectangleShape(renderer,style.background);
	this->XButton = new GE_UI_Button(renderer, {style.buttonDistanceFromRight*-1,0},style.XButton.paddingSize,"X",style.XButton.text,style.XButton.background,style.XButton.backgroundPressed,style.XButton.font);
	auto buttonCallback = [parrent] ()
	{
		delete parrent;
	};
	this->XButton->C_Pressed = buttonCallback;

	this->style = style;

	this->parrent = parrent;

	this->wantsEvents = true;
}
GE_UI_Titlebar::~GE_UI_Titlebar()
{
	delete title;
	delete background;
	delete XButton;
}
void GE_UI_Titlebar::giveEvent(Vector2 parrentPosition, double parrentWidth, SDL_Event event, Vector2* windowPosition)
{
	XButton->giveEvent({parrentPosition.x+parrentWidth-XButton->getSize().x,parrentPosition.y}, event);

	if (event.type == SDL_MOUSEBUTTONDOWN)
	{
		int x,y;
		SDL_GetMouseState(&x,&y);
		if ((!XButton->pressed) && x>=parrentPosition.x && x<= parrentPosition.x+parrentWidth && y >= parrentPosition.y && y <= parrentPosition.y+style.height)
		{
			dragging = true;
			initialDragPosition = {static_cast<double>(x)-parrentPosition.x,static_cast<double>(y)-parrentPosition.y};
		}
	}
	else if (event.type == SDL_MOUSEBUTTONUP)
	{
		dragging = false;
	}
	if (dragging)
	{
		int x,y;
		SDL_GetMouseState(&x,&y);
		*windowPosition = {static_cast<double>(x)-initialDragPosition.x,static_cast<double>(y)-initialDragPosition.y};
	}


}
void GE_UI_Titlebar::render(Vector2 parrentPosition, double parrentWidth)
{
	background->render(parrentPosition,{parrentWidth,style.height});
	title->render(parrentPosition+Vector2{style.centerTitleText? parrentWidth/2 : 0,0});
	XButton->render({parrentPosition.x+parrentWidth-XButton->getSize().x,parrentPosition.y});
}





GE_UI_Surface::GE_UI_Surface(SDL_Renderer* renderer,Vector2 position, Vector2 size, GE_Color backgroundColor)
{
	this->renderer = renderer;
	this->position = position;
	this->size = size;

	this->backgroundRect = new GE_RectangleShape(renderer,backgroundColor);

	this->wantsEvents = true;
}
GE_UI_Surface::~GE_UI_Surface() 
{
	for (int i=0;i<nextUIElement;i++)
	{
		delete elements[i];
	}
	delete backgroundRect;
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
void GE_UI_Surface::render(Vector2 parrentPosition)
{
	backgroundRect->render(position+parrentPosition,size);
	for (int i=0;i<nextUIElement;i++)
	{
		elements[i]->render(position+parrentPosition);
	}
}
void GE_UI_Surface::giveEvent(Vector2 parrentPosition, SDL_Event event)
{
	for (int i=0;i<nextUIElement;i++)
	{
		if (elements[i]->wantsEvents)
		{
			elements[i]->giveEvent(position+parrentPosition,event);
		}
	}
}


GE_UI_Window::GE_UI_Window(SDL_Renderer* renderer, std::string name, Vector2 position, Vector2 surfaceSize, GE_UI_Style style)
{
	this->titleBarHeight = style.windowStyle.titleStyle.height;
	this->borderOffset = style.windowStyle.borderSize;
	this->position = position;


	this->titlebar = new GE_UI_Titlebar(renderer, name, this, style.windowStyle.titleStyle);
	this->surface = new GE_UI_Surface(renderer, Vector2{borderOffset,style.windowStyle.titleStyle.height},surfaceSize,style.windowStyle.background);
	this->size = surfaceSize+Vector2{style.windowStyle.borderSize*2,style.windowStyle.borderSize+style.windowStyle.titleStyle.height};
	

	this->border = new GE_RectangleShape(renderer,style.windowStyle.borderColor);



	this->wantsEvents = true;
}

GE_UI_Window::~GE_UI_Window()
{
	delete titlebar;
	delete surface;
	delete border;


	//remove us from the focus stack, if we're on it
	
	GE_UI_RemoveTopLevelElement(this);
}
void GE_UI_Window::render(Vector2 parrentPosition)
{
	border->render(parrentPosition+position,size);

	surface->render(parrentPosition+position);
	
	titlebar->render(parrentPosition+position,size.x);
}
void GE_UI_Window::giveEvent(Vector2 parrentPosition, SDL_Event event)
{
	surface->giveEvent(parrentPosition+position, event); //{parrentPosition.x,parrentPosition.y+titleBarHeight}

	titlebar->giveEvent(parrentPosition+position,size.x,event,&position);
	
}
bool GE_UI_Window::checkIfFocused(int mousex, int mousey)
{
	/*if (mousex >= position.x && mousex <= position.x+size.x && mousey >= position.y && mousey <= position.y+size.y)
	{
		return true;
	}
	return false;*/
	return checkIfFocused_ForBox(mousex,mousey,position,size);
}



GE_UI_Rectangle::GE_UI_Rectangle(SDL_Renderer* renderer, Vector2 position, Vector2 size, GE_Color color)
{
	this->rectangleShape = new GE_RectangleShape(renderer,color);

	this->position = position;

	this->size = size;

	this->wantsEvents = false;
}
GE_UI_Rectangle::~GE_UI_Rectangle()
{
	delete rectangleShape;
}
void GE_UI_Rectangle::render(Vector2 parrentPosition)
{
	rectangleShape->render(position+parrentPosition,size);
}


void GE_UI_TextList::_construct_step_1(SDL_Renderer* renderer, Vector2 position, Vector2 size, std::vector<GE_UI_StringOrDivider> elements, GE_UI_TextListStyle style)
{
	this->renderer = renderer;
	this->size = size;

	double verticalPositition = 0;
	for (auto element : elements)
	{
		verticalPositition += style.spaceBetweenElements;
		if (element.isDivider)
		{
			dividers.push_back(new GE_UI_Rectangle(
						renderer,
						{style.spacer.spaceLeftAtEachEdge,verticalPositition+style.spacer.spaceLeftAtEachEdge/2},
						{size.x-(style.spacer.spaceLeftAtEachEdge*2),style.spacer.verticalSize},
						style.spacer.color)
					);

			verticalPositition += style.spacer.verticalSize+style.spacer.spaceLeftAtEachEdge;
		}
		else
		{
			textObjects.push_back(new GE_UI_Text(
						renderer,
						{style.textSpaceLeftAtEachEdge,verticalPositition},
						{size.x,static_cast<double>(style.verticalSizeOfText)},
						element.string,style.font)
					);
			textObjectsPosition.push_back(verticalPositition);

			verticalPositition += style.verticalSizeOfText;
		}
	}
	this->style = style;
	this->background = new GE_UI_Rectangle(renderer,{0,0},size,style.background);

	this->highlight = new GE_RectangleShape(renderer,style.highlight);



	this->currentClicked.happened = false;


	//potentially may set position of sub-menus
	this->setPosition(position);

	this->hasHighlightedTextObject = false;


	this->wantsEvents = true;
}
GE_UI_TextList::GE_UI_TextList(SDL_Renderer* renderer, Vector2 position, Vector2 size, std::vector<GE_UI_StringOrDivider> elements, GE_UI_TextListStyle style)
{
	_construct_step_1(renderer,position,size,elements,style);
	this->hasDropRightMenus = false;
	this->hasDropRightMenuOpen = false;
	this->hasHighlightedTextObject = false;
}


GE_UI_TextList::GE_UI_TextList(SDL_Renderer* renderer, Vector2 position, Vector2 size, std::vector<GE_UI_StringOrDivider> elements, std::vector<GE_UI_TextList*> dropRightMenus, std::vector<unsigned int> elementsWhichHostDropRightMenus, GE_UI_TextListStyle style)
{
	this->dropRightMenus = dropRightMenus;
	this->elementsWhichHostDropRightMenus = elementsWhichHostDropRightMenus;
	this->hasDropRightMenus = true;
	this->hasDropRightMenuOpen = false;
	_construct_step_1(renderer,position,size,elements,style);
}




GE_UI_TextList::~GE_UI_TextList()
{
	for (auto element : dividers)
	{
		delete element;
	}
	for (auto element : textObjects)
	{
		delete element;
	}
	delete background;
	delete highlight;
	if (hasDropRightMenus)
	{
		for (auto list : dropRightMenus)
		{
			delete list;
		}
	}
}
void internal_hack_alert_parrent(GE_UI_TopLevelElement* you)
{
	GE_UI_RemoveTopLevelElement(you);
	SDL_Event hack;
	hack.type = SDL_USEREVENT;
	SDL_PushEvent(&hack);
}
struct internal_return
{
	unsigned int ID;
	bool happened;
};
void GE_UI_TextList::giveEvent(Vector2 parrentPosition, SDL_Event event)
	//TODO: 
	//Upon an out-of-range click, this will assign click.trail[0] = 0. This is wrong
	//I may redo this entire thing instead of fixing that because this thing is weird.
{
	int x,y;
	SDL_GetMouseState(&x,&y);
	auto spotStruct = checkIfCursorOnText(parrentPosition,x,y);
	if (event.type == SDL_MOUSEBUTTONUP && !hasDropRightMenuOpen)
	{
		if (!(hasDropRightMenuOpen))
		{
			if (spotStruct.happened)
			{
				//remove old trails(incase the user didn't)
				if(currentClicked.happened)
				{
					setClicked(false);
				}
				currentClicked.trail.push_back(spotStruct.ID);
				currentClicked.happened = true;
				//check if element is a sub-menu host, which are not actually clickable.
				int o = 0;
				for (unsigned int elementID : elementsWhichHostDropRightMenus)
				{
					if (elementID == spotStruct.ID)
					{
						//we 'absorb' the click
						setClicked(false);
						break;
					}
					o++;
				}
				if(currentClicked.happened)
				{
					//TODO TEMPORARY UGLY UGLY UGLY HACK
					internal_hack_alert_parrent(this);	
				}
			}
		}
	}
	else if (event.type == SDL_MOUSEMOTION)
	{
		bool subMenuIsGettingFocusSoDontCheckThisMenu = false;
		if (hasDropRightMenuOpen)
		{
			if (dropRightMenus[dropRightMenuOpen]->checkIfFocused(x,y))
			{
				subMenuIsGettingFocusSoDontCheckThisMenu = true;
			}
		}
		if(!subMenuIsGettingFocusSoDontCheckThisMenu)
		{
			//if we don't find the cursor under something, it's moved off of something and it should no longer be highlighted.
			hasHighlightedTextObject = false;
			if (spotStruct.happened)
			{
				highlightedTextObject = spotStruct.ID;	
				hasHighlightedTextObject = true;

				//if it has a drop down menu, then open it (and add the element to the click trail)
				int o = 0;
				for (unsigned int elementID : elementsWhichHostDropRightMenus)
				{
					if (elementID == spotStruct.ID)
					{
						if(currentClicked.happened)
						{
							setClicked(false);
						}
						currentClicked.trail.push_back(spotStruct.ID);
						currentClicked.happened = true;


						hasDropRightMenuOpen = true;
						dropRightMenuOpen = o;
						break;
					}
					o++;
				}

			}
			else if (hasDropRightMenus && hasDropRightMenuOpen)
			{
				hasDropRightMenuOpen = false;
			}
			
		}
	}
	//if we have a submenu open, hand it the event reguardless of event.type
	if (hasDropRightMenus  && hasDropRightMenuOpen  )
	{
		dropRightMenus[dropRightMenuOpen]->giveEvent(parrentPosition,event);
		auto theirCurrentClicked = dropRightMenus[dropRightMenuOpen]->getClicked();
		if (theirCurrentClicked.happened)
		{
			dropRightMenus[dropRightMenuOpen]->setClicked(false);
			//add their click trail to ours	
			for (int menuElementClicked : theirCurrentClicked.trail)
			{
				currentClicked.trail.push_back(menuElementClicked);
			}
			//they're done being open cause they've reported a click
			hasDropRightMenuOpen = false;
			//report our click
			currentClicked.happened = true;
			//TODO TEMPORARY UGLY UGLY UGLY HACK
			internal_hack_alert_parrent(this);	

		}


	}
}
internal_return GE_UI_TextList::checkIfCursorOnText(Vector2 parrentPosition, int x, int y)
{
	unsigned int i = 0;
	for (auto textPosition : textObjectsPosition)
	{
		if (checkIfFocused_ForBox(x,y,Vector2{position.x,position.y+textPosition}+parrentPosition,{size.x,static_cast<double>(style.verticalSizeOfText)+style.spaceBetweenElements}))
		{
			return {i,true};
		}
		i++;
	}
	return {0,false};
}
void GE_UI_TextList::render(Vector2 parrentPosition)
{
	background->render(parrentPosition+position);
	for (auto element : dividers)
	{
		element->render(parrentPosition+position);
	}
	unsigned int i =0;
	for (auto element : textObjects)
	{
		if (hasHighlightedTextObject &&  highlightedTextObject == i)
		{
			highlight->render(Vector2{position.x,getPositionOfElement(i)+position.y}+parrentPosition,{size.x,static_cast<double>(style.verticalSizeOfText)});
		}
		element->render(parrentPosition+position);
		i++;
	}
	if (hasDropRightMenus && hasDropRightMenuOpen)
	{
		dropRightMenus[dropRightMenuOpen]->render(parrentPosition);
	}
}
bool GE_UI_TextList::checkIfFocused(int mousex, int mousey)
{
	bool isFocused = checkIfFocused_ForBox(mousex,mousey,position,size);

	if (hasDropRightMenus && hasDropRightMenuOpen)
	{
		hasDropRightMenuOpen = dropRightMenus[dropRightMenuOpen]->checkIfFocused(mousex,mousey); //re-rest if it should be open or has been clicked away from
		isFocused = isFocused || hasDropRightMenuOpen; //if either menus focused, we're good.
		if (!hasDropRightMenuOpen)
		{
			setClicked(false);
		}

	}
	if (!isFocused)
	{
		setClicked(false);
		SDL_Event hack;
		hack.type = SDL_USEREVENT;
		SDL_PushEvent(&hack);
	}
	return isFocused;
}
	
GE_UI_ClickedObject GE_UI_TextList::getClicked()
{
	return currentClicked;
}
void GE_UI_TextList::setClicked(bool happened)
{
	currentClicked.happened = happened;
	currentClicked.trail.clear();
	if (!happened)
	{
		//collapse the menu
		hasDropRightMenuOpen = false;
		//and collapse its menus
		for (auto list : dropRightMenus)
		{
			list->setClicked(false);
		}
	}
}
double GE_UI_TextList::getPositionOfElement(unsigned int ID)
{
	return textObjectsPosition[ID];
}
void GE_UI_TextList::setPosition(Vector2 position)
{
	this->position = position;
	int i=0;
	//re-set-position of the drop right menus
	for (auto elementID : elementsWhichHostDropRightMenus)
	{
		dropRightMenus[i]->setPosition({position.x+size.x,position.y+getPositionOfElement(elementID)});
		i++;
	}
}
Vector2 GE_UI_TextList::getSize()
{
	return size;
}





std::list<GE_UI_TopLevelElement*> renderOrder;
std::list<GE_UI_OmniEventReciever*> omniEventRecievers;
GE_UI_TopLevelElement* backgroundElement= NULL;
bool backgroundFocused = true;
GE_UI_TopLevelElement* cursorFollower;
bool hasCursorFollower;

void GE_UI_InsertTopLevelElement(GE_UI_TopLevelElement* element)
{
	renderOrder.insert(renderOrder.begin(),element);
}
void GE_UI_RemoveTopLevelElement(GE_UI_TopLevelElement* element)
{
	renderOrder.remove(element);
	if (renderOrder.size() <= 0) //Focus background if nothing else left
	{
		backgroundFocused = true;
	}
	if (GE_UI_GetBackgroundElement() == element)
	{
		GE_UI_RemoveBackgroundElement();
	}
}
void GE_UI_InsertOmniEventReciever(GE_UI_OmniEventReciever* element)
{
	omniEventRecievers.insert(omniEventRecievers.begin(),element);
}
void GE_UI_RemoveOmniEventReciever(GE_UI_OmniEventReciever* element)
{
	omniEventRecievers.remove(element);
	delete element;
}
void GE_UI_SetTopElement(GE_UI_TopLevelElement* element)
{
	if(element==backgroundElement)
	{
		backgroundFocused = true;
		return;
	}
	GE_UI_RemoveTopLevelElement(element); //will change the background focus to true if there are no elements
	renderOrder.insert(renderOrder.begin(), element); //gonna keep it like this rather than call own function; it might be changed later on not to bring to top focus.



	backgroundFocused = false; //do this last to avoid interference from other functions which would change it back.
}
void GE_UI_SetBackgroundElement(GE_UI_TopLevelElement* element)
{
	backgroundElement = element;
}
GE_UI_TopLevelElement* GE_UI_GetBackgroundElement()
{
	return backgroundElement;
}
void GE_UI_RemoveBackgroundElement()
{
	backgroundElement = NULL;
	backgroundFocused = false;
}
void GE_UI_SetCursorFollower(GE_UI_TopLevelElement* element)
{
	cursorFollower = element;
	hasCursorFollower = true;
}

bool GE_UI_PullEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event) != 0)
	{
		//give it to omni event recievers
		for (GE_UI_OmniEventReciever* it : omniEventRecievers)
		{
			it->giveEvent(event);
		}
		if (event.type == SDL_MOUSEBUTTONDOWN)
		{
			int x,y;
			SDL_GetMouseState(&x,&y);
			//check focus
			bool focusShift = false;
			for (auto i = renderOrder.begin();i != renderOrder.end();i++)
			{
				if ((*i)->checkIfFocused(x,y))
				{
					GE_UI_SetTopElement(*i);
					focusShift = true;
					break;
				}
			}
			//if they didn't click on any windows, then they clicked off and the background should come into focus
			if (!focusShift)
			{
				backgroundFocused = true;
			}

		}
		else if ( event.type == SDL_QUIT)
		{
			return true;
		}



		if (backgroundFocused && backgroundElement != NULL)
		{
			backgroundElement->giveEvent({0,0},event);
		}
		else
		{
			if (renderOrder.size() > 0) //extra sanity check
			{
				(*renderOrder.begin())->giveEvent({0,0},event);
			}
			else
			{
				printf("[WARNING] - No focused windows and background isn't focused\n");
			}
		}
	}
	return false;
}

void GE_UI_Render()
{
	//First render background
	if (backgroundElement != NULL)
	{
		backgroundElement->render({0,0});
	}

	for (auto i = renderOrder.rbegin();i != renderOrder.rend();i++)
	{
		(*i)->render({0,0});
	}
	
	if (hasCursorFollower)
	{
		IntVector2 cursorPos = GE_UI_GetMousePosition();
		cursorFollower->render(Vector2{static_cast<double>(cursorPos.x),static_cast<double>(cursorPos.y)});
	}

}



IntVector2 GE_UI_GetMousePosition()
{
	IntVector2 returnval;
	SDL_GetMouseState(&returnval.x,&returnval.y);
	return returnval;
}


void GE_ShutdownUI()
{
	printf("Called ShutdownUI\n");
	while (!renderOrder.empty()) //top level elements should remove themselves from the render order list. as a result this part is a little strange.
	{
		printf("Freed a window\n");
		delete *(renderOrder.begin());
		renderOrder.erase(renderOrder.begin());

	}
	while(!omniEventRecievers.empty())
	{
		delete *(omniEventRecievers.begin());
		omniEventRecievers.erase(omniEventRecievers.begin());
	}

}
