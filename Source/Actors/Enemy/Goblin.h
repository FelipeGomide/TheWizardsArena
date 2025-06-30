#pragma once
#include "../Actor.h"
#include "States.h"

class Goblin : public Actor {
public:
    explicit Goblin(Game* game, std::vector<Vector2> patrolPath);
    
    void OnUpdate(float deltaTime) override;
    void OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, AABBColliderComponent* other) override;
    void Kill() override;

private:

    class RigidBodyComponent* mRigidBodyComponent;
    class DrawAnimatedComponent* mDrawComponent;
    class AABBColliderComponent* mColliderComponent;

    States mCurrentState = States::Patrol;
    std::vector<Vector2> mPatrolPath;
    int mDirectionPatrol = +1;
    int mCurPosPatrol = 0;

    const float mForwardSpeed = 1000.0f;

    const float mTilesRadiusPerception = 10;
    const float mMaxvelocity = 500;
    const float mJumpVelocity = 600;
    const float GRAVITY = 2000;

    float mStuckTimer = 0;
    const float mStuckInterval = 1;
    Vector2 mLastPosition;

    void HandleTransition(float);
    void PatrolAction(float);
    void PersueAction(float);
    void AttackAction(float);
    void Jump();
    bool CheckHole(float deltaTime);
};