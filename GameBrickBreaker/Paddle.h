#pragma once
#include <SDL.h>                // Bao gồm thư viện SDL cơ bản
#include <vector>               // Bao gồm container vector để lưu trữ các đối tượng
#include "Bullet.h"             // Bao gồm định nghĩa của lớp Bullet
#include "PowerUp.h"            // Bao gồm định nghĩa của lớp PowerUp
#include <string>               // Bao gồm thư viện xử lý chuỗi (std::string)
#include <chrono>               // Bao gồm thư viện để xử lý thời gian

// Lớp Paddle đại diện cho thanh trượt trong trò chơi Brick Breaker.
// Nó quản lý các hành vi như di chuyển, render, và xử lý các hiệu ứng (power-up).
class Paddle {
public:
    // Constructor: khởi tạo đối tượng Paddle với tham số renderer được truyền vào.
    Paddle(SDL_Renderer* renderer);

    // Destructor: giải phóng tài nguyên của Paddle khi đối tượng bị hủy.
    ~Paddle();

    // Hàm handleEvent: xử lý sự kiện (nhấn phím, thả phím, ...) liên quan đến paddle.
    void handleEvent(const SDL_Event& e);

    // Hàm update: cập nhật vị trí, trạng thái và các viên đạn của paddle.
    void update();

    // Hàm render: vẽ paddle lên màn hình.
    void render();

    // Hàm getRect: trả về hình chữ nhật (SDL_Rect) của paddle, dùng cho mục đích va chạm.
    SDL_Rect getRect() const;

    // Hàm applyPowerUp: áp dụng hiệu ứng power-up cho paddle theo loại được truyền vào.
    void applyPowerUp(PowerUp::Type type);

    // Hàm shoot: xử lý hành vi bắn đạn bởi paddle.
    void shoot();

    // Hàm getBullets: trả về danh sách các viên đạn (Bullet) hiện có của paddle.
    std::vector<Bullet>& getBullets();

    // Hàm resetToDefault: đưa paddle về trạng thái mặc định (tốc độ, kích thước, ...) sau khi áp dụng power-up.
    void resetToDefault();

private:
    // Con trỏ renderer dùng để vẽ các thành phần của paddle lên cửa sổ.
    SDL_Renderer* renderer;

    // Hình chữ nhật (SDL_Rect) xác định vị trí và kích thước của paddle.
    SDL_Rect paddleRect;

    // Texture của paddle, được nạp từ file ảnh.
    SDL_Texture* paddleTexture;

    // Tốc độ di chuyển của paddle.
    int speed;

    // Giá trị mặc định của tốc độ di chuyển.
    int defaultSpeed = 10; // Tốc độ mặc định

    // Giá trị kích thước (chiều rộng) hiện tại của paddle.
    int width;

    // Giá trị kích thước mặc định (chiều rộng) của paddle.
    int defaultWidth = 100; // Kích thước mặc định

    // Biến xác định hướng di chuyển của paddle:
    // -1: sang trái, 1: sang phải, 0: không di chuyển.
    int moveDirection;

    // Cờ đánh dấu paddle có khả năng bắn đạn hay không.
    bool hasGun;

    // Danh sách các viên đạn mà paddle bắn ra.
    std::vector<Bullet> bullets;

    // Loại power-up hiện tại đang được áp dụng cho paddle.
    PowerUp::Type currentPowerUp;

    // Thời gian bắt đầu hiệu lực của power-up.
    std::chrono::time_point<std::chrono::steady_clock> powerUpStartTime;

    // Thời gian hiệu lực của power-up (tính theo giây).
    int powerUpDuration; // in seconds

    // Thời gian của lần bắn đạn cuối cùng, dùng để quản lý việc bắn tự động.
    std::chrono::time_point<std::chrono::steady_clock> lastBulletTime;

    // Hàm loadTexture: nạp texture từ đường dẫn file ảnh cho paddle.
    void loadTexture(const std::string& path);

    // Hàm resetPowerUp: reset lại hiệu ứng power-up đã áp dụng, đưa paddle trở lại trạng thái ban đầu.
    void resetPowerUp();
};
