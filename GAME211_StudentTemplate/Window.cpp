#include "Window.h"
#include <SDL.h>
#include <iostream> /// Umer likes this over printf() - too bad


Window::Window(int width_, int height_){
	screenSurface = nullptr;
	window = nullptr;
	width = width_;
	height = height_;
}

//bool Window::OnCreate() {
//	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
//		std::cout << "SDL_Error: " << SDL_GetError() << std::endl;
//		return false;
//	}
//
//	window = SDL_CreateWindow("GAME211 Template", width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
//    
//	if (window == nullptr) {
//		std::cout << "SDL_Error: " << SDL_GetError() << std::endl;
//		return false;
//	}
//
//	screenSurface = SDL_GetWindowSurface(window);
//	if (screenSurface == nullptr) {
//		std::cout << "SDL_Error: " << SDL_GetError() << std::endl;
//		return false;
//	}
//    
//    renderer = SDL_GetRenderer(window);
//	if (!renderer) {
//		renderer = SDL_CreateRenderer(window, nullptr);
//	}
//    if (!renderer) {
//        std::cout << "SDL_Error: " << SDL_GetError() << std::endl;
//        return false;
//    }
//            
//	return true;
//}

bool Window::OnCreate() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("GAME211 Template", width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (window == nullptr) {
        std::cout << "SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cout << "SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}


void Window::OnDestroy(){
	/// Kill the surface first
	if (screenSurface) {
		SDL_FreeSurface(screenSurface);
	}
    
    // Delete the renderer
    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }

	/// Now kill the window
	if (window){
		SDL_DestroyWindow(window);
	}
	
	///Exit the SDL subsystems
	SDL_Quit();

}

Window::~Window(){}

SDL_Window* Window::GetSDL_Window() {
	return window;
}

SDL_Renderer* Window::GetRenderer() const
{
    return renderer;
}
