#pragma once 
#include "Actor.h"

enum PuffType{
    Smoke,
    PreExplosion,
    ExplosionRed
};

class Puff : Actor {

public: 
    explicit Puff(class Game* game, class Vector2 pos, float desappearTimer = 0.5f, PuffType type = PuffType::Smoke); 
    void OnUpdate(float deltaTime) override;

private:
    float mDesappearTimer;
    class DrawAnimatedComponent* mDrawComponent;
};