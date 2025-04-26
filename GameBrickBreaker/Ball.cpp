#include "Ball.h"
#include <SDL_image.h>
#include "Game.h"
// Constructor: Khởi tạo đối tượng Ball với renderer được truyền vào.
// Ở đây, chúng ta thiết lập vận tốc ban đầu của bóng và vị trí xuất phát của nó.

Ball::Ball(SDL_Renderer* renderer)
    : renderer(renderer), velocityX(5), velocityY(-5) {
    // Khởi tạo hình chữ nhật xác định vị trí (x, y) và kích thước (width, height) của bóng.
    ballRect = { 400, 550, 25, 25 };
    // Nạp texture cho bóng từ file ảnh.
    ballTexture = IMG_LoadTexture(renderer, "assets/image/model/ball.png");

}

// Destructor: Giải phóng các tài nguyên được cấp phát cho bóng, như texture.
Ball::~Ball() {
    if (ballTexture) {
        SDL_DestroyTexture(ballTexture);
    }

}

// Hàm setPosition: Đặt vị trí của bóng theo tọa độ x và y.
void Ball::setPosition(int x, int y) {
    ballRect.x = x;
    ballRect.y = y;
}

// Hàm release: Giải phóng bóng khỏi trạng thái "gắn" (attached), đặt lại vận tốc ban đầu.
void Ball::release() {
    // Đặt vận tốc ban đầu cho bóng (ví dụ có thể được tinh chỉnh thêm)
    velocityX = 5;  // Vận tốc theo trục X
    velocityY = -5; // Vận tốc theo trục Y (âm nghĩa bóng di chuyển lên trên)
}

// Hàm update: Cập nhật trạng thái của bóng trong game.
// Thực hiện chuyển động của bóng, kiểm tra va chạm với biên của màn hình và paddle,
// xử lý khi bóng rơi ra ngoài màn hình.
void Ball::update(Paddle& paddle, bool& running, int& lives) {
    // Cập nhật vị trí bóng dựa trên vận tốc hiện tại.
    ballRect.x += velocityX;
    ballRect.y += velocityY;

    // Kiểm tra va chạm với các biên trái và phải của màn hình (chiều rộng = 800).
    if (ballRect.x <= 0 || ballRect.x + ballRect.w >= 800) {
        velocityX = -velocityX; // Đảo hướng vận tốc theo trục X
    }
    // Kiểm tra va chạm với biên trên của màn hình.
    if (ballRect.y <= 0) {
        velocityY = -velocityY; // Đảo hướng vận tốc theo trục Y
    }
    // Kiểm tra nếu bóng rơi xuống dưới (quá 600 - chiều cao màn hình).
    if (ballRect.y + ballRect.h >= 600) {

            lives--; // Giảm số mạng
            if (lives <= 0) {
                running = true; // Nếu hết mạng, báo hiệu trạng thái game (có thể là game over)
            }
     
        // Reset vị trí bóng và đảo hướng vận tốc theo trục Y
        ballRect.x = 400;
        ballRect.y = 300;
        velocityY = -velocityY;

    }

    // Lấy hình chữ nhật của paddle để kiểm tra va chạm
    SDL_Rect paddleRect = paddle.getRect();
    // Nếu bóng va chạm với paddle, đảo hướng vận tốc theo trục Y và điều chỉnh vị trí bóng
    if (SDL_HasIntersection(&ballRect, &paddleRect)) {
        velocityY = -velocityY;
        ballRect.y = paddleRect.y - ballRect.h;
    }
}


// Hàm render: Vẽ bóng lên màn hình tại vị trí xác định bởi ballRect.
void Ball::render() {
    SDL_RenderCopy(renderer, ballTexture, nullptr, &ballRect);
}

// Hàm getRect: Trả về hình chữ nhật của bóng, thường được sử dụng cho mục đích va chạm.
SDL_Rect Ball::getRect() const {
    return ballRect;
}

// Hàm bounce: Xử lý va chạm của bóng bằng cách đảo ngược vận tốc theo trục Y.
void Ball::bounce(const SDL_Rect& collisionObject) {
    // Tính vị trí trung tâm của bóng và đối tượng va chạm
    int ballCenter = ballRect.x + ballRect.w / 2;
    int objectCenter = collisionObject.x + collisionObject.w / 2;

    // Tính khoảng cách tương đối giữa trung tâm bóng và đối tượng va chạm
    int relativeHit = ballCenter - objectCenter;

    // Điều chỉnh vận tốc X dựa trên khoảng cách tương đối
    velocityX = relativeHit / 10; // Chia nhỏ để giảm độ nhạy
    if (velocityX > 3) velocityX = 3;
    if (velocityX < -3) velocityX = -3;
    // Đảo hướng vận tốc Y
    velocityY = -velocityY;

    // Thêm một chút ngẫu nhiên vào vận tốc X để tạo sự không đoán trước
    velocityX += (rand() % 3 - 1); // Thêm -1, 0 hoặc 1 vào vận tốc X
}

