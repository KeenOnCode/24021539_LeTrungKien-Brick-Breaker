#include "Bullet.h"

Bullet::Bullet(SDL_Renderer* renderer, int x, int y) : renderer(renderer), speed(10) {
    rect = { x, y, 5, 10 }; // Example dimensions
}

void Bullet::update() {
    // Update bullet position
    rect.y -= speed;
}

void Bullet::render() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White color
    SDL_RenderFillRect(renderer, &rect);
}

SDL_Rect Bullet::getRect() const {
    return rect;
}
