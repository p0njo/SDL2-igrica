#ifndef VSE_H
#define VSE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <cstdlib>
#include <ctime>

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
extern const int WORLD_WIDTH;
extern const int WORLD_HEIGHT;
extern const int ARENA_GATE_COUNT;

extern SDL_Window* gWindow;
extern SDL_Renderer* gRenderer;
extern SDL_Texture* groundTextures[4];

class ArenaGate;
class Teleporter;

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

    void handleEvent(SDL_Event& e, GameState& gameState, ArenaGate* arenaGates, Teleporter& teleporter);

    void move(GameState& gameState);

    void render(int cameraX, int cameraY);

    void setPlayerToArenaStart();

    void setPlayerToHub();

    int getX() const { return x_; }
    int getY() const { return y_; }

    bool loadTexture(const char* path);

    int getCurrentMap() const {
        return currentMap;
    }

    void setCurrentMap(int map) {
        currentMap = map;
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

class Teleporter {
public:
    static const int TELEPORTER_WIDTH = 100;
    static const int TELEPORTER_HEIGHT = 100;

    Teleporter(int x, int y) : x_(x), y_(y), teleporterTexture(nullptr) {}

    ~Teleporter() {
        if (teleporterTexture != nullptr) {
            SDL_DestroyTexture(teleporterTexture);
            teleporterTexture = nullptr;
        }
    }

    bool loadTexture(const char* path);

    void render(int cameraX, int cameraY);

    bool checkCollision(int playerX, int playerY, int& newPlayerX, int& newPlayerY);

private:
    int x_;
    int y_;
    SDL_Texture* teleporterTexture;
};

#endif
