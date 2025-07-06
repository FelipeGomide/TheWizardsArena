// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
//
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include <algorithm>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "CSV.h"
#include "Random.h"
#include "Game.h"

#include <cstring>

#include "HUD.h"
#include "SpatialHashing.h"
#include "Actors/Actor.h"
#include "Actors/Player/Player.h"
#include "Actors/Block.h"
#include "Actors/Spikes.h"
#include "Actors/Enemy/Ghost.h"
#include "Actors/Enemy/Goblin.h"
#include "Actors/Player/Warrior.h"
#include "UIElements/UIScreen.h"
#include "Components/DrawComponents/DrawComponent.h"
#include "Components/DrawComponents/DrawSpriteComponent.h"
#include "Components/DrawComponents/DrawPolygonComponent.h"
#include "Components/ColliderComponents/AABBColliderComponent.h"

Game::Game(int windowWidth, int windowHeight)
        :mWindow(nullptr)
        ,mRenderer(nullptr)
        ,mTicksCount(0)
        ,mIsRunning(true)
        ,mWindowWidth(windowWidth)
        ,mWindowHeight(windowHeight)
        ,mPlayer(nullptr)
        ,mHUD(nullptr)
        ,mBackgroundColor(0, 0, 0)
        ,mModColor(255, 255, 255)
        ,mCameraPos(Vector2::Zero)
        ,mAudio(nullptr)
        ,mGameTimer(0.0f)
        ,mGameTimeLimit(0)
        ,mSceneManagerTimer(1.0f)
        ,mSceneManagerState(SceneManagerState::None)
        ,mGameScene(GameScene::MainMenu)
        ,mNextScene(GameScene::MainMenu)
        ,mBackgroundTexture(nullptr)
        ,mBackgroundSize(Vector2::Zero)
        ,mBackgroundPosition(Vector2::Zero)
        ,mDebugMode(false)
{

}

bool Game::Initialize()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    mWindow = SDL_CreateWindow("The Wizard's Arena", 0, 0, mWindowWidth, mWindowHeight, 0);
    if (!mWindow)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!mRenderer)
    {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        return false;
    }

    if (IMG_Init(IMG_INIT_PNG) == 0)
    {
        SDL_Log("Unable to initialize SDL_image: %s", SDL_GetError());
        return false;
    }

    // Initialize SDL_ttf
    if (TTF_Init() != 0)
    {
        SDL_Log("Failed to initialize SDL_ttf");
        return false;
    }

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
    {
        SDL_Log("Failed to initialize SDL_mixer");
        return false;
    }

    // Start random number generator
    Random::Init();

    mAudio = new AudioSystem();

    mSpatialHashing = new SpatialHashing(TILE_SIZE * 4.0f,
                                         LEVEL_WIDTH * TILE_SIZE,
                                         LEVEL_HEIGHT * TILE_SIZE);
    mTicksCount = SDL_GetTicks();

    // Init all game actors
    SetGameScene(GameScene::MainMenu);


    return true;
}

void Game::SetGameScene(Game::GameScene scene, float transitionTime)
{
    bool cenaValida = scene == GameScene::MainMenu || scene == GameScene::Level1 || scene == GameScene::Level2 || scene == GameScene::Level3 || scene == GameScene::Level4 || scene == GameScene::Level5  || scene == GameScene::GameOver || scene == GameScene::HowToPlay || scene == GameScene::Credits;

    if (mSceneManagerState == SceneManagerState::None && cenaValida) {
        mNextScene = scene;
        mSceneManagerState = SceneManagerState::Entering;
        mFadeTime = transitionTime/3;
        mSceneManagerTimer = mFadeTime;
    }
    else {
        if (!cenaValida)
            SDL_Log("Cena inválida!");

        if (mSceneManagerState != SceneManagerState::None)
            SDL_Log("Estado de SceneManager inválido!");
    }
}

void Game::ResetGameScene(float transitionTime)
{
    mEnemiesAlive = 0;
    SetGameScene(mGameScene, transitionTime);
}

