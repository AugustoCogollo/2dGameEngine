#ifndef SPRITECOMPONENT_H
#define SPRITECOMPONENT_H

#include <SDL.h>
#include <string>

enum render_layers
{
	LAYER_TILEMAP,
	LAYER_VEGETATION,
	LAYER_OBSTACLES,
	LAYER_BULLETS,
	LAYER_ENEMIES,
	LAYER_PLAYER,
	LAYER_EFFECTS,
	LAYER_GUI
};

struct SpriteComponent {
	std::string assetId;
	int width;
	int height;
	render_layers layer;
	SDL_Rect srcRect;
	bool isFixed;

	SpriteComponent(std::string assetId = "", int width = 0, int height = 0, render_layers layer = LAYER_TILEMAP, bool isFixed = false, int srcRectx = 0, int srcRecty = 0) {
		this->assetId = assetId;
		this->width = width;
		this->height = height;
		this->layer = layer;
		this->isFixed = isFixed;
		this->srcRect = { srcRectx, srcRecty, width, height };
	}

	render_layers GetLayer() {
		return layer;
	}

};


#endif // !SPRITECOMPONENT_H
