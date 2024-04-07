#include "vse.h"

void Hub::renderHealthBar(Player& player, SDL_Renderer* renderer) {
    // Calculate the width of each segment
    int segmentWidth = 20; // Adjust as necessary
    int segmentHeight = 30; // Adjust as necessary
    int startX = SCREEN_WIDTH - (player.getHealthSegments() * segmentWidth) - 50; // 10 pixels padding from the right edge
    int startY = 10; // 10 pixels from the top

    SDL_Rect segmentRect = {startX, startY, segmentWidth, segmentHeight};

    // Render each health segment
    for (int i = 0; i < player.getHealthSegments(); ++i) {
        int health = player.getHealth();
        int red = 255 - ((health % 10) + i * (255 / 10));
        int green = 0;
        int blue = 0;

        SDL_SetRenderDrawColor(renderer, red, green, blue, 255);
        SDL_RenderFillRect(renderer, &segmentRect);

        // Move to the next segment position
        segmentRect.x += segmentWidth + 5; // 5 pixels spacing between segments
    }
}

void Hub::renderScore(int score, SDL_Renderer* renderer) {
    TTF_Font* font = TTF_OpenFont("assets/arial.ttf", 36); // Load a font with size 36
    if (!font) {
        std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Color textColor = {255, 255, 255}; // White color
    std::string scoreText = "Score: " + std::to_string(score); // Convert score to string
    SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
    if (!scoreSurface) {
        std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
        TTF_CloseFont(font);
        return;
    }

    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
    if (!scoreTexture) {
        std::cerr << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(scoreSurface);
        TTF_CloseFont(font);
        return;
    }

    SDL_Rect scoreRect = {10, 10, scoreSurface->w, scoreSurface->h}; // Position of the score text
    SDL_RenderCopy(renderer, scoreTexture, nullptr, &scoreRect);

    SDL_FreeSurface(scoreSurface);
    SDL_DestroyTexture(scoreTexture);
    TTF_CloseFont(font);
}