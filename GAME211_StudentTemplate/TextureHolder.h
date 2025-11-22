#ifndef TEXTUREHOLDER_H
#define TEXTUREHOLDER_H
#include <SDL.h>
#include <SDL3/SDL_image.h>
#include <string>
#include <map>

class TextureHolder
{
private:
	// Cache of loaded textures.
	// Key: filename (string), Value: GPU texture pointer (SDL_Texture*).
	// Prevents duplicate loads and allows multiple game objects to share the same texture.
	std::map<std::string, SDL_Texture*> mapTextures;

	// Singleton instance of TextureHolder.
	// Ensures only one global texture manager exists to coordinate loading and cleanup.
	static TextureHolder* textureHolderInstance;

public:
	// Constructor for TextureHolder.
	// Enforces the singleton pattern by ensuring only one instance exists.
	// Initializes the global texture manager so all texture requests
	// go through this single access point.
	TextureHolder();

	// Retrieves a texture by filename.
	// - If the texture is already cached, returns the existing SDL_Texture*.
	// - If not cached, loads the image file into an SDL_Surface,
	// creates a GPU texture with the provided renderer,
	// stores it in the map for reuse, and returns the new SDL_Texture*.
	// This ensures textures are loaded only once and shared across game objects.
	static SDL_Texture* GetTexture(const std::string& filename, SDL_Renderer* renderer);

	// Destructor for TextureHolder.
	// Iterates through all cached textures in mapTextures,
	// calls SDL_DestroyTexture() on each to free GPU memory,
	// then clears the map to prevent dangling pointers.
	// Ensures proper cleanup of all textures when the game or scene ends.
	~TextureHolder();
};

#endif // TEXTUREHOLDER_H