void Game::ChangeScene()
{
    // Unload current Scene
    UnloadScene();

    // Reset camera position
    mCameraPos.Set(0.0f, 0.0f);

    // Reset game timer
    mGameTimer = 0.0f;

    // Reset gameplay state
    mGamePlayState = GamePlayState::Playing;

    // Reset scene manager state
    mSpatialHashing = new SpatialHashing(TILE_SIZE * 4.0f, LEVEL_WIDTH * TILE_SIZE, LEVEL_HEIGHT * TILE_SIZE);

    // Scene Manager FSM: using if/else instead of switch
    if (mNextScene == GameScene::MainMenu)
    {
        // Set background color
        mBackgroundColor.Set(107.0f, 140.0f, 255.0f);
        if(mMusicHandle.IsValid()){
            mAudio->StopSound(mMusicHandle);
            mMusicHandle.Reset();
        }
        mMusicHandle = mAudio->PlaySound("theme/Treasures.mp3", true);
        // Initialize main menu actors
        LoadMainMenu();
    }
    else if (mNextScene == GameScene::Level1)
    {
        mHUD = new HUD(this, "../Assets/Fonts/SMB.ttf");
    
        mGameTimeLimit = 400;

        mAudio->StopSound(mMusicHandle);
        mMusicHandle.Reset();
        mMusicHandle = mAudio->PlaySound("theme/Retro Mystic.ogg", true);
        // Set background color
        mBackgroundColor.Set(107.0f, 140.0f, 255.0f);
        SetBackgroundImage("../Assets/Sprites/background.png", Vector2(TILE_SIZE,TILE_SIZE), Vector2((mWindowWidth-(3*TILE_SIZE))*SIZE_MULTIPLIER, (mWindowHeight-(3*TILE_SIZE))*SIZE_MULTIPLIER));

        // Initialize actors
        LoadLevel("../Assets/Levels/1.csv", LEVEL_WIDTH, LEVEL_HEIGHT);
    }
    else if (mNextScene == GameScene::Level2)
    {
    
        mHUD = new HUD(this, "../Assets/Fonts/SMB.ttf");
        mGameTimeLimit = 400;

        // Set background color
        mBackgroundColor.Set(0.0f, 0.0f, 0.0f);
        SetBackgroundImage("../Assets/Sprites/background.png", Vector2(TILE_SIZE,TILE_SIZE), Vector2((mWindowWidth-(3*TILE_SIZE))*SIZE_MULTIPLIER, (mWindowHeight-(3*TILE_SIZE))*SIZE_MULTIPLIER));


        // Initialize actors
        LoadLevel("../Assets/Levels/2.csv", LEVEL_WIDTH, LEVEL_HEIGHT);
    }
    else if (mNextScene == GameScene::Level3)
    {

        mHUD = new HUD(this, "../Assets/Fonts/SMB.ttf");
        mGameTimeLimit = 400;

        // Set background color
        mBackgroundColor.Set(255.0f, 0.0f, 0.0f);
        SetBackgroundImage("../Assets/Sprites/background.png", Vector2(TILE_SIZE,TILE_SIZE), Vector2((mWindowWidth-(3*TILE_SIZE))*SIZE_MULTIPLIER, (mWindowHeight-(3*TILE_SIZE))*SIZE_MULTIPLIER));


        // Initialize actors
        LoadLevel("../Assets/Levels/3.csv", LEVEL_WIDTH, LEVEL_HEIGHT);
    }
    else if (mNextScene == GameScene::Level4)
    {

        mHUD = new HUD(this, "../Assets/Fonts/SMB.ttf");
        mGameTimeLimit = 400;

        // Set background color
        mBackgroundColor.Set(150.0f, 150.0f, 0.0f);
        SetBackgroundImage("../Assets/Sprites/background.png", Vector2(TILE_SIZE,TILE_SIZE), Vector2((mWindowWidth-(3*TILE_SIZE))*SIZE_MULTIPLIER, (mWindowHeight-(3*TILE_SIZE))*SIZE_MULTIPLIER));


        // Initialize actors
        LoadLevel("../Assets/Levels/4.csv", LEVEL_WIDTH, LEVEL_HEIGHT);
    }
    else if (mNextScene == GameScene::Level5)
    {

        mHUD = new HUD(this, "../Assets/Fonts/SMB.ttf");
        mGameTimeLimit = 400;

        // Set background color
        mBackgroundColor.Set(50.0f, 100.0f, 150.0f);
        SetBackgroundImage("../Assets/Sprites/background.png", Vector2(TILE_SIZE,TILE_SIZE), Vector2((mWindowWidth-(3*TILE_SIZE))*SIZE_MULTIPLIER, (mWindowHeight-(3*TILE_SIZE))*SIZE_MULTIPLIER));

        // Initialize actors
        LoadLevel("../Assets/Levels/5.csv", LEVEL_WIDTH, LEVEL_HEIGHT);
    }
    else if (mNextScene == GameScene::GameOver){
        mBackgroundColor.Set(107.0f, 140.0f, 255.0f);
        if(mMusicHandle.IsValid()){
            mAudio->StopSound(mMusicHandle);
            mMusicHandle.Reset();
        }

        mMusicHandle = mAudio->PlaySound("theme/Close.mp3", true);
        LoadGameOverMenu();
    }
    else if (mNextScene == GameScene::HowToPlay) {
        LoadHowToPlay();
    }
    else if (mNextScene == GameScene::Credits) {
        LoadCredits();
    }
    // Set new scene
    mGameScene = mNextScene;
}

