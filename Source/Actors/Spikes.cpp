//
// Created by joaov on 29/06/2025.
//

#include "Spikes.h"
#include "../Game.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Components/ColliderComponents/AABBColliderComponent.h"

Spikes::Spikes(Game* game)
        :Actor(game)
{
    new DrawSpriteComponent(this, "../Assets/Sprites/Blocks/Spikes.png", Game::TILE_SIZE, Game::TILE_SIZE, 10);
    mColliderComponent = new AABBColliderComponent(this, 0, 0, Game::TILE_SIZE, Game::TILE_SIZE, ColliderLayer::Spikes, true);
    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 0.0f, false);
}

void Spikes::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Player)
        other->GetOwner()->OnVerticalCollision(minOverlap, other);
}
