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
    Spikes,
    Platform,
    Wizard,
    Explosion,
    None
};

class AABBColliderComponent : public Component
{
public:
    // Collider ignore map
    const std::map<ColliderLayer, const std::set<ColliderLayer>> ColliderIgnoreMap = {
        {ColliderLayer::Player, {ColliderLayer::None}},
        {ColliderLayer::Enemy,  {ColliderLayer::None}},
        {ColliderLayer::Goblin,  {ColliderLayer::None}},
        {ColliderLayer::Wizard, {ColliderLayer::None}},
        {ColliderLayer::Blocks, {ColliderLayer::None, ColliderLayer::Blocks}},
        {ColliderLayer::Ghost, {ColliderLayer::None, ColliderLayer::Blocks, ColliderLayer::Goblin, ColliderLayer::Ghost, ColliderLayer::Platform, ColliderLayer::Explosion}},
        {ColliderLayer::Attack, {ColliderLayer::None, ColliderLayer::Blocks, ColliderLayer::Player}},
        {ColliderLayer::Platform, {ColliderLayer::None, ColliderLayer::Blocks, ColliderLayer::Platform, ColliderLayer::Ghost}},
        {ColliderLayer::Explosion, {ColliderLayer::None, ColliderLayer::Blocks, ColliderLayer::Explosion, ColliderLayer::Ghost, ColliderLayer::Wizard, ColliderLayer::Goblin, ColliderLayer::Platform}},
        {ColliderLayer::None, {
            ColliderLayer::Player,
            ColliderLayer::Goblin,
            ColliderLayer::Blocks,
            ColliderLayer::Ghost,
            ColliderLayer::Attack,
            ColliderLayer::Enemy,
            ColliderLayer::Spikes,
            ColliderLayer::Platform,
            ColliderLayer::Wizard,
            ColliderLayer::Explosion,
            ColliderLayer::None,
        }}
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
    void SetLayer(ColliderLayer layer) { mLayer = layer; }

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