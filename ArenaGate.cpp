#include "vse.h"

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

    if (rightA < leftB || leftA > rightB || bottomA < topB || topA > bottomB) {
        return false;
    }

    if (    leftA >= leftB && rightA <= rightB && topA >= topB && bottomA <= bottomB) {
        return true;
    }
    return true;
}