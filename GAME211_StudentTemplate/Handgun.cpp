#include "Handgun.h"
#include "GameManager.h"
#include "PlayerBody.h"
#include <iostream>

void Handgun::Fire() {
    if (!owner) return;

    // Trigger shoot animation
    owner->OnPistolFired();

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
    owner->SpawnBullet(muzzlePos, bulletDir, bulletSpeed);

    // debug ray
    // float maxDist = 50.0f;
    // Vec3 end = origin + dir * maxDist;
    // owner->RegisterShotRay(origin, end);
}
