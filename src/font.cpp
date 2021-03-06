#include "font.h"

#include <map>

//Local includes
#include "json.h"
#include "FS.h"

typedef std::string fontName_t;
typedef std::string fontPath_t;

typedef std::pair<fontName_t,unsigned int> fontPair_t;

typedef std::map<fontName_t,fontPath_t> fonts_t;
typedef std::map<fontPair_t,GE_Font> fontRenders_t;

fonts_t fonts; //font name:font path
fontRenders_t fontRenders; //font name, size: font


int GE_Font_Init()
{	
	int ttferror = TTF_Init();
	if (ttferror < 0) 
	{
		//TODO: Handle error...
		printf("TTF_Init error %d\n",ttferror);
		return ttferror;
	}
	return 0;
}
void GE_Font_LoadFromList(std::string list)
{
	std::string parrentPath = filesystem::get_parrent_directory(list);
	std::string buffer = filesystem::read_all_from_file(list);
	Json::Value root;
	GE_ReadJson(buffer,&root);
	Json::Value spritelist = root["fontlist"];
	for (unsigned int i = 0; i < spritelist.size(); i++)
	{
		fontPath_t path = parrentPath+"/"+spritelist[i].asString();
		fontName_t name = filesystem::get_base_filename(filesystem::get_filename_without_extension(path));
		printf("Font name %s\n",name.c_str());
		fonts.insert(std::make_pair(name,path));
	}
}
std::optional<GE_Font> GE_Font_GetFont(std::string name, unsigned int size)
{
	//fontPath_t font = *(fonts.find(name));
	fontRenders_t::iterator fontIt = fontRenders.find(fontPair_t(name,size));
	if (fontIt == fontRenders.end())
	{
		fonts_t::iterator pathIt = fonts.find(name);
		if (pathIt == fonts.end())
		{
			return {};
		}
		GE_Font font = {TTF_OpenFont((pathIt->second).c_str(), size),size};
		if (!(font.font))
		{
			printf("TTF_OpenFont: %s\n", TTF_GetError());
			return {};
		}
		fontRenders.insert(std::make_pair(fontPair_t(name,size),font));

		return {font};
	}
	return {fontIt->second};
}
void GE_Font_Shutdown()
{
	fontRenders_t::iterator it;
	while (true)
	{
		it = fontRenders.begin();
		if (it == fontRenders.end())
		{
			break;
		}
		TTF_CloseFont(it->second.font);
		fontRenders.erase(it);
	}
	TTF_Quit();
}
