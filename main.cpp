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
    SDL_Quit();
}