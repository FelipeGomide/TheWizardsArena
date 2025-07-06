//
// Created by Lucas N. Ferreira on 08/12/23.
//

#include "HUD.h"
#include "Game.h"
#include "UIElements/UIScreen.h"
#include "UIElements/UIText.h"

HUD::HUD(class Game* game, const std::string& fontName)
    :UIScreen(game, fontName)
{
    mHealth = new UIImage*[10];
    mLost = new UIImage*[10];

    for (int i = 0; i < 10; i++) {
        mHealth[i] = AddImage("../Assets/Sprites/Hearth_filled.png", Vector2((CHAR_WIDTH) + CHAR_WIDTH * i, WORD_OFFSET), Vector2(CHAR_WIDTH, WORD_HEIGHT));
    }

    for (int i = 0; i < 10; i++) {
        mLost[i] = AddImage("../Assets/Sprites/Hearth_lost.png", Vector2(-500, WORD_OFFSET), Vector2(CHAR_WIDTH, WORD_HEIGHT));
    }
}

HUD::~HUD()
{

}

void HUD::SetHealth(int health) const
{
    for (int i = health; i < 10; i++) {
        mHealth[i]->SetPosition(Vector2(-500, -500));
        mLost[i]->SetPosition(Vector2((CHAR_WIDTH) + CHAR_WIDTH * i, WORD_OFFSET));
    }
}