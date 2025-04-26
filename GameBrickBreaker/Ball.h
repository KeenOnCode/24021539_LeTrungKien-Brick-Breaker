#ifndef BALL_H
#define BALL_H

#include <SDL.h>
#include "Paddle.h"

// Lớp Ball quản lý các thuộc tính và hành vi của bóng trong game.
class Ball {
public:
    // Constructor: Khởi tạo đối tượng Ball với renderer được truyền vào.
    // Renderer được sử dụng để tạo texture và vẽ bóng lên màn hình.
    Ball(SDL_Renderer* renderer);

    // Destructor: Giải phóng các tài nguyên đã cấp phát cho đối tượng Ball.
    ~Ball();

    // Hàm update: Cập nhật vị trí và trạng thái của bóng.
    // Tham số:
    // - Paddle& paddle: đối tượng paddle dùng để căn chỉnh vị trí bóng nếu nó còn được gắn.
    // - bool& running: cờ kiểm soát trạng thái chạy của game.
    // - int& lives: số mạng còn lại của người chơi, dùng để xử lý khi bóng ra khỏi màn hình.
    void update(Paddle& paddle, bool& running, int& lives);

    // Hàm render: Vẽ bóng lên màn hình dựa trên vị trí và texture hiện tại.
    void render();

    // Hàm getRect: Trả về hình chữ nhật (SDL_Rect) của bóng,
    // được sử dụng cho mục đích va chạm và xác định vị trí của bóng.
    SDL_Rect getRect() const;

    // Hàm bounce: Xử lý va chạm của bóng với các đối tượng,
    // đảo hướng vận tốc bóng khi xảy ra va chạm.
    void bounce();

    // Hàm setPosition: Đặt vị trí của bóng dựa vào tọa độ x và y.
    void setPosition(int x, int y);

    // Hàm release: Giải phóng bóng ra khỏi trạng thái "gắn" (attached) khi game bắt đầu,
    // cho phép bóng di chuyển độc lập sau khi được "phóng" ra từ paddle.
    void release();
   

private:
    // Con trỏ renderer dùng để vẽ các thành phần của bóng lên cửa sổ.
    SDL_Renderer* renderer;

    // ballRect lưu trữ vị trí và kích thước của bóng trong không gian 2D.
    SDL_Rect ballRect;

    // ballTexture chứa texture của bóng, sử dụng cho việc render.
    SDL_Texture* ballTexture;

    // velocityX và velocityY lưu trữ vận tốc của bóng theo trục X và Y.
    int velocityX;
    int velocityY;
};

#endif // BALL_H
