#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <cstdlib>
#include <ctime>

class ArenaGate;

SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;
SDL_Texture* groundTextures[4] = {nullptr}; // Updated for 4 levels
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int WORLD_WIDTH = 2000;
const int WORLD_HEIGHT = 2000;
const int ARENA_GATE_COUNT = 3;

enum GameState {
    GAME_STATE_NORMAL,
    GAME_STATE_ARENA
};

class Player {
public:
    static const int PLAYER_WIDTH = 50;
    static const int PLAYER_HEIGHT = 50;
    const int PLAYER_SPEED = 1;

    Player(int x, int y) : x_(x), y_(y), velocity_x_(0), velocity_y_(0), playerTexture(nullptr), currentMap(0) {}

    void handleEvent(SDL_Event& e, GameState& gameState, ArenaGate* arenaGates);

    void move(GameState& gameState);

    void render(int cameraX, int cameraY);

    void setPlayerToArenaStart();

    void setCurrentMap(int map) {
        currentMap = map;
    }

    int getX() const { return x_; }
    int getY() const { return y_; }

    bool loadTexture(const char* path);

    int getCurrentMap() const {
        return currentMap;
    }

private:
    int x_;
    int y_;
    int velocity_x_;
    int velocity_y_;
    SDL_Texture* playerTexture;
    int currentMap;
};

class ArenaGate {
public:
    static const int GATE_WIDTH = 100;
    static const int GATE_HEIGHT = 100;

    ArenaGate() : x_(rand() % (WORLD_WIDTH - GATE_WIDTH)), y_(rand() % (WORLD_HEIGHT - GATE_HEIGHT)), gateTexture(nullptr) {}

    ~ArenaGate() {
        if (gateTexture != nullptr) {
            SDL_DestroyTexture(gateTexture);
            gateTexture = nullptr;
        }
    }

    bool loadTexture(const char* path);

    void render(int cameraX, int cameraY);

    bool checkCollision(int playerX, int playerY, int& newPlayerX, int& newPlayerY);

private:
    int x_;
    int y_;
    SDL_Texture* gateTexture;
};

bool initializeSDL();

bool loadMedia();

void closeSDL();

bool stateChangeTriggered[ARENA_GATE_COUNT] = {false};

int main(int argc, char* args[]) {
    bool eKeyPressed = false;
    GameState gameState = GAME_STATE_NORMAL;

    if (!initializeSDL()) {
        std::cerr << "Failed to initialize SDL!" << std::endl;
        return -1;
    }

    if (!loadMedia()) {
        std::cerr << "Failed to load media!" << std::endl;
        closeSDL();
        return -1;
    }

    Player player(WORLD_WIDTH / 2, WORLD_HEIGHT / 2);

    // Load player texture
    const char* playerImagePath = "assets/player.png";
    if (!player.loadTexture(playerImagePath)) {
        std::cerr << "Failed to load player texture!" << std::endl;
        closeSDL();
        return -1;
    }

    // Load arena textures
    const char* arena1ImagePath = "assets/arena.png";
    const char* arena2ImagePath = "assets/arena2.png";
    const char* arena3ImagePath = "assets/arena3.png";

    if (!groundTextures[0]) groundTextures[0] = SDL_CreateTextureFromSurface(gRenderer, IMG_Load("assets/Tlakovci.png"));
    if (!groundTextures[1]) groundTextures[1] = SDL_CreateTextureFromSurface(gRenderer, IMG_Load("assets/Pesek.png"));
    if (!groundTextures[2]) groundTextures[2] = SDL_CreateTextureFromSurface(gRenderer, IMG_Load("assets/Trava.png"));
    if (!groundTextures[3]) groundTextures[3] = SDL_CreateTextureFromSurface(gRenderer, IMG_Load("assets/Lava.png"));

    ArenaGate arenaGates[ARENA_GATE_COUNT] = {
        {}, // Random Gate 1
        {}, // Random Gate 2
        {}  // Random Gate 3
    };

    // Load textures for random gates
    const char* gateImagePaths[ARENA_GATE_COUNT] = {
        "assets/arena.png",    // Random Gate 1 texture
        "assets/arena2.png",   // Random Gate 2 texture
        "assets/arena3.png"    // Random Gate 3 texture
    };

    for (int i = 0; i < ARENA_GATE_COUNT; ++i) {
        if (!arenaGates[i].loadTexture(gateImagePaths[i])) {
            std::cerr << "Failed to load gate texture!" << std::endl;
            closeSDL();
            return -1;
        }
    }

    SDL_Event e;
    bool quit = false;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }

            player.handleEvent(e, gameState, arenaGates);
        }

        player.move(gameState);

        int cameraX = player.getX() - SCREEN_WIDTH / 2;
        int cameraY = player.getY() - SCREEN_HEIGHT / 2;

        SDL_SetRenderDrawColor(gRenderer, 0x87, 0xCE, 0xEB, 0xFF);
        SDL_RenderClear(gRenderer);

        SDL_Rect backgroundRect = { -cameraX, -cameraY, WORLD_WIDTH, WORLD_HEIGHT };

        if (gameState == GAME_STATE_NORMAL) {
            SDL_RenderCopy(gRenderer, groundTextures[player.getCurrentMap()], nullptr, &backgroundRect);
        } else if (gameState == GAME_STATE_ARENA) {
            SDL_RenderCopy(gRenderer, groundTextures[1 + player.getCurrentMap()], nullptr, &backgroundRect);
        }

        player.render(cameraX, cameraY);

        for (int i = 0; i < ARENA_GATE_COUNT; ++i) {
            arenaGates[i].render(cameraX, cameraY);
        }

        for (int i = 0; i < ARENA_GATE_COUNT; ++i) {
            int newPlayerX = player.getX();
            int newPlayerY = player.getY();
            if (arenaGates[i].checkCollision(player.getX(), player.getY(), newPlayerX, newPlayerY) && eKeyPressed && !stateChangeTriggered[i]) {
                gameState = GAME_STATE_ARENA;
                player.setPlayerToArenaStart();
                stateChangeTriggered[i] = true;  // Set the flag to avoid repeated state changes for this gate
            }
            // Reset the flag when the key is released
            if (e.type == SDL_KEYUP && e.key.repeat == 0 && e.key.keysym.sym == SDLK_e) {
                stateChangeTriggered[i] = false;
            }
        }

        SDL_RenderPresent(gRenderer);
    }

    closeSDL();

    return 0;
}

