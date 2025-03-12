#include "Ball.h"

Ball::Ball(SDL_Renderer* renderer) : renderer(renderer), velocityX(3), velocityY(-3) {
    ballRect = { 390, 530, 15, 15 };
}

Ball::~Ball() {}

void Ball::update(Paddle& paddle, bool& running) {
    ballRect.x += velocityX;
    ballRect.y += velocityY;

    if (ballRect.x <= 0 || ballRect.x + ballRect.w >= 800) velocityX = -velocityX;
    if (ballRect.y <= 0) velocityY = -velocityY;

    // Va chạm với paddle
    if (ballRect.y + ballRect.h >= paddle.getRect().y &&
        ballRect.x + ballRect.w >= paddle.getRect().x &&
        ballRect.x <= paddle.getRect().x + paddle.getRect().w) {
        velocityY = -velocityY;
        ballRect.y = paddle.getRect().y - ballRect.h; // Đặt bóng ngay trên paddle
    }

    // Kiểm tra bóng rơi xuống dưới paddle
    if (ballRect.y + ballRect.h >= 600) {
        running = false; // Kết thúc game
    }
}

void Ball::render() {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &ballRect);
}
