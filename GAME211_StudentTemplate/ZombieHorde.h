#ifndef ZOMBIEHORDE_H
#define ZOMBIEHORDE_H

#include "Zombie.h"

class ZombieHorde
{
public:
	Zombie* CreateHorde(int numZombies, Vec3 playerPos, GameManager* game, SDL_Renderer* renderer);
};

#endif // ZOMBIEHORDE_H