void Game::LoadGameOverMenu(){
    auto menu = new UIScreen(this, "../Assets/Fonts/SMB.ttf");
    SetBackgroundImage("../Assets/Sprites/Fundo-tijolos.png", Vector2(0,0), Vector2(mWindowWidth, mWindowHeight));
    if (mEnemiesAlive == 0)
        menu->AddImage("../Assets/Sprites/Victory.png", Vector2::Zero, Vector2(mWindowWidth + 10, 0.8 *mWindowHeight));
    else
        menu->AddImage("../Assets/Sprites/GameOver.png", Vector2::Zero, Vector2(mWindowWidth + 10, 0.8 *mWindowHeight));
    menu->AddText("PRESS ANY KEY TO CONTINUE", Vector2(50, 650), Vector2(1000, 30), 30, true);
}

void Game::LoadMainMenu()
{
    // Esse método será usado para criar uma tela de UI e adicionar os elementos do menu principal.
    auto mainMenu = new UIScreen(this, "../Assets/Fonts/Kenney Bold.ttf");

    const Vector2 logoSize = Vector2(1080.0f, 567.42f);
    const Vector2 logoPos = Vector2(mWindowWidth/2.0f - logoSize.x/2.0f, 0);
    mainMenu->AddImage("../Assets/Sprites/Titulo-Wizard's-Arena.png", logoPos, logoSize);

    // Background
    SetBackgroundImage("../Assets/Sprites/Fundo-tijolos.png", Vector2(0,0), Vector2(1080,720));

    auto strStart = "START";
    float posStart = (mWindowWidth/3 - std::strlen(strStart) * 16) / 2.0f;
    auto buttonStart = mainMenu->AddButton(strStart, Vector2(posStart, 625), Vector2(std::strlen(strStart) * 16, 40.0f),[this]() {
        SetGameScene(GameScene::Level1, 3.0f);
    });

    auto strHowToPlay = "HOW TO PLAY";
    float posHowToPlay = (mWindowWidth/3 - std::strlen(strHowToPlay) * 16) / 2.0f + mWindowWidth/3;
    auto buttonHowToPlay = mainMenu->AddButton(strHowToPlay, Vector2(posHowToPlay, 625), Vector2(std::strlen(strHowToPlay) * 16, 40.0f),[this]() {
        SetGameScene(GameScene::HowToPlay, 3.0f);
    });

    auto strCredits = "CREDITS";
    float posCredits = (mWindowWidth/3 - std::strlen(strCredits) * 16) / 2.0f + mWindowWidth/3 * 2;
    auto buttonCredits = mainMenu->AddButton(strCredits, Vector2(posCredits, 625), Vector2(std::strlen(strCredits) * 16, 40.0f),[this]() {
        SetGameScene(GameScene::Credits, 3.0f);
    });

}

