#include "UIFont.h"
#include <vector>
#include <SDL_image.h>

UIFont::UIFont(SDL_Renderer* renderer)
    :mRenderer(renderer)
{

}

UIFont::~UIFont()
{

}

bool UIFont::Load(const std::string& fileName)
{
	// We support these font sizes
	std::vector<int> fontSizes = {8,  9,  10, 11, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32,
								  34, 36, 38, 40, 42, 44, 46, 48, 52, 56, 60, 64, 68, 72};

    auto cstr = fileName.c_str();
	for (auto size : fontSizes) {
		auto font = TTF_OpenFont(cstr, size);
		if (font != nullptr)
			mFontData[size] = font;
		else {
			SDL_Log("Error to load the font at size %d\n", size);
			return false;
		}
	}
	return true;
}

void UIFont::Unload()
{
	for (auto& [size, font] : mFontData) {
		TTF_CloseFont(font);
	}
	mFontData.clear();
}

SDL_Texture* UIFont::RenderText(const std::string& text, const Vector3& color /*= Color::White*/,
                                int pointSize /*= 24*/, unsigned wrapLength /*= 900*/)
{
    if(!mRenderer)
    {
        SDL_Log("Renderer is null. Can't Render Text!");
        return nullptr;
    }

	// Convert to SDL_Color
	SDL_Color sdlColor;

	// Swap red and blue so we get RGBA instead of BGRA
	sdlColor.b = static_cast<Uint8>(color.x * 255);
	sdlColor.g = static_cast<Uint8>(color.y * 255);
	sdlColor.r = static_cast<Uint8>(color.z * 255);
	sdlColor.a = 255;

    
	auto it = mFontData.find(pointSize);
	if (it == mFontData.end())
		return SDL_Log("The specified size is not suported!\n"), nullptr;

	SDL_Surface* surface = TTF_RenderUTF8_Blended_Wrapped(
		it->second,
		text.c_str(),
		sdlColor,
		wrapLength
	);

	if (surface == nullptr)
		return SDL_Log("Error to load the surface of text!\n"), nullptr;


	auto texture = SDL_CreateTextureFromSurface(mRenderer, surface);
	SDL_FreeSurface(surface);

	if (texture == nullptr)
		return SDL_Log("Error to create a texture from surface!\n"), nullptr;

	return texture;
}
