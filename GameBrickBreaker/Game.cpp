#include "Game.h"
#include <iostream>
#include <SDL_image.h>
#include <cstdlib>
#include <ctime>
#include <sstream>

Game::Game() : window(nullptr), renderer(nullptr), running(false), paddle(nullptr), ball(nullptr), isPaused(false), isGameOver(false) {
    lives = 3;
    score = 0;

    if (TTF_Init() == -1) {
        std::cout << "Failed to initialize SDL_ttf: " << TTF_GetError() << std::endl;
    }

    font = TTF_OpenFont("assets/font/font.ttf", 20);
    if (!font) {
        std::cout << "Failed to load font: " << TTF_GetError() << std::endl;
    }

    updateScoreTexture();
}

Game::~Game() {
    // Không gọi clean() nữa, vì nó có thể đã được gọi trước đó
}

bool Game::init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Brick Breaker", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) return false;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) return false;

    LoadBackground(renderer);
    paddle = new Paddle(renderer);
    ball = new Ball(renderer);

    initBricks();
    updateScoreTexture();
    heartTexture = IMG_LoadTexture(renderer, "assets/image/gameOver/heart.png");

    // Tải texture của nút "Pause"
    SDL_Surface* pauseSurface = IMG_Load("assets/image/gameOver/pause.png");
    if (!pauseSurface) {
        std::cout << "Failed to load pause image: " << IMG_GetError() << std::endl;
        return false;
    }
    pauseTexture = SDL_CreateTextureFromSurface(renderer, pauseSurface);
    SDL_FreeSurface(pauseSurface);
    pauseRect = { 750, 10, 40, 40 }; // Vị trí và kích thước của nút "Pause"

    loadGameOverTextures(renderer);
    loadLogoTexture(renderer);
    running = true;
    return true;
}

void Game::initBricks() {
    bricks.clear();
    srand(static_cast<unsigned>(time(0)));

    SDL_Texture* oneHitBrickTex = IMG_LoadTexture(renderer, "assets/image/model/brick1hit.png");
    SDL_Texture* twoHitBrickFullTex = IMG_LoadTexture(renderer, "assets/image/model/brick2hit_full.png");
    SDL_Texture* twoHitBrickCrackedTex = IMG_LoadTexture(renderer, "assets/image/model/brick2hit_cracked.png");

    int rows = 5;
    int cols = 8;
    int brickWidth = SCREEN_WIDTH / cols - 5;
    int brickHeight = 30;
    int startY = 50;

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int x = j * (brickWidth + 5);
            int y = startY + i * (brickHeight + 5);

            if (rand() % 2 == 0) {
                bricks.emplace_back(x, y, brickWidth, brickHeight, Brick::BrickType::ONE_HIT, oneHitBrickTex);
            }
            else {
                bricks.emplace_back(x, y, brickWidth, brickHeight, Brick::BrickType::TWO_HIT, nullptr, twoHitBrickFullTex, twoHitBrickCrackedTex);
            }
        }
    }
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT)
            running = false;
        handlePauseEvent(event);
        if (!isPaused && !isGameOver) {
            paddle->handleEvent(event);
        }
    }
}

void Game::handlePauseEvent(const SDL_Event& e) {
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        int x = e.button.x;
        int y = e.button.y;
        if (x >= pauseRect.x && x <= pauseRect.x + pauseRect.w &&
            y >= pauseRect.y && y <= pauseRect.y + pauseRect.h) {
            isPaused = !isPaused;
        }
    }
    else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) {
        isPaused = !isPaused;
    }
}

