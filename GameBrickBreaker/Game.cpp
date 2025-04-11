#include "Game.h"
#include <iostream>
#include <SDL_image.h>
#include <cstdlib>
#include <ctime>
#include <sstream>
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
Game::Game() : window(nullptr), renderer(nullptr), running(false), paddle(nullptr), ball(nullptr), isPaused(false), isGameOver(false), isStartScreen(true) {
    lives = 3;
    score = 0;

    if (TTF_Init() == -1) {
        std::cout << "Failed to initialize SDL_ttf: " << TTF_GetError() << std::endl;
    }

    font = TTF_OpenFont("assets/font/font.ttf", 20);
    if (!font) {
        std::cout << "Failed to load font: " << TTF_GetError() << std::endl;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cout << "Failed to initialize SDL_mixer: " << Mix_GetError() << std::endl;
    }

    updateScoreTexture();
}

Game::~Game() {
    clean();
}

bool Game::init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
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

    // Load start screen textures
    SDL_Surface* startSurface = IMG_Load("assets/image/background/start_screen.png");
    if (!startSurface) {
        std::cout << "Failed to load start screen image: " << IMG_GetError() << std::endl;
        return false;
    }
    startScreenTexture = SDL_CreateTextureFromSurface(renderer, startSurface);
    SDL_FreeSurface(startSurface);

    SDL_Surface* playSurface = IMG_Load("assets/image/gameOver/play_button.png");
    if (!playSurface) {
        std::cout << "Failed to load play button image: " << IMG_GetError() << std::endl;
        return false;
    }
    playButtonTexture = SDL_CreateTextureFromSurface(renderer, playSurface);
    SDL_FreeSurface(playSurface);
    playButtonRect = { 350, 300, 100, 50 }; // Position and size of the play button

    // Load start screen music
    startMusic = Mix_LoadMUS("assets/sound/start_music.mp3");
    if (!startMusic) {
        std::cout << "Failed to load start music: " << Mix_GetError() << std::endl;
        return false;
    }
    Mix_PlayMusic(startMusic, -1);
	// Load brick hit sound
    brickHitSound = Mix_LoadWAV("assets/sound/brick_hit.wav");
    if (!brickHitSound) {
        std::cout << "Failed to load brick hit sound: " << Mix_GetError() << std::endl;
        return false;
    }
    // Load gameplay music
    gameMusic = Mix_LoadMUS("assets/sound/game_music.mp3");
    if (!gameMusic) {
        std::cout << "Failed to load game music: " << Mix_GetError() << std::endl;
        return false;
    }

    // Load pause button texture
    SDL_Surface* pauseSurface = IMG_Load("assets/image/gameOver/pause.png");
    if (!pauseSurface) {
        std::cout << "Failed to load pause image: " << IMG_GetError() << std::endl;
        return false;
    }
    pauseTexture = SDL_CreateTextureFromSurface(renderer, pauseSurface);
    SDL_FreeSurface(pauseSurface);
    pauseRect = { 750, 10, 40, 40 }; // Position and size of the pause button

    loadGameOverTextures(renderer);
    loadLogoTexture(renderer);
    running = true;
    return true;
}

