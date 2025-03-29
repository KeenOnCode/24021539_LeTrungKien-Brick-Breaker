#include "Paddle.h"
#include <iostream>
#include <SDL_image.h>

Paddle::Paddle(SDL_Renderer* renderer)
    : renderer(renderer), speed(10), moveDirection(0), hasGun(false), paddleTexture(nullptr), currentPowerUp(PowerUp::NONE), powerUpDuration(0) {
    paddleRect = { 350, 550, 100, 20 };
    loadTexture("assets/image/model/paddle.png");
}

Paddle::~Paddle() {
    if (paddleTexture) {
        SDL_DestroyTexture(paddleTexture);
    }
}

void Paddle::handleEvent(const SDL_Event& e) {
    if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
        switch (e.key.keysym.sym) {
        case SDLK_LEFT: moveDirection = -1; break;
        case SDLK_RIGHT: moveDirection = 1; break;
        case SDLK_SPACE: if (hasGun) shoot(); break;
        }
    }
    else if (e.type == SDL_KEYUP && e.key.repeat == 0) {
        switch (e.key.keysym.sym) {
        case SDLK_LEFT: if (moveDirection == -1) moveDirection = 0; break;
        case SDLK_RIGHT: if (moveDirection == 1) moveDirection = 0; break;
        }
    }
}

void Paddle::update() {
    paddleRect.x += moveDirection * speed;
    if (paddleRect.x < 0) paddleRect.x = 0;
    if (paddleRect.x + paddleRect.w > 800) paddleRect.x = 800 - paddleRect.w;

    for (auto& bullet : bullets) {
        bullet.update();
    }

    // Check if power-up duration has expired
    if (currentPowerUp != PowerUp::NONE) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - powerUpStartTime).count();
        if (elapsed >= powerUpDuration) {
            resetPowerUp();
        }
    }
}

void Paddle::render() {
    if (paddleTexture) {
        SDL_RenderCopy(renderer, paddleTexture, nullptr, &paddleRect);
    }
    else {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &paddleRect);
    }

    for (auto& bullet : bullets) {
        bullet.render();
    }
}

SDL_Rect Paddle::getRect() const {
    return paddleRect;
}

void Paddle::applyPowerUp(PowerUp::Type type) {
    currentPowerUp = type;
    powerUpStartTime = std::chrono::steady_clock::now();
    powerUpDuration = 5; // Set duration to 5 seconds, you can adjust this value

    switch (type) {
    case PowerUp::INCREASE_BALL_SIZE:
        loadTexture("assets/image/model/paddle_increase_ball_size.png");
        paddleRect.w += 20; // Increase paddle width
        break;
    case PowerUp::DECREASE_BALL_SIZE:
        loadTexture("assets/image/model/paddle_decrease_ball_size.png");
        paddleRect.w -= 20; // Decrease paddle width
        break;
    case PowerUp::INCREASE_PADDLE_SPEED:
        loadTexture("assets/image/model/paddle_increase_speed.png");
        speed += 5;
        break;
    case PowerUp::DECREASE_PADDLE_SPEED:
        loadTexture("assets/image/model/paddle_decrease_speed.png");
        speed -= 5;
        break;
    case PowerUp::PADDLE_WITH_GUN:
        loadTexture("assets/image/model/paddle_with_gun.png");
        hasGun = true;
        break;
    case PowerUp::BALL_AS_MISSILE:
        loadTexture("assets/image/model/paddle_ball_as_missile.png");
        break;
    }
}

void Paddle::shoot() {
    bullets.push_back(Bullet(renderer, paddleRect.x + paddleRect.w / 2, paddleRect.y));
}

void Paddle::loadTexture(const std::string& path) {
    if (paddleTexture) {
        SDL_DestroyTexture(paddleTexture);
    }
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        std::cout << "❌ Failed to load paddle image: " << IMG_GetError() << std::endl;
        paddleTexture = nullptr;
        return;
    }
    paddleTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!paddleTexture) {
        std::cout << "❌ Failed to create texture from surface: " << SDL_GetError() << std::endl;
    }
}

std::vector<Bullet>& Paddle::getBullets() {
    return bullets;
}

void Paddle::resetPowerUp() {
    switch (currentPowerUp) {
    case PowerUp::INCREASE_BALL_SIZE:
        paddleRect.w -= 20; // Reset paddle width
        break;
    case PowerUp::DECREASE_BALL_SIZE:
        paddleRect.w += 20; // Reset paddle width
        break;
    case PowerUp::INCREASE_PADDLE_SPEED:
        speed -= 5; // Reset speed
        break;
    case PowerUp::DECREASE_PADDLE_SPEED:
        speed += 5; // Reset speed
        break;
    case PowerUp::PADDLE_WITH_GUN:
        hasGun = false; // Reset gun
        break;
    case PowerUp::BALL_AS_MISSILE:
        // Reset any changes made by this power-up
        break;
    }
    loadTexture("assets/image/model/paddle.png"); // Reset to default texture
    currentPowerUp = PowerUp::NONE;
}
