<<<<<<< HEAD
#include "vse.h"

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const int WORLD_WIDTH = 2000;
const int WORLD_HEIGHT = 2000;
const int ARENA_GATE_COUNT = 3;

SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;
SDL_Texture* groundTextures[4] = {nullptr};

bool initializeSDL();
bool loadMedia();
void closeSDL();

bool stateChangeTriggered[ARENA_GATE_COUNT] = {false};

int main(int argc, char* args[]) {
    bool eKeyPressed = false;
    GameState gameState = GAME_STATE_MENU; // Start with the menu

    Hub hub;
    Teleporter teleporter(0, 0);

    if (!initializeSDL()) {
        std::cerr << "Failed to initialize SDL!" << std::endl;
        return -1;
    }

    if (!loadMedia()) {
        std::cerr << "Failed to load media!" << std::endl;
        closeSDL();
        return -1;
    }

    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
        closeSDL();
        return -1;
    }

    Player player(WORLD_WIDTH / 2, WORLD_HEIGHT / 2);
    Enemy* enemy = nullptr;

    const char* playerImagePath = "assets/player.png";
    if (!player.loadTexture(playerImagePath)) {
        std::cerr << "Failed to load player texture!" << std::endl;
        closeSDL();
        return -1;
    }

    const char* arena1ImagePath = "assets/arena.png";
    const char* arena2ImagePath = "assets/arena2.png";
    const char* arena3ImagePath = "assets/arena3.png";

    if (!groundTextures[0]) groundTextures[0] = SDL_CreateTextureFromSurface(gRenderer, IMG_Load("assets/Tlakovci.png"));
    if (!groundTextures[1]) groundTextures[1] = SDL_CreateTextureFromSurface(gRenderer, IMG_Load("assets/Pesek.png"));
    if (!groundTextures[2]) groundTextures[2] = SDL_CreateTextureFromSurface(gRenderer, IMG_Load("assets/Trava.png"));
    if (!groundTextures[3]) groundTextures[3] = SDL_CreateTextureFromSurface(gRenderer, IMG_Load("assets/Lava.png"));

    ArenaGate arenaGates[ARENA_GATE_COUNT] = {
        {}, {}, {}
    };

    const char* gateImagePaths[ARENA_GATE_COUNT] = {
        "assets/arena.png",
        "assets/arena2.png",
        "assets/arena3.png"
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

    const char* teleporterImagePath = "assets/teleporter.png";
    if (!teleporter.loadTexture(teleporterImagePath)) {
        std::cerr << "Failed to load teleporter texture!" << std::endl;
        closeSDL();
        return -1;
    }

    Uint32 lastCollisionTime = 0;
    const Uint32 COLLISION_INTERVAL = 1000;
    const int HEALTH_REDUCTION_AMOUNT = 10;

    bool enemyKilled = false;
    const Uint32 RESPAWN_TIME = 2000;
    Uint32 respawnTimer = 0;

    int score = 0;  // Initialize score

    // Score thresholds for each arena
    const int SCORE_THRESHOLD_ARENA1 = 500;
    const int SCORE_THRESHOLD_ARENA2 = 1000;
    const int SCORE_THRESHOLD_ARENA3 = 1500;

    int unlockedArena = 0;  // Unlocked arena

    int currentArenaEnemyCount = 3;  // Initialize to 3 for the first arena

    while (!quit) {
        Uint32 currentTicks = SDL_GetTicks();
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }

            player.handleEvent(e, gameState, arenaGates, teleporter);

            if (gameState == GAME_STATE_MENU) {
                if (e.type == SDL_KEYDOWN) {
                    if (e.key.keysym.sym == SDLK_UP || e.key.keysym.sym == SDLK_DOWN) {
                        eKeyPressed = !eKeyPressed;
                    }
                    else if (e.key.keysym.sym == SDLK_RETURN) {
                        if (eKeyPressed) {
                            gameState = GAME_STATE_NORMAL;
                        } else {
                            quit = true;
                        }
                    }
                }
            }

            if (gameState == GAME_STATE_ARENA && e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_e) {
                if (enemy && player.checkCollision(*enemy)) {
                    delete enemy;
                    enemy = nullptr;
                    enemyKilled = true;
                    respawnTimer = 0;
                    score += 100;

                    // Check for arena completion or unlock next arena
                    if (unlockedArena == 0 && score >= SCORE_THRESHOLD_ARENA1) {
                        unlockedArena = 1;
                        currentArenaEnemyCount = 3;  // 3 enemies in the second arena
                    } else if (unlockedArena == 1 && score >= SCORE_THRESHOLD_ARENA2) {
                        unlockedArena = 2;
                        currentArenaEnemyCount = 5;  // 5 enemies in the third arena
                    } else if (unlockedArena == 2 && score >= SCORE_THRESHOLD_ARENA3) {
                        unlockedArena = 3;
                    }
                }
            }
        }

        if (currentTicks - lastCollisionTime >= COLLISION_INTERVAL) {
            if (enemy && player.checkCollision(*enemy)) {
                player.reduceHealth(HEALTH_REDUCTION_AMOUNT);
                lastCollisionTime = currentTicks;

                if (player.getHealth() <= 0) {
                    player.setPlayerToHub();
                    player.setHealth(100);
                    gameState = GAME_STATE_NORMAL;
                }
            }
        }

        player.move(gameState);

        if (enemy && gameState != GAME_STATE_ARENA) {
            delete enemy;
            enemy = nullptr;
        }

        if (enemy) {
            enemy->move(gameState, player);
        }

        int cameraX = player.getX() - SCREEN_WIDTH / 2;
        int cameraY = player.getY() - SCREEN_HEIGHT / 2;

        SDL_SetRenderDrawColor(gRenderer, 0x87, 0xCE, 0xEB, 0xFF);
        SDL_RenderClear(gRenderer);

        SDL_Rect backgroundRect = { -cameraX, -cameraY, WORLD_WIDTH, WORLD_HEIGHT };

        if (gameState == GAME_STATE_NORMAL) {
            SDL_RenderCopy(gRenderer, groundTextures[player.getCurrentMap()], nullptr, &backgroundRect);
        } else if (gameState == GAME_STATE_ARENA) {
            SDL_RenderCopy(gRenderer, groundTextures[1 + player.getCurrentMap()], nullptr, &backgroundRect);
            if (!enemy && currentArenaEnemyCount > 0 && !enemyKilled) {
                enemy = new Enemy(500, 500);
                const char* enemyImagePath = "assets/enemy.png";
                if (!enemy->loadTexture(enemyImagePath)) {
                    std::cerr << "Failed to load enemy texture!" << std::endl;
                    delete enemy;
                    enemy = nullptr;
                    closeSDL();
                    return -1;
                }
                currentArenaEnemyCount--;
            }
        } else if (gameState == GAME_STATE_MENU) {
            SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
            SDL_RenderClear(gRenderer);

            SDL_Color textColor = {255, 255, 255};
            TTF_Font* font = TTF_OpenFont("assets/arial.ttf", 48);
            if (!font) {
                std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
                closeSDL();
                return -1;
            }

            SDL_Surface* playSurface = TTF_RenderText_Solid(font, "Play", textColor);
            SDL_Surface* quitSurface = TTF_RenderText_Solid(font, "Quit", textColor);

            SDL_Texture* playTexture = SDL_CreateTextureFromSurface(gRenderer, playSurface);
            SDL_Texture* quitTexture = SDL_CreateTextureFromSurface(gRenderer, quitSurface);

            SDL_Rect playRect = {SCREEN_WIDTH / 2 - playSurface->w / 2, SCREEN_HEIGHT / 2 - playSurface->h};
            SDL_Rect quitRect = {SCREEN_WIDTH / 2 - quitSurface->w / 2, SCREEN_HEIGHT / 2 + quitSurface->h};

            if (eKeyPressed) {
                SDL_SetRenderDrawColor(gRenderer, 255, 0, 0, 255);
                SDL_RenderFillRect(gRenderer, &quitRect);
            } else {
                SDL_SetRenderDrawColor(gRenderer, 255, 0, 0, 255);
                SDL_RenderFillRect(gRenderer, &playRect);
            }

            SDL_RenderCopy(gRenderer, playTexture, nullptr, &playRect);
            SDL_RenderCopy(gRenderer, quitTexture, nullptr, &quitRect);

            SDL_RenderPresent(gRenderer);

            SDL_FreeSurface(playSurface);
            SDL_FreeSurface(quitSurface);
            SDL_DestroyTexture(playTexture);
            SDL_DestroyTexture(quitTexture);
            TTF_CloseFont(font);
        }

        player.render(cameraX, cameraY);

        if (gameState == GAME_STATE_NORMAL) {
            for (int i = 0; i <= unlockedArena; ++i) {
                arenaGates[i].render(cameraX, cameraY);
            }
        } else if (gameState == GAME_STATE_ARENA) {
            teleporter.render(cameraX, cameraY);
        }

        if (enemy) {
            enemy->render(cameraX, cameraY);
        }

        hub.renderHealthBar(player, gRenderer);
        hub.renderScore(score, gRenderer); 

        SDL_RenderPresent(gRenderer);

        if (enemyKilled) {
            respawnTimer += currentTicks - lastCollisionTime;
            if (respawnTimer >= RESPAWN_TIME) {
                enemy = new Enemy(500, 500);
                const char* enemyImagePath = "assets/enemy.png";
                if (!enemy->loadTexture(enemyImagePath)) {
                    std::cerr << "Failed to load enemy texture!" << std::endl;
                    delete enemy;
                    enemy = nullptr;
                    closeSDL();
                    return -1;
                }
                enemyKilled = false;
            }
        }

        if (score >= SCORE_THRESHOLD_ARENA3) {
            SDL_Color textColor = {255, 255, 255};
            TTF_Font* font = TTF_OpenFont("assets/arial.ttf", 72);
            if (!font) {
                std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
                closeSDL();
                return -1;
            }

            SDL_Surface* victorySurface = TTF_RenderText_Solid(font, "VICTORY!", textColor);
            SDL_Texture* victoryTexture = SDL_CreateTextureFromSurface(gRenderer, victorySurface);

            SDL_Rect victoryRect = {SCREEN_WIDTH / 2 - victorySurface->w / 2, SCREEN_HEIGHT / 2 - victorySurface->h / 2, victorySurface->w, victorySurface->h};
            
            SDL_RenderCopy(gRenderer, victoryTexture, nullptr, &victoryRect);
            
            SDL_FreeSurface(victorySurface);
            SDL_DestroyTexture(victoryTexture);
            TTF_CloseFont(font);

            SDL_RenderPresent(gRenderer);
            
            SDL_Delay(2000);  // Wait for 2 seconds before closing the game
            quit = true;
        }
    }

    closeSDL();

    return 0;
}

