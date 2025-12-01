#include "Scene1.h"
#include <VMath.h>

// See notes about this constructor in Scene1.h.
Scene1::Scene1(SDL_Window* sdlWindow_, GameManager* game_){
	window = sdlWindow_;
    game = game_;
	renderer = SDL_GetRenderer(window);
	xAxis = 25.0f;
	yAxis = 15.0f;
}

Scene1::~Scene1(){
	for (Bullet* b : bullets) delete b;
	bullets.clear();
}

bool Scene1::OnCreate() {
	int w, h;
	SDL_GetWindowSize(window, &w, &h);

	/* OLD CAMERA CODE FOR STATIC NO MOVEMENT CAMERA (Im using this as a moving camera and player doesn't look right when the map is a static background */
	Matrix4 ndc = MMath::viewportNDC(w, h);
	Matrix4 ortho = MMath::orthographic(0.0f, xAxis, 0.0f, yAxis, 0.0f, 1.0f);
	projectionMatrix = ndc * ortho;

	// keep this around
	viewportMatrix = MMath::viewportNDC(w, h);

	// initial camera center at player
	PlayerBody* player = game->getPlayer();
	if (player) {
		cameraCenter = player->getPos();
	}
	else {
		cameraCenter = Vec3(0.0f, 0.0f, 0.0f);
	}

	/* THIS IS THE NEW MOVING CAMERA SYSTEM.
	// build an initial projection
	float halfW = xAxis * 0.5f;
	float halfH = yAxis * 0.5f;

	float left = cameraCenter.x - halfW;
	float right = cameraCenter.x + halfW;
	float bottom = cameraCenter.y - halfH;
	float top = cameraCenter.y + halfH;

	Matrix4 ortho = MMath::orthographic(left, right, bottom, top, 0.0f, 1.0f);
	projectionMatrix = viewportMatrix * ortho;
	*/

	bgRect.x = 0.0f;
	bgRect.y = 0.0f;

	// Create a horde of zombies
	numZombies = 10;

	// Delete the previously allocated memory (if it exists)
	delete[] zombies;
	ZombieHorde hordeMaker;
	zombies = hordeMaker.CreateHorde(numZombies, game->getPlayer()->getPos(), game, renderer);
	numZombiesAlive = numZombies;

	return true;
}

void Scene1::OnDestroy() {
	delete[] zombies;
}

void Scene1::Update(const float deltaTime) {
	// Update player
	game->getPlayer()->Update(deltaTime);

	// Update bullets
	for (Bullet* b : bullets) {
		if (b) {
			b->Update(deltaTime);
		}
	}

	for (int i = 0; i < numZombies; i++) {
		if (zombies[i].IsAlive()) {
			zombies[i].Update(deltaTime, game->getPlayer()->getPos());
		}
	}

	// --- bullet vs zombie collisions ---
	for (Bullet* b : bullets) {
		if (!b || !b->IsAlive()) continue;

		Vec3 bp = b->getPos();
		float br = b->getRadius();

		for (int i = 0; i < numZombies; i++) {
			if (!zombies[i].IsAlive()) continue;

			Vec3 zp = zombies[i].getPos();
			float zr = zombies[i].getRadius();

			Vec3 diff = zp - bp;
			float distSq = VMath::dot(diff, diff);
			float radSum = br + zr;

			if (distSq <= radSum * radSum) {
				// bullet hit this zombie
				bool died = zombies[i].Hit(b->GetDamage());

				if (died) {
					numZombiesAlive--;
					// you could spawn blood splatters or sounds here
				}

				// bullet is consumed when it hits something
				b->MarkDead();
				break; // stop checking this bullet against other zombies
			}
		}
	}

	// Remove dead bullets
	bullets.erase(
		std::remove_if(bullets.begin(), bullets.end(),
			[](Bullet* b) {
				if (!b) return true;
				if (!b->IsAlive()) {
					delete b;
					return true;
				}
				return false;
			}),
		bullets.end()
	);

	// UpdateCamera(deltaTime);
}

void Scene1::UpdateCamera(float deltaTime) {
	PlayerBody* player = game->getPlayer();
	if (!player) return;

	Vec3 target = player->getPos();

	// smooth follow
	cameraCenter += (target - cameraCenter) * followSpeed * deltaTime;

	float halfW = xAxis * 0.5f;
	float halfH = yAxis * 0.5f;

	float left = cameraCenter.x - halfW;
	float right = cameraCenter.x + halfW;
	float bottom = cameraCenter.y - halfH;
	float top = cameraCenter.y + halfH;

	Matrix4 ortho = MMath::orthographic(left, right, bottom, top, 0.0f, 1.0f);

	// IMPORTANT: keep viewport transform
	projectionMatrix = viewportMatrix * ortho;
}

void Scene1::Render() {
	int windowWidth, windowHeight;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);

	bgRect.w = static_cast<float>(windowWidth);  // full width
	bgRect.h = static_cast<float>(windowHeight); // full height

	SDL_RenderTexture(renderer, game->getBackgroundTexture(), nullptr, &bgRect);

	// Draw bullets under player
	for (Bullet* b : bullets) {
		if (b) {
			b->Render(renderer, getProjectionMatrix());
		}
	}

	// render the player
	game->RenderPlayer(0.5f);
	
		// Render zombies
	for (int i = 0; i < numZombies; i++) {
		if (zombies[i].IsAlive()) {
			zombies[i].Render(1.0f); // pass scale factor
		}
	}

	SDL_RenderPresent(renderer);
}

void Scene1::HandleEvents(const SDL_Event& event)
{
	// send events to player as needed
	game->getPlayer()->HandleEvents(event);
}

void Scene1::SpawnBullet(const Vec3& startPos, const Vec3& dir, const float damage, float speed, float bulletLife) {
	bullets.push_back(new Bullet(startPos, dir, damage, speed, bulletLife));
}
