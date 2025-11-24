#include "Knife.h"
#include "PlayerBody.h"
#include <iostream>

void Knife::Fire() {
    if (!owner) return;

    // Trigger melee animation
    owner->OnKnifeMelee();

    // Short ray in facing direction (melee reach)
    Vec3 origin = owner->getPos();
    float angle = owner->getOrientation();
    Vec3 dir(std::cos(angle), std::sin(angle), 0.0f);

    float maxDist = 2.5f; // melee range
    Vec3 end = origin + dir * maxDist;

    owner->RegisterShotRay(origin, end);

}