void Game::LoadHowToPlay() {
    auto HTP = new UIScreen(this, "../Assets/Fonts/Kenney Bold.ttf");
    SetBackgroundImage("../Assets/Sprites/Fundo-tijolos.png", Vector2(0,0), Vector2(mWindowWidth, mWindowHeight));

    auto str = "CONTROLS";
    HTP->AddText(str, Vector2(mWindowWidth/2 - std::strlen(str) * 25.0f/2, 25), Vector2(std::strlen(str) * 25, 70), 40, false, 2000, Color::Yellow);
    str = "A and D to move";
    HTP->AddText(str, Vector2(150, 175), Vector2(std::strlen(str) * 25, 30), 40, false, 2000, Color::Yellow);
    str = "SPACE BAR to jump";
    HTP->AddText(str, Vector2(150, 275), Vector2(std::strlen(str) * 25, 30), 40, false, 2000, Color::Yellow);
    str = "K to attack";
    HTP->AddText(str, Vector2(150, 375), Vector2(std::strlen(str) * 25, 30), 40, false, 2000, Color::Yellow);
    str = "ENTER to pause";
    HTP->AddText(str, Vector2(150, 475), Vector2(std::strlen(str) * 25, 30), 40, false, 2000, Color::Yellow);
    str = "PRESS ANY KEY TO BACK TO MAIN MENU";
    HTP->AddText(str, Vector2(150, 625), Vector2(std::strlen(str) * 25, 30), 40, true, 2000);

}

void Game::LoadCredits() {
    auto credits = new UIScreen(this, "../Assets/Fonts/SMB.ttf");
    SetBackgroundImage("../Assets/Sprites/Fundo-tijolos.png", Vector2(0,0), Vector2(mWindowWidth, mWindowHeight));
    auto str = "CREDITS";
    credits->AddText(str, Vector2(mWindowWidth/2 - std::strlen(str) * 25.0f/2, 25), Vector2(std::strlen(str) * 25, 50), 40, false, 2000, Color::Yellow);
    str = "Game made by:";
    credits->AddText(str, Vector2(150, 150), Vector2(std::strlen(str) * 25, 30), 40, false, 2000, Color::Yellow);
    str = "- Felipe Lopes Gomide";
    credits->AddText(str, Vector2(150, 250), Vector2(std::strlen(str) * 25, 30), 40, false, 2000, Color::Yellow);
    str = "- João Victor Evangelista Cruz";
    credits->AddText(str, Vector2(150, 350), Vector2(std::strlen(str) * 25, 30), 40, false, 2000, Color::Yellow);
    str = "- Marcos Daniel Souza Neto";
    credits->AddText(str, Vector2(150, 450), Vector2(std::strlen(str) * 25, 30), 40, false, 2000, Color::Yellow);
    str = "PRESS ANY KEY TO BACK TO MAIN MENU";
    credits->AddText(str, Vector2(150, 625), Vector2(std::strlen(str) * 25, 30), 40, true, 2000);
}

void Game::LoadLevel(const std::string& levelName, const int levelWidth, const int levelHeight)
{
    // Load level data
    int **mLevelData = ReadLevelData(levelName, levelWidth, levelHeight);

    if (!mLevelData) {
        SDL_Log("Failed to load level data");
        return;
    }

    // Instantiate level actors
    BuildLevel(mLevelData, levelWidth, levelHeight);
}

void Game::BuildLevel(int** levelData, int width, int height)
{

    // Const map to convert tile ID to block type
    const std::map<int, const std::string> tileMap = {
        {0, "../Assets/Sprites/Blocks/CenterBrick.png"},
        {1, "../Assets/Sprites/Blocks/LeftBrick.png"},
        {2, "../Assets/Sprites/Blocks/RightBrick.png"},
        {3, "../Assets/Sprites/Blocks/Platform.png"}
    };
    mEnemiesAlive = 0;

    for (int y = 0; y < LEVEL_HEIGHT; ++y)
    {
        for (int x = 0; x < LEVEL_WIDTH; ++x)
        {
            int tile = levelData[y][x];

            if (tile == 4) { //Spikes
                Spikes* spikes = new Spikes(this);
                spikes->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
            }
            else if(tile == 7) // Player
            {
                mPlayer = new Warrior(this);
                mPlayer->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));

                mCameraPos = Vector2(x * TILE_SIZE, y * TILE_SIZE);
            }
            else if(tile == 6) // Goblin
            {
                auto* goblin = new Goblin(this, std::vector<Vector2>{Vector2(x * TILE_SIZE -50, y * TILE_SIZE), Vector2(x*TILE_SIZE+50, y*TILE_SIZE)});
                goblin->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
                mEnemiesAlive += 1;
            }
            else if(tile == 5) // Ghost
            {
                auto* ghost = new Ghost(this, std::vector<Vector2>{Vector2(x * TILE_SIZE -50, y * TILE_SIZE), Vector2(x*TILE_SIZE+50, y*TILE_SIZE)});
                ghost->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
                mEnemiesAlive += 1;
            }
            else // Blocks
            {
                auto it = tileMap.find(tile);
                if (it != tileMap.end())
                {
                    // Create a block actor
                    Block* block = new Block(this, it->second);
                    block->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));

                    if (tile == 3) {
                        block->GetComponent<AABBColliderComponent>()->SetLayer(ColliderLayer::Platform);
                    }
                }
            }
        }
    }
}

