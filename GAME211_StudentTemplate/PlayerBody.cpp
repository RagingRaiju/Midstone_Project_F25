/*
*   Complete remake of the Player class by Shayan Hamayun with sprite, animations, and etc.
*/

#include "PlayerBody.h"

const float RUN_START_SPEED_SQ = 4.0f;  // must exceed this to go to run
const float RUN_STOP_SPEED_SQ = 1.0f;  // must drop below this to go idle

bool PlayerBody::OnCreate() {
    SDL_Renderer* renderer = game->getRenderer();

    // --- Load feet animations ---
    if (!feetIdleAnim.LoadFromFolder(renderer,
        "Graphics/PlayerSprites/feet/idle", 0.08f)) {
        std::cerr << "Failed to load feet idle animation\n";
        return false;
    }
    if (!feetRunAnim.LoadFromFolder(renderer,
        "Graphics/PlayerSprites/feet/run", 0.06f)) {
        std::cerr << "Failed to load feet run animation\n";
        return false;
    }

    // --- Load weapon idle / action animations ---
    if (!handgunIdleAnim.LoadFromFolder(renderer,
        "Graphics/PlayerSprites/handgun/idle", 0.08f)) {
        std::cerr << "Failed to load handgun idle animation\n";
        return false;
    }
    if (!handgunShootAnim.LoadFromFolder(renderer,
        "Graphics/PlayerSprites/handgun/shoot", 0.05f, false)) {
        std::cerr << "Failed to load handgun shoot animation\n";
        return false;
    }
    if (!handgunReloadAnim.LoadFromFolder(renderer,
        "Graphics/PlayerSprites/handgun/reload", 0.1f, false)) {
        std::cerr << "Failed to load handgun reload animation\n";
        return false;
    }

    if (!knifeIdleAnim.LoadFromFolder(renderer,
        "Graphics/PlayerSprites/knife/idle", 0.08f)) {
        std::cerr << "Failed to load knife idle animation\n";
        return false;
    }
    if (!knifeMeleeAnim.LoadFromFolder(renderer,
        "Graphics/PlayerSprites/knife/meleeattack", 0.04f, false)) {
        std::cerr << "Failed to load knife melee animation\n";
        return false;
    }
    knifeMeleeAnim.SetOffsetPixels(19.0f, 35.0f);

    // Set active animations & state
    activeFeetAnim = &feetIdleAnim;
    activeWeaponAnim = &handgunIdleAnim;        // matches currentWeaponIndex = 1
    weaponVisualState = WeaponVisualState::Idle;

    image = nullptr;  // no longer used
    texture = nullptr;
    return true;
}

void PlayerBody::InitWeapons() {
    weapons[0] = nullptr;          // main slot (1)
    weapons[1] = new Handgun(this); // 2
    weapons[2] = new Knife(this);  // 3
    currentWeaponIndex = 1;        // start with pistol for example
}

void PlayerBody::SpawnBullet(const Vec3& startPos, const Vec3& dir, float speed) {
    if (game) {
        game->SpawnBullet(startPos, dir, speed);
    }
}

