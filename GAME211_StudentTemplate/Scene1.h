#ifndef SCENE1_H
#define SCENE1_H

#include <SDL_image.h>
#include <MMath.h>
#include "Scene.h"
#include <vector>
#include "Bullet.h"
#include "TextureHolder.h"
#include "ZombieHorde.h"

using namespace MATH;
class Scene1 : public Scene {
private:
	float xAxis;	// scene width, in game coords, set in constructor
	float yAxis;	// scene height, in game coords, set in constructor
	SDL_Window* window;		// an SDL window with a SDL renderer
	SDL_Renderer* renderer;	// the renderer associated with SDL window
	Matrix4 projectionMatrix;	// set in OnCreate()
    Matrix4     inverseProjection;	// set in OnCreate()
	TextureHolder textureHolder;
	
	// Prepare for a horde of zombies
	int numZombies;
	int numZombiesAlive;
	Zombie* zombies = nullptr;

	// background rect to render the bg image on
	SDL_FRect bgRect;

	// BULLETS HANDLED BY SCENE (for cleanup, collisions, etc.)
	std::vector<Bullet*> bullets;

	// custom camera system to follow player around
	const float followSpeed = 8.0f;
	Matrix4 viewportMatrix;   // stores NDC->screen transform
	Vec3 cameraCenter;
	void UpdateCamera(float deltaTime);
public:
	// This constructor may be different from what you've seen before
	// Notice the second parameter, and look in GameManager.cpp
	// to see how this constructor is called.
	Scene1(SDL_Window* sdlWindow, GameManager* game_);
	~Scene1();
	bool OnCreate();
	void OnDestroy();
	void Update(const float time);
	void Render();
    void HandleEvents(const SDL_Event &event);
	float getxAxis() { return xAxis; }
	float getyAxis() { return yAxis; }
	SDL_Window* getWindow() { return window; }
    Matrix4 getProjectionMatrix() { return projectionMatrix; }
	Matrix4 getInverseMatrix() { return inverseProjection; }

	void SpawnBullet(const Vec3& startPos, const Vec3& dir, float speed, float bulletLife);
};

#endif
