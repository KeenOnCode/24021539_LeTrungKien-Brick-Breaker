#include "Brick.h"
#include <random>

// Constructor: Khởi tạo đối tượng Brick với các tham số như vị trí (x, y),
// kích thước (width, height), loại gạch (BrickType) và các texture cho trạng thái gạch khác nhau.
// Nếu loại gạch là TWO_HIT thì hitPoints sẽ được đặt là 2, còn lại là 1.
// Ngoài ra, cũng xác định ngẫu nhiên xem gạch có "rơi" power-up hay không và loại power-up được gán.
Brick::Brick(int x, int y, int width, int height, BrickType type, SDL_Texture* oneHitTexture, SDL_Texture* twoHitFullTexture, SDL_Texture* twoHitCrackedTexture)
    : rect{ x, y, width, height },
    type(type),
    destroyed(false),
    oneHitTexture(oneHitTexture),
    twoHitFullTexture(twoHitFullTexture),
    twoHitCrackedTexture(twoHitCrackedTexture) {
    // Gán hitPoints tùy theo type của gạch: 2 cho TWO_HIT, 1 cho gạch 1-hit.
    hitPoints = (type == BrickType::TWO_HIT) ? 2 : 1;
    // Xác định ngẫu nhiên xem gạch có rơi power-up hay không.
    // (Ở đây dùng rand() % 1 luôn trả về 0, nên việc này có thể không như ý định ban đầu.)
    hasPowerUp = (rand() % 1 == 0);
    // Gán ngẫu nhiên loại power-up (chuyển đổi từ số nguyên sang PowerUp::Type).
    powerUpType = static_cast<PowerUp::Type>(rand() % 8);
}

// Hàm Render: Vẽ gạch lên màn hình sử dụng renderer nếu gạch chưa bị hủy.
void Brick::Render(SDL_Renderer* renderer) {
    if (destroyed)
        return; // Nếu gạch đã bị phá hủy, không render nữa.

    // Với gạch 1-hit, sử dụng texture oneHitTexture nếu có.
    if (type == BrickType::ONE_HIT && oneHitTexture) {
        SDL_RenderCopy(renderer, oneHitTexture, nullptr, &rect);
    }
    // Với gạch 2-hit, hiển thị texture khác nhau tùy thuộc vào số hitPoints còn lại.
    else if (type == BrickType::TWO_HIT) {
        if (hitPoints == 2 && twoHitFullTexture) {
            SDL_RenderCopy(renderer, twoHitFullTexture, nullptr, &rect);
        }
        else if (hitPoints == 1 && twoHitCrackedTexture) {
            SDL_RenderCopy(renderer, twoHitCrackedTexture, nullptr, &rect);
        }
    }
}

// Hàm Hit: Giảm số hitPoints khi gạch bị va chạm với bóng (hoặc đạn) và kiểm tra xem gạch đã bị phá hủy chưa.
// Nếu hitPoints bằng 0 sau khi giảm, gạch đánh dấu là destroyed và trả về true.
bool Brick::Hit() {
    if (hitPoints > 0)
        hitPoints--;

    if (hitPoints == 0) {
        destroyed = true;
        return true;
    }
    return false;
}

// Hàm GetRect: Trả về hình chữ nhật (SDL_Rect) của gạch, dùng cho việc kiểm tra va chạm.
SDL_Rect Brick::GetRect() const {
    return rect;
}

// Hàm IsDestroyed: Trả về trạng thái của gạch, true nếu đã bị phá hủy.
bool Brick::IsDestroyed() const {
    return destroyed;
}

// Hàm shouldDropPowerUp: Kiểm tra xem gạch này có cho phép rơi power-up hay không.
bool Brick::shouldDropPowerUp() const {
    return hasPowerUp;
}

// Hàm getPowerUpType: Trả về loại power-up mà gạch sẽ rơi khi bị phá hủy.
PowerUp::Type Brick::getPowerUpType() const {
    return powerUpType;
}

// Hàm SetRect: Đặt lại vị trí và kích thước của gạch (SDL_Rect) dựa vào đối số newRect.
void Brick::SetRect(const SDL_Rect& newRect) {
    rect = newRect;
}