void PlayerBody::Render(float scale) {
    SDL_Renderer* renderer = game->getRenderer();
    Matrix4 projectionMatrix = game->getProjectionMatrix();

    Vec3 screenCoords = projectionMatrix * pos;

    float angleDegrees = orientation * 180.0f / static_cast<float>(M_PI);

    // Feet
    if (activeFeetAnim && activeFeetAnim->IsValid()) {
        activeFeetAnim->RenderRotated(renderer, screenCoords.x, screenCoords.y,
            scale, -angleDegrees);
    }

    // Weapon / upper body
    if (activeWeaponAnim && activeWeaponAnim->IsValid()) {
        activeWeaponAnim->RenderRotated(renderer, screenCoords.x, screenCoords.y,
            scale, -angleDegrees);
    }

    // Debug ray (with red color as we added before)
    if (showShotRay) {
        Uint8 oldR, oldG, oldB, oldA;
        SDL_GetRenderDrawColor(renderer, &oldR, &oldG, &oldB, &oldA);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        Vec3 s = projectionMatrix * shotStart;
        Vec3 e = projectionMatrix * shotEnd;
        SDL_RenderDrawLineF(renderer, s.x, s.y, e.x, e.y);

        SDL_SetRenderDrawColor(renderer, oldR, oldG, oldB, oldA);
    }
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

void PlayerBody::Update(float deltaTime) {
    // Start with no acceleration each frame
    Vec3 dir(0.0f, 0.0f, 0.0f);
    UpdateAimFromMouse();

    // Build a direction vector from input
    if (moveUp)    dir.y += 1.0f;   // up (y+)
    if (moveDown)  dir.y -= 1.0f;   // down (y-)
    if (moveLeft)  dir.x -= 1.0f;   // left (x-)
    if (moveRight) dir.x += 1.0f;   // right (x+)

    if (VMath::mag(dir) > 0.0f) {
        dir = VMath::normalize(dir);       // length = 1, even on diagonals
        accel = dir * moveAccel;           // constant accel magnitude
    }
    else {
        accel = Vec3(0.0f, 0.0f, 0.0f);
    }

    // simple damping
    vel *= 0.925f;

    // Update all weapons (for cooldown timers, etc.)
    for (int i = 0; i < 3; ++i) {
        if (weapons[i]) {
            weapons[i]->Update(deltaTime);
        }
    }

    float speedSq = vel.x * vel.x + vel.y * vel.y;

    if (activeFeetAnim == &feetRunAnim) {
        // currently running, only go idle if we're clearly slow
        if (speedSq < RUN_STOP_SPEED_SQ) {
            activeFeetAnim = &feetIdleAnim;
            activeFeetAnim->Reset(); // optional, to start idle from frame 0
        }
    }
    else {
        // currently idle, only go to run if we're clearly moving
        if (speedSq > RUN_START_SPEED_SQ) {
            activeFeetAnim = &feetRunAnim;
            activeFeetAnim->Reset(); // optional, to start run from frame 0
        }
    }

    // Choose weapon animation based on weapon + visual state
    switch (currentWeaponIndex) {
    case 1: // pistol
        if (weaponVisualState == WeaponVisualState::Reload) {
            activeWeaponAnim = &handgunReloadAnim;
        }
        else if (weaponVisualState == WeaponVisualState::Shooting) {
            activeWeaponAnim = &handgunShootAnim;
        }
        else {
            activeWeaponAnim = &handgunIdleAnim;
        }
        break;
    case 2: // knife
        if (weaponVisualState == WeaponVisualState::Melee ||
            weaponVisualState == WeaponVisualState::Shooting) {
            activeWeaponAnim = &knifeMeleeAnim;
        }
        else {
            activeWeaponAnim = &knifeIdleAnim;
        }
        break;
    default:
        activeWeaponAnim = nullptr;
        break;
    }

    // Update weapon visual timer
    if (weaponVisualState != WeaponVisualState::Idle) {
        weaponVisualTimer += deltaTime;
        if (weaponVisualTimer >= weaponVisualDuration) {
            weaponVisualState = WeaponVisualState::Idle;
            weaponVisualTimer = 0.0f;
            weaponVisualDuration = 0.0f;
        }
    }

    // Update animations
    if (activeFeetAnim)   activeFeetAnim->Update(deltaTime);
    if (activeWeaponAnim) activeWeaponAnim->Update(deltaTime);

    // Fade out debug shot ray
    if (showShotRay) {
        shotRayTimer -= deltaTime;
        if (shotRayTimer <= 0.0f) {
            showShotRay = false;
        }
    }

    // Update physics
    Body::Update(deltaTime);
}

void PlayerBody::UpdateAimFromMouse() {
    // Get mouse in screen space
    float mx, my;
    SDL_GetMouseState(&mx, &my);

    // Convert mouse to world space using inverse projection
    Matrix4 proj = game->getProjectionMatrix();
    Matrix4 invProj = MMath::inverse(proj);
    Vec3 mouseWorld = invProj * Vec3(static_cast<float>(mx), static_cast<float>(my), 0.0f);

    Vec3 toMouse = mouseWorld - pos;
    if (VMath::mag(toMouse) > 0.0001f) {
        orientation = std::atan2(toMouse.y, toMouse.x); // radians, y-up in world
    }
}

void PlayerBody::OnPistolFired() {
    weaponVisualState = WeaponVisualState::Shooting;
    weaponVisualTimer = 0.0f;
    weaponVisualDuration =
        handgunShootAnim.GetFrameCount() * handgunShootAnim.GetFrameTime();
    handgunShootAnim.Reset();
}

void PlayerBody::OnPistolReload() {
    weaponVisualState = WeaponVisualState::Reload;
    weaponVisualTimer = 0.0f;
    weaponVisualDuration =
        handgunReloadAnim.GetFrameCount() * handgunReloadAnim.GetFrameTime();
    handgunReloadAnim.Reset();
}

void PlayerBody::OnKnifeMelee() {
    weaponVisualState = WeaponVisualState::Melee;
    weaponVisualTimer = 0.0f;
    weaponVisualDuration =
        knifeMeleeAnim.GetFrameCount() * knifeMeleeAnim.GetFrameTime();
    knifeMeleeAnim.Reset();
}

void PlayerBody::RegisterShotRay(const Vec3& start, const Vec3& end) {
    shotStart = start;
    shotEnd = end;
    shotRayTimer = 0.2f;
    showShotRay = true;
}
