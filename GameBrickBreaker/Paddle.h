#ifndef PADDLE_H
#define PADDLE_H

#include <SDL.h>

class Paddle {
public:
    Paddle(SDL_Renderer* renderer);
    ~Paddle();

    // Xử lý phím bấm để di chuyển
    void handleEvent(const SDL_Event& e);

    // Cập nhật vị trí paddle
    void update();

    // Vẽ paddle lên màn hình
    void render();

    // Lấy hitbox (khung chữ nhật) của paddle
    SDL_Rect getRect() const;

private:
    SDL_Renderer* renderer;
    SDL_Rect paddleRect;
    int speed;         // Tốc độ di chuyển paddle
    int moveDirection; // -1: sang trái, +1: sang phải, 0: đứng yên
};

#endif
