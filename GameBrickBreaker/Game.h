#ifndef GAME_H
#define GAME_H

// Bao gồm các thư viện SDL cần thiết và các module hỗ trợ
#include <SDL.h>                // Thư viện SDL cơ bản
#include <SDL_ttf.h>            // Hỗ trợ xử lý font chữ với TTF
#include <SDL_mixer.h>          // Hỗ trợ âm thanh với Mixer
#include <vector>               // Container vector để lưu trữ các đối tượng
#include "Paddle.h"             // Định nghĩa lớp Paddle (thanh trượt)
#include "Ball.h"               // Định nghĩa lớp Ball (bóng)
#include "Brick.h"              // Định nghĩa lớp Brick (viên gạch)
#include "PowerUp.h"            // Định nghĩa lớp PowerUp (hiệu ứng tăng lực, v.v...)

// Lớp Game quản lý toàn bộ quá trình của trò chơi Brick Breaker
class Game {
public:
    // Hàm khởi tạo, thiết lập các giá trị ban đầu cho Game
    Game();
    // Hàm hủy, giải phóng tài nguyên đã cấp phát
    ~Game();

    // Hàm khởi tạo các thành phần của game, trả về true nếu thành công
    bool init();
    // Hàm chạy vòng lặp chính của game
    void run();
    // Hàm xử lý các sự kiện (bàn phím, chuột, cửa sổ, ...)
    void handleEvents();
    // Hàm cập nhật trạng thái của game (di chuyển, va chạm, tính điểm, ...)
    void update();
    // Hàm render (vẽ) các thành phần game lên màn hình
    void render();
    // Hàm dọn dẹp để giải phóng tài nguyên sau khi kết thúc game
    void clean();

private:
    // Các hàm xử lý nội bộ và các thành phần riêng của game

    // Hàm render màn hình chiến thắng và nút Restart
    void renderWinner();
    // Hàm vẽ một đường thẳng với tọa độ xác định và màu sắc
    void drawLine(int x1, int y1, int x2, int y2, SDL_Color color);

    // Biến kiểm soát đường giới hạn (tính từ vị trí của paddle)
    float limitLineY;

    // Texture hiển thị hình chiến thắng và hình chữ nhật xác định vị trí hiển thị
    SDL_Texture* winnerTexture;
    SDL_Rect winnerRect;

    // Cờ kiểm tra trạng thái chiến thắng của game
    bool isWinner; // Biến để kiểm tra trạng thái chiến thắng

    // Cờ kiểm tra xem bóng có được gắn vào paddle không
    bool isBallAttached;

    // Danh sách các texture cho gạch 1-hit
    std::vector<SDL_Texture*> oneHitBrickTextures; // Danh sách texture cho gạch 1-hit
    // Danh sách các texture cho gạch 2-hit ở trạng thái nguyên vẹn
    std::vector<SDL_Texture*> twoHitBrickFullTextures; // Danh sách texture cho gạch 2-hit (nguyên vẹn)
    // Danh sách các texture cho gạch 2-hit ở trạng thái bị nứt
    std::vector<SDL_Texture*> twoHitBrickCrackedTextures; // Danh sách texture cho gạch 2-hit (bị nứt)

    // Con trỏ cửa sổ SDL và renderer dùng để hiển thị giao diện game
    SDL_Window* window;
    SDL_Renderer* renderer;

    // Biến kiểm tra trạng thái chạy của game
    bool running;

    // Đối tượng Paddle (thanh trượt) và Ball (bóng chính)
    Paddle* paddle;
    Ball* ball;

    // Danh sách các viên gạch hiện có trên màn hình
    std::vector<Brick> bricks;
    // Danh sách các PowerUp đang được áp dụng trong game
    std::vector<PowerUp> powerUps;

    // Texture của hình nền game
    SDL_Texture* backgroundTexture; // Add this line
    // Texture hiển thị số mạng của người chơi (thường là hình trái tim)
    SDL_Texture* heartTexture;
    // Texture nút pause được hiển thị trong quá trình chơi
    SDL_Texture* pauseTexture;
    // Texture hiển thị logo khi game bị tạm dừng
    SDL_Texture* logoTexture;
    // Texture hiển thị điểm số trên màn hình
    SDL_Texture* scoreTexture;
    // Texture hiển thị màn hình bắt đầu của game
    SDL_Texture* startScreenTexture;
    // Texture hiển thị nút chơi (Play button) trên màn hình bắt đầu
    SDL_Texture* playButtonTexture;
    // Texture của ảnh game over được hiển thị khi kết thúc game
    SDL_Texture* gameOverTexture;
    // Texture của nút Restart được hiển thị trên màn hình game over
    SDL_Texture* restartTexture;

    // Các hình chữ nhật định vị cho các thành phần giao diện
    SDL_Rect playButtonRect;
    SDL_Rect pauseRect;
    SDL_Rect logoRect;
    SDL_Rect gameOverRect;
    SDL_Rect restartRect;

    // Con trỏ font chữ dùng cho việc hiển thị văn bản (điểm số, v.v...)
    TTF_Font* font;

    // Các đối tượng nhạc nền cho màn hình bắt đầu và quá trình chơi game
    Mix_Music* startMusic;
    Mix_Music* gameMusic;

    // Số mạng (lives) và điểm số của người chơi
    int lives;
    int score;

    // Các cờ trạng thái của game: tạm dừng, game over, màn hình bắt đầu
    bool isPaused;
    bool isGameOver;
    bool isStartScreen;

    // Các hàm nội bộ để tải và render hình nền
    void LoadBackground(SDL_Renderer* renderer);
    void RenderBackground(SDL_Renderer* renderer);
    void CleanupBackground();

    // Hàm hiển thị số mạng của người chơi trên màn hình
    void renderHearts();
    // Hàm cập nhật texture hiển thị điểm số khi điểm số thay đổi
    void updateScoreTexture();

    // Hàm tải các texture cần thiết cho màn hình game over và nút Restart
    void loadGameOverTextures(SDL_Renderer* renderer);
    // Hàm render màn hình game over
    void renderGameOver();
    // Hàm tải texture cho logo hiển thị khi game bị tạm dừng
    void loadLogoTexture(SDL_Renderer* renderer);
    // Hàm reset lại trạng thái game để bắt đầu một ván mới
    void resetGame();
    // Hàm khởi tạo các viên gạch ban đầu cho trò chơi
    void initBricks();
    // Hàm xử lý sự kiện cho màn hình bắt đầu, ví dụ như nhấn nút Start
    void handleStartScreenEvent(const SDL_Event& e);
    // Hàm xử lý sự kiện tạm dừng, khi nhấn nút pause hoặc phím space
    void handlePauseEvent(const SDL_Event& e);
    // Hàm render màn hình bắt đầu của game
    void renderStartScreen();

    // Hàm thêm một hàng gạch mới ở đầu màn hình và dời các viên gạch hiện có xuống dưới
    void addNewBrickRow();

    // Danh sách các bóng phụ (extra balls) nếu có, được sử dụng cho power-up triple-ball
    std::vector<Ball*> extraBalls;
};

#endif // GAME_H
