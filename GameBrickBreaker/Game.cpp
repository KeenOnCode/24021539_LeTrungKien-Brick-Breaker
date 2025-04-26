#include "Game.h"
#include <iostream>
#include <SDL_image.h>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <random>
#include <chrono>

// Biến toàn cục để theo dõi thời gian cập nhật hàng gạch cuối cùng
std::chrono::time_point<std::chrono::steady_clock> lastBrickUpdateTime;

// Định nghĩa các hằng số cho kích thước màn hình
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Constructor của lớp Game
Game::Game()
    : isBallAttached(true)                // Ban đầu, bóng được gắn trên paddle
    , window(nullptr)                     // Con trỏ cửa sổ SDL không được khởi tạo
    , renderer(nullptr)                   // Con trỏ renderer không được khởi tạo
    , running(false)                      // Game bắt đầu chưa chạy
    , paddle(nullptr)                     // Paddle chưa được khởi tạo
    , ball(nullptr)                       // Bóng chưa được khởi tạo
    , isPaused(false)                     // Trạng thái tạm dừng ban đầu: không tạm dừng
    , isGameOver(false)                   // Ban đầu game chưa kết thúc
    , isStartScreen(true)                 // Bắt đầu ở màn hình start
{
    lives = 3;                           // Số mạng ban đầu là 3
    score = 0;                           // Điểm ban đầu là 0

    // Khởi tạo hệ thống font TTF để xử lý văn bản
    if (TTF_Init() == -1) {
        std::cout << "Không khởi tạo được SDL_ttf: " << TTF_GetError() << std::endl;
    }

    // Mở file font với kích thước 20
    font = TTF_OpenFont("assets/font/font.ttf", 20);
    if (!font) {
        std::cout << "Không tải được font: " << TTF_GetError() << std::endl;
    }

    // Khởi tạo hệ thống âm thanh SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cout << "Không khởi tạo được SDL_mixer: " << Mix_GetError() << std::endl;
    }

    // Cập nhật texture hiển thị điểm số ban đầu
    updateScoreTexture();
}

// Destructor của lớp Game, giải phóng tài nguyên thông qua hàm clean()
Game::~Game() {
    clean();
}