void Game::update() {
    if (isPaused || isGameOver) return;

    paddle->update();
    ball->update(*paddle, running, lives);

    if (lives <= 0) {
        isGameOver = true;
        return;
    }

    for (auto& brick : bricks) {
        SDL_Rect ballRect = ball->getRect();
        SDL_Rect brickRect = brick.GetRect();

        if (!brick.IsDestroyed() && SDL_HasIntersection(&ballRect, &brickRect)) {
            ball->bounce();

            bool destroyed = brick.Hit();
            if (destroyed) {
                score += 100;
                updateScoreTexture();

                if (brick.shouldDropPowerUp()) {
                    powerUps.push_back(PowerUp(renderer, brick.getPowerUpType(), brickRect.x, brickRect.y));
                }
            }
        }
    }

    for (auto& powerUp : powerUps) {
        powerUp.update();
    }

    for (auto it = powerUps.begin(); it != powerUps.end();) {
        SDL_Rect paddleRect = paddle->getRect();
        SDL_Rect powerUpRect = it->getRect();

        if (SDL_HasIntersection(&paddleRect, &powerUpRect)) {
            paddle->applyPowerUp(it->getType());
            it = powerUps.erase(it);
        }
        else {
            ++it;
        }
    }

    for (auto& bullet : paddle->getBullets()) {
        bullet.update();
    }

    for (auto& bullet : paddle->getBullets()) {
        SDL_Rect bulletRect = bullet.getRect();
        for (auto& brick : bricks) {
            SDL_Rect brickRect = brick.GetRect();
            if (!brick.IsDestroyed() && SDL_HasIntersection(&bulletRect, &brickRect)) {
                brick.Hit();
                bullet = paddle->getBullets().back();
                paddle->getBullets().pop_back();
                break;
            }
        }
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    RenderBackground(renderer);
    paddle->render();
    ball->render();

    for (auto& brick : bricks) {
        brick.Render(renderer);
    }

    for (auto& powerUp : powerUps) {
        powerUp.render();
    }

    if (scoreTexture) {
        SDL_Rect scoreRect = { 600, 10, 150, 40 };
        SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);
    }

    renderHearts();

    // Hiển thị nút "Pause"
    if (pauseTexture) {
        SDL_RenderCopy(renderer, pauseTexture, NULL, &pauseRect);
    }

    // Hiển thị logo game nếu game đang tạm dừng
    if (isPaused) {
        if (logoTexture) {
            SDL_RenderCopy(renderer, logoTexture, nullptr, &logoRect);
        }
    }

    // Hiển thị màn hình "Game Over" nếu game kết thúc
    if (isGameOver) {
        renderGameOver();
    }

    SDL_RenderPresent(renderer);
}

void Game::clean() {
    if (paddle) {
        delete paddle;
        paddle = nullptr;
    }
    if (ball) {
        delete ball;
        ball = nullptr;
    }
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    if (heartTexture) {
        SDL_DestroyTexture(heartTexture);
        heartTexture = nullptr;
    }
    if (pauseTexture) {
        SDL_DestroyTexture(pauseTexture);
        pauseTexture = nullptr;
    }
    if (logoTexture) {
        SDL_DestroyTexture(logoTexture);
        logoTexture = nullptr;
    }
    if (scoreTexture) SDL_DestroyTexture(scoreTexture);
    if (font) TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
}

void Game::run() {
    while (running) {
        handleEvents();
        update();
        render();
        SDL_Delay(16);
    }
    clean();
}

void Game::LoadBackground(SDL_Renderer* renderer) {
    SDL_Surface* bgSurface = IMG_Load("assets/image/background/AnhNen.png");
    if (!bgSurface) {
        printf("Failed to load background image: %s\n", IMG_GetError());
        return;
    }
    backgroundTexture = SDL_CreateTextureFromSurface(renderer, bgSurface);
    SDL_FreeSurface(bgSurface);
}

void Game::RenderBackground(SDL_Renderer* renderer) {
    if (backgroundTexture) {
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
    }
}

void Game::CleanupBackground() {
    if (backgroundTexture) {
        SDL_DestroyTexture(backgroundTexture);
        backgroundTexture = nullptr;
    }
}

void Game::renderHearts() {
    for (int i = 0; i < lives; i++) {
        SDL_Rect heartRect = { 20 + i * 40, 10, 30, 30 };
        SDL_RenderCopy(renderer, heartTexture, NULL, &heartRect);
    }
}

void Game::updateScoreTexture() {
    SDL_Color textColor = { 255, 255, 255, 255 };

    std::stringstream ss;
    ss << "Score: " << score;
    std::string scoreText = ss.str();

    SDL_Surface* textSurface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
    if (!textSurface) return;

    scoreTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);
}

void Game::loadGameOverTextures(SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load("assets/image/gameOver/gameover.png");
    if (!surface) {
        std::cout << "❌ Failed to load game over image: " << IMG_GetError() << std::endl;
        return;
    }
    gameOverTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    surface = IMG_Load("assets/image/gameOver/restart.png");
    if (!surface) {
        std::cout << "❌ Failed to load restart image: " << IMG_GetError() << std::endl;
        return;
    }
    restartTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    gameOverRect = { 200, 150, 400, 200 }; // Vị trí và kích thước của ảnh "Game Over"
    restartRect = { 300, 400, 200, 100 }; // Vị trí và kích thước của ảnh "Restart"
}

void Game::renderGameOver() {
    if (gameOverTexture) {
        SDL_RenderCopy(renderer, gameOverTexture, nullptr, &gameOverRect);
    }
    if (restartTexture) {
        SDL_RenderCopy(renderer, restartTexture, nullptr, &restartRect);
    }
}

void Game::loadLogoTexture(SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load("assets/image/gameOver/logo.png");
    if (!surface) {
        std::cout << "❌ Failed to load logo image: " << IMG_GetError() << std::endl;
        return;
    }
    logoTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    logoRect = { 200, 150,600 , 200 }; // Vị trí và kích thước của logo game
}

