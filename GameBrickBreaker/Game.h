#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include "Brick.h"
#include "Paddle.h"
#include "Ball.h"
#include "PowerUp.h"
#include "Bullet.h"

class Game {
public:
    Game();
    ~Game();
    bool init();
    void run();
    void clean();

private:
    void handleEvents();
    void update();
    void render();
    void initBricks();
    void LoadBackground(SDL_Renderer* renderer);
    void RenderBackground(SDL_Renderer* renderer);
    void CleanupBackground();
    void renderHearts();
    void updateScoreTexture();

    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    Paddle* paddle;
    Ball* ball;
    std::vector<Brick> bricks;
    std::vector<PowerUp> powerUps;
    int lives;
    int score;
    SDL_Texture* heartTexture;
    SDL_Texture* scoreTexture;
    TTF_Font* font;
    SDL_Texture* backgroundTexture;
    static const int SCREEN_WIDTH = 800;
    static const int SCREEN_HEIGHT = 600;
    // Thêm các biến cho nút "Pause"
    SDL_Texture* pauseTexture;
    SDL_Rect pauseRect;
    bool isPaused;

    // Thêm các biến cho ảnh "Game Over" và "Restart"
    SDL_Texture* gameOverTexture;
    SDL_Texture* restartTexture;
    SDL_Rect gameOverRect;
    SDL_Rect restartRect;
    bool isGameOver;

    void loadGameOverTextures(SDL_Renderer* renderer);
    void renderGameOver();
    void handlePauseEvent(const SDL_Event& e);
};