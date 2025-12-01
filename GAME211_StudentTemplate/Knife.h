#pragma once
#ifndef KNIFE_H
#define KNIFE_H

#include "Weapon.h"

class Knife : public Weapon {
public:
    Knife(Player* owner_) :
        Weapon(owner_, 30.0f, 0.6f, "Knife") {
    }

    void Fire() override;
};

#endif
