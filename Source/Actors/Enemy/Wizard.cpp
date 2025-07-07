#include "Wizard.h"
#include "../Explosion.h"
#include "../Puff.h"
#include "../Player/Player.h"
#include "../../Components/RigidBodyComponent.h"
#include "../../Components/ColliderComponents/AABBColliderComponent.h"
#include "../../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../../Components/AI/SteeringBehavior.h"
#include "../../Constants.h"
#include "../../Game.h"

Wizard::Wizard(Game* game, std::vector<Vector2> pointsTeleport, float teleportTimer, float attackTimer, float attackCoolDown)
: Actor(game)
, mPointsTeleportation(pointsTeleport)
, mCurrentState(States::Patrol)
, mTeleportTimer(teleportTimer)
, mCurTeleportTimer(teleportTimer)
, mAttackTimer(attackTimer)
, mCurAttackTimer(attackTimer)
, mAlertAttack(false)
, mAttackCoolDown(attackCoolDown)
, mCurAttackCoolDown(attackCoolDown){


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
    mDrawComponent->AddAnimation("idle", {0, 1, 2, 3});
    mDrawComponent->AddAnimation("attack", {0, 1, 2, 3});
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

    switch (mCurrentState) {
        case States::Patrol:
            if (dist <= mTilesRadiusPerception) {
                mCurrentState = (dist > 1.5f) ? States::Persue : States::Attack;
                mGame->GetAudio()->PlaySound("monster/grunt1.wav");
                mDrawComponent->SetAnimation("idle");
            }
            break;
        case States::Persue:
            if (dist > mTilesRadiusPerception) {
                mCurrentState = States::Patrol;
                mDrawComponent->SetAnimation("idle");
            }
            else if (dist <= 5.0f) 
                mCurrentState = States::Attack;
                mDrawComponent->SetAnimation("attack");
            break;
        case States::Attack:
            if (dist > 5.0f) {
                mCurrentState = States::Persue;
                mDrawComponent->SetAnimation("walk");
            }
            else if (dist > mTilesRadiusPerception){
                mCurrentState = States::Patrol;
                mDrawComponent->SetAnimation("idle");
            } 
            break;
        default: break;
    }
}

void Wizard::PatrolAction(float deltaTime){

    mCurTeleportTimer -= deltaTime;
    if(mCurTeleportTimer < 0){
        mCurTeleportTimer = mTeleportTimer;
        // new Puff(mGame, GetPosition());
        // auto nextPos = mPointsTeleportation[rand()%mPointsTeleportation.size()];
        // SetPosition(nextPos);
        // new Puff(mGame, nextPos);
    }

}

void Wizard::PersueAction(float deltaTime){

    if(!mGame->GetPlayer()) return;

    auto playerPosition = mGame->GetPlayer()->GetPosition();
    auto myVelocity = mRigidBodyComponent->GetVelocity();
    auto force = SteeringBehavior::Seek(GetPosition(), playerPosition, myVelocity, mMaxvelocity);
    force.y = 0;
    mRigidBodyComponent->ApplyForce(force * 100);
}

void Wizard::AttackAction(float deltaTime){
    auto vel = mRigidBodyComponent->GetVelocity();
    if(!Math::NearZero(vel.x, 0.01))
        mRigidBodyComponent->ApplyForce(Vector2(-vel.x, 0));
    
    if(mCurAttackCoolDown > 0) return;

    mCurAttackTimer -= deltaTime;
    if(mCurAttackTimer < 1 and mAlertAttack == false){

        auto curPos = GetPosition();
        auto posPlayer = mGame->GetPlayer()->GetPosition();
        auto direction = posPlayer - curPos;
        direction.Normalize();
        const float offSet = 3 * mGame->TILE_SIZE;

        mPointsAttack.clear();

        mAlertAttack = true;
        for(int i = 0; i < 3; i ++){
            curPos += offSet * direction;
            new Puff(mGame, curPos, 1, PuffType::PreExplosion);
            mPointsAttack.push_back(curPos);
        }
    }

    if(mCurAttackTimer < 0 and mAlertAttack == true){
        mCurAttackTimer = mAttackTimer;
        mCurAttackCoolDown = mAttackCoolDown;
        mAlertAttack = false;

        for(auto pt : mPointsAttack){
            new Explosion(mGame, pt, 2);
        }
    }
    
}

void Wizard::Jump(){
    if(not mIsOnGround) return;
    // auto vel = mRigidBodyComponent->GetVelocity();
    // mRigidBodyComponent->SetVelocity(Vector2(vel.x, -mJumpVelocity));
    // mIsOnGround = false;
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

    mCurAttackCoolDown -= deltaTime;
    if(mCurAttackCoolDown < 0)
        mCurAttackCoolDown = -1;
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


