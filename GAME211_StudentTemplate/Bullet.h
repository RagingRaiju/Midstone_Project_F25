#ifndef BULLET_H
#define BULLET_H

#define SDL_ENABLE_OLD_NAMES

#include "Body.h"
#include <SDL.h>
#include <MMath.h>

using namespace MATH;

class Bullet : public Body {
public:
    Bullet(const Vec3& startPos,
        const Vec3& direction,
        const float damage,
        float speed,
        float maxLife);

    ~Bullet() override = default;

    void Update(float deltaTime) override;
    void Render(SDL_Renderer* renderer, const Matrix4& projection) const;
    bool IsAlive() const { return alive; }

    // Zombie Handling
    float GetDamage() const { return dmg; }
    void MarkDead() { alive = false; }

private:
    const float dmg;    // amount of damage the bullet deals
    float life;   // remaining lifetime in seconds
    bool  alive;  // false when bullet should be removed
};

#endif
