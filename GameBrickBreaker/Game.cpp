#include "Game.h"
#include "Brick.h"
#include <iostream>

Game::Game() : window(nullptr), renderer(nullptr), running(false), paddle(nullptr), ball(nullptr) {}

Game::~Game() {
    clean();
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

    paddle = new Paddle(renderer);
    ball = new Ball(renderer);

    initBricks(); // Khởi tạo gạch

    running = true;
    return true;
}

void Game::initBricks() {
    int rows = 5;
    int cols = 10;
    int brickWidth = SCREEN_WIDTH / cols;
    int brickHeight = 30;

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            Brick::BrickType type = (row % 2 == 0) ? Brick::BrickType::ONE_HIT : Brick::BrickType::TWO_HIT;
            bricks.emplace_back(col * brickWidth, row * brickHeight, brickWidth - 5, brickHeight - 5, type);
        }
    }
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT)
            running = false;
        paddle->handleEvent(event);
    }
}

void Game::update() {
    paddle->update();
    ball->update(*paddle, running);

    for (auto& brick : bricks) {
        SDL_Rect ballRect = ball->getRect();
        SDL_Rect brickRect = brick.GetRect();
        if (!brick.IsDestroyed() && SDL_HasIntersection(&ballRect, &brickRect)) {

            ball->bounce();
            brick.Hit();
        }
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    paddle->render();
    ball->render();

    for (auto& brick : bricks) {
        brick.Render(renderer);
    }

    SDL_RenderPresent(renderer);
}

void Game::clean() {
    delete paddle;
    delete ball;
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
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
