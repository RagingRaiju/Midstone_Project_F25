#include "Shotgun.h"
#include "GameManager.h"
#include "PlayerBody.h"
#include <iostream>
#include <random>

// simple helper for random floats
static float RandRange(float minVal, float maxVal) {
    static std::mt19937 rng{ std::random_device{}() };
    std::uniform_real_distribution<float> dist(minVal, maxVal);
    return dist(rng);
}

void Shotgun::Reload() {
    if (reloading) return; // return if its already reloading
    if (magRemaining == magSize) return;

    reloading = true;
    timeSinceInitatedReload = 0.0f;
    owner->PlayWeaponReload();
}

void Shotgun::Update(float deltaTime) {
    // base Weapon cooldown
    Weapon::Update(deltaTime);

    // handle reload timing
    if (reloading) {
        timeSinceInitatedReload += deltaTime;

        if (timeSinceInitatedReload >= reloadTime) {
            // reload finished
            magRemaining = magSize;
            reloading = false;
            timeSinceInitatedReload = 0.0f;
        }
    }
}

void Shotgun::Fire() {
    if (!owner) return;

    // can’t fire while reloading
    if (reloading) return;

    // out of ammo? start reload on first attempt
    if (magRemaining <= 0) {
        Reload();
        return;
    }

    // Trigger shoot animation
    owner->PlayWeaponShoot();
    magRemaining--; // deduct bullet for shot

    Vec3 origin = owner->getPos();
    float bodyAngle = owner->getOrientation();   // radians

    Vec3 forward(std::cos(bodyAngle), std::sin(bodyAngle), 0.0f);
    if (VMath::mag(forward) > 0.0f) {
        forward = VMath::normalize(forward);
    }

    // Perpendicular right-vector (gun in right hand)
    Vec3 right(forward.y, -forward.x, 0.0f);

    Vec3 muzzlePos = origin
        + forward * forwardOffset
        + right * sideOffset;

    // Direction from muzzle to mouse
    GameManager* gm = owner->GetGameManager();
    if (!gm) return;

    Matrix4 proj = gm->getProjectionMatrix();
    Vec3 muzzleScreen = proj * muzzlePos;

    float mx, my;
    SDL_GetMouseState(&mx, &my);

    float dx = static_cast<float>(mx) - muzzleScreen.x;
    float dy = -static_cast<float>(my) + muzzleScreen.y;

    float distSq = dx * dx + dy * dy;
    const float minDist = 150.0f;
    const float minDistSq = minDist * minDist;

    Vec3 bulletDir;

    if (distSq < minDistSq) {
        // Cursor is very close to the muzzle: just shoot forward
        bulletDir = forward;
    }
    else {
        // Use precise direction to cursor
        float angleToMouse = std::atan2(dy, dx);
        bulletDir = Vec3(std::cos(angleToMouse),
            std::sin(angleToMouse),
            0.0f);
        if (VMath::mag(bulletDir) > 0.0f) {
            bulletDir = VMath::normalize(bulletDir);
        }
    }

    // angular spread in degrees (total cone width)
    const float spreadAngleDeg = 18.0f;
    const float spreadRad = spreadAngleDeg * static_cast<float>(M_PI) / 180.0f;

    // For each pellet, rotate bulletDir by a small random angle and add a tiny
    // positional jitter around the muzzle so they don't all stack perfectly.
    for (int i = 0; i < pelletCount; ++i) {

        // random angle offset in [-spreadRad/2, +spreadRad/2]
        float angleOffset = RandRange(-0.5f * spreadRad, 0.5f * spreadRad);

        float c = std::cos(angleOffset);
        float s = std::sin(angleOffset);

        // rotate the direction in 2D
        Vec3 dirRotated(
            bulletDir.x * c - bulletDir.y * s,
            bulletDir.x * s + bulletDir.y * c,
            0.0f
        );

        if (VMath::mag(dirRotated) > 0.0f) {
            dirRotated = VMath::normalize(dirRotated);
        }

        // slight random offset around muzzle (in screen-space right/forward)
        float sideJitter = RandRange(-0.15f, 0.15f);  // left/right
        float forwardJitter = RandRange(0.0f, 0.20f);    // a bit forward

        Vec3 pelletStart = muzzlePos
            + right * sideJitter
            + forward * forwardJitter;

        owner->SpawnBullet(pelletStart, dirRotated, bulletSpeed, bulletLife);
    }

    // debug ray
    // float maxDist = 50.0f;
    // Vec3 end = origin + dir * maxDist;
    // owner->RegisterShotRay(origin, end);
}

void Shotgun::HandleEvents(const SDL_Event& event) {
    // Let base class handle left-click firing, etc.
    Weapon::HandleEvents(event);

    // Extra Shotgun-specific input (e.g. reload on R)
    if (event.type == SDL_EVENT_KEY_DOWN &&
        !event.key.repeat &&
        event.key.scancode == SDL_SCANCODE_R)
    {
        Reload();
    }
}