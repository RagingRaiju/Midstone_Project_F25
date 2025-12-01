/*
*   Complete remake player class by Shayan Hamayun
*/

#ifndef PLAYERBODY_H
#define PLAYERBODY_H

#define SDL_ENABLE_OLD_NAMES

#define _USE_MATH_DEFINES
#include <math.h>

#include <stdio.h>
#include "Body.h"
#include "GameManager.h"

#include "Weapon.h"
#include "Shotgun.h"
#include "Rifle.h"
#include "Handgun.h"
#include "Knife.h"

#include "SpriteAnimation.h"

class PlayerBody : public Body
{
protected:
    class GameManager* game;
private:
    void InitWeapons();
    void UpdateAimFromMouse();

    // Player Stats
    float moveAccel = 72.0f;   // How strong the player accelerates

    // Weapons: 3 slots
    static const int MAX_WEAPONS = 3;
    Weapon* weapons[MAX_WEAPONS] = { nullptr, nullptr, nullptr };
    int currentWeaponIndex = 0;

    // Animations
    // One set of animations per weapon slot
    struct WeaponAnimSet {
        SpriteAnimation idle;
        SpriteAnimation shoot;
        SpriteAnimation reload;
        SpriteAnimation melee;
        bool hasShoot = false;
        bool hasReload = false;
        bool hasMelee = false;
    };
    WeaponAnimSet weaponAnims[MAX_WEAPONS];

    SpriteAnimation feetIdleAnim;
    SpriteAnimation feetRunAnim;

    SpriteAnimation* activeFeetAnim = nullptr;
    SpriteAnimation* activeWeaponAnim = nullptr;

    // Weapon visual state
    enum class WeaponVisualState {
        Idle,
        Shooting,
        Reload,
        Melee
    };
    WeaponVisualState weaponVisualState = WeaponVisualState::Idle;
    float weaponVisualTimer = 0.0f;
    float weaponVisualDuration = 0.0f;

    // Debug shot line
    bool showShotRay = false;
    float shotRayTimer = 0.0f;
    Vec3  shotStart;
    Vec3  shotEnd;

public:
    PlayerBody() : Body{}
    {
        game = nullptr;
        InitWeapons();

    }

    // Note the last parameter in this constructor!
    // Look in GameManager.cpp to see how this is called.
    // Look in .cpp file, at Render(), to see why game_ is a parameter.
    PlayerBody(
        Vec3 pos_, Vec3 vel_, Vec3 accel_,
        float mass_,
        float radius_,
        float orientation_,
        float rotation_,
        float angular_,
        GameManager *game_
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
        , game{ game_ }
    {
        InitWeapons();
    }

    ~PlayerBody() {
        for (int i = 0; i < 3; ++i) {
            delete weapons[i];
            weapons[i] = nullptr;
        }
    }

    GameManager* GetGameManager() const { return game; }

    // use the base class versions of getters

    bool OnCreate();
    void Render( float scale = 1.0f );
    void HandleEvents( const SDL_Event& event );
    void Update( float deltaTime ); 

    // Weapon animation helpers (used by all weapons)
    void PlayWeaponIdle();
    void PlayWeaponShoot();
    void PlayWeaponReload();
    void PlayWeaponMelee();

    void RegisterShotRay(const Vec3& start, const Vec3& end);
    void SpawnBullet(const Vec3& startPos, const Vec3& dir, float speed, float bulletLife);

    // Input state from events
    bool moveUp = false;
    bool moveDown = false;
    bool moveLeft = false;
    bool moveRight = false;
};

#endif /* PLAYERBODY_H */