int **Game::ReadLevelData(const std::string& fileName, int width, int height)
{
    std::ifstream file(fileName);
    if (!file.is_open())
    {
        SDL_Log("Failed to load paths: %s", fileName.c_str());
        return nullptr;
    }

    // Create a 2D array of size width and height to store the level data
    int** levelData = new int*[height];
    for (int i = 0; i < height; ++i)
    {
        levelData[i] = new int[width];
    }

    // Read the file line by line
    int row = 0;

    std::string line;
    while (!file.eof())
    {
        std::getline(file, line);
        if(!line.empty())
        {
            auto tiles = CSVHelper::Split(line);

            if (tiles.size() != width) {
                SDL_Log("Invalid level data");
                return nullptr;
            }

            for (int i = 0; i < width; ++i) {
                levelData[row][i] = tiles[i];
            }
        }

        ++row;
    }

    // Close the file
    file.close();

    return levelData;
}

void Game::RunLoop()
{
    while (mIsRunning)
    {
        ProcessInput();
        UpdateGame();
        GenerateOutput();
    }
}

void Game::ProcessInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                Quit();
                break;
            case SDL_KEYDOWN:
                // Handle key press for UI screens
                if (!mUIStack.empty()) {
                    if (mGameScene == GameScene::MainMenu || mGameScene == GameScene::GameOver || mGameScene == GameScene::HowToPlay || mGameScene == GameScene::Credits)
                        mUIStack.back()->HandleKeyPress(event.key.keysym.sym);
                }

                HandleKeyPressActors(event.key.keysym.sym, event.key.repeat == 0);

                // Check if the Return key has been pressed to pause/unpause the game
                if (event.key.keysym.sym == SDLK_RETURN)
                {
                    TogglePause();
                }

                if (event.key.keysym.sym == SDLK_F2) {
                    ToggleDebug();
                }
                break;
        }
    }

    ProcessInputActors();
}

void Game::ProcessInputActors()
{
    if(mGamePlayState == GamePlayState::Playing)
    {
        // Get actors on camera
        std::vector<Actor*> actorsOnCamera =
                mSpatialHashing->QueryOnCamera(mCameraPos,mWindowWidth,mWindowHeight);

        const Uint8* state = SDL_GetKeyboardState(nullptr);

        bool isMarioOnCamera = false;
        for (auto actor: actorsOnCamera)
        {
            actor->ProcessInput(state);

            if (actor == mPlayer) {
                isMarioOnCamera = true;
            }
        }

        // If Mario is not on camera, process input for him
        if (!isMarioOnCamera && mPlayer) {
            mPlayer->ProcessInput(state);
        }
    }
}

void Game::HandleKeyPressActors(const int key, const bool isPressed)
{
    if(mGamePlayState == GamePlayState::Playing)
    {
        // Get actors on camera
        std::vector<Actor*> actorsOnCamera =
                mSpatialHashing->QueryOnCamera(mCameraPos,mWindowWidth,mWindowHeight);

        // Handle key press for actors
        bool isMarioOnCamera = false;
        for (auto actor: actorsOnCamera) {
            actor->HandleKeyPress(key, isPressed);

            if (actor == mPlayer) {
                isMarioOnCamera = true;
            }
        }

        // If Mario is not on camera, handle key press for him
        if (!isMarioOnCamera && mPlayer)
        {
            mPlayer->HandleKeyPress(key, isPressed);
        }
    }

}

void Game::TogglePause()
{

    if (mGameScene != GameScene::MainMenu and mGameScene != GameScene::GameOver and mGameScene != GameScene::HowToPlay and mGameScene != GameScene::Credits)
    {
        if (mGamePlayState == GamePlayState::Playing)
        {
            mGamePlayState = GamePlayState::Paused;

            mAudio->PauseSound(mMusicHandle);
            mAudio->PlaySound("Block.wav");
        }
        else if (mGamePlayState == GamePlayState::Paused)
        {
            mGamePlayState = GamePlayState::Playing;

            mAudio->ResumeSound(mMusicHandle);
            mAudio->PlaySound("Block.wav");
        }
    }
}

