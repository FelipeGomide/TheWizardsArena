//
// Created by fgomide on 6/29/25.
//

#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "Actor.h"

enum class ProjectileType {
    MeleeProjectile,
    RangedProjectile,
    DroppedArrowProjectile,
};

class Projectile : public Actor
{
public:
    explicit Projectile(class Game* game, class Player* owner, ProjectileType type = ProjectileType::RangedProjectile, float deathTimer = 1.0f);

    void OnUpdate(float deltaTime) override;

    void OnHorizontalCollision(float minOverlap, AABBColliderComponent* other) override = 0;
    void OnVerticalCollision(float minOverlap, AABBColliderComponent* other) override = 0;

protected:
    float mDeathTimer;

    ProjectileType mType;

    class DrawAnimatedComponent* mDrawComponent;
    class RigidBodyComponent* mRigidBodyComponent;
    class AABBColliderComponent* mColliderComponent;

    class Player* mOwner;
};

#endif //PROJECTILE_H
