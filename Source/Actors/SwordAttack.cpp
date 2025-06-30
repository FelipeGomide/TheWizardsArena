//
// Created by fgomide on 6/29/25.
//

#include "SwordAttack.h"

#include <charconv>

#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "Player/Player.h"
#include "../Game.h"
#include "../Random.h"

SwordAttack::SwordAttack(class Game *game, class Player* owner, float deathTimer, Vector2 offset)
    : Projectile(game, owner, ProjectileType::MeleeProjectile, deathTimer)
{
    //mRigidBodyComponent = new RigidBodyComponent(this, 1, 0, false);

    mColliderComponent = new AABBColliderComponent(this, offset.x + 5, offset.y, 50*SIZE_MULTIPLIER, 45*SIZE_MULTIPLIER, ColliderLayer::Attack, false,
                                                   false);
}

void SwordAttack::OnHorizontalCollision(const float minOverlap, AABBColliderComponent *other) {

}

void SwordAttack::OnVerticalCollision(const float minOverlap, AABBColliderComponent *other) {

}