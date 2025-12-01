/*
*   Complete remake of the Player class by Shayan Hamayun with sprite, animations, and etc.
*/

#include "PlayerBody.h"
#include "TextureHolder.h"
#include <format>

const float RUN_START_SPEED_SQ = 4.0f;  // must exceed this to go to run
const float RUN_STOP_SPEED_SQ = 1.0f;  // must drop below this to go idle

bool PlayerBody::OnCreate() {
    SDL_Renderer* renderer = game->getRenderer();

    // --- Load weapon idle / action animations ---
    // slot 1: Rifle / Shotgun
    {
        std::string slot1WeaponName = "shotgun";
        WeaponAnimSet& set = weaponAnims[0];
        if (!set.idle.LoadFromFolder(renderer,
            std::format("Graphics/PlayerSprites/shotgun/idle", slot1WeaponName), 0.08f)) {
            std::cerr << "Failed to load shotgun idle animation\n";
            return false;
        }
        if (!set.shoot.LoadFromFolder(renderer,
            std::format("Graphics/PlayerSprites/shotgun/shoot", slot1WeaponName), 0.05f, false)) {
            std::cerr << "Failed to load shotgun shoot animation\n";
            return false;
        }
        if (!set.reload.LoadFromFolder(renderer,
            std::format("Graphics/PlayerSprites/shotgun/reload", slot1WeaponName), 0.1f, false)) {
            std::cerr << "Failed to load shotgun reload animation\n";
            return false;
        }
    //image = IMG_Load("Pacman.png");
    //if (image == nullptr) {
    //    std::cerr << "Can't open the image" << std::endl;
    //    return false;
    //}

        // optional offsets for all rifle/shotgun anims
        set.idle.SetOffsetPixels(25.0f, -6.0f);
        set.shoot.SetOffsetPixels(25.0f, -6.0f);
        set.reload.SetOffsetPixels(25.0f, -6.0f);

        set.hasShoot = true;
        set.hasReload = true;
        set.hasMelee = false;
    }

    // slot 2: Handgun
    {
        WeaponAnimSet& set = weaponAnims[1];

        if (!set.idle.LoadFromFolder(renderer,
            "Graphics/PlayerSprites/handgun/idle", 0.08f)) {
            std::cerr << "Failed to load handgun idle animation\n";
            return false;
        }
        if (!set.shoot.LoadFromFolder(renderer,
            "Graphics/PlayerSprites/handgun/shoot", 0.05f, false)) {
            std::cerr << "Failed to load handgun shoot animation\n";
            return false;
        }
        if (!set.reload.LoadFromFolder(renderer,
            "Graphics/PlayerSprites/handgun/reload", 0.1f, false)) {
            std::cerr << "Failed to load handgun reload animation\n";
            return false;
        }

        set.hasShoot = true;
        set.hasReload = true;
        set.hasMelee = false;
    }

    // slot 3: Knife
    {
        WeaponAnimSet& set = weaponAnims[2];

        if (!set.idle.LoadFromFolder(renderer,
            "Graphics/PlayerSprites/knife/idle", 0.08f)) {
            std::cerr << "Failed to load knife idle animation\n";
            return false;
        }
        if (!set.melee.LoadFromFolder(renderer,
            "Graphics/PlayerSprites/knife/meleeattack", 0.04f, false)) {
            std::cerr << "Failed to load knife melee animation\n";
            return false;
        }
        set.melee.SetOffsetPixels(19.0f, 35.0f);

        set.hasShoot = false;
        set.hasReload = false;
        set.hasMelee = true;
    }

    // Set active animations & state
    activeFeetAnim = &feetIdleAnim;
    currentWeaponIndex = 1;                // pistol start
    PlayWeaponIdle();                      // sets activeWeaponAnim etc.

    image = nullptr;
    texture = nullptr;
    SDL_Renderer* renderer = game->getRenderer();
    //texture = SDL_CreateTextureFromSurface(renderer, image);
    texture = TextureHolder::GetTexture("Pacman.png", renderer);
    if (!texture) {
        std::cerr << "Failed to create texture\n";
        return false;
    }

    return true;
}

void PlayerBody::InitWeapons() {
    weapons[0] = new Shotgun(this);          // main slot (1)
    weapons[1] = new Handgun(this); // 2
    weapons[2] = new Knife(this);  // 3
    currentWeaponIndex = 1;        // start with pistol for example
}

void PlayerBody::SpawnBullet(const Vec3& startPos, const Vec3& dir, float speed, float bulletLife) {
    if (game) {
        game->SpawnBullet(startPos, dir, speed, bulletLife);
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
        case SDL_SCANCODE_1:
            currentWeaponIndex = 0;
            PlayWeaponIdle();
            break;
        case SDL_SCANCODE_2:
            currentWeaponIndex = 1;
            PlayWeaponIdle();
            break;
        case SDL_SCANCODE_3:
            currentWeaponIndex = 2;
            PlayWeaponIdle();
            break;

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

    // Weapon visual timer: when a one-shot anim finishes, go back to idle
    if (weaponVisualState != WeaponVisualState::Idle) {
        weaponVisualTimer += deltaTime;
        if (weaponVisualTimer >= weaponVisualDuration) {
            PlayWeaponIdle();
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

void PlayerBody::PlayWeaponIdle() {
    WeaponAnimSet& set = weaponAnims[currentWeaponIndex];
    activeWeaponAnim = &set.idle;
    weaponVisualState = WeaponVisualState::Idle;
    weaponVisualTimer = 0.0f;
    weaponVisualDuration = 0.0f; // idle loops forever
}

void PlayerBody::PlayWeaponShoot() {
    WeaponAnimSet& set = weaponAnims[currentWeaponIndex];

    if (!set.hasShoot) {
        PlayWeaponIdle();
        return;
    }

    activeWeaponAnim = &set.shoot;
    set.shoot.Reset();

    weaponVisualState = WeaponVisualState::Shooting;
    weaponVisualTimer = 0.0f;
    weaponVisualDuration =
        set.shoot.GetFrameCount() * set.shoot.GetFrameTime();
}

void PlayerBody::PlayWeaponReload() {
    WeaponAnimSet& set = weaponAnims[currentWeaponIndex];

    if (!set.hasReload) {
        PlayWeaponIdle();
        return;
    }

    activeWeaponAnim = &set.reload;
    set.reload.Reset();

    weaponVisualState = WeaponVisualState::Reload;
    weaponVisualTimer = 0.0f;
    weaponVisualDuration =
        set.reload.GetFrameCount() * set.reload.GetFrameTime();
}

void PlayerBody::PlayWeaponMelee() {
    WeaponAnimSet& set = weaponAnims[currentWeaponIndex];

    if (!set.hasMelee) {
        PlayWeaponIdle();
        return;
    }

    activeWeaponAnim = &set.melee;
    set.melee.Reset();

    weaponVisualState = WeaponVisualState::Melee;
    weaponVisualTimer = 0.0f;
    weaponVisualDuration =
        set.melee.GetFrameCount() * set.melee.GetFrameTime();
}


void PlayerBody::RegisterShotRay(const Vec3& start, const Vec3& end) {
    shotStart = start;
    shotEnd = end;
    shotRayTimer = 0.2f;
    showShotRay = true;
}
