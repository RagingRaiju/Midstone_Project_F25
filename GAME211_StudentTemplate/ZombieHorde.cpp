#include "ZombieHorde.h"

Zombie* ZombieHorde::CreateHorde(int numZombies, Vec3 playerPos, GameManager* game, SDL_Renderer* renderer)
{
	Zombie* zombies = new Zombie[numZombies];

	int minX = playerPos.x - 20;
	int maxX = playerPos.x + 20;
	int minY = playerPos.y - 20;
	int maxY = playerPos.y + 20;

	srand(time(0)); // seed once

	for (int i = 0; i < numZombies; i++)
	{
		int side = rand() % 4;
		float x, y;

		switch (side)
		{
		case 0: // left
			x = minX;
			y = (rand() % (maxY - minY)) + minY;
			break;
		case 1: // right
			x = maxX;
			y = (rand() % (maxY - minY)) + minY;
			break;
		case 2: // top
			x = (rand() % (maxX - minX)) + minX;
			y = minY;
			break;
		case 3: // bottom
			x = (rand() % (maxX - minX)) + minX;
			y = maxY;
			break;
		}

		int type = rand() % 3; // Bloater, Chaser, Crawler

		zombies[i].Init(game, renderer);
		zombies[i].OnCreate(x, y, type, i);

		// Assign stats based on type
		switch (type) {

		case 0:
			zombies[i].setStats(zombies[i].getBloaterHealth(), zombies[i].getBloaterSpeed());
			break;

		case 1:
			zombies[i].setStats(zombies[i].getChaserHealth(), zombies[i].getChaserSpeed());
			break;

		case 2:
			zombies[i].setStats(zombies[i].getCrawlerHealth(), zombies[i].getCrawlerSpeed());
			break;
		}
	}

	return zombies;
}