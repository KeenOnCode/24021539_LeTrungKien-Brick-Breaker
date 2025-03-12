#include "Ball.h"

Ball::Ball(SDL_Renderer* renderer) : renderer(renderer), velocityX(3), velocityY(-3) {
    ballRect = { 390, 530, 15, 15 };
}

Ball::~Ball() {}

void Ball::update() {
    ballRect.x += velocityX;
    ballRect.y += velocityY;

    if (ballRect.x <= 0 || ballRect.x + ballRect.w >= 800) velocityX = -velocityX;
    if (ballRect.y <= 0) velocityY = -velocityY;
}

void Ball::render() {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &ballRect);
}
