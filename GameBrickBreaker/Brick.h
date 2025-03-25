#ifndef BRICK_H
#define BRICK_H

#include <SDL.h>
#include "Ball.h"

class Brick {
public:
    enum class BrickType {
        ONE_HIT,
        TWO_HIT
    };

    Brick(int x, int y, int width, int height, BrickType type,
        SDL_Texture* oneHitTexture = nullptr,
        SDL_Texture* twoHitFullTexture = nullptr,
        SDL_Texture* twoHitCrackedTexture = nullptr);

    void Render(SDL_Renderer* renderer);
    bool Hit();

    SDL_Rect GetRect() const;
    bool IsDestroyed() const;
    bool CheckCollision(const Ball& ball);

private:
    SDL_Rect rect;
    BrickType type;
    int hitPoints;
    bool destroyed;

    SDL_Texture* oneHitTexture;
    SDL_Texture* twoHitFullTexture;
    SDL_Texture* twoHitCrackedTexture;
};

#endif