#pragma once
#ifndef RIFLE_H
#define RIFLE_H

#include "Weapon.h"

class Rifle : public Weapon {
private:
    const float bulletSpeed = 100.0f;
    const float bulletLife = 1.4f;
    const float damage = 24.0f;

    const float forwardOffset = 0.525f; // how far out from the body
    const float sideOffset = 0.6f; // how far to the right of the center

    // ammo variables
    const int magSize = 30;
    int magRemaining = magSize; // spawn with a full mag of course

    const float reloadTime = 2.0f;
    float timeSinceInitatedReload = 0.0f;
    bool reloading = false;

    // NEW: for automatic fire
    bool triggerHeld = false;
public:
    Rifle(PlayerBody* owner_) :
        Weapon(owner_, damage, 0.1f, "Rifle") {
    }

    void Update(float deltaTime) override;
    void Fire() override;
    void Reload();

    void HandleEvents(const SDL_Event& event) override;
};

#endif
