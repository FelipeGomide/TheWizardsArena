//
// Created by fgomide on 6/26/25.
//

#ifndef PLAYER_H
#define PLAYER_H
#include "../Actor.h"
#include "../Projectile.h"
#include "../../Components/DrawComponents/DrawAnimatedComponent.h"


class Player : public Actor
{
public:
    explicit Player(Game* game, int healthPoints, float forwardSpeed = 1500.0f, float jumpSpeed = -750.0f);

    void OnProcessInput(const Uint8* keyState) override = 0;
    void OnUpdate(float deltaTime) override = 0;
    void OnHandleKeyPress(int key, bool isPressed) override = 0;

    void OnHorizontalCollision(float minOverlap, AABBColliderComponent* other) override = 0;
    void OnVerticalCollision(float minOverlap, AABBColliderComponent* other) override = 0;

    void Kill() override = 0;
    void TakeDamage();

    int GetHealth() {return mHpCounter;};

protected:
    virtual void ManageAnimations() = 0;

    virtual void Attack() = 0;

    // Speeds
    float mForwardSpeed;
    float mJumpSpeed;

    // Bools for animations
    bool mIsRunning;
    bool mIsDying;
    bool mIsAttacking;

    // Timers
    float mAttackTimer;
    float mAttackCooldown;
    float mAttackEffectTimer;
    float mDyingTimer;
    float mInvincibilityTimer;

    // Counters
    int mHpCounter;

    // Basic components
    class RigidBodyComponent* mRigidBodyComponent;
    class DrawAnimatedComponent* mDrawComponent;
    class AABBColliderComponent* mColliderComponent;

    // Attack
    Projectile* mAttackProjectile;

    float mSpikesCollisionVelocity = -400000.0f;
};

#endif //PLAYER_H

