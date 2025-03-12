#ifndef BRICK_H
#define BRICK_H

#include <SDL.h>
#include "Ball.h" // Thêm dòng này

class Brick {
public:
    enum class BrickType {
        ONE_HIT,
        TWO_HIT
    };

    Brick(int x, int y, int width, int height, BrickType type);

    void Render(SDL_Renderer* renderer);
    bool Hit(); // Trả về true nếu gạch bị phá hủy hoàn toàn

    SDL_Rect GetRect() const;
    bool IsDestroyed() const;

    bool CheckCollision(const Ball& ball); // Thêm hàm kiểm tra va chạm

private:
    SDL_Rect rect;
    BrickType type;
    int hitPoints; // Số lần chạm trước khi vỡ
    bool destroyed;
};

#endif
