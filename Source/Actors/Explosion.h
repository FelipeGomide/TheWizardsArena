#pragma once
#include "Actor.h"

class Explosion : public Actor
{
public:
    explicit Explosion(class Game* game, class Vector2 pos, float desappearTimer = 2.0f);
    void OnUpdate(float deltaTime) override;

    void OnHorizontalCollision(float minOverlap, AABBColliderComponent* other) override;
    void OnVerticalCollision(float minOverlap, AABBColliderComponent* other) override;

protected:
    float mDesappearTimer;

    class DrawAnimatedComponent* mDrawComponent;
    class RigidBodyComponent* mRigidBodyComponent;
    class AABBColliderComponent* mColliderComponent;
};