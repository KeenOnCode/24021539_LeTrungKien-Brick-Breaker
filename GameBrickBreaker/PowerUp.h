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
        BALL_AS_MISSILE,
		TRIPLE_BALL
    };

    PowerUp(SDL_Renderer* renderer, Type type, int x, int y);
    ~PowerUp();

    void update();
    void render();
    SDL_Rect getRect() const;
    Type getType() const;
	bool isExpired() const;// Kiểm tra xem power-up đã hết thời gian hiệu lực hay chưa
private:
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    SDL_Rect rect;
    Type type;
    int speed;

    Uint32 activationTime; // Thời gian kích hoạt
    Uint32 duration = 5000; // Thời gian hiệu lực (5 giây)
    void loadTexture(SDL_Renderer* renderer);
};

#endif // POWERUP_H


