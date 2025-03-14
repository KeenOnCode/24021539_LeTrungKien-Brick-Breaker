﻿#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <vector>
#include "Paddle.h"
#include "Ball.h"
#include "Brick.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

class Game {
public:
    Game();
    ~Game();

    bool init();
    void handleEvents();
    void update();
    void render();
    void clean();
    void run();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;

    Paddle* paddle;
    Ball* ball;
    std::vector<Brick> bricks; // Danh sách gạch

    void initBricks(); // Hàm khởi tạo gạch
};

#endif
