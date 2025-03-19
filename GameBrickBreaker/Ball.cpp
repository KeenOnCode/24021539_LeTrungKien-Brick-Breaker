#include "Ball.h"
#include <SDL_image.h>
#include <iostream>
Ball::Ball(SDL_Renderer* renderer) : renderer(renderer), velocityX(3), velocityY(-3) {
    ballRect = { 390, 530, 40, 40 };
    LoadTexture(renderer);
}

Ball::~Ball() {
    if (ballTexture) {
        SDL_DestroyTexture(ballTexture);
        ballTexture = nullptr;
    }
}

void Ball::LoadTexture(SDL_Renderer* renderer) {
    SDL_Surface* ballSurface = IMG_Load("assets/image/model/nhanVat.png");
    if (!ballSurface) {
        std::cout << "Failed to load ball image: " << IMG_GetError() << std::endl;
        return;
    }
    ballTexture = SDL_CreateTextureFromSurface(renderer, ballSurface);
    SDL_FreeSurface(ballSurface);
}

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
    if (ballTexture) {
        SDL_RenderCopy(renderer, ballTexture, nullptr, &ballRect);
    }
    else {
        std::cout << "⚠️ Ball texture is NULL, rendering red box instead!" << std::endl;
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &ballRect);
    }
}


// === Thêm 2 hàm này vào cuối file ===
SDL_Rect Ball::getRect() const {
    return ballRect;
}

void Ball::bounce() {
    velocityY = -velocityY;
}
