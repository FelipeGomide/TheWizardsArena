//
// Created by Lucas N. Ferreira on 08/12/23.
//

#include "HUD.h"
#include "Game.h"
#include "UIElements/UIText.h"

HUD::HUD(class Game* game, const std::string& fontName)
    :UIScreen(game, fontName)
{
    int leftX = WORD_OFFSET * 2;  // Left margin
    int centerX = mGame->GetWindowWidth() / 2 - CHAR_WIDTH * 2;
    int rightX = mGame->GetWindowWidth() - 5*CHAR_WIDTH;  // Right margin

    // AddText("TIME", Vector2(rightX, HUD_POS_Y), Vector2(CHAR_WIDTH * 4, WORD_HEIGHT), POINT_SIZE);
    // mTimeText = AddText("400", Vector2(rightX, HUD_POS_Y + WORD_HEIGHT), Vector2(CHAR_WIDTH * 3, WORD_HEIGHT), POINT_SIZE);
    //
    // AddText("WORLD", Vector2(centerX, HUD_POS_Y), Vector2(CHAR_WIDTH * 5, WORD_HEIGHT), POINT_SIZE);
    // mLevelName = AddText("1-1", Vector2(centerX, HUD_POS_Y + WORD_HEIGHT), Vector2(CHAR_WIDTH * 3, WORD_HEIGHT), POINT_SIZE);
    // //AddText("MARIO", Vector2(leftX, HUD_POS_Y), Vector2(CHAR_WIDTH * 5, WORD_HEIGHT), POINT_SIZE);
    // mScoreCounter = AddText("000000", Vector2(leftX, HUD_POS_Y + WORD_HEIGHT), Vector2(CHAR_WIDTH * 6, WORD_HEIGHT), POINT_SIZE);

    mHealthCount = AddText("♡♡♡♡♡♡♡♡♡♡", Vector2(leftX, HUD_POS_Y), Vector2(CHAR_WIDTH * 10, WORD_HEIGHT), POINT_SIZE);
}

HUD::~HUD()
{

}

void HUD::SetHealth(int health) const
{
    std::string text = "";

    for (int i = 0; i < health; i++) {
        text += "♡";
    }
    mHealthCount->SetText(text);
    mHealthCount->SetSize(Vector2( CHAR_WIDTH * health, WORD_HEIGHT));
}

// void HUD::SetTime(int time)
// {
//     mTimeText->SetText(std::to_string(time));
//
//     // mTimeText->SetPosition()
// }
//
// void HUD::SetLevelName(const std::string &levelName)
// {
//     mLevelName->SetText(levelName);
// }