void Game::ChangeRound() {

    if (mEnemiesAlive == 0 && mGameScene == GameScene::Level1) {
        SetGameScene(GameScene::Level2, 3.0f);
    }

    if (mEnemiesAlive == 0 && mGameScene == GameScene::Level2) {
        SetGameScene(GameScene::Level3, 1.0f);
    }

    if (mEnemiesAlive == 0 && mGameScene == GameScene::Level3) {
        SetGameScene(GameScene::Level4, 1.0f);
    }

    if (mEnemiesAlive == 0 && mGameScene == GameScene::Level4) {
        SetGameScene(GameScene::Level5, 1.0f);
    }

    if (mEnemiesAlive == 0 && mGameScene == GameScene::Level5) {
        // mMusicHandle = mAudio->PlaySound("theme/Retro Comedy.ogg");
        SetGameScene(GameScene::GameOver, 3.0f);
    }
}

void Game::UpdateGame()
{
    while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16));

    float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
    if (deltaTime > 0.05f)
    {
        deltaTime = 0.05f;
    }

    mTicksCount = SDL_GetTicks();

    if(mGamePlayState != GamePlayState::Paused)
    {
        // Reinsert all actors and pending actors
        UpdateActors(deltaTime);
    }

    // Reinsert audio system
    mAudio->Update(deltaTime);

    // Reinsert UI screens
    for (auto ui : mUIStack) {
        if (ui->GetState() == UIScreen::UIState::Active) {
            ui->Update(deltaTime);
        }
    }

    // Delete any UIElements that are closed
    auto iter = mUIStack.begin();
    while (iter != mUIStack.end()) {
        if ((*iter)->GetState() == UIScreen::UIState::Closing) {
            delete *iter;
            iter = mUIStack.erase(iter);
        } else {
            ++iter;
        }
    }

    // ---------------------
    // Game Specific Updates
    // ---------------------
    UpdateCamera();

    UpdateSceneManager(deltaTime);

    if (mGameScene != GameScene::MainMenu and mGameScene != GameScene::GameOver and mGameScene != GameScene::HowToPlay and mGameScene != GameScene::Credits and mGamePlayState == GamePlayState::Playing)
        UpdateLevelTime(deltaTime);

    ChangeRound();
}

void Game::UpdateSceneManager(float deltaTime)
{

    if(mGameScene == GameScene::GameOver){
    }
    if (mSceneManagerState == SceneManagerState::Entering) {
        mSceneManagerTimer -= deltaTime;
        mFade = 255.0f - mSceneManagerTimer/mFadeTime * 255.0f;
        if (mSceneManagerTimer <= 0) {
            mSceneManagerTimer = mFadeTime;
            mSceneManagerState = SceneManagerState::Active;
        }
    }

    if (mSceneManagerState == SceneManagerState::Active) {
        mSceneManagerTimer -= deltaTime;
        mFade = 255.0f;
        if (mSceneManagerTimer <= 0) {
            mSceneManagerTimer = mFadeTime;
            ChangeScene();
            mSceneManagerState = SceneManagerState::Exiting;
        }
    }

    if (mSceneManagerState == SceneManagerState::Exiting) {
        mSceneManagerTimer -= deltaTime;
        mFade = mSceneManagerTimer/mFadeTime * 255.0f;
        if (mSceneManagerTimer <= 0) {
            mSceneManagerState = SceneManagerState::None;
        }
    }

    if (mSceneManagerState == SceneManagerState::None)
        mFade = 0;
}

void Game::UpdateLevelTime(float deltaTime)
{
    mGameTimer += deltaTime;
    if (mGameTimer >= 1.0f) {
        mGameTimer = 0.0f;
        mGameTimeLimit--;
        if (mHUD)
            //mHUD->SetTime(mGameTimeLimit);
        if (mGameTimeLimit <= 0) {
            mPlayer->Kill();
        }
    }
}

