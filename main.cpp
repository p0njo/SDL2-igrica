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

void resetGame(Player& player, Enemy*& enemy, int& score, int& unlockedArena, int& currentArenaEnemyCount, bool& enemyKilled, Uint32& respawnTimer, const std::string& playerName, GameState& gameState) {
    Hub hub;
    player.setX(WORLD_WIDTH / 2);
    player.setY(WORLD_HEIGHT / 2);
    player.setHealth(100);
    player.setMap(0);
    hub.saveScoreToFile(playerName, score);
    
    if (enemy) {
        delete enemy;
        enemy = nullptr;
    }

    score = 0;
    unlockedArena = 0;
    currentArenaEnemyCount = 3;
    enemyKilled = false;
    respawnTimer = 0;

    gameState = GAME_STATE_MENU;
}

bool isReplayMode = false;
Replay replay;

int main(int argc, char* args[]) {
    bool eKeyPressed = false;
    int currentSelection = 0;
    int score = 0; 

    GameState gameState = GAME_STATE_MENU;

    std::vector<SDL_Event> events;
    Replay replay;

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

    const int SCORE_THRESHOLD_ARENA1 = 500;
    const int SCORE_THRESHOLD_ARENA2 = 1000;

    int unlockedArena = 0; 

    int currentArenaEnemyCount = 3; 

    bool playerDead = false;
    bool enteringName = false;
    std::string playerName = "";

    while (!quit) {
        Uint32 currentTicks = SDL_GetTicks();
        std::vector<SDL_Event> currentMovements;

if (isReplayMode) {
            currentMovements = replay.loadMovementsFromFile();
        }

        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }

            events.push_back(e);

            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) {  
                    quit = true;
                }
            }

            if (e.key.keysym.sym == SDLK_e) {
    if (enemy && player.checkCollision(*enemy)) {
        delete enemy;
        enemy = nullptr;
        score += 100;

        if (unlockedArena == 0 && score >= SCORE_THRESHOLD_ARENA1) {
            unlockedArena = 1;
            currentArenaEnemyCount = 3;
        } else if (unlockedArena == 1 && score >= SCORE_THRESHOLD_ARENA2) {
            unlockedArena = 2;
            currentArenaEnemyCount = 5;
        }

        enemyKilled = true;
    }
}
            if (gameState == GAME_STATE_ENTER_NAME) {
                if (e.type == SDL_KEYDOWN) {
                    if (e.key.keysym.sym == SDLK_BACKSPACE && playerName.length() > 0) {
                        playerName.pop_back();
                    } else if (e.key.keysym.sym == SDLK_RETURN) {
                        enteringName = false;  
                        gameState = GAME_STATE_NORMAL; 
                        remove("movements.txt");
                    } else if ((e.key.keysym.sym >= SDLK_a && e.key.keysym.sym <= SDLK_z) || 
                               (e.key.keysym.sym >= SDLK_0 && e.key.keysym.sym <= SDLK_9) ||
                               e.key.keysym.sym == SDLK_SPACE) {
                        playerName += (char)e.key.keysym.sym;
                    }
                }
            }

            if (gameState == GAME_STATE_MENU) {
    if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
            case SDLK_UP:
                currentSelection = (currentSelection - 1 + 4) % 4;
                break;
            case SDLK_DOWN:
                currentSelection = (currentSelection + 1) % 4; 
                break;
            case SDLK_RETURN:
                if (currentSelection == 0) { 
                    gameState = GAME_STATE_ENTER_NAME;
                } else if (currentSelection == 1) { 
                    gameState = GAME_STATE_TOP_SCORES;
                } else if (currentSelection == 2) {
                    gameState = GAME_STATE_REPLAY;
                } else if (currentSelection == 3) { 
                    quit = true;
                }
                break;
        }
    }
}

            player.handleEvent(e, gameState, arenaGates, teleporter);


             if (gameState == GAME_STATE_REPLAY) {
                isReplayMode = true;
                replay.loadMovementsFromFile();
            } else {
                isReplayMode = false;
            }

            if (gameState == GAME_STATE_NORMAL || gameState == GAME_STATE_ARENA) {
            replay.saveMovementsToFile(events);
        }

           if (gameState == GAME_STATE_ARENA) {
        if (!enemy && !enemyKilled) {
            enemy = new Enemy(500, 500);
            const char* enemyImagePath = "assets/enemy.png";
            if (!enemy->loadTexture(enemyImagePath)) {
                std::cerr << "Failed to load enemy texture!" << std::endl;
                delete enemy;
                enemy = nullptr;
                closeSDL();
                return -1;
            }
        }  
}

        }
        if (currentTicks - lastCollisionTime >= COLLISION_INTERVAL) {
            if (enemy && player.checkCollision(*enemy)) {
                player.reduceHealth(HEALTH_REDUCTION_AMOUNT);
                lastCollisionTime = currentTicks;

                if (player.getHealth() <= 0) {
                    resetGame(player, enemy, score, unlockedArena, currentArenaEnemyCount, enemyKilled, respawnTimer, playerName, gameState);
                    playerDead = true;
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
        
        if (gameState == GAME_STATE_MENU) {
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
            SDL_Surface* topScoresSurface = TTF_RenderText_Solid(font, "Top Scores", textColor); 
            SDL_Surface* replaySurface = TTF_RenderText_Solid(font, "Replay", textColor); 
            SDL_Surface* quitSurface = TTF_RenderText_Solid(font, "Quit", textColor);

            if (!playSurface || !topScoresSurface || !quitSurface || !replaySurface) {
                std::cerr << "Failed to render text surfaces! SDL_ttf Error: " << TTF_GetError() << std::endl;
                closeSDL();
                return -1;
            }

            SDL_Texture* playTexture = SDL_CreateTextureFromSurface(gRenderer, playSurface);
            SDL_Texture* topScoresTexture = SDL_CreateTextureFromSurface(gRenderer, topScoresSurface);
            SDL_Texture* replayTexture = SDL_CreateTextureFromSurface(gRenderer, replaySurface);
            SDL_Texture* quitTexture = SDL_CreateTextureFromSurface(gRenderer, quitSurface);

            int verticalSpacing = 60;

SDL_Rect playRect = {
    SCREEN_WIDTH / 2 - playSurface->w / 2,
    SCREEN_HEIGHT / 2 - (3 * verticalSpacing) / 2,
    playSurface->w,
    playSurface->h
};

SDL_Rect topScoresRect = {
    SCREEN_WIDTH / 2 - topScoresSurface->w / 2,
    SCREEN_HEIGHT / 2 - verticalSpacing / 2,
    topScoresSurface->w,
    topScoresSurface->h
};

SDL_Rect replayRect = {
    SCREEN_WIDTH / 2 - replaySurface->w / 2,
    SCREEN_HEIGHT / 2 + verticalSpacing / 2,
    replaySurface->w,
    replaySurface->h
};

SDL_Rect quitRect = {
    SCREEN_WIDTH / 2 - quitSurface->w / 2,
    SCREEN_HEIGHT / 2 + (3 * verticalSpacing) / 2,
    quitSurface->w,
    quitSurface->h
};


            if (currentSelection == 0) {
                SDL_SetRenderDrawColor(gRenderer, 255, 0, 0, 255);
                SDL_RenderFillRect(gRenderer, &playRect);
            } else if (currentSelection == 1) {
                SDL_SetRenderDrawColor(gRenderer, 255, 0, 0, 255);
                SDL_RenderFillRect(gRenderer, &topScoresRect);
            }  else if (currentSelection == 2) { 
                SDL_SetRenderDrawColor(gRenderer, 255, 0, 0, 255);
                SDL_RenderFillRect(gRenderer, &replayRect);
            }  else {
                SDL_SetRenderDrawColor(gRenderer, 255, 0, 0, 255);
                SDL_RenderFillRect(gRenderer, &quitRect);
            }

            SDL_RenderCopy(gRenderer, playTexture, nullptr, &playRect);
            SDL_RenderCopy(gRenderer, topScoresTexture, nullptr, &topScoresRect);
            SDL_RenderCopy(gRenderer, replayTexture, nullptr, &replayRect); 
            SDL_RenderCopy(gRenderer, quitTexture, nullptr, &quitRect);

            SDL_RenderPresent(gRenderer);

            SDL_FreeSurface(playSurface);
            SDL_FreeSurface(topScoresSurface);
            SDL_FreeSurface(replaySurface);
            SDL_FreeSurface(quitSurface);
            SDL_DestroyTexture(playTexture);
            SDL_DestroyTexture(topScoresTexture);
            SDL_DestroyTexture(replayTexture);
            SDL_DestroyTexture(quitTexture);
            TTF_CloseFont(font);
        } else if (gameState == GAME_STATE_ENTER_NAME) {
            TTF_Font* font = TTF_OpenFont("assets/arial.ttf", 24);
            if (!font) {
                std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
                closeSDL();
                return -1;
            }

            SDL_Color textColor = {255, 255, 255};
            SDL_Surface* surface = TTF_RenderText_Solid(font, ("Player name: " + playerName).c_str(), textColor);
            SDL_Texture* texture = SDL_CreateTextureFromSurface(gRenderer, surface);

            int width = surface->w;
            int height = surface->h;

            SDL_Rect rect = {SCREEN_WIDTH / 2 - width / 2, SCREEN_HEIGHT / 2 - height / 2, width, height};
            SDL_RenderCopy(gRenderer, texture, nullptr, &rect);

            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);
            TTF_CloseFont(font);

            SDL_RenderPresent(gRenderer);
        } else if (gameState == GAME_STATE_TOP_SCORES) {
    std::vector<std::pair<std::string, int>> scores = hub.loadTopScoresFromFile();

    SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0xFF, 0xFF);
    SDL_RenderClear(gRenderer);

    hub.renderTopScores(scores, gRenderer);

    SDL_RenderPresent(gRenderer);

    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            quit = true;
        }
        
        if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_BACKSPACE) {
                gameState = GAME_STATE_MENU;
            }
        }
    }
}

 else {
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
        }

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
        respawnTimer = 0;
    }
}

        events.clear();
    }

    closeSDL();

    return 0;
}

bool initializeSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    gWindow = SDL_CreateWindow("SDL Igrica", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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