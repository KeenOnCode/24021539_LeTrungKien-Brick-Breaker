#pragma once
#include <SDL.h>
#include "PowerUp.h"

class Brick {
public:
    enum class BrickType { ONE_HIT, TWO_HIT };

    Brick(int x, int y, int width, int height, BrickType type, SDL_Texture* oneHitTexture, SDL_Texture* twoHitFullTexture = nullptr, SDL_Texture* twoHitCrackedTexture = nullptr);
    void Render(SDL_Renderer* renderer);
    bool Hit();
    SDL_Rect GetRect() const;
    bool IsDestroyed() const;
    bool shouldDropPowerUp() const;
    PowerUp::Type getPowerUpType() const;

private:
    SDL_Rect rect;
    BrickType type;
    bool destroyed;
    int hitPoints;
    SDL_Texture* oneHitTexture;
    SDL_Texture* twoHitFullTexture;
    SDL_Texture* twoHitCrackedTexture;
    bool hasPowerUp;
    PowerUp::Type powerUpType;
};