void Game::UpdateCamera()
{
    if (!mPlayer) return;

    float horizontalCameraPos = mPlayer->GetPosition().x - (mWindowWidth / 2.0f);
    float verticalCameraPos = mPlayer->GetPosition().y - (mWindowHeight / 2.0f) - TILE_SIZE;

    // Limit camera to the right side of the level
    float maxCameraPosX = (LEVEL_WIDTH * TILE_SIZE) - mWindowWidth;
    horizontalCameraPos = Math::Clamp(horizontalCameraPos, 0.0f, maxCameraPosX);

    float maxCameraPosY = (LEVEL_HEIGHT * TILE_SIZE) - mWindowHeight;
    verticalCameraPos = Math::Clamp(verticalCameraPos, 0.0f, maxCameraPosY);

    mCameraPos.y = verticalCameraPos;
    mCameraPos.x = horizontalCameraPos;
}

void Game::UpdateActors(float deltaTime)
{
    // Get actors on cameramGamePlayState
    std::vector<Actor*> actorsOnCamera =
            mSpatialHashing->QueryOnCamera(mCameraPos,mWindowWidth,mWindowHeight);

    bool isMarioOnCamera = false;
    for (auto actor : actorsOnCamera)
    {
        actor->Update(deltaTime);
        if (actor == mPlayer)
        {
            isMarioOnCamera = true;
        }
    }

    // If Mario is not on camera, reset camera position
    if (!isMarioOnCamera && mPlayer)
    {
        mPlayer->Update(deltaTime);
    }

    for (auto actor : actorsOnCamera)
    {
        if (actor->GetState() == ActorState::Destroy)
        {
            delete actor;
            if (actor == mPlayer) {
                mPlayer = nullptr;
            }
        }
    }
}

void Game::AddActor(Actor* actor)
{
    if (mSpatialHashing == nullptr) return;
    mSpatialHashing->Insert(actor);
}

void Game::RemoveActor(Actor* actor)
{
    if (mSpatialHashing == nullptr) return;
    mSpatialHashing->Remove(actor);
}
void Game::Reinsert(Actor* actor)
{
    if (mSpatialHashing == nullptr) return;
    mSpatialHashing->Reinsert(actor);
}

std::vector<Actor *> Game::GetNearbyActors(const Vector2& position, const int range)
{
    return mSpatialHashing->Query(position, range);
}

std::vector<AABBColliderComponent *> Game::GetNearbyColliders(const Vector2& position, const int range)
{
    return mSpatialHashing->QueryColliders(position, range);
}