bool initializeSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (gWindow == nullptr) {
        std::cerr << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
    if (gRenderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
        return false;
    }

    return true;
}

bool loadMedia() {
    return true;
}

void closeSDL() {
    for (int i = 0; i < 4; ++i) {
        if (groundTextures[i] != nullptr) {
            SDL_DestroyTexture(groundTextures[i]);
            groundTextures[i] = nullptr;
        }
    }

    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = nullptr;
    gRenderer = nullptr;

    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}
=======
#include "vse.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int WORLD_WIDTH = 2000;
const int WORLD_HEIGHT = 2000;
const int ARENA_GATE_COUNT = 3;

SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;
SDL_Texture* groundTextures[4] = {nullptr};

bool initializeSDL();

bool loadMedia();

void closeSDL();

bool stateChangeTriggered[ARENA_GATE_COUNT] = {false};

int main(int argc, char* args[]) {
    bool eKeyPressed = false;
    GameState gameState = GAME_STATE_NORMAL;

    Teleporter teleporter(0, 0);

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

    const char* teleporterImagePath = "assets/teleporter.png";
    if (!teleporter.loadTexture(teleporterImagePath)) {
        std::cerr << "Failed to load teleporter texture!" << std::endl;
        closeSDL();
        return -1;
    }

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }

            player.handleEvent(e, gameState, arenaGates, teleporter);
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

        if (gameState == GAME_STATE_NORMAL) {
            for (int i = 0; i < ARENA_GATE_COUNT; ++i) {
                arenaGates[i].render(cameraX, cameraY);
            }
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

        if (gameState == GAME_STATE_ARENA) {
            teleporter.render(cameraX, cameraY);

            int newPlayerX = player.getX();
            int newPlayerY = player.getY();
            if (teleporter.checkCollision(player.getX(), player.getY(), newPlayerX, newPlayerY) && e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_t) {
                gameState = GAME_STATE_NORMAL;
                player.setPlayerToHub();
            }
        }

        SDL_RenderPresent(gRenderer);
    }

    closeSDL();

    return 0;
}

bool initializeSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    gWindow = SDL_CreateWindow("Igrca v0.1.5", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (gWindow == nullptr) {
        std::cerr << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
    if (gRenderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
        return false;
    }

    return true;
}

bool loadMedia() {
    return true;
}

void closeSDL() {
    for (int i = 0; i < 4; ++i) {
        if (groundTextures[i] != nullptr) {
            SDL_DestroyTexture(groundTextures[i]);
            groundTextures[i] = nullptr;
        }
    }

    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = nullptr;
    gRenderer = nullptr;

    IMG_Quit();
    SDL_Quit();
}
>>>>>>> d04f3e9a6bc4a6ecccf3f1dab7ae157ba673a0ea
