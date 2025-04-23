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
    void setPosition(int x, int y);
    void release();

private:
    SDL_Renderer* renderer;
    SDL_Rect ballRect;
    SDL_Texture* ballTexture;
    int velocityX, velocityY;
};