// Hàm init: khởi tạo các thành phần cơ bản của game, bao gồm cửa sổ, renderer, đối tượng game và tài nguyên
bool Game::init() {
    // Thiết lập kích thước và vị trí để hiển thị ảnh chiến thắng
    int originalWidth = 2304;
    int originalHeight = 1296;
    float scale = std::min((float)SCREEN_WIDTH / originalWidth, (float)SCREEN_HEIGHT / originalHeight);
    int scaledWidth = static_cast<int>(originalWidth * scale);
    int scaledHeight = static_cast<int>(originalHeight * scale);

    // Căn giữa ảnh chiến thắng trên màn hình
    winnerRect = {
        (SCREEN_WIDTH - scaledWidth) / 2,  // Tọa độ X của ảnh chiến thắng
        (SCREEN_HEIGHT - scaledHeight) / 2, // Tọa độ Y của ảnh chiến thắng
        scaledWidth,                       // Chiều rộng sau khi scale
        scaledHeight                       // Chiều cao sau khi scale
    };

    // Khởi tạo SDL với các hệ thống video và audio
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cout << "Không khởi tạo được SDL: " << SDL_GetError() << std::endl;
        return false;
    }

    // Tạo cửa sổ game với vị trí và kích thước đã được định sẵn
    window = SDL_CreateWindow("Brick Breaker", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window)
        return false;

    // Tạo renderer với tăng tốc phần cứng
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
        return false;

    // Tải hình nền cho game và tạo texture tương ứng
    LoadBackground(renderer);

    // Khởi tạo đối tượng paddle và ball
    paddle = new Paddle(renderer);
    ball = new Ball(renderer);

    // Lấy hình chữ nhật của paddle để xác định đường giới hạn cho việc tạo gạch
    SDL_Rect paddleRect = paddle->getRect();
    limitLineY = paddleRect.y;              // Đặt đường giới hạn dựa trên vị trí của paddle

    // Tạo danh sách các viên gạch
    initBricks();

    // Cập nhật lại texture hiển thị điểm số
    updateScoreTexture();

    isWinner = false; // Ban đầu chưa chiến thắng

    // Tải texture hiển thị số mạng (trái tim) cho người chơi
    heartTexture = IMG_LoadTexture(renderer, "assets/image/gameOver/heart.png");

    // Tải ảnh chiến thắng và tạo texture từ ảnh
    SDL_Surface* winnerSurface = IMG_Load("assets/image/gameOver/winner.png");
    if (!winnerSurface) {
        std::cout << "Không tải được ảnh chiến thắng: " << IMG_GetError() << std::endl;
        return false;
    }
    winnerTexture = SDL_CreateTextureFromSurface(renderer, winnerSurface);
    SDL_FreeSurface(winnerSurface);

    // Tải texture cho màn hình bắt đầu và tạo texture
    SDL_Surface* startSurface = IMG_Load("assets/image/background/start_screen.png");
    if (!startSurface) {
        std::cout << "Không tải được ảnh màn hình bắt đầu: " << IMG_GetError() << std::endl;
        return false;
    }
    startScreenTexture = SDL_CreateTextureFromSurface(renderer, startSurface);
    SDL_FreeSurface(startSurface);

    // Tải texture cho nút Play trên màn hình bắt đầu
    SDL_Surface* playSurface = IMG_Load("assets/image/gameOver/play_button.png");
    if (!playSurface) {
        std::cout << "Không tải được ảnh nút chơi: " << IMG_GetError() << std::endl;
        return false;
    }
    playButtonTexture = SDL_CreateTextureFromSurface(renderer, playSurface);
    SDL_FreeSurface(playSurface);
    playButtonRect = { 350, 300, 100, 50 };

    // Tải và phát nhạc cho màn hình bắt đầu (với chế độ lặp vô hạn)
    startMusic = Mix_LoadMUS("assets/sound/start_music.mp3");
    if (!startMusic) {
        std::cout << "Không tải được nhạc màn hình bắt đầu: " << Mix_GetError() << std::endl;
        return false;
    }
    Mix_PlayMusic(startMusic, -1);

    // Tải nhạc cho game play
    gameMusic = Mix_LoadMUS("assets/sound/game_music.mp3");
    if (!gameMusic) {
        std::cout << "Không tải được nhạc game: " << Mix_GetError() << std::endl;
        return false;
    }

    // Tải texture cho nút pause và xác định vị trí hiển thị trên màn hình
    SDL_Surface* pauseSurface = IMG_Load("assets/image/gameOver/pause.png");
    if (!pauseSurface) {
        std::cout << "Không tải được ảnh pause: " << IMG_GetError() << std::endl;
        return false;
    }
    pauseTexture = SDL_CreateTextureFromSurface(renderer, pauseSurface);
    SDL_FreeSurface(pauseSurface);
    pauseRect = { 750, 10, 40, 40 };

    // Tải texture cho màn hình game over và logo (hiển thị khi game over hoặc tạm dừng)
    loadGameOverTextures(renderer);
    loadLogoTexture(renderer);

    running = true; // Đánh dấu game đã sẵn sàng chạy
    return true;
}

