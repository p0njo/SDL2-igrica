#include "vse.h"

bool Teleporter::loadTexture(const char* path) {
    teleporterTexture = IMG_LoadTexture(gRenderer, path);
    if (teleporterTexture == nullptr) {
        std::cerr << "Failed to load texture! SDL_image Error: " << IMG_GetError() << std::endl;
        return false;
    }
    return true;
}

void Teleporter::render(int cameraX, int cameraY) {
    SDL_Rect teleporterRect = {x_ - cameraX, y_ - cameraY, TELEPORTER_WIDTH, TELEPORTER_HEIGHT};
    SDL_RenderCopy(gRenderer, teleporterTexture, nullptr, &teleporterRect);
}

bool Teleporter::checkCollision(int playerX, int playerY, int& newPlayerX, int& newPlayerY) {
    int leftA = x_;
    int rightA = x_ + TELEPORTER_WIDTH;
    int topA = y_;
    int bottomA = y_ + TELEPORTER_HEIGHT;

    int leftB = playerX;
    int rightB = playerX + Player::PLAYER_WIDTH;
    int topB = playerY;
    int bottomB = playerY + Player::PLAYER_HEIGHT;

    //ni v teleporterju
    if (rightA < leftB || leftA > rightB || bottomA < topB || topA > bottomB) {
        return false;
    }
    //je v teleporteju
    if (leftA >= leftB && rightA <= rightB && topA >= topB && bottomA <= bottomB) {  
        return true;
    }
    return true;
}