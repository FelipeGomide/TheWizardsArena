#pragma once 
#include "Actor.h"

class Puff : Actor {

public: 
    explicit Puff(class Game* game, class Vector2 pos, float desappearTimer = 0.5f); 
    void OnUpdate(float deltaTime) override;

private:
    float mDesappearTimer;
    class DrawAnimatedComponent* mDrawComponent;
};