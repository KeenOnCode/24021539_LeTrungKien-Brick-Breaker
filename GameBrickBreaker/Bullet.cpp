#include "Bullet.h"

// Constructor: Khởi tạo viên đạn với renderer được truyền vào và tọa độ ban đầu (x, y).
// Ở đây, ta thiết lập kích thước hình chữ nhật của viên đạn (5x10) và tốc độ di chuyển ban đầu là 10.
Bullet::Bullet(SDL_Renderer* renderer, int x, int y)
    : renderer(renderer), speed(10) {
    rect = { x, y, 5, 10 }; // Kích thước mẫu của viên đạn: rộng 5, cao 10
}

// Hàm update: Cập nhật vị trí của viên đạn.
// Viên đạn di chuyển theo hướng lên (giảm giá trị y) với tốc độ đã được thiết lập.
void Bullet::update() {
    rect.y -= speed;
}

// Hàm render: Vẽ viên đạn lên màn hình.
// Đặt màu trắng và vẽ hình chữ nhật đại diện cho viên đạn.
void Bullet::render() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Đặt màu vẽ: trắng
    SDL_RenderFillRect(renderer, &rect); // Vẽ hình chữ nhật của viên đạn
}

// Hàm getRect: Trả về hình chữ nhật (SDL_Rect) của viên đạn, dùng để kiểm tra va chạm.
SDL_Rect Bullet::getRect() const {
    return rect;
}
