#pragma once
#include "../Actor.h"
#include "States.h"
class Ghost : public Actor {
public:
    explicit Ghost(Game* game, std::vector<Vector2> patrolPath);
    
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

    float mTilesRadiusPerception = 10;
    float mTimerPersueSound;
    float mMaxvelocity = 70;

    void HandleTransition();
    void PatrolAction();
    void PersueAction();
    void AttackAction();

};