bool Player::loadTexture(const char* path) {
    SDL_Surface* playerSurface = IMG_Load(path);
    if (playerSurface == nullptr) {
        std::cerr << "Failed to load player texture! SDL_image Error: " << IMG_GetError() << std::endl;
        return false;
    }

    playerTexture = SDL_CreateTextureFromSurface(gRenderer, playerSurface);
    if (playerTexture == nullptr) {
        std::cerr << "Failed to create texture from player surface! SDL Error: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(playerSurface);
        return false;
    }

    SDL_FreeSurface(playerSurface);
    return true;
}

void Player::handleEvent(SDL_Event& e, GameState& gameState, ArenaGate* arenaGates) {
    if (gameState == GAME_STATE_NORMAL) {
        if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
            switch (e.key.keysym.sym) {
                case SDLK_a:
                    velocity_x_ = -PLAYER_SPEED;
                    break;
                case SDLK_d:
                    velocity_x_ = PLAYER_SPEED;
                    break;
                case SDLK_w:
                    velocity_y_ = -PLAYER_SPEED;
                    break;
                case SDLK_s:
                    velocity_y_ = PLAYER_SPEED;
                    break;
                case SDLK_e:
                    for (int i = 0; i < ARENA_GATE_COUNT; ++i) {
                        int newPlayerX = getX();
                        int newPlayerY = getY();
                        if (arenaGates[i].checkCollision(getX(), getY(), newPlayerX, newPlayerY)) {
                            gameState = GAME_STATE_ARENA;
                            setPlayerToArenaStart();
                            setCurrentMap(i);
                        }
                    }
                    break;
            }
        } else if (e.type == SDL_KEYUP && e.key.repeat == 0) {
            switch (e.key.keysym.sym) {
                case SDLK_a:
                case SDLK_d:
                    velocity_x_ = 0;
                    break;
                case SDLK_w:
                case SDLK_s:
                    velocity_y_ = 0;
                    break;
            }
        }
    } else if (gameState == GAME_STATE_ARENA) {
        // Handle events specific to the arena state
    }
}

