#pragma once
#include "../Actor.h"
#include "States.h"

class Wizard : public Actor {
public:
    explicit Wizard(class Game* game, std::vector<Vector2> pointsTeleport, float teleportTimer=3.f);

    void OnUpdate(float deltaTime) override;
    void OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, AABBColliderComponent* other) override;
    void Kill() override;

private:
    class RigidBodyComponent* mRigidBodyComponent;
    class DrawAnimatedComponent* mDrawComponent;
    class AABBColliderComponent* mColliderComponent;

    States mCurrentState = States::Patrol;
    std::vector<Vector2> mPointsTeleportation;

    const float mForwardSpeed = 1000.0f;

    const float mTilesRadiusPerception = 10;
    const float mMaxvelocity = 500;
    const float mJumpVelocity = 600;
    const float GRAVITY = 2000;

    const float mTeleportTimer;

    void HandleTransition(float);
    void PatrolAction(float);
    void PersueAction(float);
    void AttackAction(float);
    void Jump();
};