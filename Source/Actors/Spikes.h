//
// Created by joaov on 29/06/2025.
//

#pragma once

#include "Actor.h"
#include <string>

class Spikes : public Actor
{
public:
    explicit Spikes(Game* game);
    void OnVerticalCollision(const float minOverlap, AABBColliderComponent* other);

    float GetCollisionVelocity() {return mCollisionVelocity;}

private:
    class AABBColliderComponent* mColliderComponent;
    class RigidBodyComponent* mRigidBodyComponent;
    float mCollisionVelocity = -4000.0f;
};
