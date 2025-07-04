//
// Created by Lucas N. Ferreira on 28/09/23.
//

#pragma once
#include "../Component.h"
#include "../../Math.h"
#include "../RigidBodyComponent.h"
#include <vector>
#include <map>
#include <set>

enum class ColliderLayer
{
    Player,
    Goblin,
    Blocks,
    Ghost,
    Attack,
    Enemy,
    Spikes
};

class AABBColliderComponent : public Component
{
public:
    // Collider ignore map
    const std::map<ColliderLayer, const std::set<ColliderLayer>> ColliderIgnoreMap = {
        {ColliderLayer::Player, {}},
        {ColliderLayer::Enemy,  {}},
        {ColliderLayer::Goblin,  {}},
        {ColliderLayer::Blocks, {ColliderLayer::Blocks}},
        {ColliderLayer::Ghost, {ColliderLayer::Blocks, ColliderLayer::Goblin, ColliderLayer::Ghost}},
        {ColliderLayer::Attack, {ColliderLayer::Blocks, ColliderLayer::Player}}
    };

    AABBColliderComponent(class Actor* owner, int dx, int dy, int w, int h,
                                ColliderLayer layer, bool isStatic = false, bool isRigid = true, int updateOrder = 10);
    ~AABBColliderComponent() override;

    bool Intersect(const AABBColliderComponent& b) const;

    float DetectHorizontalCollision(RigidBodyComponent *rigidBody);
    float DetectVertialCollision(RigidBodyComponent *rigidBody);

    void SetStatic(bool isStatic) { mIsStatic = isStatic; }

    Vector2 GetMin() const;
    Vector2 GetMax() const;
    Vector2 GetCenter() const;
    ColliderLayer GetLayer() const { return mLayer; }

protected:
    float GetMinVerticalOverlap(AABBColliderComponent* b) const;
    float GetMinHorizontalOverlap(AABBColliderComponent* b) const;

    void ResolveHorizontalCollisions(RigidBodyComponent *rigidBody, const float minOverlap);
    void ResolveVerticalCollisions(RigidBodyComponent *rigidBody, const float minOverlap);

    Vector2 mOffset;
    int mWidth;
    int mHeight;
    bool mIsStatic;
    bool mIsRigid;

    ColliderLayer mLayer;
};