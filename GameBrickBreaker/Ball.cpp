#include "Ball.h"
#include <SDL_image.h>

Ball::Ball(SDL_Renderer* renderer)
    : renderer(renderer), velocityX(5), velocityY(-5) {
    ballRect = { 400, 300, 25, 25 };
    ballTexture = IMG_LoadTexture(renderer, "assets/image/model/ball.png");
}

Ball::~Ball() {
    if (ballTexture) {
        SDL_DestroyTexture(ballTexture);
    }
}

void Ball::update(Paddle& paddle, bool& running, int& lives) {
    ballRect.x += velocityX;
    ballRect.y += velocityY;

    if (ballRect.x <= 0 || ballRect.x + ballRect.w >= 800) {
        velocityX = -velocityX;
    }
    if (ballRect.y <= 0) {
        velocityY = -velocityY;
    }
    if (ballRect.y + ballRect.h >= 600) {
        lives--;
        if (lives <= 0) {
            running = false;
        }
        else {
            ballRect.x = 400;
            ballRect.y = 300;
            velocityY = -velocityY;
        }
    }

    SDL_Rect paddleRect = paddle.getRect();
    if (SDL_HasIntersection(&ballRect, &paddleRect)) {
        velocityY = -velocityY;
        ballRect.y = paddleRect.y - ballRect.h;
    }
}

void Ball::render() {
    SDL_RenderCopy(renderer, ballTexture, nullptr, &ballRect);
}

SDL_Rect Ball::getRect() const {
    return ballRect;
}

void Ball::bounce() {
    velocityY = -velocityY;
}
