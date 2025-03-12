#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <iostream>
#include <vector>
#include <string>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int PADDLE_WIDTH = 100;
const int PADDLE_HEIGHT = 10;
const int BALL_SIZE = 10;
const int BRICK_ROWS = 5;
const int BRICK_COLS = 8;
const int BRICK_WIDTH = SCREEN_WIDTH / BRICK_COLS;
const int BRICK_HEIGHT = 20;
const int FRAME_RATE = 60;


struct Brick {
    SDL_Rect rect;
    bool active;
};
SDL_Texture* backgroundTexture = nullptr;
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Rect paddle, ball;
std::vector<Brick> bricks;
bool running = true;
int ballVelX = 3, ballVelY = -3;
bool moveLeft = false, moveRight = false;
int score = 0;
bool gameOver = false;
TTF_Font* font = nullptr;

void resetGame() {
    paddle = { SCREEN_WIDTH / 2 - PADDLE_WIDTH / 2, SCREEN_HEIGHT - 50, PADDLE_WIDTH, PADDLE_HEIGHT };
    ball = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, BALL_SIZE, BALL_SIZE };
    ballVelX = 3;
    ballVelY = -3;
    score = 0;
    gameOver = false;
    bricks.clear();

    // Tính lại chiều rộng viên gạch
    const int BRICK_WIDTH = SCREEN_WIDTH / BRICK_COLS;

    for (int i = 0; i < BRICK_ROWS; ++i) {
        for (int j = 0; j < BRICK_COLS; ++j) {
            bricks.push_back({ { j * BRICK_WIDTH, i * BRICK_HEIGHT, BRICK_WIDTH, BRICK_HEIGHT }, true });
        }
    }
}


void handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) running = false;
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_LEFT) moveLeft = true;
            if (event.key.keysym.sym == SDLK_RIGHT) moveRight = true;
            if (gameOver && event.key.keysym.sym == SDLK_RETURN) resetGame();
        }
        if (event.type == SDL_KEYUP) {
            if (event.key.keysym.sym == SDLK_LEFT) moveLeft = false;
            if (event.key.keysym.sym == SDLK_RIGHT) moveRight = false;
        }
    }
}

void update() {
    if (moveLeft) paddle.x -= 6;
    if (moveRight) paddle.x += 6;
    if (paddle.x < 0) paddle.x = 0;
    if (paddle.x + PADDLE_WIDTH > SCREEN_WIDTH) paddle.x = SCREEN_WIDTH - PADDLE_WIDTH;

    ball.x += ballVelX;
    ball.y += ballVelY;
    if (ball.x <= 0 || ball.x + BALL_SIZE >= SCREEN_WIDTH) ballVelX = -ballVelX;
    if (ball.y <= 0) ballVelY = -ballVelY;

    if (SDL_HasIntersection(&ball, &paddle)) ballVelY = -ballVelY;
    for (auto& brick : bricks) {
        if (brick.active && SDL_HasIntersection(&ball, &brick.rect)) {
            brick.active = false;
            ballVelY = -ballVelY;
            score++;
        }
    }

    if (ball.y + BALL_SIZE >= SCREEN_HEIGHT) gameOver = true;
}

void renderText(const std::string& text, int x, int y) {
    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), white);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dst = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &dst);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void render() {
    SDL_RenderClear(renderer);


    if (backgroundTexture) {
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
    }


    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &paddle);
    SDL_RenderFillRect(renderer, &ball);

    for (const auto& brick : bricks) {
        if (brick.active) {
            SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
            SDL_RenderFillRect(renderer, &brick.rect);
        }
    }

    renderText("Score: " + std::to_string(score), 10, 10);
    if (gameOver) renderText("Game Over!", 200, 250);

    SDL_RenderPresent(renderer);
}


int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    window = SDL_CreateWindow("Brick Breaker", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Surface* bgSurface = IMG_Load("assets/image/nen.png");
    if (!bgSurface) {
        std::cerr << "Failed to load background image: " << IMG_GetError() << std::endl;
    }
    else {
        backgroundTexture = SDL_CreateTextureFromSurface(renderer, bgSurface);
        SDL_FreeSurface(bgSurface);
    }

    font = TTF_OpenFont("assets/font/VT323-Regular.ttf", 30);
    if (!font) std::cerr << "Failed to load font!" << std::endl;
    resetGame();

    Uint32 frameDelay = 1000 / FRAME_RATE;
    while (running) {
        Uint32 frameStart = SDL_GetTicks();
        handleEvents();
        if (!gameOver) update();
        render();
        Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime) SDL_Delay(frameDelay - frameTime);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(backgroundTexture);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    return 0;
}