void Player::move(GameState& gameState) {
    x_ += static_cast<int>(velocity_x_);
    y_ += static_cast<int>(velocity_y_);

    // Add movement constraints for the entire world
    if (x_ < 0) {
        x_ = 0;
    } else if (x_ > WORLD_WIDTH - PLAYER_WIDTH) {
        x_ = WORLD_WIDTH - PLAYER_WIDTH;
    }

    if (y_ < 0) {
        y_ = 0;
    } else if (y_ > WORLD_HEIGHT - PLAYER_HEIGHT) {
        y_ = WORLD_HEIGHT - PLAYER_HEIGHT;
    }
}



void Player::render(int cameraX, int cameraY) {
    SDL_Rect playerRect = {x_ - cameraX, y_ - cameraY, PLAYER_WIDTH, PLAYER_HEIGHT};
    SDL_RenderCopy(gRenderer, playerTexture, nullptr, &playerRect);
}

void Player::setPlayerToArenaStart() {
    switch (currentMap) {
        case 0: // Default map (Hub)
            x_ = SCREEN_WIDTH / 2;
            y_ = SCREEN_HEIGHT / 2;
            break;
        case 1: // Arena map 1
            x_ = 100;
            y_ = 100;
            break;
        case 2: // Arena map 2
            x_ = 2000;
            y_ = 3500;
            break;
        case 3: // Arena map 3
            x_ = 5000;
            y_ = 5000;
            break;
    }
}

bool ArenaGate::loadTexture(const char* path) {
    gateTexture = IMG_LoadTexture(gRenderer, path);
    if (gateTexture == nullptr) {
        std::cerr << "Failed to load texture! SDL_image Error: " << IMG_GetError() << std::endl;
        return false;
    }
    return true;
}

void ArenaGate::render(int cameraX, int cameraY) {
    SDL_Rect gateRect = {x_ - cameraX, y_ - cameraY, GATE_WIDTH, GATE_HEIGHT};
    SDL_RenderCopy(gRenderer, gateTexture, nullptr, &gateRect);
}

bool ArenaGate::checkCollision(int playerX, int playerY, int& newPlayerX, int& newPlayerY) {
    int leftA = x_;
    int rightA = x_ + GATE_WIDTH;
    int topA = y_;
    int bottomA = y_ + GATE_HEIGHT;

    int leftB = playerX;
    int rightB = playerX + Player::PLAYER_WIDTH;
    int topB = playerY;
    int bottomB = playerY + Player::PLAYER_HEIGHT;

    // Check if the player is outside the gate
    if (rightA < leftB || leftA > rightB || bottomA < topB || topA > bottomB) {
        return false;
    }

    // Check if the player is fully inside the gate
    if (leftA >= leftB && rightA <= rightB && topA >= topB && bottomA <= bottomB) {
        return true;
    }

    // Adjust player position to prevent collision
    if (leftA < leftB && rightA > leftB) {
        newPlayerX = leftA - Player::PLAYER_WIDTH;
    } else if (rightA > rightB && leftA < rightB) {
        newPlayerX = rightA;
    } else if (topA < topB && bottomA > topB) {
        newPlayerY = topA - Player::PLAYER_HEIGHT;
    } else if (bottomA > bottomB && topA < bottomB) {
        newPlayerY = bottomA;
    }

    return true;
}

bool initializeSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
        return false;
    }

    gWindow = SDL_CreateWindow("Igrca-prototip", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);

    if (gWindow == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);

    if (gRenderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

    return true;
}

bool loadMedia() {
    const char* backgroundImages[4] = {
        "assets/Tlakovci.png",
        "assets/Pesek.png",
        "assets/Trava.png",
        "assets/Lava.png"
    };

    for (int i = 0; i < 4; ++i) {
        SDL_Surface* backgroundSurface = IMG_Load(backgroundImages[i]);
        if (backgroundSurface == nullptr) {
            std::cerr << "Failed to load background image! SDL_image Error: " << IMG_GetError() << std::endl;
            return false;
        }

        groundTextures[i] = SDL_CreateTextureFromSurface(gRenderer, backgroundSurface);
        if (groundTextures[i] == nullptr) {
            std::cerr << "Failed to create texture from background surface! SDL Error: " << SDL_GetError() << std::endl;
            SDL_FreeSurface(backgroundSurface);
            return false;
        }

        SDL_FreeSurface(backgroundSurface);
    }

    return true;
}

void closeSDL() {
    for (int i = 0; i < 4; ++i) {
        SDL_DestroyTexture(groundTextures[i]);
    }

    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    IMG_Quit();
    SDL_Quit();
}
