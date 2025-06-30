//
// Created by Lucas N. Ferreira on 22/05/25.
//

#include "UIButton.h"

UIButton::UIButton(const std::string& text, class UIFont* font, std::function<void()> onClick,
                    const Vector2& pos, const Vector2& size, const Vector3& color,
                    int pointSize , unsigned wrapLength,
                    const Vector2 &textPos, const Vector2 &textSize, const Vector3& textColor)
        :UIElement(pos, size, color)
        ,mOnClick(onClick)
        ,mHighlighted(false)
        ,mText(text, font, pointSize, wrapLength, textPos, textSize, textColor)
{

}

UIButton::~UIButton()
{

}


void UIButton::Draw(SDL_Renderer *renderer, const Vector2 &screenPos)
{
    
    SDL_Rect titleQuad{
        .x = static_cast<int>(mPosition.x + screenPos.x),
        .y = static_cast<int>(mPosition.y + screenPos.y),
        .w = static_cast<int>(mSize.x),
        .h = static_cast<int>(mSize.y),
    };

    if (mHighlighted) SDL_SetRenderDrawColor(renderer, 200, 100, 0, 255);
    else SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_RenderFillRect(renderer, &titleQuad);

    auto size= mText.GetSize();
    size.x *= 0.25;
    size.y *= 0.5;
    mText.Draw(renderer,  mPosition + size);
}

void UIButton::OnClick()
{
    if (mOnClick != nullptr)
        mOnClick();
}
