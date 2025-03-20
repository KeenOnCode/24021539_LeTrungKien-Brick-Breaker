#ifndef BALL_H
#define BALL_H

#include <SDL.h>
#include "Paddle.h"
#include <SDL_image.h>
class Ball {
public:
    Ball(SDL_Renderer* renderer);
    ~Ball();

    void update(Paddle& paddle, bool& running,int &lives);
    void render();

    
    SDL_Rect getRect() const;  // Lấy hitbox của bóng
    void bounce();             // Xử lý va chạm với gạch
    void LoadTexture(SDL_Renderer* renderer); // Hàm load ảnh bóng
private:
    SDL_Texture* ballTexture; // ✅ Biến chứa ảnh quả bóng
    SDL_Renderer* renderer;
    SDL_Rect ballRect;
    int velocityX, velocityY;
};

#endif
