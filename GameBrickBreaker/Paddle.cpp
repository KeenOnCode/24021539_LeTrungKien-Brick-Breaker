#include "Paddle.h"
#include <iostream>
#include <SDL_image.h>

// Constructor của lớp Paddle, khởi tạo các giá trị mặc định và nạp texture
Paddle::Paddle(SDL_Renderer* renderer)
    : renderer(renderer),               // Gán renderer nhận được từ đối tượng gọi
    speed(10),                        // Tốc độ di chuyển mặc định là 10
    defaultSpeed(10),                 // Lưu tốc độ mặc định để reset sau này
    moveDirection(0),                 // Ban đầu không di chuyển (0)
    hasGun(false),                    // Chưa có gun (để bắn đạn)
    paddleTexture(nullptr),           // Texture của paddle chưa được tạo
    currentPowerUp(PowerUp::NONE),    // Chưa có power-up nào được áp dụng
    powerUpDuration(0)                // Thời gian hiệu lực của power-up ban đầu là 0
{
    // Khởi tạo hình chữ nhật định vị paddle (x, y, width, height)
    paddleRect = { 350, 550, 100, 20 };
    // Nạp texture của paddle từ file ảnh mặc định
    loadTexture("assets/image/model/paddle.png");
    // Ghi nhận thời điểm ban đầu cho việc bắn đạn (để xử lý tự động bắn)
    lastBulletTime = std::chrono::steady_clock::now();
}

// Destructor của lớp Paddle, giải phóng texture của paddle nếu tồn tại
Paddle::~Paddle() {
    if (paddleTexture) {
        SDL_DestroyTexture(paddleTexture);
    }
}

// Hàm xử lý sự kiện bàn phím cho paddle
void Paddle::handleEvent(const SDL_Event& e) {
    // Kiểm tra xem sự kiện là nhấn phím và không phải lặp phím
    if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
        switch (e.key.keysym.sym) {
        case SDLK_LEFT:
            moveDirection = -1; // Di chuyển sang trái
            break;
        case SDLK_RIGHT:
            moveDirection = 1;  // Di chuyển sang phải
            break;
        case SDLK_SPACE:
            // Nếu có gun, khi nhấn space sẽ bắn đạn
            if (hasGun)
                shoot();
            break;
        }
    }
    // Khi thả phím, nếu phím đang tác động thì dừng di chuyển tương ứng
    else if (e.type == SDL_KEYUP && e.key.repeat == 0) {
        switch (e.key.keysym.sym) {
        case SDLK_LEFT:
            if (moveDirection == -1)
                moveDirection = 0; // Dừng di chuyển về bên trái
            break;
        case SDLK_RIGHT:
            if (moveDirection == 1)
                moveDirection = 0; // Dừng di chuyển về bên phải
            break;
        }
    }
}

// Hàm cập nhật vị trí và trạng thái của paddle
void Paddle::update() {
    // Cập nhật vị trí theo hướng di chuyển và tốc độ
    paddleRect.x += moveDirection * speed;
    // Giới hạn paddle không vượt khỏi bên trái của màn hình
    if (paddleRect.x < 0)
        paddleRect.x = 0;
    // Giới hạn paddle không vượt ra ngoài bên phải của màn hình (chiều rộng màn hình = 800)
    if (paddleRect.x + paddleRect.w > 800)
        paddleRect.x = 800 - paddleRect.w;

    // Cập nhật tất cả các viên đạn đang được phát ra
    for (auto& bullet : bullets) {
        bullet.update();
    }

    // Kiểm tra xem power-up hiện tại có còn hiệu lực hay không
    if (currentPowerUp != PowerUp::NONE) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - powerUpStartTime).count();
        if (elapsed >= powerUpDuration) {
            resetPowerUp(); // Reset lại hiệu ứng của power-up khi hết thời gian
        }
    }

    // Xử lý bắn tự động nếu paddle có gun (tạo hiệu ứng bắn liên tục)
    if (hasGun) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastBulletTime).count();
        if (elapsed >= 333) { // Mỗi 333ms bắn 1 viên đạn (tương đương 3 viên đạn trên giây)
            shoot();
            lastBulletTime = now; // Cập nhật thời điểm bắn đạn
        }
    }
}

// Hàm vẽ paddle và các viên đạn lên màn hình
void Paddle::render() {
    // Vẽ paddle từ texture nếu đã nạp thành công
    if (paddleTexture) {
        SDL_RenderCopy(renderer, paddleTexture, nullptr, &paddleRect);
    }
    else {
        // Nếu không có texture, vẽ paddle bằng màu trắng
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &paddleRect);
    }

    // Vẽ tất cả các viên đạn
    for (auto& bullet : bullets) {
        bullet.render();
    }
}

// Hàm trả về hình chữ nhật của paddle, dùng để kiểm tra va chạm
SDL_Rect Paddle::getRect() const {
    return paddleRect;
}