// Hàm initBricks: tạo danh sách các viên gạch cho trò chơi theo bố cục ngẫu nhiên
void Game::initBricks() {
    bricks.clear();                      // Xóa danh sách gạch hiện có
    srand(static_cast<unsigned>(time(0))); // Seed cho hàm rand với thời gian hiện tại

    // Nạp danh sách texture cho gạch 1-hit
    oneHitBrickTextures = {
        IMG_LoadTexture(renderer, "assets/image/1hit/full1.png"),
        IMG_LoadTexture(renderer, "assets/image/1hit/full2.png"),
        IMG_LoadTexture(renderer, "assets/image/1hit/full3.png"),
        IMG_LoadTexture(renderer, "assets/image/1hit/full4.png"),
        IMG_LoadTexture(renderer, "assets/image/1hit/full5.png"),
        IMG_LoadTexture(renderer, "assets/image/1hit/full6.png"),
        IMG_LoadTexture(renderer, "assets/image/1hit/full7.png"),
        IMG_LoadTexture(renderer, "assets/image/1hit/full8.png"),
        IMG_LoadTexture(renderer, "assets/image/1hit/full9.png"),
        IMG_LoadTexture(renderer, "assets/image/1hit/full10.png")
    };

    // Nạp danh sách texture cho gạch 2-hit ở trạng thái nguyên vẹn
    twoHitBrickFullTextures = {
        IMG_LoadTexture(renderer, "assets/image/2hit/full1.png"),
        IMG_LoadTexture(renderer, "assets/image/2hit/full2.png"),
        IMG_LoadTexture(renderer, "assets/image/2hit/full3.png"),
        IMG_LoadTexture(renderer, "assets/image/2hit/full4.png"),
        IMG_LoadTexture(renderer, "assets/image/2hit/full5.png"),
        IMG_LoadTexture(renderer, "assets/image/2hit/full6.png"),
        IMG_LoadTexture(renderer, "assets/image/2hit/full7.png"),
        IMG_LoadTexture(renderer, "assets/image/2hit/full8.png"),
        IMG_LoadTexture(renderer, "assets/image/2hit/full9.png"),
        IMG_LoadTexture(renderer, "assets/image/2hit/full10.png")
    };

    // Nạp danh sách texture cho gạch 2-hit ở trạng thái bị nứt
    twoHitBrickCrackedTextures = {
        IMG_LoadTexture(renderer, "assets/image/2hit/cracked1.png"),
        IMG_LoadTexture(renderer, "assets/image/2hit/cracked2.png"),
        IMG_LoadTexture(renderer, "assets/image/2hit/cracked3.png"),
        IMG_LoadTexture(renderer, "assets/image/2hit/cracked4.png"),
        IMG_LoadTexture(renderer, "assets/image/2hit/cracked5.png"),
        IMG_LoadTexture(renderer, "assets/image/2hit/cracked6.png"),
        IMG_LoadTexture(renderer, "assets/image/2hit/cracked7.png"),
        IMG_LoadTexture(renderer, "assets/image/2hit/cracked8.png"),
        IMG_LoadTexture(renderer, "assets/image/2hit/cracked9.png"),
        IMG_LoadTexture(renderer, "assets/image/2hit/cracked10.png")
    };

    // Kiểm tra texture gạch 1-hit được nạp thành công
    for (auto tex : oneHitBrickTextures) {
        if (!tex) {
            std::cout << "Không tải được texture gạch 1-hit: " << IMG_GetError() << std::endl;
            return;
        }
    }
    // Kiểm tra texture gạch 2-hit (nguyên vẹn) được nạp thành công
    for (auto tex : twoHitBrickFullTextures) {
        if (!tex) {
            std::cout << "Không tải được texture gạch 2-hit (nguyên vẹn): " << IMG_GetError() << std::endl;
            return;
        }
    }
    // Kiểm tra texture gạch 2-hit (nứt) được nạp thành công
    for (auto tex : twoHitBrickCrackedTextures) {
        if (!tex) {
            std::cout << "Không tải được texture gạch 2-hit (nứt): " << IMG_GetError() << std::endl;
            return;
        }
    }

    // Xác định số cột và kích thước viên gạch sẽ được tạo ra
    int cols = 8;
    int brickWidth = SCREEN_WIDTH / cols - 5;
    int brickHeight = 30;
    int startY = 50; // Vị trí y khởi tạo các viên gạch
    SDL_Rect paddleRect = paddle->getRect();
    int paddleY = paddleRect.y;
    int safeMargin = paddleRect.h + 150; // Khoảng cách an toàn giữa paddle và gạch
    int maxAllowedY = paddleY - safeMargin;
    int maxRows = (maxAllowedY - startY) / (brickHeight + 5);
    int minRows = 2;                    // Số hàng tối thiểu của gạch
    if (maxRows > 6) {
        maxRows = 6;
    }
    if (maxRows < minRows) {
        maxRows = minRows;
    }

    // Tạo các viên gạch theo từng cột với số hàng ngẫu nhiên.
    for (int j = 0; j < cols; ++j) {
        int randomRows = (rand() % maxRows) + 2; // Tạo số hàng ngẫu nhiên cho mỗi cột
        for (int i = 0; i < randomRows; ++i) {
            int x = j * (brickWidth + 5);
            int y = startY + i * (brickHeight + 5);

            // Kiểm tra để đảm bảo viên gạch không vượt quá đường giới hạn của paddle
            if (y + brickHeight >= limitLineY) {
                continue; // Bỏ qua nếu vượt qua giới hạn
            }

            // Tạo viên gạch ngẫu nhiên là loại ONE_HIT hoặc TWO_HIT
            if (rand() % 2 == 0) {
                // Lấy texture ngẫu nhiên từ danh sách gạch 1-hit
                SDL_Texture* randomOneHitTexture = oneHitBrickTextures[rand() % oneHitBrickTextures.size()];
                bricks.emplace_back(x, y, brickWidth, brickHeight, Brick::BrickType::ONE_HIT, randomOneHitTexture);
            }
            else {
                // Lấy texture ngẫu nhiên từ danh sách gạch 2-hit cho hai trạng thái: nguyên vẹn và nứt
                SDL_Texture* randomTwoHitFullTexture = twoHitBrickFullTextures[rand() % twoHitBrickFullTextures.size()];
                SDL_Texture* randomTwoHitCrackedTexture = twoHitBrickCrackedTextures[rand() % twoHitBrickCrackedTextures.size()];
                bricks.emplace_back(x, y, brickWidth, brickHeight, Brick::BrickType::TWO_HIT, nullptr,
                    randomTwoHitFullTexture, randomTwoHitCrackedTexture);
            }
        }
    }
}

