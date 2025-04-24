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
	void renderWinner();
    void drawLine(int x1, int y1, int x2, int y2, SDL_Color color);
    float limitLineY;
    SDL_Texture* winnerTexture;
    SDL_Rect winnerRect;
    bool isWinner; // Biến để kiểm tra trạng thái chiến thắng
    bool isBallAttached;
    std::vector<SDL_Texture*> oneHitBrickTextures; // Danh sách texture cho gạch 1-hit
    std::vector<SDL_Texture*> twoHitBrickFullTextures; // Danh sách texture cho gạch 2-hit (nguyên vẹn)
	std::vector<SDL_Texture*> twoHitBrickCrackedTextures; // Danh sách texture cho gạch 2-hit (bị nứt)
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
	void addNewBrickRow();
    std::vector<Ball*> extraBalls;
};

#endif // GAME_H
