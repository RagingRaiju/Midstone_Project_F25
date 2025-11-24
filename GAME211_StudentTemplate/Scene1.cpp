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

	// build an initial projection
	float halfW = xAxis * 0.5f;
	float halfH = yAxis * 0.5f;

	float left = cameraCenter.x - halfW;
	float right = cameraCenter.x + halfW;
	float bottom = cameraCenter.y - halfH;
	float top = cameraCenter.y + halfH;

	Matrix4 ortho = MMath::orthographic(left, right, bottom, top, 0.0f, 1.0f);
	projectionMatrix = viewportMatrix * ortho;

	/// Turn on the SDL imaging subsystem
	//IMG_Init(IMG_INIT_PNG);

	return true;
}

void Scene1::OnDestroy() {}

void Scene1::Update(const float deltaTime) {
	// Update player
	game->getPlayer()->Update(deltaTime);

	// Update bullets
	for (Bullet* b : bullets) {
		if (b) {
			b->Update(deltaTime);
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

	UpdateCamera(deltaTime);
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
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);

	// Draw bullets under player
	for (Bullet* b : bullets) {
		if (b) {
			b->Render(renderer, getProjectionMatrix());
		}
	}

	// render the player
	game->RenderPlayer(0.5f);

	SDL_RenderPresent(renderer);
}

void Scene1::HandleEvents(const SDL_Event& event)
{
	// send events to player as needed
	game->getPlayer()->HandleEvents(event);
}

void Scene1::SpawnBullet(const Vec3& startPos, const Vec3& dir, float speed) {
	const float bulletLife = 2.0f; // seconds, can later be per-weapon too

	bullets.push_back(new Bullet(startPos, dir, speed, bulletLife));
}
