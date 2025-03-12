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

    
    SDL_Rect getRect() const;  // Lấy hitbox của bóng
    void bounce();             // Xử lý va chạm với gạch

private:
    SDL_Renderer* renderer;
    SDL_Rect ballRect;
    int velocityX, velocityY;
};

#endif
