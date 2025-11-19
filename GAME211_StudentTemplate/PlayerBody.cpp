//  PlayerBody.cpp
//  DemoAI
//
//  Created by Gail Harris on 2021-Dec-23.
//

#include "PlayerBody.h"

bool PlayerBody::OnCreate() {
    image = IMG_Load("Pacman.png");
    if (image == nullptr) {
        std::cerr << "Can't open the image" << std::endl;
        return false;
    }

    SDL_Renderer* renderer = game->getRenderer();
    texture = SDL_CreateTextureFromSurface(renderer, image);
    if (!texture) {
        std::cerr << "Failed to create texture\n";
        return false;
    }
    return true;
}

void PlayerBody::InitWeapons() {
    weapons[0] = nullptr;          // main slot (1)
    weapons[1] = new Pistol(this); // 2
    weapons[2] = new Knife(this);  // 3
    currentWeaponIndex = 1;        // start with pistol for example
}

void PlayerBody::Render( float scale )
{
    // This is why we need game in the constructor, to get the renderer, etc.
    SDL_Renderer *renderer = game->getRenderer();
    Matrix4 projectionMatrix = game->getProjectionMatrix();

    // square represents the position and dimensions for where to draw the image
    SDL_FRect square;
    Vec3 screenCoords;
    float    w, h;

    // convert the position from game coords to screen coords.
    screenCoords = projectionMatrix * pos;

    // Scale the image, in case the .png file is too big or small
    w = image->w * scale;
    h = image->h * scale;

    // The square's x and y values represent the top left corner of 
    // where SDL will draw the .png image.
    // The 0.5f * w/h offset is to place the .png so that pos represents the center
    // (Note the y axis for screen coords points downward, hence subtraction!!!!)
    square.x = static_cast<int>(screenCoords.x - 0.5f * w);
    square.y = static_cast<int>(screenCoords.y - 0.5f * h);
    square.w = static_cast<int>(w);
    square.h = static_cast<int>(h);

    // Convert character orientation from radians to degrees.
    float orientationDegrees = orientation * 180.0f / M_PI ;

    SDL_RenderCopyEx( renderer, texture, nullptr, &square,
        orientationDegrees, nullptr, SDL_FLIP_NONE );
}

void PlayerBody::HandleEvents(const SDL_Event& event)
{
    if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat) {
        switch (event.key.scancode) {
        case SDL_SCANCODE_W: moveUp = true;  break;
        case SDL_SCANCODE_S: moveDown = true;  break;
        case SDL_SCANCODE_A: moveLeft = true;  break;
        case SDL_SCANCODE_D: moveRight = true;  break;

        // Weapon slot switching
        case SDL_SCANCODE_1: currentWeaponIndex = 0; break;
        case SDL_SCANCODE_2: currentWeaponIndex = 1; break;
        case SDL_SCANCODE_3: currentWeaponIndex = 2; break;

        default: break;
        }
    }
    else if (event.type == SDL_EVENT_KEY_UP && !event.key.repeat) {
        switch (event.key.scancode) {
        case SDL_SCANCODE_W: moveUp = false; break;
        case SDL_SCANCODE_S: moveDown = false; break;
        case SDL_SCANCODE_A: moveLeft = false; break;
        case SDL_SCANCODE_D: moveRight = false; break;
        default: break;
        }
    }

    // after handling movement / switching:
    Weapon* active = weapons[currentWeaponIndex];
    if (active) {
        active->HandleEvents(event);
    }
}

void PlayerBody::Update(float deltaTime)
{
    // Start with no acceleration each frame
    accel = Vec3(0.0f, 0.0f, 0.0f);

    if (moveUp)    accel.y += moveAccel;   // up (y+)
    if (moveDown)  accel.y -= moveAccel;   // down (y-)
    if (moveLeft)  accel.x -= moveAccel;   // left (x-)
    if (moveRight) accel.x += moveAccel;   // right (x+)

    // simple damping so you don't accelerate forever
    vel *= 0.925f;

    /* UPDATE METHODS RUN UNDER HERE */
    for (int i = 0; i < 3; ++i) {
        if (weapons[i]) {
            weapons[i]->Update(deltaTime);
        }
    }
    Body::Update(deltaTime);
}
