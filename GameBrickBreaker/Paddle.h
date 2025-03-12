#ifndef PADDLE_H
#define PADDLE_H

#include <SDL.h>

class Paddle {
public:
    Paddle(SDL_Renderer* renderer);
    ~Paddle();

    void handleEvent(SDL_Event& e);
    void update();
    void render();

private:
    SDL_Renderer* renderer;
    SDL_Rect paddleRect;
    int velocity;
};

#endif
