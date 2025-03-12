#ifndef BALL_H
#define BALL_H

#include <SDL.h>
#include "Paddle.h"

class Ball {
public:
    Ball(SDL_Renderer* renderer);
    ~Ball();

    void update(Paddle& paddle, bool& running);
    void render();

private:
    SDL_Renderer* renderer;
    SDL_Rect ballRect;
    int velocityX, velocityY;
};

#endif
