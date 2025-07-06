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
        ,mText(text, font, pointSize, wrapLength, textPos, textSize, textColor, true)
{

}

UIButton::~UIButton()
{

}


void UIButton::Draw(SDL_Renderer *renderer, const Vector2 &screenPos, Uint8 alpha)
{
    const Vector2 posText = screenPos + mPosition; //+ mSize * 0.5f - mText.GetSize() * 0.5f;
    mHighlighted ? mText.Draw(renderer, posText, alpha) : mText.Draw(renderer, posText, 255);
}

void UIButton::OnClick()
{
    if (mOnClick != nullptr)
        mOnClick();
}
