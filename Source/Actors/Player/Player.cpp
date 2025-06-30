//
// Created by fgomide on 6/28/25.
//

#include "Player.h"
#include "../Block.h"
#include "../../Game.h"
#include "../../HUD.h"
#include "../../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../../Components/DrawComponents/DrawPolygonComponent.h"

Player::Player(Game *game, int healthPoints, float forwardSpeed, float jumpSpeed)
    : Actor(game)
    , mForwardSpeed(forwardSpeed)
    , mJumpSpeed(jumpSpeed)
    , mIsRunning(false)
    , mIsDying(false)
    , mIsAttacking(false)
    , mAttackTimer(0)
    , mAttackCooldown(0)
    , mAttackEffectTimer(0)
    , mDyingTimer(0)
    , mHpCounter(healthPoints)
    ,mAttackProjectile(nullptr)
{
    mRigidBodyComponent = nullptr;
    mColliderComponent = nullptr;
    mDrawComponent = nullptr;
    // mColliderComponent = new AABBColliderComponent(this, 32, 0, 32, 48, ColliderLayer::Player);
    //
    // mDrawComponent = new DrawAnimatedComponent(this,
    //     "../Assets/Sprites/Warrior/Warrior.png",
    //     "../Assets/Sprites/Warrior/Warrior.json");
    //
    // mDrawComponent->AddAnimation("Idle", {0});
    // mDrawComponent->AddAnimation("Walk", {0,1,2,3});
    // mDrawComponent->AddAnimation("Attack", {4,4,5,5,5,5,5,6,7,8});cd
    //
    // mDrawComponent->SetAnimation("Idle");
    // mDrawComponent->SetAnimFPS(10.0f);
}

// void Player::OnProcessInput(const uint8_t* state)
// {
//     if(mGame->GetGamePlayState() != Game::GamePlayState::Playing) return;
//
//     if (state[SDL_SCANCODE_D])
//     {
//         mRigidBodyComponent->ApplyForce(Vector2::UnitX * mForwardSpeed);
//         mRotation = 0.0f;
//         mIsRunning = true;
//     }
//
//     if (state[SDL_SCANCODE_A])
//     {
//         mRigidBodyComponent->ApplyForce(Vector2::UnitX * -mForwardSpeed);
//         mRotation = Math::Pi;
//         mIsRunning = true;
//     }
//
//     if (!state[SDL_SCANCODE_D] && !state[SDL_SCANCODE_A])
//     {
//         mIsRunning = false;
//
//         if (mIsOnGround)
//         {
//             auto vel = mRigidBodyComponent->GetVelocity();
//             mRigidBodyComponent->SetVelocity(Vector2(0, vel.y));
//         }
//     }
// }
//
// void Player::OnHandleKeyPress(const int key, const bool isPressed)
// {
//     if(mGame->GetGamePlayState() != Game::GamePlayState::Playing) return;
//
//     // Jump
//     if (key == SDLK_SPACE && isPressed && mIsOnGround)
//     {
//         mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mJumpSpeed));
//         mIsOnGround = false;
//
//         mGame->GetAudio()->PlaySound("Jump.wav");
//     }
//
//     if (key == SDLK_k) {
//         mIsAttacking = true;
//         mAttackTimer = 1.0f;
//         mAttackCooldown = 1.0f;
//     }
// }
//
// void Player::OnUpdate(float deltaTime)
// {
//     mAttackTimer -= deltaTime;
//     mAttackCooldown -= deltaTime;
//
//     if (mRigidBodyComponent && mRigidBodyComponent->GetVelocity().y != 0)
//     {
//         mIsOnGround = false;
//     }
//
//     mPosition.x = Math::Max(mPosition.x, mGame->GetCameraPos().x);
//
//     // Kill player if he falls below the screen
//     if (mGame->GetGamePlayState() == Game::GamePlayState::Playing && mPosition.y > mGame->GetWindowHeight()) {
//         Kill();
//     }
//
//     ManageAnimations();
// }
//
// void Player::ManageAnimations()
// {
//     if(mIsDying)
//     {
//         mDrawComponent->SetAnimation("Dead");
//     }
//     else if (mIsAttacking) {
//         mDrawComponent->SetAnimation("Attack");
//         mIsAttacking = false;
//     }
//     else if (mAttackTimer <= 0.0f) {
//         if (mIsOnGround && mIsRunning)
//         {
//             mDrawComponent->SetAnimation("Walk");
//         }
//         else
//         {
//             mDrawComponent->SetAnimation("Idle");
//         }
//     }
//
// }

void Player::Kill()
{
    mIsDying = true;
    //mDrawComponent->SetAnimation("Dead");

    // Disable collider and rigid body
    mRigidBodyComponent->SetEnabled(false);
    mColliderComponent->SetEnabled(false);

    mGame->GetAudio()->StopAllSounds();
    mGame->GetAudio()->PlaySound("Dead.wav");

    mGame->SetGameScene(Game::GameScene::GameOver, 3);
    // mGame->ResetGameScene(3.5f); // Reset the game scene after 3 seconds
}

void Player::TakeDamage() {
    mHpCounter -= 1;
    GetGame()->GetHUD()->SetHealth(mHpCounter);
    if (mHpCounter <= 0) {
        Kill();
    }

    mInvincibilityTimer = .5f;
}


// void Player::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
// {
//     if (other->GetLayer() == ColliderLayer::Enemy)
//     {
//         Kill();
//     }
//     else if (other->GetLayer() == ColliderLayer::Coin)
//     {
//         mGame->AddCoin();
//         SDL_Log("Got coin! %d", mGame->GetCoins());
//         mGame->GetAudio()->PlaySound("Coin.wav");
//         other->GetOwner()->SetPosition(Vector2(-100, -100));
//     }
// }
//
// void Player::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
// {
//     if (other->GetLayer() == ColliderLayer::Enemy)
//     {
//         other->GetOwner()->Kill();
//         mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mJumpSpeed / 2.5f));
//
//         mGame->GetAudio()->PlaySound("Stomp.wav");
//
//     }
//     else if (other->GetLayer() == ColliderLayer::Blocks)
//     {
//         if (!mIsOnGround)
//         {
//             mGame->GetAudio()->PlaySound("Bump.wav");
//
//             // Cast actor to Block to call OnBump
//             auto* block = dynamic_cast<Block*>(other->GetOwner());
//             block->OnBump();
//         }
//     }
//     else if (other->GetLayer() == ColliderLayer::Coin)
//     {
//         mGame->AddCoin();
//         SDL_Log("Got coin! %d", mGame->GetCoins());
//         mGame->GetAudio()->PlaySound("Coin.wav");
//         other->GetOwner()->SetPosition(Vector2(-100, -100));
//     }
// }