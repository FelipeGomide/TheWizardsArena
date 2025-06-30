//
// Created by Lucas N. Ferreira on 28/09/23.
//

#include "DrawAnimatedComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "../../Json.h"
#include <fstream>

DrawAnimatedComponent::DrawAnimatedComponent(class Actor* owner, const std::string &spriteSheetPath, const std::string &spriteSheetData, int drawOrder)
        :DrawSpriteComponent(owner, spriteSheetPath, 0, 0, drawOrder)
{
    LoadSpriteSheet(spriteSheetPath, spriteSheetData);
}

DrawAnimatedComponent::~DrawAnimatedComponent()
{
    DrawSpriteComponent::~DrawSpriteComponent();

    for (const auto& rect : mSpriteSheetData)
    {
        delete rect;
    }
    mSpriteSheetData.clear();
}

void DrawAnimatedComponent::LoadSpriteSheet(const std::string& texturePath, const std::string& dataPath)
{
    // Load sprite sheet texture
    mSpriteSheetSurface = mOwner->GetGame()->LoadTexture(texturePath);

    // Load sprite sheet data
    std::ifstream spriteSheetFile(dataPath);
    nlohmann::json spriteSheetData = nlohmann::json::parse(spriteSheetFile);

    SDL_Rect* rect = nullptr;
    for(const auto& frame : spriteSheetData["frames"]) {

        int x = frame["frame"]["x"].get<int>();
        int y = frame["frame"]["y"].get<int>();
        int w = frame["frame"]["w"].get<int>();
        int h = frame["frame"]["h"].get<int>();
        rect = new SDL_Rect({x, y, w, h});

        mSpriteSheetData.emplace_back(rect);
    }
}

void DrawAnimatedComponent::Draw(SDL_Renderer* renderer, const Vector3 &modColor)
{
    int spriteIdx = mAnimations[mAnimName][static_cast<int>(mAnimTimer)];
    SDL_Rect* srcRect = mSpriteSheetData[spriteIdx];

    SDL_Rect dstRect = {
            static_cast<int>(mOwner->GetPosition().x - mOwner->GetGame()->GetCameraPos().x),
            static_cast<int>(mOwner->GetPosition().y - mOwner->GetGame()->GetCameraPos().y),
            (int) (srcRect->w*SIZE_MULTIPLIER),
            (int) (srcRect->h*SIZE_MULTIPLIER),
    };

    SDL_RendererFlip flip = SDL_FLIP_NONE;
    auto rotation = mOwner->GetRotation();
    if (mOwner->GetRotation() == Math::Pi) {
        flip = SDL_FLIP_HORIZONTAL;
        rotation = 0;
    }

    SDL_SetTextureBlendMode(mSpriteSheetSurface, SDL_BLENDMODE_BLEND);
    SDL_SetTextureColorMod(mSpriteSheetSurface,
                           static_cast<Uint8>(modColor.x),
                           static_cast<Uint8>(modColor.y),
                           static_cast<Uint8>(modColor.z));

    SDL_RenderCopyEx(renderer, mSpriteSheetSurface, srcRect, &dstRect, rotation, nullptr, flip);
}

void DrawAnimatedComponent::Update(float deltaTime)
{
    if (mIsPaused) {
        return;
    }
    if(mAnimations.count(mAnimName) == 0){
        SDL_Log("Animation not found! Shutting down!");
        exit(1);
    }

    mAnimTimer += mAnimFPS * deltaTime;
    if (mAnimTimer >= mAnimations[mAnimName].size()) {
        while (mAnimTimer >= mAnimations[mAnimName].size()) {
            mAnimTimer -= mAnimations[mAnimName].size();
        }
    }
}

void DrawAnimatedComponent::SetAnimation(const std::string& name)
{
    if (name != mAnimName) {
        mAnimTimer = 0;
    }

    mAnimName = name;
    Update(0.0f);
}

void DrawAnimatedComponent::AddAnimation(const std::string& name, const std::vector<int>& spriteNums)
{
    mAnimations.emplace(name, spriteNums);
}