void Game::initBricks() {
    bricks.clear();
    srand(static_cast<unsigned>(time(0)));

    // Tải danh sách texture cho gạch 1 hit
    std::vector<SDL_Texture*> oneHitBrickTextures = {   
        IMG_LoadTexture(renderer, "assets/image/1hit/full1.png"),
        IMG_LoadTexture(renderer, "assets/image/1hit/full2.png"),
        IMG_LoadTexture(renderer, "assets/image/1hit/full3.png"),
        IMG_LoadTexture(renderer, "assets/image/1hit/full4.png"),
        IMG_LoadTexture(renderer, "assets/image/1hit/full5.png"),
        IMG_LoadTexture(renderer, "assets/image/1hit/full6.png"),
        IMG_LoadTexture(renderer, "assets/image/1hit/full7.png"),
        IMG_LoadTexture(renderer, "assets/image/1hit/full8.png"),
        IMG_LoadTexture(renderer, "assets/image/1hit/full9.png"),
        IMG_LoadTexture(renderer, "assets/image/1hit/full10.png")
    };

    // Tải danh sách texture cho gạch 2 hit (nguyên vẹn và bị nứt)
    std::vector<SDL_Texture*> twoHitBrickFullTextures = {
        IMG_LoadTexture(renderer, "assets/image/2hit/full1.png"),
        IMG_LoadTexture(renderer, "assets/image/2hit/full2.png"),
        IMG_LoadTexture(renderer, "assets/image/2hit/full3.png"),
        IMG_LoadTexture(renderer, "assets/image/2hit/full4.png"),
        IMG_LoadTexture(renderer, "assets/image/2hit/full5.png"),
        IMG_LoadTexture(renderer, "assets/image/2hit/full6.png"),
        IMG_LoadTexture(renderer, "assets/image/2hit/full7.png"),
        IMG_LoadTexture(renderer, "assets/image/2hit/full8.png"),
        IMG_LoadTexture(renderer, "assets/image/2hit/full9.png"),
        IMG_LoadTexture(renderer, "assets/image/2hit/full10.png")
    };

    std::vector<SDL_Texture*> twoHitBrickCrackedTextures = {
        IMG_LoadTexture(renderer, "assets/image/2hit/cracked1.png"),
        IMG_LoadTexture(renderer, "assets/image/2hit/cracked2.png"),
        IMG_LoadTexture(renderer, "assets/image/2hit/cracked3.png"),
        IMG_LoadTexture(renderer, "assets/image/2hit/cracked4.png"),
        IMG_LoadTexture(renderer, "assets/image/2hit/cracked5.png"),
        IMG_LoadTexture(renderer, "assets/image/2hit/cracked6.png"),
        IMG_LoadTexture(renderer, "assets/image/2hit/cracked7.png"),
        IMG_LoadTexture(renderer, "assets/image/2hit/cracked8.png"),
        IMG_LoadTexture(renderer, "assets/image/2hit/cracked9.png"),
        IMG_LoadTexture(renderer, "assets/image/2hit/cracked10.png")
    };

    // Kiểm tra nếu bất kỳ texture nào không tải được
    for (auto tex : oneHitBrickTextures) {
        if (!tex) {
            std::cout << "Failed to load one-hit brick texture: " << IMG_GetError() << std::endl;
            return;
        }
    }
    for (auto tex : twoHitBrickFullTextures) {
        if (!tex) {
            std::cout << "Failed to load two-hit full brick texture: " << IMG_GetError() << std::endl;
            return;
        }
    }
    for (auto tex : twoHitBrickCrackedTextures) {
        if (!tex) {
            std::cout << "Failed to load two-hit cracked brick texture: " << IMG_GetError() << std::endl;
            return;
        }
    }

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
                // Gạch 1 hit: Chọn ngẫu nhiên texture từ danh sách
                SDL_Texture* randomOneHitTexture = oneHitBrickTextures[rand() % oneHitBrickTextures.size()];
                bricks.emplace_back(x, y, brickWidth, brickHeight, Brick::BrickType::ONE_HIT, randomOneHitTexture);
            } else {
                // Gạch 2 hit: Chọn ngẫu nhiên texture từ danh sách
                SDL_Texture* randomTwoHitFullTexture = twoHitBrickFullTextures[rand() % twoHitBrickFullTextures.size()];
                SDL_Texture* randomTwoHitCrackedTexture = twoHitBrickCrackedTextures[rand() % twoHitBrickCrackedTextures.size()];
                bricks.emplace_back(x, y, brickWidth, brickHeight, Brick::BrickType::TWO_HIT, nullptr, randomTwoHitFullTexture, randomTwoHitCrackedTexture);
            }
        }
    }
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT){
            running = false;
		}   
        if (isStartScreen) {
            handleStartScreenEvent(event);
        }
        else {
            handlePauseEvent(event);
            if (!isPaused && !isGameOver) {
                paddle->handleEvent(event);
            }
            if (isGameOver && event.type == SDL_MOUSEBUTTONDOWN) {
                int x = event.button.x;
                int y = event.button.y;
                if (x >= restartRect.x && x <= restartRect.x + restartRect.w &&
                    y >= restartRect.y && y <= restartRect.y + restartRect.h) {
                    resetGame();
                }
            }
        }
    }
}
void Game::handleStartScreenEvent(const SDL_Event& e) {
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        int x = e.button.x;
        int y = e.button.y;
        if (x >= playButtonRect.x && x <= playButtonRect.x + playButtonRect.w &&
            y >= playButtonRect.y && y <= playButtonRect.y + playButtonRect.h) {
            isStartScreen = false;
            Mix_HaltMusic();
            Mix_PlayMusic(gameMusic, -1); // Play the game music
            Mix_Chunk* clickSound = Mix_LoadWAV("assets/sound/click.wav");
            if (clickSound) {
                Mix_PlayChannel(-1, clickSound, 0);
            }
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
    if (isPaused || isGameOver || isStartScreen) return;

    paddle->update();
    ball->update(*paddle, running, lives);

    if (lives <= 0 && !isGameOver) {
        isGameOver = true;
    }

    bool allBricksDestroyed = true; // Add this line

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

            // Phát âm thanh khi bóng va chạm gạch
            Mix_PlayChannel(-1, brickHitSound, 0);
        }

        if (!brick.IsDestroyed()) {
            allBricksDestroyed = false; // Add this line
        }
    }

    if (allBricksDestroyed && !isGameOver) { // Add this block
        isGameOver = true;
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

    if (isStartScreen) {
        renderStartScreen();
    }
    else {
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

        // Render pause button
        if (pauseTexture) {
            SDL_RenderCopy(renderer, pauseTexture, NULL, &pauseRect);
        }

        // Render game logo if paused
        if (isPaused) {
            if (logoTexture) {
                SDL_RenderCopy(renderer, logoTexture, nullptr, &logoRect);
            }
        }

        // Render game over screen if game is over
        if (isGameOver) {
            renderGameOver();
        }
    }

    SDL_RenderPresent(renderer);
}

