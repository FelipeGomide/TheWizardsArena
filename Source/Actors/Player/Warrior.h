//
// Created by fgomide on 6/26/25.
//

#ifndef WARRIOR_H
#define WARRIOR_H
#include "Player.h"
#include "../Projectile.h"
#include "../../Components/DrawComponents/DrawAnimatedComponent.h"

class Warrior : public Player
{
public:
    static constexpr int WARRIOR_HP = 10;

    explicit Warrior(Game* game, float forwardSpeed = 1500.0f, float jumpSpeed = -1500.0f);

    void OnProcessInput(const Uint8* keyState) override;
    void OnUpdate(float deltaTime) override;
    void OnHandleKeyPress(const int key, const bool isPressed) override;

    void OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, AABBColliderComponent* other) override;

    void Kill() override;
    void Attack() override;

private:
    void ManageAnimations() override;
};



#endif //WARRIOR_H
