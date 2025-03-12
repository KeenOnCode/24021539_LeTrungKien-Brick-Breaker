#ifndef BALL_H
#define BALL_H

#include <SDL.h>

class Ball {
public:
    Ball(SDL_Renderer* renderer);
    ~Ball();

    void update();
    void render();

private:
    SDL_Renderer* renderer;
    SDL_Rect ballRect;
    int velocityX, velocityY;
};

#endif

