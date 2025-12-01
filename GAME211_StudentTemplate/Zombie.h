#ifndef ZOMBIE_H
#define ZOMBIE_H

#include <math.h>

#include <stdio.h>
#include "Body.h"
#include "GameManager.h"

class Zombie : public Body
{
protected:
    class GameManager* game;

private:

    // --- Baseline movement speeds for zombie archetypes ---
    // These constants define the intended starting speed for each type.
    // Used when initializing velocity in ZombieHorde.
    const float BLOATER_SPEED = 4.0f;   // Slow but durable
    const float CHASER_SPEED = 8.0f;   // Fast but fragile
    const float CRAWLER_SPEED = 2.0f;   // Very slow, moderate resilience

    // --- Baseline health values for zombie archetypes ---
    // These constants set the initial health pool for each type.
    // Actual health decreases during gameplay as zombies take damage.
    const float BLOATER_HEALTH = 150.0f;   // High endurance
    const float CHASER_HEALTH = 50.0f;   // Low endurance
    const float CRAWLER_HEALTH = 100.0f;   // Moderate endurance

    // --- Variance speed values for varying individual velocity ---
    const int MAX_VARIANCE = 30;
    const int OFFSET = 101 - MAX_VARIANCE;

    // --- Runtime state for this zombie instance ---
    // Current health, initialized from the type constant, updated as damage is applied
    float health{};

    float speed{};

    // Flag indicating whether this zombie is active in the game world
    // Used to skip updates/rendering once the zombie is "dead"
    bool alive{ false };

    SDL_Renderer* renderer{ nullptr };
    SDL_Texture* texture{ nullptr };

public:
    Zombie() : Body{} {
        game = nullptr;
    }

    Zombie(
        Vec3 pos_,
        Vec3 vel_,
        Vec3 accel_,
        float mass_,
        float radius_,
        float orientation_,
        float rotation_,
        float angular_,
        GameManager* game_,
        SDL_Renderer* renderer_
    ) : Body{
          pos_
        , vel_
        , accel_
        , mass_
        , radius_
        , orientation_
        , rotation_
        , angular_
    }
        , game{ game_ }, renderer{renderer_}
    {
    }

    // Handle bullet collision with zombie
    bool Hit(float damage);

    // Find out if zombie is alive
    bool IsAlive();

    bool OnCreate(float startX, float starY, int type, int seed);
    void Render(float scale = 1.0f);
    //void HandleEvents(const SDL_Event& event);
    void Update(float deltaTime, Vec3 playerPos);

    // --- Getter implementations for archetype constants ---

    void Init(GameManager* game_, SDL_Renderer* renderer_) {
        game = game_;
        renderer = renderer_;
    }
    void setStats(float health_, float speed_);

    float getBloaterSpeed() const { return BLOATER_SPEED; };
    float getBloaterHealth() { return BLOATER_HEALTH; }

    float getChaserSpeed() { return CHASER_SPEED; }
    float getChaserHealth() { return CHASER_HEALTH; }

    float getCrawlerSpeed() { return CRAWLER_SPEED; }
    float getCrawlerHealth() { return CRAWLER_HEALTH; }
};

#endif // ZOMBIE_H