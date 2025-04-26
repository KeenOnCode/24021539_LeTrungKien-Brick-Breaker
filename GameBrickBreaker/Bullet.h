#pragma once
#include <SDL.h>

/*
 * Lớp Bullet đại diện cho viên đạn trong trò chơi.
 * Lớp này cung cấp các phương thức để cập nhật, vẽ viên đạn lên màn hình
 * và lấy hình chữ nhật (SDL_Rect) tương ứng với viên đạn (dùng cho va chạm).
 */
class Bullet {
public:
    // Constructor: khởi tạo viên đạn với renderer được cung cấp và vị trí ban đầu (x, y)
    Bullet(SDL_Renderer* renderer, int x, int y);

    // Hàm update: cập nhật vị trí của viên đạn (di chuyển theo hướng)
    void update();

    // Hàm render: vẽ viên đạn lên màn hình sử dụng renderer đã cung cấp
    void render();

    // Hàm getRect: trả về hình chữ nhật (SDL_Rect) của viên đạn, dùng để xử lý va chạm
    SDL_Rect getRect() const;

private:
    SDL_Renderer* renderer; // Con trỏ renderer dùng để vẽ viên đạn lên cửa sổ
    SDL_Rect rect;          // Hình chữ nhật định vị vị trí và kích thước của viên đạn
    int speed;              // Tốc độ di chuyển của viên đạn
};
