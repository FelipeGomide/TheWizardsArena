#include "Explosion.h"
#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/ColliderComponents/AABBColliderComponent.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"

Explosion::Explosion(Game* game, Vector2 pos, float desappearTimer)
    : Actor(game)
    , mDesappearTimer(desappearTimer)
{
    mDrawComponent = new DrawAnimatedComponent(
        this,
        "../Assets/Sprites/Explosion/RedExplosion.png",
        "../Assets/Sprites/Explosion/RedExplosion.json"
    );

    mDrawComponent->AddAnimation("explode", {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11});
    mDrawComponent->SetAnimFPS(9);
    mDrawComponent->SetAnimation("explode");
    SetPosition(pos);

    mRigidBodyComponent = new RigidBodyComponent(this, 0, 0, false);
    mColliderComponent = new AABBColliderComponent(
        this, 0, 25*SIZE_MULTIPLIER, 50*SIZE_MULTIPLIER, 25*SIZE_MULTIPLIER, ColliderLayer::Explosion, false, false
    );
}


void Explosion::OnUpdate(float deltaTime){
    mDesappearTimer -= deltaTime;

    if(mDesappearTimer < 0){
        SetState(ActorState::Destroy);
        mDrawComponent->SetIsPaused(true);
    }
}

void Explosion::OnHorizontalCollision(float minOverlap, AABBColliderComponent* other){
    if (other->GetLayer() == ColliderLayer::Player){
        mColliderComponent->SetEnabled(false);
        mColliderComponent->SetLayer(ColliderLayer::None);
        other->GetOwner()->OnHorizontalCollision(-minOverlap, other);
        SetState(ActorState::Destroy);
    }

}

void Explosion::OnVerticalCollision(float minOverlap, AABBColliderComponent* other){
    if (other->GetLayer() == ColliderLayer::Player){
        mColliderComponent->SetEnabled(false);
        mColliderComponent->SetLayer(ColliderLayer::None);
        other->GetOwner()->OnVerticalCollision(-minOverlap, other);
        SetState(ActorState::Destroy);
    }
}
