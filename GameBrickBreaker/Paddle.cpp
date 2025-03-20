#include "Paddle.h"
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
Paddle::Paddle(SDL_Renderer* renderer)
    : renderer(renderer), speed(8), moveDirection(0), paddleTexture(nullptr)
{
    // Khởi tạo vị trí paddle
    paddleRect = { (800 / 2) - 50, 550, 100, 20 };

    LoadTexture(renderer); // ✅ Load ảnh paddle
}

Paddle::~Paddle() {}

void Paddle::handleEvent(const SDL_Event& e) {
    // Kiểm tra sự kiện phím bấm
    if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
        switch (e.key.keysym.sym) {
        case SDLK_LEFT:
            moveDirection = -1;
            break;
        case SDLK_RIGHT:
            moveDirection = 1;
            break;
        }
    }
    // Nếu thả phím ra thì dừng di chuyển
    else if (e.type == SDL_KEYUP && e.key.repeat == 0) {
        switch (e.key.keysym.sym) {
        case SDLK_LEFT:
            if (moveDirection == -1) moveDirection = 0;
            break;
        case SDLK_RIGHT:
            if (moveDirection == 1) moveDirection = 0;
            break;
        }
    }
}

void Paddle::update() {
    // Di chuyển paddle theo hướng
    paddleRect.x += moveDirection * speed;

    // Giữ paddle không ra khỏi màn hình
    if (paddleRect.x < 0) {
        paddleRect.x = 0;
    }
    if (paddleRect.x + paddleRect.w > 800) {
        paddleRect.x = 800 - paddleRect.w;
    }
}

void Paddle::LoadTexture(SDL_Renderer* renderer) {
    SDL_Surface* paddleSurface = IMG_Load("assets/image/model/paddle.png");
    if (!paddleSurface) {
        std::cout << "❌ Failed to load paddle image: " << IMG_GetError() << std::endl;
        return;
    }
    paddleTexture = SDL_CreateTextureFromSurface(renderer, paddleSurface);
    SDL_FreeSurface(paddleSurface);
}

void Paddle::render() {
    if (paddleTexture) {
        SDL_RenderCopy(renderer, paddleTexture, nullptr, &paddleRect);
    }
    else {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(renderer, &paddleRect);
    }
}


SDL_Rect Paddle::getRect() const {
    return paddleRect;
}