void Game::GenerateOutput()
{
    // Clear frame with background color
    SDL_SetRenderDrawColor(mRenderer, mBackgroundColor.x, mBackgroundColor.y, mBackgroundColor.z, 255);

    // Clear back buffer
    SDL_RenderClear(mRenderer);

    // Draw background texture considering camera position
    if (mBackgroundTexture)
    {
        SDL_Rect dstRect = { static_cast<int>(mBackgroundPosition.x - mCameraPos.x),
                             static_cast<int>(mBackgroundPosition.y - mCameraPos.y),
                             static_cast<int>(mBackgroundSize.x),
                             static_cast<int>(mBackgroundSize.y) };

        SDL_RenderCopy(mRenderer, mBackgroundTexture, nullptr, &dstRect);
    }

    // Get actors on camera
    std::vector<Actor*> actorsOnCamera =
            mSpatialHashing->QueryOnCamera(mCameraPos,mWindowWidth,mWindowHeight);

    // Get list of drawables in draw order
    std::vector<DrawComponent*> drawables;

    for (auto actor : actorsOnCamera)
    {
        auto drawable = actor->GetComponent<DrawComponent>();
        if (drawable && drawable->IsVisible())
        {
            drawables.emplace_back(drawable);
        }
    }

    // Sort drawables by draw order
    std::sort(drawables.begin(), drawables.end(),
              [](const DrawComponent* a, const DrawComponent* b) {
                  return a->GetDrawOrder() < b->GetDrawOrder();
              });

    // Draw all drawables
    for (auto drawable : drawables)
    {
        drawable->Draw(mRenderer, mModColor);
    }

    // Draw all UI screens
    for (auto ui :mUIStack)
    {
        ui->Draw(mRenderer);
    }

    if (mDebugMode) {
        for (auto actor: actorsOnCamera)
        {
            if (auto collider = actor->GetComponent<AABBColliderComponent>()) {
                SDL_Rect rect = {
                    (int) (collider->GetMin().x - GetCameraPos().x),
                    (int) (collider->GetMin().y - GetCameraPos().y),
                    (int) (collider->GetMax().x - collider->GetMin().x),
                    (int) (collider->GetMax().y - collider->GetMin().y)
                };

                switch (collider->GetLayer()) {
                    case ColliderLayer::Player:
                        SDL_SetRenderDrawColor(mRenderer, Color::Green.x * 255, Color::Green.y  * 255, Color::Green.z  * 255, 255);
                        break;
                    case ColliderLayer::Blocks:
                        SDL_SetRenderDrawColor(mRenderer, Color::Blue.x  * 255, Color::Blue.y  * 255, Color::Blue.z  * 255, 255);
                        break;
                    case ColliderLayer::Enemy:
                        SDL_SetRenderDrawColor(mRenderer, Color::Red.x  * 255, Color::Red.y  * 255, Color::Red.z  * 255,  255);
                        break;
                    default: ;
                        SDL_SetRenderDrawColor(mRenderer, Color::Yellow.x  * 255, Color::Yellow.y  * 255, Color::Yellow.z  * 255, 255);
                }

                SDL_RenderDrawRect(mRenderer, &rect);
            }
        }
    }

    // if (mSceneManagerState == SceneManagerState::Active or mSceneManagerState == SceneManagerState::Entering) {
    //     SDL_Rect rect{
    //         .x = 0, .y = 0, .w = GetWindowWidth(), .h = GetWindowHeight()
    //     };
    //     mAlphaTransition = mSceneManagerTimer/(double)TRANSITION_TIME;
    //     Uint8 alpha;
    //     if (mSceneManagerState == SceneManagerState::Entering) {
    //         alpha = static_cast<Uint8>((1.0f - mAlphaTransition) * 255);
    //     } else {
    //         alpha = static_cast<Uint8>(mAlphaTransition* 255);
    //     }
    //
    //     SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
    //     SDL_SetRenderDrawColor(mRenderer, 0,0,0, alpha);
    //     SDL_RenderFillRect(mRenderer, &rect);
    // }

    SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, mFade);
    SDL_Rect retangulo = { 0, 0, mWindowWidth, mWindowWidth };
    SDL_RenderFillRect(mRenderer, &retangulo);

    // Swap front buffer and back buffer
    SDL_RenderPresent(mRenderer);
}

void Game::SetBackgroundImage(const std::string& texturePath, const Vector2 &position, const Vector2 &size)
{
    if (mBackgroundTexture) {
        SDL_DestroyTexture(mBackgroundTexture);
        mBackgroundTexture = nullptr;
    }

    // Load background texture
    mBackgroundTexture = LoadTexture(texturePath);
    if (!mBackgroundTexture) {
        SDL_Log("Failed to load background texture: %s", texturePath.c_str());
    }

    // Set background position
    mBackgroundPosition.Set(position.x, position.y);

    // Set background size
    mBackgroundSize.Set(size.x, size.y);
}

SDL_Texture* Game::LoadTexture(const std::string& texturePath)
{
    SDL_Surface* surface = IMG_Load(texturePath.c_str());

    if (!surface) {
        SDL_Log("Failed to load image: %s", IMG_GetError());
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(mRenderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        SDL_Log("Failed to create texture: %s", SDL_GetError());
        return nullptr;
    }

    return texture;
}


UIFont* Game::LoadFont(const std::string& fileName)
{
    if (mFonts.count(fileName) > 0) return mFonts[fileName];
    UIFont *font = new UIFont(mRenderer);
    if (not font->Load(fileName)) {
        delete font;
        return nullptr;
    }

    mFonts[fileName] = font;
    return font;
}

void Game::UnloadScene()
{
    // Delete actors
    delete mSpatialHashing;
    mSpatialHashing = nullptr;

    // Delete UI screens
    for (auto ui : mUIStack) {
        delete ui;
    }
    mUIStack.clear();
    mPlayer = nullptr;
    mHUD = nullptr;

    // Delete background texture
    if (mBackgroundTexture) {
        SDL_DestroyTexture(mBackgroundTexture);
        mBackgroundTexture = nullptr;
    }
}

void Game::Shutdown()
{
    UnloadScene();

    for (auto font : mFonts) {
        font.second->Unload();
        delete font.second;
    }
    mFonts.clear();

    delete mAudio;
    mAudio = nullptr;

    Mix_CloseAudio();

    Mix_Quit();
    TTF_Quit();
    IMG_Quit();

    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}
