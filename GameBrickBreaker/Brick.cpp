#include "Brick.h"
#include <random>

Brick::Brick(int x, int y, int width, int height, BrickType type, SDL_Texture* oneHitTexture, SDL_Texture* twoHitFullTexture, SDL_Texture* twoHitCrackedTexture)
    : rect{ x, y, width, height }, type(type), destroyed(false), oneHitTexture(oneHitTexture), twoHitFullTexture(twoHitFullTexture), twoHitCrackedTexture(twoHitCrackedTexture) {
    hitPoints = (type == BrickType::TWO_HIT) ? 2 : 1;
    hasPowerUp = (rand() % 2 == 0);
    powerUpType = static_cast<PowerUp::Type>(rand() % 7);
}

void Brick::Render(SDL_Renderer* renderer) {
    if (destroyed) return;

    if (type == BrickType::ONE_HIT && oneHitTexture) {
        SDL_RenderCopy(renderer, oneHitTexture, nullptr, &rect);
    }
    else if (type == BrickType::TWO_HIT) {
        if (hitPoints == 2 && twoHitFullTexture) {
            SDL_RenderCopy(renderer, twoHitFullTexture, nullptr, &rect);
        }
        else if (hitPoints == 1 && twoHitCrackedTexture) {
            SDL_RenderCopy(renderer, twoHitCrackedTexture, nullptr, &rect);
        }
    }
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

bool Brick::shouldDropPowerUp() const {
    return hasPowerUp;
}

PowerUp::Type Brick::getPowerUpType() const {
    return powerUpType;
}
