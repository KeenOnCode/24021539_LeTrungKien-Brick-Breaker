#ifndef POWERUP_H
#define POWERUP_H

#include <SDL.h>
#include <SDL_image.h>

class PowerUp {
public:
    enum Type {
        NONE,
        INCREASE_BALL_SIZE,
        DECREASE_BALL_SIZE,
        INCREASE_PADDLE_SPEED,
        DECREASE_PADDLE_SPEED,
        PADDLE_WITH_GUN,
        BALL_AS_MISSILE
    };

    PowerUp(SDL_Renderer* renderer, Type type, int x, int y);
    ~PowerUp();

    void update();
    void render();
    SDL_Rect getRect() const;
    Type getType() const;

private:
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    SDL_Rect rect;
    Type type;
    int speed;

    void loadTexture(SDL_Renderer* renderer);
};

#endif // POWERUP_H