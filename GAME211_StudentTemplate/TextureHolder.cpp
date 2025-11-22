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
		// SFML Book Code
		// auto& texture = m[filename];
		//texture.loadFromFile(filename);
		//return texture;

		// My code

		SDL_Surface* image = IMG_Load(filename.c_str());
		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, image);
		if (image) {
			SDL_DestroySurface(image);
		}
		m[filename] = texture;
		return texture;
	}
}

TextureHolder::~TextureHolder()
{
}
