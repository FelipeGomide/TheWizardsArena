#include "../Enemy/Ghost.h"
#include "../../Game.h"
#include "../Player/Player.h"
#include "../../Components/DrawComponents/DrawPolygonComponent.h"
#include "../../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../../Components/AI/SteeringBehavior.h"
#include "../../Constants.h"

#include <iostream>

Ghost::Ghost(Game* game, std::vector<Vector2> patrolPath) :
    Actor(game), mPatrolPath(patrolPath), mTimerPersueSound(3) {

    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 0, false, 10, mMaxvelocity);
    mColliderComponent = new AABBColliderComponent(this, 0, 0, 32*SIZE_MULTIPLIER, 40*SIZE_MULTIPLIER, ColliderLayer::Ghost);

    mDrawComponent = new DrawAnimatedComponent(
        this, 
        "../Assets/Sprites/Ghost/Ghost_normal_angry.png",
        "../Assets/Sprites/Ghost/Ghost_normal_angry.json"
    );

    mDrawComponent->AddAnimation("walk", {0, 1, 2, 3});
    mDrawComponent->AddAnimation("attack", {4, 5, 6, 7});
    mDrawComponent->SetAnimation("walk");
    mDrawComponent->SetAnimFPS(5.0f);
}

void Ghost::HandleTransition(){
    if(!mGame->GetPlayer()){
        mCurrentState = States::Patrol;
        return;
    }

    auto playerPosition = mGame->GetPlayer()->GetPosition();
    auto dist = (playerPosition - GetPosition()).Length();
    dist /= Game::TILE_SIZE;

    if(dist <= this->mTilesRadiusPerception){
        mDrawComponent->SetAnimation("attack");
        if(dist > 1.5f)
            this->mCurrentState = States::Persue;
        else 
            this->mCurrentState = States::Attack;
    }
    else {
        this->mCurrentState = States::Patrol;
        mDrawComponent->SetAnimation("walk");
    }
}

void Ghost::PatrolAction(){
    auto dist = GetPosition() - this->mPatrolPath[this->mCurPosPatrol];
    if(Math::NearZero(dist.Length(), 0.1f)){
        if(mDirectionPatrol == 1 and mCurPosPatrol == mPatrolPath.size()-1){
            mDirectionPatrol= -1;
        }
        else if(mDirectionPatrol==-1 and mCurPosPatrol == 0){
            mDirectionPatrol = +1;
        }
        mCurPosPatrol += mDirectionPatrol;
    }

    auto force = SteeringBehavior::Arrive(
        GetPosition(), mPatrolPath[mCurPosPatrol], mRigidBodyComponent->GetVelocity(),
        Game::TILE_SIZE * 0.1f, mMaxvelocity
    );

    mRigidBodyComponent->ApplyForce(force*70); 
}

void Ghost::PersueAction(){
    auto playerPosition = mGame->GetPlayer()->GetPosition();
    auto myVelocity = mRigidBodyComponent->GetVelocity();
    auto force = SteeringBehavior::Seek(GetPosition(), playerPosition, myVelocity, mMaxvelocity);
    mRigidBodyComponent->ApplyForce(force * 100);
}

void Ghost::AttackAction(){
    auto force = SteeringBehavior::Arrive(
        GetPosition(), mGame->GetPlayer()->GetPosition(), mRigidBodyComponent->GetVelocity(),
        Game::TILE_SIZE * 0.1f, mMaxvelocity
    );

    mRigidBodyComponent->ApplyForce(force * 70);
}

void Ghost::OnUpdate(float deltaTime){
    HandleTransition();

    switch (mCurrentState)
    {
    case States::Patrol:
        PatrolAction();    
        break;
    
    case States::Persue:
        PersueAction();    
        break;
    
    case States::Attack:
        AttackAction();    
        break;
    
    default:
        break;
    }

    auto vel = mRigidBodyComponent->GetVelocity();
    if(vel.x <= 0) SetRotation(Math::Pi);
    else SetRotation(0);

    mTimerPersueSound -= deltaTime;
    if(mTimerPersueSound < 0) mTimerPersueSound = -1;
    if(mCurrentState == States::Persue){
        if(mTimerPersueSound < 0){
            mGame->GetAudio()->PlaySound("ghostbreath.mp3");
            mTimerPersueSound = 3;
        }
    }

}


void Ghost::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other){
    if(other->GetLayer() == ColliderLayer::Player){
        mGame->GetPlayer()->OnHorizontalCollision(-minOverlap, mColliderComponent);
    }

    if (other->GetLayer() == ColliderLayer::Attack) {
        Kill();
    }
}

void Ghost::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other){
    if(other->GetLayer() == ColliderLayer::Player){
        if(minOverlap < 0){
            mGame->GetPlayer()->OnVerticalCollision(-minOverlap, mColliderComponent);
        }
    }

    if (other->GetLayer() == ColliderLayer::Attack) {
        Kill();
    }
}

void Ghost::Kill(){
    if (mState != ActorState::Destroy) {
        mState = ActorState::Destroy;
        mGame->DecreaseEnemiesAlive();
    }
}