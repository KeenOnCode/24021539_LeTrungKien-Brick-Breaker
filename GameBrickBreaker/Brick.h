#pragma once
#include <SDL.h>
#include "PowerUp.h"

// Lớp Brick đại diện cho viên gạch trong trò chơi Brick Breaker.
// Lớp này quản lý thông tin hình học, trạng thái, texture và danh mục power-up của viên gạch.
class Brick {
public:
    // Enum định nghĩa các loại gạch:
    // - ONE_HIT: gạch chỉ cần một lần va chạm để phá hủy.
    // - TWO_HIT: gạch cần hai lần va chạm; hiển thị texture khác nhau tùy theo số lần va chạm.
    enum class BrickType { ONE_HIT, TWO_HIT };

    // Constructor: khởi tạo một viên gạch với các thông số như vị trí (x, y), kích thước (width, height),
    // loại gạch (BrickType) và các texture tương ứng.
    // Nếu viên gạch là ONE_HIT, oneHitTexture sẽ được sử dụng.
    // Đối với gạch TWO_HIT, có thể có hai texture: full khi gạch nguyên vẹn và cracked khi gạch bị nứt.
    Brick(int x, int y, int width, int height, BrickType type, SDL_Texture* oneHitTexture, SDL_Texture* twoHitFullTexture = nullptr, SDL_Texture* twoHitCrackedTexture = nullptr);

    // Hàm Render: vẽ viên gạch lên màn hình bằng renderer truyền vào.
    // Nếu viên gạch đã bị phá hủy, không vẽ.
    void Render(SDL_Renderer* renderer);

    // Hàm Hit: xử lý va chạm của viên gạch. Giảm số hitPoints của gạch và đánh dấu gạch là phá hủy nếu hitPoints bằng 0.
    // Trả về true nếu viên gạch bị phá hủy sau khi va chạm.
    bool Hit();

    // Hàm GetRect: trả về hình chữ nhật (SDL_Rect) định vị vị trí và kích thước của viên gạch.
    SDL_Rect GetRect() const;

    // Hàm IsDestroyed: trả về trạng thái của viên gạch (đã bị phá hủy hay chưa).
    bool IsDestroyed() const;

    // Hàm shouldDropPowerUp: kiểm tra xem viên gạch này có chứa power-up hay không.
    bool shouldDropPowerUp() const;

    // Hàm getPowerUpType: trả về loại power-up được gán cho viên gạch này.
    PowerUp::Type getPowerUpType() const;

    // Hàm SetRect: cập nhật hình chữ nhật (vị trí và kích thước) của viên gạch.
    void SetRect(const SDL_Rect& newRect);

private:
    SDL_Rect rect;                        // Hình chữ nhật định vị vị trí (x, y) và kích thước (w, h) của viên gạch.
    BrickType type;                       // Loại gạch: ONE_HIT hay TWO_HIT.
    bool destroyed;                       // Trạng thái của viên gạch: true nếu đã bị phá hủy.
    int hitPoints;                        // Số lần va chạm cần thiết để phá hủy viên gạch (1 đối với ONE_HIT; 2 đối với TWO_HIT).
    SDL_Texture* oneHitTexture;           // Texture của viên gạch khi chỉ cần một va chạm.
    SDL_Texture* twoHitFullTexture;       // Texture của viên gạch hai va chạm khi gạch còn nguyên vẹn.
    SDL_Texture* twoHitCrackedTexture;    // Texture của viên gạch hai va chạm khi gạch đã nứt.
    bool hasPowerUp;                      // Cờ cho biết viên gạch có rơi power-up khi bị phá hủy hay không.
    PowerUp::Type powerUpType;            // Loại power-up rơi ra khi viên gạch phá hủy.
};
