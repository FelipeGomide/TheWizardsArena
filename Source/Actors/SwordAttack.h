//
// Created by fgomide on 6/29/25.
//

#ifndef SWORD_ATTACK_H
#define SWORD_ATTACK_H
#include "Projectile.h"


class SwordAttack : public Projectile{
public:
    explicit SwordAttack(class Game* game, class Player* owner, float deathTimer, Vector2 position);

    void OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, AABBColliderComponent* other) override;
};



#endif //SWORD_ATTACK_H
