#include "vse.h"

bool Enemy::loadTexture(const char* path) {
    SDL_Surface* enemySurface = IMG_Load(path);
    if (enemySurface == nullptr) {
        std::cerr << "Failed to load player texture! SDL_image Error: " << IMG_GetError() << std::endl;
        return false;
    }

    enemyTexture = SDL_CreateTextureFromSurface(gRenderer, enemySurface);
    if (enemyTexture == nullptr) {
        std::cerr << "Failed to create texture from player surface! SDL Error: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(enemySurface);
        return false;
    }

    SDL_FreeSurface(enemySurface);
    return true;
}

void Enemy::move(GameState& gameState, const Player& player) {
    // a je u razdalji
    double distance = std::sqrt(std::pow(player.getX() - x_, 2) + std::pow(player.getY() - y_, 2));
    double followRange = 500;
    isFollowingPlayer = (distance < followRange);

    // spremeni state
    if (!isFollowingPlayer && SDL_GetTicks() - lastDirectionChangeTime >= DIRECTION_CHANGE_DELAY) {
        velocity_x_ = (rand() % 3) - 1;
        velocity_y_ = (rand() % 3) - 1;
        velocity_x_ *= 0.25;
        velocity_y_ *= 0.25;

        lastDirectionChangeTime = SDL_GetTicks();
    }

    //če je v razdalji sledi
    if (isFollowingPlayer) {
        double dx = player.getX() - x_;
        double dy = player.getY() - y_;
        double magnitude = std::sqrt(dx * dx + dy * dy);
        
        if (magnitude > 0) {
            velocity_x_ = ENEMY_SPEED * dx / magnitude;
            velocity_y_ = ENEMY_SPEED * dy / magnitude;
        }
    }

    x_ += velocity_x_;
    y_ += velocity_y_;

    //rob
    if (x_ < 0) {
        x_ = 0; 
    } else if (x_ > WORLD_WIDTH - ENEMY_WIDTH) {
        x_ = WORLD_WIDTH - ENEMY_WIDTH; 
    }

    if (y_ < 0) {
        y_ = 0; 
    } else if (y_ > WORLD_HEIGHT - ENEMY_HEIGHT) {
        y_ = WORLD_HEIGHT - ENEMY_HEIGHT; 
    }
}



void Enemy::render(int cameraX, int cameraY) {
    int renderX = x_ - cameraX;
    int renderY = y_ - cameraY;
    SDL_Rect enemyRect = { renderX, renderY, ENEMY_WIDTH, ENEMY_HEIGHT };
    SDL_RenderCopy(gRenderer, enemyTexture, nullptr, &enemyRect);
}