// Hàm renderWinner: vẽ màn hình chiến thắng và nút Restart khi game chiến thắng.
void Game::renderWinner() {
    if (winnerTexture) {
        SDL_RenderCopy(renderer, winnerTexture, nullptr, &winnerRect);
    }
    if (restartTexture) {
        SDL_RenderCopy(renderer, restartTexture, nullptr, &restartRect);
    }
}

// Hàm handleEvents: xử lý các sự kiện nhập từ người dùng (bàn phím, chuột, cửa sổ,...)
void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        // Nếu người dùng đóng cửa sổ, đặt cờ running = false để kết thúc game.
        if (event.type == SDL_QUIT) {
            running = false;
        }
        // Nếu game đang ở màn hình bắt đầu, chuyển xử lý sự kiện cho màn hình bắt đầu.
        if (isStartScreen) {
            handleStartScreenEvent(event);
        }
        // Nếu game đã chiến thắng, chỉ xử lý sự kiện nút restart.
        else if (isWinner) {
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x = event.button.x;
                int y = event.button.y;
                if (x >= restartRect.x && x <= restartRect.x + restartRect.w &&
                    y >= restartRect.y && y <= restartRect.y + restartRect.h) {
                    resetGame(); // Reset game khi nhấn nút Restart.
                }
            }
        }
        // Xử lý các sự kiện game play thông thường.
        else {
            handlePauseEvent(event);
            if (!isPaused && !isGameOver) {
                paddle->handleEvent(event);
                // Khi nhấn phím Enter, nếu bóng đang gắn trên paddle, giải phóng bóng.
                if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
                    if (isBallAttached) {
                        isBallAttached = false;
                        ball->release();
                    }
                }
            }
            // Nếu game over, xử lý sự kiện nhấn chuột để reset game.
            if (isGameOver && event.type == SDL_MOUSEBUTTONDOWN) {
                int x = event.button.x;
                int y = event.button.y;
                if (x >= restartRect.x && x <= restartRect.x + restartRect.w &&
                    y >= restartRect.y && y <= restartRect.y + restartRect.h) {
                    resetGame();
                }
            }
        }
    }
}

// Hàm handleStartScreenEvent: xử lý sự kiện của màn hình bắt đầu (ví dụ: nhấn nút Play)
void Game::handleStartScreenEvent(const SDL_Event& e) {
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        int x = e.button.x;
        int y = e.button.y;
        if (x >= playButtonRect.x && x <= playButtonRect.x + playButtonRect.w &&
            y >= playButtonRect.y && y <= playButtonRect.y + playButtonRect.h) {
            isStartScreen = false;
            Mix_HaltMusic();             // Dừng nhạc màn hình bắt đầu
            Mix_PlayMusic(gameMusic, -1); // Phát nhạc trong quá trình chơi
            Mix_Chunk* clickSound = Mix_LoadWAV("assets/sound/click.wav");
            if (clickSound) {
                Mix_PlayChannel(-1, clickSound, 0); // Phát âm thanh khi nhấn nút
            }
        }
    }
}

// Hàm handlePauseEvent: xử lý sự kiện tạm dừng game khi nhấn chuột vào nút pause hoặc phím space.
void Game::handlePauseEvent(const SDL_Event& e) {
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        int x = e.button.x;
        int y = e.button.y;
        if (x >= pauseRect.x && x <= pauseRect.x + pauseRect.w &&
            y >= pauseRect.y && y <= pauseRect.y + pauseRect.h) {
            isPaused = !isPaused; // Đảo trạng thái tạm dừng.
        }
    }
    else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) {
        isPaused = !isPaused; // Nhấn phím space cũng đảo trạng thái tạm dừng.
    }
}

