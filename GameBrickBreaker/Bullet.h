#pragma once
#include <SDL.h>

class Bullet {
public:
    Bullet(SDL_Renderer* renderer, int x, int y);
    void update();
    void render();
    SDL_Rect getRect() const;

private:
    SDL_Renderer* renderer;
    SDL_Rect rect;
    int speed;
};
