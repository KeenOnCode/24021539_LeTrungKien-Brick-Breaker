#pragma once
#include <SDL.h>
#include <vector>
#include "Bullet.h"
#include "PowerUp.h"
#include <string>
#include <chrono>

class Paddle {
public:
    Paddle(SDL_Renderer* renderer);
    ~Paddle();
    void handleEvent(const SDL_Event& e);
    void update();
    void render();
    SDL_Rect getRect() const;
    void applyPowerUp(PowerUp::Type type);
    void shoot();
    std::vector<Bullet>& getBullets();

private:
    SDL_Renderer* renderer;
    SDL_Rect paddleRect;
    SDL_Texture* paddleTexture;
    int speed;
    int moveDirection;
    bool hasGun;
    std::vector<Bullet> bullets;

    PowerUp::Type currentPowerUp;
    std::chrono::time_point<std::chrono::steady_clock> powerUpStartTime;
    int powerUpDuration; // in seconds

    void loadTexture(const std::string& path);
    void resetPowerUp();
};