// Hàm update: cập nhật tất cả các thành phần của game (vị trí, va chạm, trạng thái,...)
void Game::update() {
    // Nếu game đang tạm dừng, đã game over hoặc đang ở màn hình bắt đầu thì bỏ qua cập nhật.
    if (isPaused || isGameOver || isStartScreen)
        return;

    // Cập nhật paddle và lấy lại hình chữ nhật của paddle.
    paddle->update();
    SDL_Rect paddleRect = paddle->getRect();
    limitLineY = paddleRect.y; // Cập nhật đường giới hạn dựa trên vị trí paddle

    // Nếu bóng đang được gắn trên paddle, cập nhật vị trí bóng theo paddle.
    if (isBallAttached) {
        ball->setPosition(paddleRect.x + paddleRect.w / 2 - ball->getRect().w / 2,
            paddleRect.y - ball->getRect().h);
    }
    else {
        // Nếu bóng đã được giải phóng, cập nhật vị trí và vận tốc của bóng.
        ball->update(*paddle, running, lives);

        // Nếu bóng rơi ra ngoài màn hình và còn mạng, gắn lại bóng vào paddle.
        if (ball->getRect().y > SCREEN_HEIGHT && lives > 0) {
            isBallAttached = true;
            SDL_Rect paddleRect = paddle->getRect();
            ball->setPosition(paddleRect.x + paddleRect.w / 2 - ball->getRect().w / 2,
                paddleRect.y - ball->getRect().h);
        }
    }

    // Cập nhật tất cả các bóng phụ (extra balls) nếu có.
    for (auto extraBall : extraBalls) {
        extraBall->update(*paddle, running, lives);
    }

    // Kiểm tra thời gian kể từ lần cập nhật hàng gạch cuối cùng.
    auto now = std::chrono::steady_clock::now();
    auto elapsedRow = std::chrono::duration_cast<std::chrono::seconds>(now - lastBrickUpdateTime).count();
    if (elapsedRow >= 10) {
        addNewBrickRow(); // Thêm hàng gạch mới sau mỗi 10 giây.
        lastBrickUpdateTime = now;
    }

    // Kiểm tra điều kiện chiến thắng: nếu điểm đạt 10,000 trở lên.
    if (score >= 10000 && !isWinner) {
        isWinner = true;
        Mix_HaltMusic();   // Dừng nhạc game
        Mix_HaltChannel(-1);
        Mix_Chunk* winSound = Mix_LoadWAV("assets/sound/win.wav");
        if (winSound) {
            Mix_PlayChannel(-1, winSound, 0); // Phát âm thanh chiến thắng.
        }
    }
    // Nếu hết mạng, báo game over.
    if (lives <= 0 && !isGameOver) {
        isGameOver = true;
    }

    // Xử lý va chạm giữa các bóng (bóng chính và bóng phụ) với các viên gạch.
    std::vector<Ball*> allBalls;
    allBalls.push_back(ball);
    for (auto extraBall : extraBalls) {
        allBalls.push_back(extraBall);
    }
    bool allBricksDestroyed = true; // Cờ kiểm tra nếu tất cả gạch đã bị phá hủy
    for (auto& brick : bricks) {
        SDL_Rect brickRect = brick.GetRect();
        for (auto currBall : allBalls) {
            SDL_Rect ballRect = currBall->getRect();
            if (!brick.IsDestroyed() && SDL_HasIntersection(&ballRect, &brickRect)) {
                currBall->bounce(); // Đảo hướng bóng khi va chạm với gạch.
                bool destroyed = brick.Hit(); // Xử lý va chạm bằng việc giảm hitPoints của gạch.
                if (destroyed) {
                    score += 100; // Tăng điểm khi một viên gạch bị phá hủy.
                    updateScoreTexture();
                    // Nếu viên gạch có khả năng rơi power-up, thêm power-up vào danh sách.
                    if (brick.shouldDropPowerUp()) {
                        powerUps.push_back(PowerUp(renderer, brick.getPowerUpType(), brickRect.x, brickRect.y));
                    }
                }
            }
        }
        if (!brick.IsDestroyed()) {
            allBricksDestroyed = false;
        }
        // Nếu viên gạch chạm gần paddle, báo game over.
        int gameOverMargin = 10;
        if (brick.GetRect().y + brick.GetRect().h >= limitLineY - gameOverMargin) {
            isGameOver = true;
            break;
        }
    }

    // Nếu tất cả các viên gạch đã bị phá hủy mà game chưa báo game over, thì báo game over.
    if (allBricksDestroyed && !isGameOver) {
        isGameOver = true;
    }

    // Cập nhật trạng thái các power-up đang xuất hiện.
    for (auto& powerUp : powerUps) {
        powerUp.update();
        if (powerUp.isExpired()) {
            paddle->resetToDefault(); // Reset paddle về trạng thái mặc định nếu power-up hết hiệu lực.
        }
    }

    // Xử lý khi paddle thu thập power-up.
    for (auto it = powerUps.begin(); it != powerUps.end();) {
        SDL_Rect paddleRect = paddle->getRect();
        SDL_Rect powerUpRect = it->getRect();
        if (SDL_HasIntersection(&paddleRect, &powerUpRect)) {
            if (it->getType() == PowerUp::TRIPLE_BALL) {
                // Nếu power-up là triple-ball, nếu bóng chính đang gắn thì giải phóng nó,
                // sau đó thêm thêm bóng phụ (tối đa 3 bóng)
                if (isBallAttached) {
                    isBallAttached = false;
                    ball->release();
                }
                int currentBallCount = 1 + extraBalls.size();
                int ballsToSpawn = 3 - currentBallCount;
                for (int i = 0; i < ballsToSpawn; i++) {
                    Ball* extra = new Ball(renderer);
                    extra->setPosition(
                        paddleRect.x + paddleRect.w / 2 - extra->getRect().w / 2,
                        paddleRect.y - extra->getRect().h
                    );
                    extraBalls.push_back(extra);
                }
            }
            else {
                // Áp dụng hiệu ứng power-up khác cho paddle.
                paddle->applyPowerUp(it->getType());
            }
            it = powerUps.erase(it); // Xóa power-up sau khi thu thập.
        }
        else {
            ++it;
        }
    }

    // Cập nhật tất cả các viên đạn được bắn ra từ paddle.
    for (auto& bullet : paddle->getBullets()) {
        bullet.update();
    }
    // Xử lý va chạm giữa đạn và viên gạch.
    for (auto& bullet : paddle->getBullets()) {
        SDL_Rect bulletRect = bullet.getRect();
        for (auto& brick : bricks) {
            SDL_Rect brickRect = brick.GetRect();
            if (!brick.IsDestroyed() && SDL_HasIntersection(&bulletRect, &brickRect)) {
                brick.Hit(); // Gọi hàm Hit cho viên gạch bị va chạm.
                bullet = paddle->getBullets().back();
                paddle->getBullets().pop_back(); // Loại bỏ viên đạn sau khi va chạm.
                break;
            }
        }
    }
}

