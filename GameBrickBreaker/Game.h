#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <vector>
#include "Paddle.h"
#include "Ball.h"
#include "Brick.h"
#include "PowerUp.h"

class Game {
public:
    Game();
    ~Game();
    bool init();
    void run();
    void handleEvents();
    void update();
    void render();
    void clean();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    Paddle* paddle;
    Ball* ball;
    std::vector<Brick> bricks;
    std::vector<PowerUp> powerUps;
    SDL_Texture* backgroundTexture; // Add this line
    SDL_Texture* heartTexture;
    SDL_Texture* pauseTexture;
    SDL_Texture* logoTexture;
    SDL_Texture* scoreTexture;
    SDL_Texture* startScreenTexture;
    SDL_Texture* playButtonTexture;
    SDL_Texture* gameOverTexture;
    SDL_Texture* restartTexture;
    SDL_Rect playButtonRect;
    SDL_Rect pauseRect;
    SDL_Rect logoRect;
    SDL_Rect gameOverRect;
    SDL_Rect restartRect;
    TTF_Font* font;
    Mix_Music* startMusic;
    Mix_Music* gameMusic;
    int lives;
    int score;
    bool isPaused;
    bool isGameOver;
    bool isStartScreen;
    void LoadBackground(SDL_Renderer* renderer);
    void RenderBackground(SDL_Renderer* renderer);
    void CleanupBackground();
    void renderHearts();
    void updateScoreTexture();
    void loadGameOverTextures(SDL_Renderer* renderer);
    void renderGameOver();
    void loadLogoTexture(SDL_Renderer* renderer);
    void resetGame();
    void initBricks();
    void handleStartScreenEvent(const SDL_Event& e);
    void handlePauseEvent(const SDL_Event& e);
    void renderStartScreen();
    std::vector<SDL_Texture*> oneHitBrickTextures;
    std::vector<SDL_Texture*> twoHitBrickFullTextures;
    std::vector<SDL_Texture*> twoHitBrickCrackedTextures;

};

#endif // GAME_H
