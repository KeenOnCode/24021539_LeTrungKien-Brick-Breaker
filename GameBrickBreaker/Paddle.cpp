#include "Paddle.h"

Paddle::Paddle(SDL_Renderer* renderer) : renderer(renderer), velocity(0) {
    paddleRect = { 350, 550, 100, 20 };
}

Paddle::~Paddle() {}

void Paddle::handleEvent(SDL_Event& e) {
    if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
        if (e.key.keysym.sym == SDLK_LEFT) velocity = -5;
        else if (e.key.keysym.sym == SDLK_RIGHT) velocity = 5;
    }
    else if (e.type == SDL_KEYUP && e.key.repeat == 0) {
        if (e.key.keysym.sym == SDLK_LEFT || e.key.keysym.sym == SDLK_RIGHT) velocity = 0;
    }
}

void Paddle::update() {
    paddleRect.x += velocity;
    if (paddleRect.x < 0) paddleRect.x = 0;
    if (paddleRect.x + paddleRect.w > 800) paddleRect.x = 800 - paddleRect.w;
}

void Paddle::render() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &paddleRect);
}
