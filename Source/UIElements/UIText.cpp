//
// Created by Lucas N. Ferreira on 22/05/25.
//

#include "UIText.h"
#include "UIFont.h"

UIText::UIText(const std::string &text, class UIFont* font, int pointSize, const unsigned wrapLength,
               const Vector2 &pos, const Vector2 &size, const Vector3 &color, bool fade)
   :UIElement(pos, size, color, fade)
   ,mFont(font)
   ,mPointSize(pointSize)
   ,mWrapLength(wrapLength)
   ,mTextTexture(nullptr)
{
    SetText(text);
}

UIText::~UIText()
{

}

void UIText::SetText(const std::string &text)
{
    
    if (mTextTexture != nullptr)
        SDL_DestroyTexture(mTextTexture),
        mTextTexture = nullptr;

    mTextTexture = mFont->RenderText(
        text,
        mColor,
        mPointSize,
        mWrapLength
    );
    mText = text;
}

void UIText::Draw(SDL_Renderer *renderer, const Vector2 &screenPos, Uint8 alpha)
{

    if (mTextTexture == nullptr) return;

    SDL_Rect titleQuad{
        .x = static_cast<int>(mPosition.x + screenPos.x),
        .y = static_cast<int>(mPosition.y + screenPos.y),
        .w = static_cast<int>(mSize.x),
        .h = static_cast<int>(mSize.y),
    };
    if (mFade)
        SDL_SetTextureAlphaMod(mTextTexture, alpha);

    SDL_RenderCopyEx(
        renderer,
        mTextTexture,
        nullptr,
        &titleQuad,
        0,
        nullptr,
        SDL_FLIP_NONE
    );
}