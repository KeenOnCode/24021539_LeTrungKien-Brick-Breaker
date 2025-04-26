#ifndef POWERUP_H
#define POWERUP_H

#include <SDL.h>
#include <SDL_image.h>

// Lớp PowerUp quản lý các hiệu ứng đặc biệt trong trò chơi Brick Breaker.
// Các hiệu ứng có thể ảnh hưởng đến bóng, paddle, hoặc tạo ra các hiệu ứng khác.
class PowerUp {
public:
    // Enum định nghĩa các loại hiệu ứng (PowerUp) có thể xảy ra.
    enum Type {
        NONE,                   // Không có hiệu ứng nào
        INCREASE_BALL_SIZE,     // Tăng kích thước bóng
        DECREASE_BALL_SIZE,     // Giảm kích thước bóng
        INCREASE_PADDLE_SPEED,  // Tăng tốc độ của thanh trượt
        DECREASE_PADDLE_SPEED,  // Giảm tốc độ của thanh trượt
        PADDLE_WITH_GUN,        // Thanh trượt có thêm chức năng bắn đạn
        BALL_AS_MISSILE,        // Bóng có hiệu ứng như tên lửa
        TRIPLE_BALL             // Phát sinh thêm bóng (tối đa 3 bóng)
    };

    // Constructor: Khởi tạo đối tượng PowerUp với renderer, loại hiệu ứng và vị trí biểu diễn (x, y)
    PowerUp(SDL_Renderer* renderer, Type type, int x, int y);

    // Destructor: Giải phóng các tài nguyên (như texture) đã cấp phát cho PowerUp
    ~PowerUp();

    // Hàm update: Cập nhật vị trí và các trạng thái của PowerUp
    void update();

    // Hàm render: Vẽ PowerUp lên màn hình tại vị trí xác định bởi rect
    void render();

    // Hàm getRect: Trả về hình chữ nhật (SDL_Rect) của PowerUp
    // Dùng để kiểm tra va chạm với các đối tượng khác trong game.
    SDL_Rect getRect() const;

    // Hàm getType: Trả về loại hiệu ứng của PowerUp hiện tại.
    Type getType() const;

    // Hàm isExpired: Kiểm tra xem PowerUp đã hết thời gian hiệu lực hay chưa.
    bool isExpired() const;

private:
    SDL_Renderer* renderer;   // Con trỏ renderer dùng để vẽ PowerUp lên màn hình
    SDL_Texture* texture;     // Texture của PowerUp, nạp từ file ảnh
    SDL_Rect rect;            // Hình chữ nhật xác định vị trí và kích thước của PowerUp
    Type type;                // Loại hiệu ứng của PowerUp
    int speed;                // Tốc độ di chuyển của PowerUp (nếu có hiệu ứng di chuyển)

    Uint32 activationTime;  // Thời gian PowerUp được kích hoạt (tính theo miligiây)
    Uint32 duration = 5000; // Thời gian hiệu lực của PowerUp (5 giây)

    // Hàm loadTexture: Nạp texture từ file cho PowerUp bằng renderer
    // Phương thức này được gọi trong constructor để thiết lập texture cho đối tượng PowerUp.
    void loadTexture(SDL_Renderer* renderer);
};

#endif // POWERUP_H
