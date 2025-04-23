#include "PowerUp.h"
#include <iostream>
#include <random>
#include <vector>
PowerUp::PowerUp(SDL_Renderer* renderer, Type type, int x, int y)
    : renderer(renderer), type(type), speed(5) {
    rect = { x, y, 30, 30 };
    loadTexture(renderer);
    
}
bool PowerUp::isExpired() const {
    return SDL_GetTicks() > activationTime + duration; // duration là thời gian hiệu lực
}

PowerUp::~PowerUp() {
    if (texture) {
        SDL_DestroyTexture(texture);
    }
}

void PowerUp::update() {
    rect.y += speed;
    
}

void PowerUp::render() {
    if (texture) {
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
    }
    else {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
    
}

SDL_Rect PowerUp::getRect() const {
    return rect;
}

PowerUp::Type PowerUp::getType() const {
    return type;
}

void PowerUp::loadTexture(SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load("assets/image/model/powerup.png");
    if (!surface) {
        std::cout << "❌ Failed to load power-up image: " << IMG_GetError() << std::endl;
        return;
    }
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
}
