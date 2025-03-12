#pragma once
#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include "Paddle.h"
#include "Ball.h"


const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

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
};

#endif
