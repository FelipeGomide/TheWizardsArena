// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "UIScreen.h"
#include "../Game.h"
#include "UIFont.h"

UIScreen::UIScreen(Game* game, const std::string& fontName)
	:mGame(game)
	,mPos(0.f, 0.f)
	,mSize(0.f, 0.f)
	,mState(UIState::Active)
    ,mSelectedButtonIndex(-1)
{
    mGame->PushUI(this);
    mFont = mGame->LoadFont(fontName);
}

UIScreen::~UIScreen()
{
    for (auto& text : mTexts)
        delete text;
    mTexts.clear();

    for (auto& button : mButtons)
        delete button;
    mButtons.clear();

    for (auto& image : mImages)
        delete image;
    mImages.clear();
}

void UIScreen::Update(float deltaTime)
{
	//float t = SDL_GetTicks() / 300.0f;
    mTimePassed += deltaTime * 5;
    float alphaFLoat = (sinf(mTimePassed) * 0.5f + 0.5f) * 255.0f;
    mAlpha = static_cast<Uint8>(alphaFLoat);
}

void UIScreen::Draw(SDL_Renderer *renderer)
{
    for (auto& text: mTexts)
        text->Draw(renderer, mPos, mAlpha);

    for (auto& button: mButtons) {
        button-> Draw(renderer, mPos, mAlpha);
    }

    for (auto& image : mImages)
        image->Draw(renderer, mPos);
}

void UIScreen::ProcessInput(const uint8_t* keys)
{

}

void UIScreen::HandleKeyPress(int key)
{
    if (mGame->GetGameScene() == Game::GameScene::MainMenu) {
        if (key == SDLK_a) {
            mButtons[mSelectedButtonIndex]->SetHighlighted(false);
            mSelectedButtonIndex--;
            mTimePassed = 0.f;
            if (mSelectedButtonIndex < 0)
                mSelectedButtonIndex = mButtons.size() - 1;
            mButtons[mSelectedButtonIndex]->SetHighlighted(true);
        }

        if (key == SDLK_d) {
            mButtons[mSelectedButtonIndex]->SetHighlighted(false);
            mSelectedButtonIndex++;
            mTimePassed = 0.f;
            if (mSelectedButtonIndex >= mButtons.size())
                mSelectedButtonIndex = 0;
            mButtons[mSelectedButtonIndex]->SetHighlighted(true);
        }

        if (key == SDLK_RETURN) {
            if (mSelectedButtonIndex >= 0 && mSelectedButtonIndex < mButtons.size())
                mButtons[mSelectedButtonIndex]->OnClick();
        }
    }

    else if(mGame->GetGameScene() == Game::GameScene::GameOver){
        mGame->SetGameScene(Game::GameScene::MainMenu, 3.0f);
    }
    else if (mGame->GetGameScene() == Game::GameScene::HowToPlay){
        SDL_Log("How to play");
        mGame->SetGameScene(Game::GameScene::MainMenu, 3.0f);
    }
    else if (mGame->GetGameScene() == Game::GameScene::Credits){
        mGame->SetGameScene(Game::GameScene::MainMenu, 3.0f);
    }
    else SDL_Log("Default screen to go not setted!");
}

void UIScreen::Close()
{
	mState = UIState::Closing;
}

UIText* UIScreen::AddText(const std::string &name, const Vector2 &pos, const Vector2 &dims, const int pointSize, bool fade, const int unsigned wrapLength, const Vector3 &color)
{
    UIText* t = new UIText(name, mFont, pointSize, wrapLength, pos, dims, color, fade);
    mTexts.push_back(t);
    return t;
}

UIButton* UIScreen::AddButton(const std::string& name, const Vector2 &pos, const Vector2& dims, std::function<void()> onClick)
{
    auto* button = new UIButton(name, mFont, onClick, pos, dims,Vector3(200, 100, 0), 16, dims.x, Vector2::Zero, dims);
    mButtons.push_back(button);

    if (mButtons.size() == 1)
        mSelectedButtonIndex = 0, button->SetHighlighted(true);

    return button;
}

UIImage* UIScreen::AddImage(const std::string &imagePath, const Vector2 &pos, const Vector2 &dims, const Vector3 &color)
{
    UIImage* image = new UIImage(imagePath, pos, dims, color, mGame->GetRenderer());
    mImages.push_back(image);
    return image;
}