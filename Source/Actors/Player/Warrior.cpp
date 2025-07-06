//
// Created by fgomide on 6/26/25.
//

#include "Warrior.h"
#include "Player.h"
#include "../Block.h"
#include "../Spikes.h"
#include "../SwordAttack.h"
#include "../../Game.h"
#include "../../Random.h"
#include "../../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../../Game.h"

Warrior::Warrior(Game *game, float forwardSpeed, float jumpSpeed)
    : Player(game, WARRIOR_HP, forwardSpeed, jumpSpeed)
{
    mRigidBodyComponent = new RigidBodyComponent(this, .5f, 5.0f);

    mColliderComponent = new AABBColliderComponent(this, 32*SIZE_MULTIPLIER, 0, 32*SIZE_MULTIPLIER, 48*SIZE_MULTIPLIER, ColliderLayer::Player);

    mDrawComponent = new DrawAnimatedComponent(this,
        "../Assets/Sprites/Warrior/Warrior.png",
        "../Assets/Sprites/Warrior/Warrior.json");

    mDrawComponent->AddAnimation("Idle", {0});
    mDrawComponent->AddAnimation("Walk", {0,1,2,3});
    mDrawComponent->AddAnimation("Attack", {4,4,5,5,5,5,5,6,7,8});

    mDrawComponent->SetAnimation("Idle");
    mDrawComponent->SetAnimFPS(10.0f);

    // mAttackProjectile = new SwordAttack(mGame, 1.0f);
    // mAttackProjectile->SetState(ActorState::Paused);
}

void Warrior::OnProcessInput(const uint8_t* state)
{
    if(mGame->GetGamePlayState() != Game::GamePlayState::Playing) return;

    if (state[SDL_SCANCODE_D])
    {
        mRigidBodyComponent->ApplyForce(Vector2::UnitX * mForwardSpeed);
        mRotation = 0.0f;
        mIsRunning = true;
    }

    if (state[SDL_SCANCODE_A])
    {
        mRigidBodyComponent->ApplyForce(Vector2::UnitX * -mForwardSpeed);
        mRotation = Math::Pi;
        mIsRunning = true;
    }

    if (!state[SDL_SCANCODE_D] && !state[SDL_SCANCODE_A])
    {
        mIsRunning = false;

        if (mIsOnGround)
        {
            auto vel = mRigidBodyComponent->GetVelocity();
            mRigidBodyComponent->SetVelocity(Vector2(0, vel.y));
        }
    }
}

void Warrior::OnHandleKeyPress(const int key, const bool isPressed)
{
    if(mGame->GetGamePlayState() != Game::GamePlayState::Playing) return;

    // Jump
    if (key == SDLK_SPACE && isPressed && mIsOnGround)
    {
        mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mJumpSpeed));
        mIsOnGround = false;

        mGame->GetAudio()->PlaySound("Player/jump4.ogg");
    }

    if (key == SDLK_k) {
        if (mIsAttacking == false) {
            mIsAttacking = true;
            mAttackTimer = .8f;
            mAttackCooldown = 1.0f;
            Attack();
        }
    }
}

void Warrior::OnUpdate(float deltaTime)
{
    //SDL_Log("Warrior HP %d", mHpCounter);
    mAttackTimer -= deltaTime;
    mAttackCooldown -= deltaTime;
    mAttackEffectTimer -= deltaTime;
    mInvincibilityTimer -= deltaTime;

    if (mAttackTimer <= 0.0f) {
        mIsAttacking = false;
    }

    if (mAttackEffectTimer > 0.0f) {
        mAttackProjectile->SetRotation(mRotation);
        if (mRotation != Math::Pi) {
            mAttackProjectile->SetPosition(GetPosition() + Vector2(100, 0));
        } else {
            //auto xOffset = (mColliderComponent->GetMax() - mColliderComponent->GetMin()).x;
            mAttackProjectile->SetPosition(GetPosition() - Vector2(30, 0));
        }
    }

    if (mRigidBodyComponent && mRigidBodyComponent->GetVelocity().y != 0)
    {
        mIsOnGround = false;
    }

    mPosition.x = Math::Max(mPosition.x, mGame->GetCameraPos().x);

    // Kill player if he falls below the screen
    if (mGame->GetGamePlayState() == Game::GamePlayState::Playing && mPosition.y > mGame->GetWindowHeight() + mGame->GetCameraPos().y) {
        Kill();
    }

    ManageAnimations();
}

void Warrior::ManageAnimations()
{
    if(mIsDying)
    {
        // mDrawComponent->SetAnimation("Dead");
        SDL_Log("Dying animation\n");
    }
    else if (mIsAttacking) {
        mDrawComponent->SetAnimation("Attack");
    }
    else if (mAttackTimer <= 0.0f) {
        if (mIsOnGround && mIsRunning)
        {
            mDrawComponent->SetAnimation("Walk");
        }
        else
        {
            mDrawComponent->SetAnimation("Idle");
        }
    }

}

void Warrior::Attack() {
    mAttackEffectTimer = .4f;
    auto xOffset = (mColliderComponent->GetMax() - mColliderComponent->GetMin()).x;

    mAttackProjectile = new SwordAttack(mGame, this, mAttackEffectTimer, Vector2(2*xOffset, 0));

    auto rand = Random::GetIntRange(1, 4);
    GetGame()->GetAudio()->PlaySound("Player/hit" + std::to_string(rand) + ".ogg");
}


void Warrior::Kill()
{
    mIsDying = true;
    //mDrawComponent->SetAnimation("Dead");

    // Disable collider and rigid body
    mRigidBodyComponent->SetEnabled(false);
    mColliderComponent->SetEnabled(false);

    mGame->GetAudio()->StopAllSounds();
    mGame->GetAudio()->PlaySound("Player/lose2.ogg");

    mGame->SetGameScene(Game::GameScene::GameOver, 3);
    // mGame->ResetGameScene(3.5f); // Reset the game scene after 3 seconds
}

void Warrior::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Goblin or other->GetLayer() == ColliderLayer::Ghost)
    {
        if (mInvincibilityTimer <= 0)
        {
            TakeDamage();
            if (minOverlap > 0.0f) {
                mRigidBodyComponent->SetVelocity(Vector2(-600, -100));
            }
            else {
                mRigidBodyComponent->SetVelocity(Vector2(600, -100));
            }
        }
    }
}

void Warrior::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Goblin || other->GetLayer() == ColliderLayer::Ghost)
    {
        if (minOverlap > 0)
        {
            other->GetOwner()->Kill();
            mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mJumpSpeed / 2.5f));
            mGame->GetAudio()->PlaySound("Player/hurt2.ogg");
        }
        else
        {
            if (mInvincibilityTimer <= 0)
            {
                TakeDamage();
                if (minOverlap > 0.0f) {
                    mRigidBodyComponent->SetVelocity(Vector2(-3000, mJumpSpeed));
                }
                else {
                    mRigidBodyComponent->SetVelocity(Vector2(+3000, mJumpSpeed));
                }
            }
        }
    }
    else if (other->GetLayer() == ColliderLayer::Spikes) {
        TakeDamage();
        mRigidBodyComponent->SpikeCollide();
        mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mSpikesCollisionVelocity));
    }
}