void Game::renderStartScreen() {
    if (startScreenTexture) {
        SDL_RenderCopy(renderer, startScreenTexture, NULL, NULL);
    }
    if (playButtonTexture) {
        SDL_RenderCopy(renderer, playButtonTexture, NULL, &playButtonRect);
    }
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
    if (scoreTexture) {
        SDL_DestroyTexture(scoreTexture);
        scoreTexture = nullptr;
    }
    if (font) {
        TTF_CloseFont(font);
        font = nullptr; // Set font to nullptr after closing it
    }
    if (startScreenTexture) {
        SDL_DestroyTexture(startScreenTexture);
        startScreenTexture = nullptr;
    }
    if (playButtonTexture) {
        SDL_DestroyTexture(playButtonTexture);
        playButtonTexture = nullptr;
    }
    if (startMusic) {
        Mix_FreeMusic(startMusic);
        startMusic = nullptr;
    }
    if (gameMusic) {
        Mix_HaltMusic(); // Stop the game music
        Mix_FreeMusic(gameMusic);
        gameMusic = nullptr;
    }
    if (brickHitSound) {
        Mix_FreeChunk(brickHitSound);
        brickHitSound = nullptr;
    }
    for (auto tex : oneHitBrickTextures) {
        SDL_DestroyTexture(tex);
    }
    for (auto tex : twoHitBrickFullTextures) {
        SDL_DestroyTexture(tex);
    }
    for (auto tex : twoHitBrickCrackedTextures) {
        SDL_DestroyTexture(tex);
    }

    TTF_Quit();
    Mix_Quit();
    SDL_Quit();
}

void Game::run() {
    running = true;
    while (running) {
        handleEvents();  // Xử lý các sự kiện
        if (isGameOver) {
            // Game over - không update thêm gì nữa
            render();  // Chỉ render game over
            SDL_Delay(16);
            continue;
        }
        update();  // Cập nhật trạng thái game (chỉ khi chưa game over)
        render();  // Render game
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

    gameOverRect = { 200, 150, 400, 200 }; // Position and size of the game over image
    restartRect = { 300, 400, 200, 100 }; // Position and size of the restart image
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

    logoRect = { 200, 150, 400, 200 }; // Position and size of the game logo
}

void Game::resetGame() {
    lives = 3;
    score = 0;
    isGameOver = false;
    initBricks();
    updateScoreTexture();
    delete ball;
    ball = new Ball(renderer);
    delete paddle;
    paddle = new Paddle(renderer);
    powerUps.clear();
}
 