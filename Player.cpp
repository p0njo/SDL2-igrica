#include "vse.h"

Player::Player() : name(""), score(0) {}

void Player::setName(const std::string& playerName) {
    this->name = playerName;
}

void Player::inputPlayerName(SDL_Renderer* renderer) {
    TTF_Font* font = TTF_OpenFont("assets/arial.ttf", 24);
    if (!font) {
        std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Color textColor = {255, 255, 255};
    std::string name = "";
    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_BACKSPACE && name.length() > 0) {
                    name.pop_back();
                } else if (e.key.keysym.sym == SDLK_RETURN) {
                    quit = true;
                } else if ((e.key.keysym.sym >= SDLK_a && e.key.keysym.sym <= SDLK_z) || 
                           (e.key.keysym.sym >= SDLK_0 && e.key.keysym.sym <= SDLK_9) ||
                           e.key.keysym.sym == SDLK_SPACE) {
                    name += (char)e.key.keysym.sym;
                }
            }
        }

        SDL_Surface* surface = TTF_RenderText_Solid(font, name.c_str(), textColor);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

        int width = surface->w;
        int height = surface->h;

        SDL_Rect rect = {SCREEN_WIDTH / 2 - width / 2, SCREEN_HEIGHT / 2 - height / 2, width, height};
        SDL_RenderCopy(renderer, texture, nullptr, &rect);

        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);

        SDL_RenderPresent(renderer);
    }

    this->name = name;
    TTF_CloseFont(font);
}

std::string Player::getName() const {
    return name;
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

void Player::handleEvent(SDL_Event& e, GameState& gameState, ArenaGate* arenaGates, Teleporter& teleporter) {
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
    if (gameState == GAME_STATE_NORMAL) {
        switch (e.key.keysym.sym) {
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
    } else if (gameState == GAME_STATE_ARENA) {
        switch (e.key.keysym.sym) {
            case SDLK_e:
                int newPlayerX = getX();
                int newPlayerY = getY();
                if (teleporter.checkCollision(getX(), getY(), newPlayerX, newPlayerY)) {
                    gameState = GAME_STATE_NORMAL;
                    setPlayerToHub();
                    setCurrentMap(0);
                }
                break;
        }
    }
}

void Player::move(GameState& gameState) {
    x_ += static_cast<int>(velocity_x_);
    y_ += static_cast<int>(velocity_y_);
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

void Player::setPlayerToHub() {
    x_ = WORLD_WIDTH / 2;
    y_ = WORLD_HEIGHT / 2;
    setCurrentMap(0); 
}

void Player::setPlayerToArenaStart() {
    switch (currentMap) {
        case 0:
            x_ = SCREEN_WIDTH / 2;
            y_ = SCREEN_HEIGHT / 2;
            break;
        case 1:
            x_ = 100;
            y_ = 100;
            break;
        case 2:
            x_ = 2000;
            y_ = 3500;
            break;
        case 3:
            x_ = 5000;
            y_ = 5000;
            break;
    }
}

void Player::reduceHealth(int amount) {
    PLAYER_HEALTH -= amount;
}

bool Player::checkCollision(const Enemy& enemy) {
    int leftA = x_;
    int rightA = x_ + PLAYER_WIDTH;
    int topA = y_;
    int bottomA = y_ + PLAYER_HEIGHT;

    int leftB = enemy.getX();
    int rightB = enemy.getX() + Enemy::ENEMY_WIDTH;
    int topB = enemy.getY();
    int bottomB = enemy.getY() + Enemy::ENEMY_HEIGHT;

    // No collision
    if (rightA < leftB || leftA > rightB || bottomA < topB || topA > bottomB) {
        return false;
    }

    return true;
}