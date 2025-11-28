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

    // Weapons: 3 slots
    Weapon* weapons[3] = { nullptr, nullptr, nullptr };
    int currentWeaponIndex = 0;

    // Stats
    float moveAccel = 80.0f;   // how strong the acceleration feels

    // Animations
    SpriteAnimation feetIdleAnim;
    SpriteAnimation feetRunAnim;

    SpriteAnimation handgunIdleAnim;
    SpriteAnimation handgunShootAnim;
    SpriteAnimation handgunReloadAnim;

    SpriteAnimation knifeIdleAnim;
    SpriteAnimation knifeMeleeAnim;

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

    // Weapon stuff
    void OnPistolFired();
    void OnPistolReload();

    void OnKnifeMelee();
    void RegisterShotRay(const Vec3& start, const Vec3& end);
    void SpawnBullet(const Vec3& startPos, const Vec3& dir, float speed);

    // Input state from events
    bool moveUp = false;
    bool moveDown = false;
    bool moveLeft = false;
    bool moveRight = false;
};

#endif /* PLAYERBODY_H */
