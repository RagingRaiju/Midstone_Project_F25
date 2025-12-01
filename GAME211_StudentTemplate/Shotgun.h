#pragma once
#ifndef SHOTGUN_H
#define SHOTGUN_H

#include "Weapon.h"

class Shotgun : public Weapon {
private:
    const float bulletSpeed = 52.0f;
    const float bulletLife = 0.11f;
    const float damage = 12.5f; // 100 / 8 = 12.5; meaning if all 8 pellets hit an enemy thats 100 dmg.
    const float pelletCount = 8; // how many pellets per shell

    const float forwardOffset = 1.9f; // how far out from the body
    const float sideOffset = 0.6f; // how far to the right of the center

    // ammo variables
    const int magSize = 5;
    int magRemaining = magSize; // spawn with a full mag of course

    const float reloadTime = 1.8f;
    float timeSinceInitatedReload = 0.0f;
    bool reloading = false;
public:
    Shotgun(PlayerBody* owner_) :
        Weapon(owner_, damage, 0.8f, "Shotgun") {
    }

    void Update(float deltaTime) override;
    void Fire() override;
    void Reload();

    void HandleEvents(const SDL_Event& event) override;
};

#endif
