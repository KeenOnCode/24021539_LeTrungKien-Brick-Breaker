#pragma once
#include <SDL.h>
#include "Paddle.h"

class Ball {
public:
    Ball(SDL_Renderer* renderer);
    ~Ball();
    void update(Paddle& paddle, bool& running, int& lives);
    void render();
    SDL_Rect getRect() const;
    void bounce();

private:
    SDL_Renderer* renderer;
    SDL_Rect ballRect;
    SDL_Texture* ballTexture;
    int velocityX, velocityY;
};
