#include "GameManager.h"
#include "Scene1.h"

GameManager::GameManager() {
	windowPtr = nullptr;
	timer = nullptr;
	isRunning = true;
	currentScene = nullptr;
    player = nullptr;
}

bool GameManager::OnCreate() {
    // My display is 1920 x 1080 but the following seems to work best to fill the screen.
    //const int SCREEN_WIDTH = 1540;
    //const int SCREEN_HEIGHT = 860;

    // Use 1000x600 for less than full screen
    const int SCREEN_WIDTH = 1000;
    const int SCREEN_HEIGHT = 600;

    windowPtr = new Window(SCREEN_WIDTH, SCREEN_HEIGHT);
	if (windowPtr == nullptr) {
		OnDestroy();
		return false;
	}
	if (windowPtr->OnCreate() == false) {
		OnDestroy();
		return false;
	}

	timer = new Timer();
	if (timer == nullptr) {
		OnDestroy();
		return false;
	}

    SDL_Surface* bgSurface = IMG_Load("graphics/background_sheet.png");
    if (!bgSurface) {
        std::cerr << "Failed to load background: " << SDL_GetError() << std::endl;
        OnDestroy();
        return false;
    }

    backgroundTexture = SDL_CreateTextureFromSurface(windowPtr->GetRenderer(), bgSurface);
    SDL_FreeSurface(bgSurface);

    if (!backgroundTexture) {
        std::cerr << "Failed to create background texture: " << SDL_GetError() << std::endl;
        OnDestroy();
        return false;
    }

    currentScene = new Scene1(windowPtr->GetSDL_Window(), this);
    
    // create player
    float mass = 1.0f;
    float radius = 0.5f;
    float orientation = 0.0f;
    float rotation = 0.0f;
    float angular = 0.0f;
    Vec3 position(0.5f * currentScene->getxAxis(), 0.5f * currentScene->getyAxis(), 0.0f);
    Vec3 velocity(0.0f, 0.0f, 0.0f);
    Vec3 acceleration(0.0f, 0.0f, 0.0f);

    player = new PlayerBody
    (
        position,
        velocity,
        acceleration,
        mass,
        radius,
        orientation,
        rotation,
        angular,
        this
    );
    if ( player->OnCreate() == false ) {
        OnDestroy();
        return false;
    }

    // need to create Player before validating scene
    if (!ValidateCurrentScene()) {
        OnDestroy();
        return false;
    }
           
	return true;
}

void GameManager::SpawnBullet(const Vec3& startPos, const Vec3& dir, float speed, float bulletLife) {
    Scene1* s1 = dynamic_cast<Scene1*>(currentScene);
    if (s1) {
        s1->SpawnBullet(startPos, dir, speed, bulletLife);
    }
}

/// Here's the whole game loop
void GameManager::Run() {

    const float FIXED_DT = 1.0f / 60.0f;   // 60 Hz physics
    float accumulator = 0.0f;

    timer->Start();

    while (isRunning) {

        handleEvents();

        // Measure time since last frame
        timer->UpdateFrameTicks();
        float frameTime = timer->GetDeltaTime();

        // Clamp to avoid spiral of death if the window stalls
        if (frameTime > 0.25f) {
            frameTime = 0.25f;
        }

        accumulator += frameTime;

        // Step physics in fixed-size chunks
        while (accumulator >= FIXED_DT) {
            currentScene->Update(FIXED_DT);
            accumulator -= FIXED_DT;
        }

        // Render as often as possible
        currentScene->Render();

        // tiny sleep so we don't peg the CPU at 100%
        SDL_Delay(1);
    }
}


void GameManager::handleEvents() 
{
    SDL_Event event;

    // Let's add mouse movement and position
    // https://wiki.libsdl.org/SDL_GetMouseState

    SDL_PumpEvents();  // make sure we have the latest mouse state.

    //https://www.youtube.com/watch?v=SYrRMr4BaD4&list=PLM7LHX-clszBIGsrh7_3B2Pi74AhMpKhj&index=3

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            isRunning = false;
        }
        else if (event.type == SDL_KEYDOWN)
        {
            switch (event.key.scancode)
            {
            case SDL_SCANCODE_ESCAPE:
                isRunning = false;
                break;
            case SDL_SCANCODE_Q:
                isRunning = false;
                break;
            case SDL_SCANCODE_DELETE:
                isRunning = false;
                break;
            case SDL_SCANCODE_0: // I made this 0 instead of 1 for conflicting keybinds with the player weapon handling. (Shayan)
                LoadScene(1);
                break;
            default:
                break;
            }
        }
        currentScene->HandleEvents(event);
    }
}

GameManager::~GameManager() {}

void GameManager::OnDestroy(){
	if (windowPtr) delete windowPtr;
	if (timer) delete timer;
	if (currentScene) delete currentScene;
}

// This might be unfamiliar
float GameManager::getSceneHeight() { return currentScene->getyAxis(); }

// This might be unfamiliar
float GameManager::getSceneWidth() { return currentScene->getxAxis(); }

// This might be unfamiliar
Matrix4 GameManager::getProjectionMatrix()
{ return currentScene->getProjectionMatrix(); }

// This might be unfamiliar
SDL_Renderer* GameManager::getRenderer()
{
    // [TODO] might be missing some SDL error checking
    SDL_Window* window = currentScene->getWindow();
    SDL_Renderer* renderer = SDL_GetRenderer(window);
    return renderer;
}

// This might be unfamiliar
void GameManager::RenderPlayer(float scale)
{
    player->Render(scale);
}

void GameManager::LoadScene( int i )
{
    // cleanup of current scene before loading another one
    currentScene->OnDestroy();
    delete currentScene;

    switch ( i )
    {
        case 1:
            currentScene = new Scene1( windowPtr->GetSDL_Window(), this);
            break;
        default:
            currentScene = new Scene1( windowPtr->GetSDL_Window(), this );
            break;
    }

    // using ValidateCurrentScene() to safely run OnCreate
    if (!ValidateCurrentScene())
    {
        isRunning = false;
    }
}

bool GameManager::ValidateCurrentScene()
{
    if (currentScene == nullptr) {
        return false;
    }
    if (currentScene->OnCreate() == false) {
        return false;
    }
    return true;
}