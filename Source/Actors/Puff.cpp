#include "Puff.h"
#include "../Game.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"

Puff::Puff(Game* game, Vector2 pos, float desappearTimer, PuffType type)
    : Actor(game)
    , mDesappearTimer(desappearTimer)
{
    std::string spriteToLoad;
    if(type == PuffType::Smoke){
        mDrawComponent = new DrawAnimatedComponent(
            this,
            "../Assets/Sprites/Explosion/MagicExplosion.png",
            "../Assets/Sprites/Explosion/MagicExplosion.json"
        );

        mDrawComponent->AddAnimation("explode", {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11});
        mDrawComponent->SetAnimFPS(9);
        mDrawComponent->SetAnimation("explode");
    }
    else if(type == PuffType::PreExplosion){
        spriteToLoad = "MagicExplosion";

        mDrawComponent = new DrawAnimatedComponent(
            this,
            "../Assets/Sprites/Explosion/PreExplosion.png",
            "../Assets/Sprites/Explosion/PreExplosion.json"
        );

        mDrawComponent->AddAnimation("explode", {0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
        mDrawComponent->SetAnimFPS(9);
        mDrawComponent->SetAnimation("explode");
    }

    SetPosition(pos);
}


void Puff::OnUpdate(float deltaTime){
    mDesappearTimer -= deltaTime;

    if(mDesappearTimer < 0){
        SetState(ActorState::Destroy);
        mDrawComponent->SetIsPaused(true);
    }
}