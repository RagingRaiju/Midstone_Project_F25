#ifndef WINDOW_H
#define WINDOW_H

#define SDL_ENABLE_OLD_NAMES

class Window {
private:
	struct SDL_Window* window;   
	struct SDL_Surface* screenSurface;
    struct SDL_Renderer* renderer;
	int width, height;
	
public:
	Window(int width_, int height_ );
	~Window();
	bool OnCreate();
	void OnDestroy();
	SDL_Window* GetSDL_Window();
	SDL_Renderer* GetRenderer() const;
};

#endif