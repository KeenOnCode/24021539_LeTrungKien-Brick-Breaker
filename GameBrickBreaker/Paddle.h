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
	void resetToDefault();
	
private:
    SDL_Renderer* renderer;
    SDL_Rect paddleRect;
    SDL_Texture* paddleTexture;
    int speed;
	int defaultSpeed = 10; // Tốc độ mặc định
	int width;
	int defaultWidth = 100; // Kích thước mặc định
    int moveDirection;
    bool hasGun;
    std::vector<Bullet> bullets;

    PowerUp::Type currentPowerUp;
    std::chrono::time_point<std::chrono::steady_clock> powerUpStartTime;
    int powerUpDuration; // in seconds

    std::chrono::time_point<std::chrono::steady_clock> lastBulletTime; // Thời gian của lần bắn đạn cuối cùng

    void loadTexture(const std::string& path);
    void resetPowerUp();
};
