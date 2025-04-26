#include "PowerUp.h"
#include <iostream>
#include <random>
#include <vector>

// Constructor: Khởi tạo đối tượng PowerUp với renderer, loại hiệu ứng và vị trí hiển thị (x, y)
PowerUp::PowerUp(SDL_Renderer* renderer, Type type, int x, int y)
    : renderer(renderer), type(type), speed(5) {
    // Khởi tạo hình chữ nhật biểu diễn vị trí và kích thước của PowerUp (30x30)
    rect = { x, y, 30, 30 };
    // Nạp texture cho PowerUp từ file ảnh
    loadTexture(renderer);
}

// Hàm isExpired: Kiểm tra xem PowerUp đã hết thời gian hiệu lực hay chưa
// Sử dụng SDL_GetTicks() để lấy thời gian hiện tại (miligiây)
bool PowerUp::isExpired() const {
    return SDL_GetTicks() > activationTime + duration; // duration là thời gian hiệu lực (5000ms)
}

// Destructor: Giải phóng tài nguyên, đặc biệt là texture được cấp phát cho PowerUp
PowerUp::~PowerUp() {
    if (texture) {
        SDL_DestroyTexture(texture);
    }
}

// Hàm update: Cập nhật vị trí của PowerUp theo hướng di chuyển (ở đây chỉ di chuyển theo trục Y)
void PowerUp::update() {
    rect.y += speed;
}

// Hàm render: Vẽ PowerUp lên màn hình tại vị trí được xác định bởi rect
void PowerUp::render() {
    if (texture) {
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
    }
    else {
        // Nếu không có texture, vẽ PowerUp bằng màu đỏ
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
}

// Hàm getRect: Trả về hình chữ nhật (SDL_Rect) của PowerUp
SDL_Rect PowerUp::getRect() const {
    return rect;
}

// Hàm getType: Trả về loại hiệu ứng (Type) của PowerUp
PowerUp::Type PowerUp::getType() const {
    return type;
}

// Hàm loadTexture: Nạp texture cho PowerUp từ file ảnh
void PowerUp::loadTexture(SDL_Renderer* renderer) {
    // Nạp một surface từ file ảnh
    SDL_Surface* surface = IMG_Load("assets/image/model/powerup.png");
    if (!surface) {
        std::cout << "❌ Failed to load power-up image: " << IMG_GetError() << std::endl;
        return;
    }
    // Tạo texture từ surface vừa nạp và giải phóng surface sau đó
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
}
