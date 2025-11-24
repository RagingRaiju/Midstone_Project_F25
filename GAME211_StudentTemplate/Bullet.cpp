#include "Bullet.h"
#include <VMath.h>

Bullet::Bullet(const Vec3& startPos,
    const Vec3& direction,
    float speed,
    float maxLife)
    : Body(startPos,
        Vec3(0, 0, 0),
        Vec3(0, 0, 0),
        1.0f,
        0.1f,
        0.0f,
        0.0f,
        0.0f),
    life(maxLife),
    alive(true)
{
    Vec3 dir = direction;
    if (VMath::mag(dir) > 0.0f)
        dir = VMath::normalize(dir);
    else
        dir = Vec3(1, 0, 0);

    vel = dir * speed;
}


void Bullet::Update(float deltaTime) {
    if (!alive) return;

    pos = pos + vel * deltaTime;

    life -= deltaTime;
    if (life <= 0.0f) {
        alive = false;
    }
}

void Bullet::Render(SDL_Renderer* renderer, const Matrix4& projection) const {
    if (!alive) return;

    // Bullet start position in world -> screen
    Vec3 startScreen = projection * pos;

    // Direction from velocity (normalized)
    Vec3 dir = vel;
    if (VMath::mag(dir) > 0.0f) {
        dir = VMath::normalize(dir);
    }

    // How long the bullet appears on screen (pixels in world space)
    const float bulletLengthWorld = 0.4f;

    Vec3 endWorld = pos + dir * bulletLengthWorld;
    Vec3 endScreen = projection * endWorld;

    // Save old color
    Uint8 oldR, oldG, oldB, oldA;
    SDL_GetRenderDrawColor(renderer, &oldR, &oldG, &oldB, &oldA);

    // White (or pick any color)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    SDL_RenderDrawLineF(renderer,
        startScreen.x, startScreen.y,
        endScreen.x, endScreen.y);

    // Restore previous color
    SDL_SetRenderDrawColor(renderer, oldR, oldG, oldB, oldA);
}