// Hàm áp dụng hiệu ứng power-up lên paddle theo loại power-up nhận được
void Paddle::applyPowerUp(PowerUp::Type type) {
    // Nếu đã có power-up đang hoạt động, thông báo và không áp dụng thêm
    if (currentPowerUp != PowerUp::NONE) {
        std::cout << "⚠️ Power-up đang hoạt động, không thể áp dụng power-up mới!" << std::endl;
        return;
    }
    // Gán loại power-up và lưu thời gian bắt đầu hiệu lực
    currentPowerUp = type;
    powerUpStartTime = std::chrono::steady_clock::now();
    powerUpDuration = 2; // Hiệu lực của power-up kéo dài 2 giây

    // Thay đổi thuộc tính của paddle theo từng loại power-up
    switch (type) {
    case PowerUp::INCREASE_BALL_SIZE:
        loadTexture("assets/image/model/paddle_increase_ball_size.png");
        paddleRect.w += 50; // Tăng chiều rộng paddle
        break;
    case PowerUp::DECREASE_BALL_SIZE:
        loadTexture("assets/image/model/paddle_decrease_ball_size.png");
        paddleRect.w -= 25; // Giảm chiều rộng paddle
        break;
    case PowerUp::INCREASE_PADDLE_SPEED:
        loadTexture("assets/image/model/paddle_increase_speed.png");
        speed += 5;         // Tăng tốc độ di chuyển
        break;
    case PowerUp::DECREASE_PADDLE_SPEED:
        loadTexture("assets/image/model/paddle_decrease_speed.png");
        speed -= 5;         // Giảm tốc độ di chuyển
        break;
    case PowerUp::PADDLE_WITH_GUN:
        loadTexture("assets/image/model/paddle_with_gun.png");
        hasGun = true;      // Kích hoạt chế độ bắn đạn
        break;
    case PowerUp::BALL_AS_MISSILE:
        loadTexture("assets/image/model/paddle_ball_as_missile.png");
        // Có thể thêm các thay đổi khác nếu cần cho hiệu ứng này
        break;
    }
}

// Hàm bắn đạn, tạo một viên đạn mới từ vị trí trung tâm của paddle
void Paddle::shoot() {
    bullets.push_back(Bullet(renderer, paddleRect.x + paddleRect.w / 2, paddleRect.y));
}

// Hàm reset paddle về trạng thái mặc định (tốc độ, kích thước, ...)
void Paddle::resetToDefault() {
    speed = defaultSpeed; // Reset tốc độ về giá trị mặc định
    width = defaultWidth; // Reset kích thước thanh paddle (cần khai báo defaultWidth ở class nếu chưa có)
    // Reset các thuộc tính khác nếu cần thiết
}

// Hàm nạp texture cho paddle từ đường dẫn file ảnh cho trước
void Paddle::loadTexture(const std::string& path) {
    if (paddleTexture) {
        SDL_DestroyTexture(paddleTexture); // Hủy texture cũ nếu đã nạp trước đó
    }
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        std::cout << "❌ Failed to load paddle image: " << IMG_GetError() << std::endl;
        paddleTexture = nullptr;
        return;
    }
    // Tạo texture từ surface đã nạp
    paddleTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!paddleTexture) {
        std::cout << "❌ Failed to create texture from surface: " << SDL_GetError() << std::endl;
    }
}

// Hàm trả về danh sách các viên đạn mà paddle bắn ra (dùng để xử lý va chạm)
std::vector<Bullet>& Paddle::getBullets() {
    return bullets;
}

// Hàm reset lại hiệu ứng power-up cho paddle khi thời gian hiệu lực hết
void Paddle::resetPowerUp() {
    switch (currentPowerUp) {
    case PowerUp::INCREASE_BALL_SIZE:
        paddleRect.w -= 20; // Giảm lại chiều rộng paddle ban đầu
        break;
    case PowerUp::DECREASE_BALL_SIZE:
        paddleRect.w += 20; // Tăng lại chiều rộng paddle ban đầu
        break;
    case PowerUp::INCREASE_PADDLE_SPEED:
        speed -= 5;         // Giảm tốc độ về giá trị ban đầu
        break;
    case PowerUp::DECREASE_PADDLE_SPEED:
        speed += 5;         // Tăng tốc độ về giá trị ban đầu
        break;
    case PowerUp::PADDLE_WITH_GUN:
        hasGun = false;     // Tắt chế độ bắn đạn
        break;
    case PowerUp::BALL_AS_MISSILE:
        // Reset các thay đổi nếu có đối với hiệu ứng này
        break;
    }
    // Nạp lại texture mặc định cho paddle
    loadTexture("assets/image/model/paddle.png");
    currentPowerUp = PowerUp::NONE; // Đánh dấu không có power-up nào đang hiệu lực
}
