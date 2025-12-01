#include "Rifle.h"
#include "GameManager.h"
#include "Player.h"
#include <iostream>

void Rifle::Reload() {
    if (reloading) return; // return if its already reloading
    if (magRemaining == magSize) return;

    reloading = true;
    timeSinceInitatedReload = 0.0f;
    owner->PlayWeaponReload();
}

void Rifle::Update(float deltaTime) {
    // base Weapon cooldown timer
    Weapon::Update(deltaTime);  // increases timeSinceLastShot up to cooldown

    // handle reload timing
    if (reloading) {
        timeSinceInitatedReload += deltaTime;

        if (timeSinceInitatedReload >= reloadTime) {
            magRemaining = magSize;
            reloading = false;
            timeSinceInitatedReload = 0.0f;
        }
    }

    // automatic fire
    if (triggerHeld && !reloading &&
        timeSinceLastShot >= cooldown)
    {
        Fire(); // will decrement mag & maybe start reload
        timeSinceLastShot = 0.0f;  // reset cooldown
    }
}

void Rifle::Fire() {
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

    // Spawn bullet from muzzle toward cursor
    owner->SpawnBullet(muzzlePos, bulletDir, damage, bulletSpeed, bulletLife);

    // debug ray
    // float maxDist = 50.0f;
    // Vec3 end = origin + dir * maxDist;
    // owner->RegisterShotRay(origin, end);
}

void Rifle::HandleEvents(const SDL_Event& event) {
    // LMB pressed: start firing
    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN &&
        event.button.button == SDL_BUTTON_LEFT)
    {
        triggerHeld = true;
    }
    // LMB released: stop firing
    else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP &&
        event.button.button == SDL_BUTTON_LEFT)
    {
        triggerHeld = false;
    }

    // Reload on R
    if (event.type == SDL_EVENT_KEY_DOWN &&
        !event.key.repeat &&
        event.key.scancode == SDL_SCANCODE_R)
    {
        Reload();
    }
}