// Hàm drawLine: vẽ đường thẳng xác định bởi hai điểm và màu sắc cho trước.
void Game::drawLine(int x1, int y1, int x2, int y2, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

// Hàm render: vẽ tất cả các thành phần của game lên màn hình theo thứ tự hợp lý.
// Bao gồm nền, các đối tượng game, điểm số, giao diện, và các hiệu ứng.
void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Đặt màu nền đen.
    SDL_RenderClear(renderer);                       // Xóa màn hình.

    // Kiểm tra trạng thái game để xác định nội dung vẽ
    if (isWinner) {
        renderWinner(); // Nếu chiến thắng, hiển thị màn hình chiến thắng.
    }
    else if (isStartScreen) {
        renderStartScreen(); // Nếu ở màn hình bắt đầu, hiển thị màn hình bắt đầu.
    }
    else {
        RenderBackground(renderer); // Vẽ nền game.
        paddle->render();           // Vẽ thanh paddle.
        ball->render();             // Vẽ bóng chính.
        // Vẽ các bóng phụ (extra balls)
        for (auto extraBall : extraBalls) {
            extraBall->render();
        }
        // Vẽ tất cả các viên gạch.
        for (auto& brick : bricks) {
            brick.Render(renderer);
        }
        // Vẽ các power-up đang xuất hiện.
        for (auto& powerUp : powerUps) {
            powerUp.render();
        }
        // Vẽ điểm số lên màn hình.
        if (scoreTexture) {
            SDL_Rect scoreRect = { 600, 10, 150, 40 };
            SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);
        }
        renderHearts(); // Vẽ số mạng (thể hiện bằng hình trái tim)
        if (pauseTexture) {
            SDL_RenderCopy(renderer, pauseTexture, NULL, &pauseRect); // Vẽ nút pause.
        }
        if (isPaused) {
            if (logoTexture) {
                SDL_RenderCopy(renderer, logoTexture, nullptr, &logoRect); // Nếu tạm dừng, hiển thị logo.
            }
        }
        if (isGameOver) {
            renderGameOver(); // Nếu game over, hiển thị màn hình game over.
        }
        // Vẽ đường giới hạn dựa trên vị trí của paddle (màu trắng)
        drawLine(0, static_cast<int>(limitLineY), SCREEN_WIDTH, static_cast<int>(limitLineY), { 255, 255, 255, 255 });
    }

    // Cập nhật giao diện ra màn hình.
    SDL_RenderPresent(renderer);
}

