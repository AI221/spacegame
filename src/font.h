/*!
 * @file
 * @author Jackson McNeill
 *
 * A font loading system
 */
#pragma once

#include <SDL2/SDL_ttf.h>
#include <cassert>
#include <string>
#include <optional>





struct GE_Font
{
	TTF_Font* font;
	unsigned int size;
	GE_Font(TTF_Font* font, unsigned int size)
	{
		this->font = font;
		this->size = size;

		assert(font != NULL);
	}
	GE_Font(GE_Font const &font_struct)
	{
		this->font = font_struct.font;
		this->size = font_struct.size;

		assert(font != NULL);
	}
	GE_Font(GE_Font&& font_struct) noexcept
	{
		font = std::move(font_struct.font);
		size = std::move(font_struct.size);
	}
	GE_Font(){}
	GE_Font operator = (GE_Font other)
	{
		font = other.font;
		size = other.size;
		return *this;
	}

};


int GE_Font_Init();

void GE_Font_LoadFromList(std::string list);
std::optional<GE_Font> GE_Font_GetFont(std::string name, unsigned int size);
void GE_Font_Shutdown();

