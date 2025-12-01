#include "Zombie.h"
#include "TextureHolder.h"
#include <cstdlib>
#include <ctime>
#include <MMath.h>
#include <SDL.h>

bool Zombie::Hit()
{
    health--;
    if (health < 0) {
        // Dead
        alive = false;
        texture = TextureHolder::GetTexture("Graphics/blood.png", renderer);
        return true;
    }

    // Injured, but not dead yet
    return false;
}

bool Zombie::IsAlive()
{
    return alive;
}

bool Zombie::OnCreate(float startX, float startY, int type, int seed) {
	//SDL_Renderer* renderer = renderer;
	switch (type) {

	case 0:
		// Bloater
		texture = TextureHolder::GetTexture("Graphics/bloater.png",renderer);
        if (!texture) {
            std::cerr << "Failed to create texture\n";
            return false;
        }
		break;

	case 1:
		// Chaser
		texture = TextureHolder::GetTexture("Graphics/chaser.png", renderer);
        if (!texture) {
            std::cerr << "Failed to create texture\n";
            return false;
        }
		break;

	case 2:
		// Crawler
		texture = TextureHolder::GetTexture("Graphics/crawler.png", renderer);
        if (!texture) {
            std::cerr << "Failed to create texture\n";
            return false;
        }
        break;
	}

    return true;
}

void Zombie::Render(float scale) {
    // This is why we need game in the constructor, to get the renderer, etc.
    //SDL_Renderer* renderer = renderer;
    Matrix4 projectionMatrix = game->getProjectionMatrix();

    // square represents the position and dimensions for where to draw the image
    SDL_FRect square;
    Vec3 screenCoords;
    float    w, h;

    // convert the position from game coords to screen coords.
    screenCoords = projectionMatrix * pos;

    float texW = 0;
    float texH = 0;

    if (SDL_GetTextureSize(texture, &texW, &texH)) {
        float w = texW * scale;
        float h = texH * scale;

        square.x = static_cast<int>(screenCoords.x - 0.5f * w);
        square.y = static_cast<int>(screenCoords.y - 0.5f * h);
        square.w = static_cast<int>(w);
        square.h = static_cast<int>(h);

        float orientationDegrees = -orientation;

        SDL_RenderTextureRotated(renderer, texture, nullptr, &square,
            orientationDegrees, nullptr, SDL_FLIP_NONE);
    }
 
}

void Zombie::Update(float deltaTime, Vec3 playerPos) {
    if (!alive) {
        return;
    }

    // 1. Compute direction toward player
    float dx = playerPos.x - pos.x;
    float dy = playerPos.y - pos.y;

    // 2. Normalize direction
    float length = sqrt(dx * dx + dy * dy);
    if (length > 0.0f) {
        dx /= length;
        dy /= length;
    }

    // 3. Apply type-specific speed (already stored in this zombie)
    vel.x = dx * speed;   // speed is your float constant (Bloater/Chaser/Crawler)
    vel.y = dy * speed;
    vel.z = 0.0f;         // keep zombies moving in 2D

    // 4. Integrate physics
    Body::Update(deltaTime);

    // 5. Update orientation (facing angle)
    float angle = atan2(dy, dx) * 180.0f / M_PI;
    orientation = angle;
}

void Zombie::setStats(float health_, float speed_) {
    health = health_;
    speed = speed_;
    alive = true;
}