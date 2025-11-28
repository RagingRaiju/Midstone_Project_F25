#pragma once
#ifndef HANDGUN_H
#define HANDGUN_H

#include "Weapon.h"

class Handgun : public Weapon {
private:
    const float bulletSpeed = 72.0f;

    const float forwardOffset = 1.25f; // how far out from the body
    const float sideOffset = 0.675f; // how far to the right of the center

    // ammo variables
    const int magSize = 20;
    int magRemaining = magSize; // spawn with a full mag of course

    const float reloadTime = 1.5f;
    float timeSinceInitatedReload = 0.0f;
    bool reloading = false;
public:
    Handgun(PlayerBody* owner_) :
        Weapon(owner_, 15.0f, 0.3f, "Handgun") {
    }

    void Update(float deltaTime) override;
    void Fire() override;
    void Reload();

    void HandleEvents(const SDL_Event& event) override;
};

#endif
