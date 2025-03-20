#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <vector>
#include "Paddle.h"
#include "Ball.h"
#include "Brick.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

class Game {
public:
    Game();
    ~Game();

    bool init();
    void handleEvents();
    void update();
    void render();
    void clean();
    void run();
    void LoadBackground(SDL_Renderer* renderer);
    void RenderBackground(SDL_Renderer* renderer);
    void CleanupBackground();

private:
	SDL_Window* window; // ✅ Thêm biến window
	SDL_Renderer* renderer; // ✅ Thêm biến renderer
	bool running;   // Trạng thái game đang chạy hay không

	Paddle* paddle; // ✅ Thêm biến thanh chắn
	Ball* ball; // ✅ Thêm biến quả bóng
    std::vector<Brick> bricks; // Danh sách gạch
	int lives; // Số mạng còn lại
	SDL_Texture* heartTexture; // Ảnh mạng
	void renderHearts(); // Vẽ mạng
    void initBricks(); // Hàm khởi tạo gạch
};

#endif
