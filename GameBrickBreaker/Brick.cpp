#include "Brick.h"

Brick::Brick(int x, int y, int width, int height, BrickType type)
    : rect{ x, y, width, height }, type(type), destroyed(false) {
    hitPoints = (type == BrickType::TWO_HIT) ? 2 : 1;
}

void Brick::Render(SDL_Renderer* renderer) {
    if (destroyed) return;

    // Đặt màu theo loại gạch
    if (type == BrickType::ONE_HIT) {
        SDL_SetRenderDrawColor(renderer, 0,255, 0, 255); // Đỏ
    }
    else {
        if (hitPoints == 2) {
            SDL_SetRenderDrawColor(renderer, 255, 175, 0, 255); // Vàng
        }
        else {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Cam
        }
    }

    SDL_RenderFillRect(renderer, &rect);
}

bool Brick::Hit() {
    if (hitPoints > 0) hitPoints--;
    if (hitPoints == 0) {
        destroyed = true;
        return true;
    }
    return false;
}


SDL_Rect Brick::GetRect() const {
    return rect;
}

bool Brick::IsDestroyed() const {
    return destroyed;
}
bool Brick::CheckCollision(const Ball& ball) {
    SDL_Rect ballRect = ball.getRect();
    return SDL_HasIntersection(&rect, &ballRect);
}

