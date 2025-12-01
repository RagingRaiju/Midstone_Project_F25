#include "TextureHolder.h"
#include <assert.h>

TextureHolder* TextureHolder::textureHolderInstance = nullptr;

TextureHolder::TextureHolder() {
	assert(textureHolderInstance == nullptr);
	textureHolderInstance = this;
}

SDL_Texture* TextureHolder::GetTexture(std::string const& filename, SDL_Renderer* renderer) {
	auto& m = textureHolderInstance->mapTextures;
	auto keyValuePair = m.find(filename);

	if (keyValuePair != m.end()) {
		return keyValuePair->second;
	}

	else {
		SDL_Surface* image = IMG_Load(filename.c_str());
		if (!image) {
			// Log error
			SDL_Log("Failed to load image: %s", filename.c_str());
			return nullptr;
		}
		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, image);
		SDL_DestroySurface(image);
		m[filename] = texture;
		return texture;
	}
}

SDL_Surface* TextureHolder::GetSurface(const std::string& filename)
{
	SDL_Surface* image = IMG_Load(filename.c_str());
	if (!image) {
		// Log error
		SDL_Log("Failed to load image: %s", filename.c_str());
		return nullptr;
	}
	return image;
}

TextureHolder::~TextureHolder() {
	auto& m = textureHolderInstance->mapTextures;
	for (auto& pair : m) {
		SDL_DestroyTexture(pair.second);
	}
	m.clear();
}