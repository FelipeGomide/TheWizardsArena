//
// Created by fgomide on 6/29/25.
//

#include "Projectile.h"
#include "Actor.h"
#include "../Random.h"
#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"

Projectile::Projectile(Game* game, Player* owner, ProjectileType type, const float deathTimer)
        :Actor(game)
        ,mDeathTimer(deathTimer)
        ,mType(type)
        ,mDrawComponent(nullptr)
        ,mRigidBodyComponent(nullptr)
        ,mColliderComponent(nullptr)
        ,mOwner(owner)
{

}

void Projectile::OnUpdate(float deltaTime)
{
    mDeathTimer -= deltaTime;
    if (mDeathTimer <= 0.0f) {
        SetState(ActorState::Destroy);
    }
}