// Hàm renderStartScreen: hiển thị màn hình bắt đầu với background và nút Play.
void Game::renderStartScreen() {
    if (startScreenTexture) {
        SDL_RenderCopy(renderer, startScreenTexture, NULL, NULL);
    }
    if (playButtonTexture) {
        SDL_RenderCopy(renderer, playButtonTexture, NULL, &playButtonRect);
    }
}

// Hàm clean: dọn dẹp và giải phóng các tài nguyên được cấp phát cho game.
void Game::clean() {
    if (paddle) {
        delete paddle;
        paddle = nullptr;
    }
    if (ball) {
        delete ball;
        ball = nullptr;
    }
    // Giải phóng các bóng phụ.
    for (auto extraBall : extraBalls) {
        delete extraBall;
    }
    extraBalls.clear();
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    if (heartTexture) {
        SDL_DestroyTexture(heartTexture);
        heartTexture = nullptr;
    }
    if (pauseTexture) {
        SDL_DestroyTexture(pauseTexture);
        pauseTexture = nullptr;
    }
    if (logoTexture) {
        SDL_DestroyTexture(logoTexture);
        logoTexture = nullptr;
    }
    if (scoreTexture) {
        SDL_DestroyTexture(scoreTexture);
        scoreTexture = nullptr;
    }
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
    if (startScreenTexture) {
        SDL_DestroyTexture(startScreenTexture);
        startScreenTexture = nullptr;
    }
    if (playButtonTexture) {
        SDL_DestroyTexture(playButtonTexture);
        playButtonTexture = nullptr;
    }
    if (startMusic) {
        Mix_FreeMusic(startMusic);
        startMusic = nullptr;
    }
    if (gameMusic) {
        Mix_HaltMusic();                  // Dừng nhạc game trước khi giải phóng
        Mix_FreeMusic(gameMusic);
        gameMusic = nullptr;
    }
    // Giải phóng các texture của gạch.
    for (auto tex : oneHitBrickTextures) {
        SDL_DestroyTexture(tex);
    }
    for (auto tex : twoHitBrickFullTextures) {
        SDL_DestroyTexture(tex);
    }
    for (auto tex : twoHitBrickCrackedTextures) {
        SDL_DestroyTexture(tex);
    }
    if (winnerTexture) {
        SDL_DestroyTexture(winnerTexture);
        winnerTexture = nullptr;
    }

    // Dọn dẹp hệ thống SDL, TTF và Mix.
    TTF_Quit();
    Mix_Quit();
    SDL_Quit();
}

// Hàm run: vòng lặp chính của game, liên tục xử lý sự kiện, cập nhật trạng thái và vẽ giao diện.
void Game::run() {
    running = true;
    while (running) {
        handleEvents();  // Xử lý các sự kiện nhập từ người dùng
        if (isGameOver) {
            render();    // Nếu game over, chỉ render màn hình game over
            SDL_Delay(16);
            continue;
        }
        update();        // Cập nhật trạng thái game (di chuyển, va chạm, v.v...)
        render();        // Render tất cả các thành phần của game lên màn hình
        SDL_Delay(16);   // Delay khoảng 16ms để đạt 60 FPS (60 frames/giây)
    }
    clean();             // Sau khi game kết thúc, giải phóng tài nguyên.
}

// Hàm LoadBackground: nạp hình nền từ file ảnh và tạo texture cho nó.
void Game::LoadBackground(SDL_Renderer* renderer) {
    SDL_Surface* bgSurface = IMG_Load("assets/image/background/AnhNen.png");
    if (!bgSurface) {
        printf("Không tải được ảnh nền: %s\n", IMG_GetError());
        return;
    }
    backgroundTexture = SDL_CreateTextureFromSurface(renderer, bgSurface);
    SDL_FreeSurface(bgSurface);
}

// Hàm RenderBackground: vẽ hình nền lên màn hình.
void Game::RenderBackground(SDL_Renderer* renderer) {
    if (backgroundTexture) {
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
    }
}

// Hàm CleanupBackground: giải phóng tài nguyên của texture hình nền.
void Game::CleanupBackground() {
    if (backgroundTexture) {
        SDL_DestroyTexture(backgroundTexture);
        backgroundTexture = nullptr;
    }
}

