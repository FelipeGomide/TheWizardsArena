#include "Wizard.h"
#include "../Puff.h"
#include "../Player/Player.h"
#include "../../Components/RigidBodyComponent.h"
#include "../../Components/ColliderComponents/AABBColliderComponent.h"
#include "../../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../../Constants.h"
#include "../../Game.h"

Wizard::Wizard(Game* game, std::vector<Vector2> pointsTeleport, float teleportTimer)
: Actor(game)
, mPointsTeleportation(pointsTeleport)
, mCurrentState(States::Patrol)
, mTeleportTimer(teleportTimer){


    mRigidBodyComponent = new RigidBodyComponent(this);
    mColliderComponent  = new AABBColliderComponent(
        this,
        0, 0, 48*SIZE_MULTIPLIER, 48*SIZE_MULTIPLIER, ColliderLayer::Wizard
    );

    mDrawComponent = new DrawAnimatedComponent(
        this,
        "../Assets/Sprites/Wizard/Wizard.png",
        "../Assets/Sprites/Wizard/Wizard.json"
    );

    mDrawComponent->AddAnimation("walk", {0, 1, 2, 3});
    mDrawComponent->SetAnimation("walk");
    mDrawComponent->SetAnimFPS(7.0f);
}

void Wizard::HandleTransition(float deltaTime){
    if(!mGame->GetPlayer()){
        mCurrentState = States::Patrol;
        return;
    }

    auto playerPosition = mGame->GetPlayer()->GetPosition();
    auto dist = (playerPosition - GetPosition()).Length();
    dist /= Game::TILE_SIZE;

    // auto resetPatrol = [&](){
    //     mPatrolPath = {GetPosition() + Vector2(-30, 0), GetPosition() + Vector2(30, 0)};
    //     mCurPosPatrol = 0;
    // };

    switch (mCurrentState) {
        case States::Patrol:
            if (dist <= mTilesRadiusPerception) {
                mCurrentState = (dist > 1.5f) ? States::Persue : States::Attack;
                mGame->GetAudio()->PlaySound("monster/piggrunt1.wav");
            }
            break;
        case States::Persue:
            if (dist > mTilesRadiusPerception) {
                mCurrentState = States::Patrol;
                // resetPatrol();
            }
            else if (dist <= 1.5f) 
                mCurrentState = States::Attack;
            break;
        case States::Attack:
            if (dist > 1.5f) 
                mCurrentState = States::Persue;
            else if (dist > mTilesRadiusPerception){
                mCurrentState = States::Patrol;
                // resetPatrol();
            } 
            break;
        default: break;
    }
}

void Wizard::PatrolAction(float deltaTime){

}

void Wizard::PersueAction(float deltaTime){

}

void Wizard::AttackAction(float deltaTime){

}

void Wizard::Jump(){
    if(not mIsOnGround) return;
    auto vel = mRigidBodyComponent->GetVelocity();
    mRigidBodyComponent->SetVelocity(Vector2(vel.x, -mJumpVelocity));
    mIsOnGround = false;
}

void Wizard::OnUpdate(float deltaTime) {
 
    if(mRigidBodyComponent and mRigidBodyComponent->GetVelocity().y != 0){
        mIsOnGround = false;
    }

    HandleTransition(deltaTime);

    switch (mCurrentState)
    {
    case States::Patrol:
        PatrolAction(deltaTime);    
        break;
    
    case States::Persue:
        PersueAction(deltaTime);    
        break;
    
    case States::Attack:
        AttackAction(deltaTime);    
        break;
    
    default:
        break;
    }
    
    auto vel = mRigidBodyComponent->GetVelocity();
    if(vel.x <= 0) SetRotation(Math::Pi);
    else SetRotation(0);
}


void Wizard::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other){
    if(other->GetLayer() == ColliderLayer::Blocks){
        Jump();
    }

    if(other->GetLayer() == ColliderLayer::Player){
        mGame->GetPlayer()->OnHorizontalCollision(-minOverlap, mColliderComponent);
    }

    if (other->GetLayer() == ColliderLayer::Attack) {
        Kill();
    }
}


void Wizard::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other){
    if(minOverlap > 0 and other->GetLayer() == ColliderLayer::Blocks) mIsOnGround = true;
    
    if(other->GetLayer() == ColliderLayer::Player){
        if(minOverlap < 0){
            mGame->GetPlayer()->OnVerticalCollision(-minOverlap, mColliderComponent);
        }
    }

    if (other->GetLayer() == ColliderLayer::Attack) {
        Kill();
    }
}

void Wizard::Kill(){
    if (mState != ActorState::Destroy) {
        mState = ActorState::Destroy;
        int number = rand() % 6;
        std::string sound = "monster/pain"+std::to_string(number)+".wav";
        mGame->GetAudio()->PlaySound(sound);
        mGame->DecreaseEnemiesAlive();
        new Puff(mGame, GetPosition());
    }
}


