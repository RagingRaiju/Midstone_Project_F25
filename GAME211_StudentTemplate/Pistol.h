#pragma once
#ifndef PISTOL_H
#define PISTOL_H

#include "Weapon.h"

class Pistol : public Weapon {
public:
    Pistol(PlayerBody* owner_) :
        Weapon(owner_, 15.0f, 0.3f, "Pistol") {
    }

    void Fire() override;
};

#endif
