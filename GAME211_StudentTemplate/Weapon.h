#pragma once
#ifndef WEAPON_H
#define WEAPON_H

#include <string>
#include <SDL.h>
#include <MMath.h>

using namespace MATH;

class PlayerBody;

class Weapon {
protected:
    PlayerBody* owner;

    float damage;
    float cooldown;
    float timeSinceLastShot;

    std::string weaponName;

public:
    Weapon(PlayerBody* owner_, float damage_, float cooldown_, const std::string& name_)
        : owner(owner_), damage(damage_), cooldown(cooldown_),
        timeSinceLastShot(0.0f), weaponName(name_) {
    }

    virtual ~Weapon() {}

    virtual void Update(float deltaTime) {
        // update the cooldown timer
        if (timeSinceLastShot < cooldown) {
            timeSinceLastShot += deltaTime;
        }
    }

    virtual void HandleEvents(const SDL_Event& event) {
        if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN &&
            event.button.button == SDL_BUTTON_LEFT)
        {
            TryFire();
        }
    }

    void TryFire() {
        // ensure the player can fire the weapon
        if (timeSinceLastShot >= cooldown) {
            Fire();
            timeSinceLastShot = 0.0f;
        }
    }

    virtual void Fire() = 0; // subclasses for particular weapons will have different firing methods.

    std::string GetName() const { return weaponName; }
};

#endif

