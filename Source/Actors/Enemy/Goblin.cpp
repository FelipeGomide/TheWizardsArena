#include "Goblin.h"
#include "../../Game.h"
#include "../Player/Player.h"
#include "../../Components/DrawComponents/DrawPolygonComponent.h"
#include "../../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../../Components/AI/SteeringBehavior.h"
#include "../../Constants.h"
#include "../Puff.h"

Goblin::Goblin(Game* game, std::vector<Vector2> patrolPath) :
    Actor(game), mPatrolPath(patrolPath) {

    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 5, true);//, 10, mMaxvelocity);
    mColliderComponent = new AABBColliderComponent(this, 0, 0, 48*SIZE_MULTIPLIER, 48*SIZE_MULTIPLIER, ColliderLayer::Goblin);

    std::vector<Vector2> vertices = {Vector2(Game::TILE_SIZE/2, 0), Vector2(Game::TILE_SIZE, Game::TILE_SIZE), Vector2(0, Game::TILE_SIZE)};
    mDrawComponent = new DrawAnimatedComponent(
        this,
        "../Assets/Sprites/Goblin/Goblin.png",
        "../Assets/Sprites/Goblin/Goblin.json"
    );

    mDrawComponent->AddAnimation("walk", {0, 1, 2, 3});
    mDrawComponent->SetAnimation("walk");
    mDrawComponent->SetAnimFPS(7.0f);
}

void Goblin::HandleTransition(float deltaTime){
    if(!mGame->GetPlayer()){
        mCurrentState = States::Patrol;
        return;
    }

    auto playerPosition = mGame->GetPlayer()->GetPosition();
    auto dist = (playerPosition - GetPosition()).Length();
    dist /= Game::TILE_SIZE;

    auto resetPatrol = [&](){
        mPatrolPath = {GetPosition() + Vector2(-30, 0), GetPosition() + Vector2(30, 0)};
        mCurPosPatrol = 0;
    };

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
                resetPatrol();
            }
            else if (dist <= 1.5f) 
                mCurrentState = States::Attack;
            break;
        case States::Attack:
            if (dist > 1.5f) 
                mCurrentState = States::Persue;
            else if (dist > mTilesRadiusPerception){
                mCurrentState = States::Patrol;
                resetPatrol();
            } 
            break;
        default: break;
    }
}

bool Goblin::CheckHole(float deltaTime)
{ 
    float facingDir = (this->GetForward().x > 0? +1: -1);

    Vector2 groundCheckPoint = Vector2(
        GetPosition().x + (facingDir * Game::TILE_SIZE),
        groundCheckPoint.y = GetPosition().y + Game::TILE_SIZE
    );

    auto near = mGame->GetSpatialHashing()->QueryColliders(groundCheckPoint, 1);

    bool isGroundAhead = false;
    for (AABBColliderComponent* collider : near)
    {
        if (collider->GetLayer() == ColliderLayer::Blocks)
        {
            isGroundAhead = true;
            break; 
        }
    }
    return not isGroundAhead;
}

void Goblin::PatrolAction(float deltaTime){
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
        Game::TILE_SIZE * 0.5f, mMaxvelocity
    );
    
    force.y = 0;
    mRigidBodyComponent->ApplyForce(force);

    if((GetPosition() - mLastPosition).Length() < 0.5f){
        mStuckTimer += deltaTime;
        if(mStuckTimer > mStuckInterval){
            Jump();
            mStuckTimer = 0;
        }
    }
    else mStuckTimer = 0;

    bool hole = CheckHole(deltaTime);
    if(hole and mIsOnGround) Jump();

    mLastPosition = GetPosition();
}

void Goblin::PersueAction(float deltaTime){
    if(!mGame->GetPlayer()) return;

    auto playerPosition = mGame->GetPlayer()->GetPosition();
    auto force = (playerPosition - GetPosition());
    force.y = 0;
    force.Normalize(), force *= mForwardSpeed;

    // if (force.x > 0) {
    //     mRigidBodyComponent->ApplyForce(Vector2::UnitX * mForwardSpeed);
    // } else {
    //     mRigidBodyComponent->ApplyForce(Vector2::UnitX * (- mForwardSpeed));
    // }
    mRigidBodyComponent->ApplyForce(force);

    bool hole = CheckHole(deltaTime);
    if(hole and mIsOnGround) Jump();
}

void Goblin::AttackAction(float deltaTime){
    PersueAction(deltaTime);
}

void Goblin::Jump(){
    if(not mIsOnGround) return;
    auto vel = mRigidBodyComponent->GetVelocity();
    mRigidBodyComponent->SetVelocity(Vector2(vel.x, -mJumpVelocity));
    mIsOnGround = false;
}


void Goblin::OnUpdate(float deltaTime){
    
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


void Goblin::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other){

    if(other->GetLayer() == ColliderLayer::Blocks or other->GetLayer() == ColliderLayer::Goblin){
        Jump();
    }

    if(other->GetLayer() == ColliderLayer::Player){
        mGame->GetPlayer()->OnHorizontalCollision(-minOverlap, mColliderComponent);
    }

    if (other->GetLayer() == ColliderLayer::Attack) {
        Kill();
    }
}

void Goblin::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other){
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

void Goblin::Kill(){
    if (mState != ActorState::Destroy) {
        mState = ActorState::Destroy;
        int number = rand() % 6;
        std::string sound = "monster/pain"+std::to_string(number)+".wav";
        mGame->GetAudio()->PlaySound(sound);
        mGame->DecreaseEnemiesAlive();
        new Puff(mGame, GetPosition());
    }
}