//
//  PlayerBody.h
//  DemoAI
//
//  Created by Gail Harris on 2021-Dec-23.
//

#ifndef PLAYERBODY_H
#define PLAYERBODY_H

#define SDL_ENABLE_OLD_NAMES

#define _USE_MATH_DEFINES
#include <math.h>

#include <stdio.h>
#include "Body.h"
#include "GameManager.h"

#include "Weapon.h"
#include "Pistol.h"
#include "Knife.h"

class PlayerBody : public Body
{
protected:
    class GameManager* game;
private:
    void InitWeapons();
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
    
    // Weapons: 3 slots
    Weapon* weapons[3] = { nullptr, nullptr, nullptr };
    int currentWeaponIndex = 0;

    // use the base class versions of getters

    bool OnCreate();
    void Render( float scale = 1.0f );
    void HandleEvents( const SDL_Event& event );
    void Update( float deltaTime ); 

    // Tweakables
    float moveAccel = 80.0f;   // how strong the acceleration feels

    // Input state from events
    bool moveUp = false;
    bool moveDown = false;
    bool moveLeft = false;
    bool moveRight = false;
};

#endif /* PLAYERBODY_H */