// Hàm renderHearts: vẽ số mạng (mỗi mạng được biểu diễn bằng một trái tim) lên màn hình.
void Game::renderHearts() {
    for (int i = 0; i < lives; i++) {
        SDL_Rect heartRect = { 20 + i * 40, 10, 30, 30 };
        SDL_RenderCopy(renderer, heartTexture, NULL, &heartRect);
    }
}

// Hàm updateScoreTexture: cập nhật texture hiển thị điểm số.
void Game::updateScoreTexture() {
    SDL_Color textColor = { 255, 255, 255, 255 }; // Màu chữ trắng

    std::stringstream ss;
    ss << "Score: " << score;
    std::string scoreText = ss.str();

    SDL_Surface* textSurface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
    if (!textSurface)
        return;

    scoreTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);
}

// Hàm loadGameOverTextures: nạp texture cho màn hình game over và nút Restart.
void Game::loadGameOverTextures(SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load("assets/image/gameOver/gameover.png");
    if (!surface) {
        std::cout << "❌ Không tải được ảnh game over: " << IMG_GetError() << std::endl;
        return;
    }
    gameOverTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    surface = IMG_Load("assets/image/gameOver/restart.png");
    if (!surface) {
        std::cout << "❌ Không tải được ảnh restart: " << IMG_GetError() << std::endl;
        return;
    }
    restartTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    gameOverRect = { 200, 150, 400, 200 };  // Vị trí và kích thước ảnh game over
    restartRect = { 300, 450, 200, 100 };     // Vị trí và kích thước nút restart
}

// Hàm renderGameOver: hiển thị màn hình game over và nút Restart.
void Game::renderGameOver() {
    if (gameOverTexture) {
        SDL_RenderCopy(renderer, gameOverTexture, nullptr, &gameOverRect);
    }
    if (restartTexture) {
        SDL_RenderCopy(renderer, restartTexture, nullptr, &restartRect);
    }
}

// Hàm loadLogoTexture: nạp texture cho logo hiển thị khi game tạm dừng.
void Game::loadLogoTexture(SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load("assets/image/gameOver/logo.png");
    if (!surface) {
        std::cout << "❌ Không tải được logo: " << IMG_GetError() << std::endl;
        return;
    }
    logoTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    logoRect = { 200, 150, 400, 200 }; // Vị trí và kích thước logo
}

// Hàm resetGame: đặt lại trạng thái của game để bắt đầu một ván chơi mới.
void Game::resetGame() {
    lives = 3;
    score = 0;
    isGameOver = false;
    isWinner = false;
    isBallAttached = true; // Gắn lại bóng với paddle
    updateScoreTexture();
    delete paddle;
    paddle = new Paddle(renderer);
    delete ball;
    ball = new Ball(renderer);
    // Giải phóng các bóng phụ từ ván trước.
    for (auto extraBall : extraBalls) {
        delete extraBall;
    }
    extraBalls.clear();

    paddle->resetToDefault();
    initBricks();    // Tạo lại các viên gạch mới
    powerUps.clear(); // Xóa hết power-up hiện có

    // Đưa bóng về vị trí ban đầu ngay trên paddle.
    SDL_Rect paddleRect = paddle->getRect();
    ball->setPosition(paddleRect.x + paddleRect.w / 2 - ball->getRect().w / 2,
        paddleRect.y - ball->getRect().h);
}

// Hàm addNewBrickRow: thêm một hàng gạch mới ở đầu màn hình và dời các viên gạch hiện có xuống dưới.
void Game::addNewBrickRow() {
    const int cols = 8;
    const int gap = 5;
    const int brickHeight = 30;
    // Dời tất cả các viên gạch xuống dưới theo khoảng cách bằng (brickHeight + gap)
    for (auto& brick : bricks) {
        SDL_Rect r = brick.GetRect();
        r.y += brickHeight + gap;
        brick.SetRect(r);
    }

    // Tạo hàng gạch mới gồm các viên gạch 1-hit có texture ngẫu nhiên.
    int brickWidth = SCREEN_WIDTH / cols - gap;
    int startY = 50;
    for (int col = 0; col < cols; col++) {
        int x = col * (brickWidth + gap);
        int y = startY;
        if (oneHitBrickTextures.empty()) {
            // Nếu không có texture nào được nạp, thoát khỏi hàm.
            return;
        }
        // Chọn một texture ngẫu nhiên từ danh sách texture cho gạch 1-hit.
        SDL_Texture* tex = oneHitBrickTextures[rand() % oneHitBrickTextures.size()];
        bricks.emplace_back(x, y, brickWidth, brickHeight, Brick::BrickType::ONE_HIT, tex);
    }
}
