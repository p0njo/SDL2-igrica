<<<<<<< HEAD
#ifndef VSE_H
#define VSE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <string.h>
#include <cstring>
#include <cstdlib> 
#include <ctime>
#include <cmath>
#include <fstream>
#include <vector>

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
    GAME_STATE_ARENA,
    GAME_STATE_MENU,
    GAME_STATE_QUIT
};

class Enemy;

class Player {
public:
    static const int PLAYER_WIDTH = 50;
    static const int PLAYER_HEIGHT = 50;
    const int PLAYER_SPEED = 1;

    Player(int x, int y) 
    : x_(x), y_(y), velocity_x_(0), velocity_y_(0), playerTexture(nullptr), currentMap(0), PLAYER_HEALTH(100) {}

    void handleEvent(SDL_Event& e, GameState& gameState, ArenaGate* arenaGates, Teleporter& teleporter);
    void move(GameState& gameState);
    void render(int cameraX, int cameraY);
    void setPlayerToArenaStart();
    void setPlayerToHub();

    int getX() const { return x_; }
    int getY() const { return y_; }

    int getHealth() const { return PLAYER_HEALTH; }  
    void setHealth(int health) { PLAYER_HEALTH = health; } 
    void reduceHealth(int amount);

    bool loadTexture(const char* path);
    bool checkCollision(const Enemy& enemy);  // Now it should work

    int getCurrentMap() const { return currentMap; }
    void setCurrentMap(int map) { currentMap = map; }
    
    int getHealthSegments() const { 
        return PLAYER_HEALTH / 10; 
    }

private:
    int x_;
    int y_;
    int velocity_x_;
    int velocity_y_;
    SDL_Texture* playerTexture;
    int currentMap;
    int PLAYER_HEALTH;
};

class Enemy {
public:
    static const int ENEMY_WIDTH = 100;
    static const int ENEMY_HEIGHT = 100;
    const float ENEMY_SPEED = 0.5;
    const int DIRECTION_CHANGE_DELAY = 1000;

    Enemy(int x, int y) : x_(x), y_(y), velocity_x_(0), velocity_y_(0), enemyTexture(nullptr), currentMap(0), isFollowingPlayer(false) {}

    void move(GameState& gameState, const Player& player);

    void render(int cameraX, int cameraY);

    int getX() const { return x_; }
    int getY() const { return y_; }

    bool loadTexture(const char* path);

private:
    double x_;
    double y_;
    double velocity_x_;
    double velocity_y_; 
    SDL_Texture* enemyTexture;
    int currentMap;
    bool isFollowingPlayer;
    int lastDirectionChangeTime;
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

class Hub {
public:
    Hub() {}

     void renderHealthBar(Player& player, SDL_Renderer* renderer);
     void renderScore(int score, SDL_Renderer* renderer);

private:
};

#endif
=======
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
>>>>>>> d04f3e9a6bc4a6ecccf3f1dab7ae157ba673a0ea
