#include "vse.h"

void Hub::renderHealthBar(Player& player, SDL_Renderer* renderer) {
    int segmentWidth = 20;
    int segmentHeight = 30; 
    int startX = SCREEN_WIDTH - (player.getHealthSegments() * segmentWidth) - 50;
    int startY = 10;

    SDL_Rect segmentRect = {startX, startY, segmentWidth, segmentHeight};
    for (int i = 0; i < player.getHealthSegments(); ++i) {
        int health = player.getHealth();
        int red = 255 - ((health % 10) + i * (255 / 10));
        int green = 0;
        int blue = 0;

        SDL_SetRenderDrawColor(renderer, red, green, blue, 255);
        SDL_RenderFillRect(renderer, &segmentRect);

        segmentRect.x += segmentWidth + 5;
    }
}

void Hub::renderScore(int score, SDL_Renderer* renderer) {
    TTF_Font* font = TTF_OpenFont("assets/arial.ttf", 36);
    if (!font) {
        std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Color textColor = {255, 255, 255};
    std::string scoreText = "Score: " + std::to_string(score);
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

    SDL_Rect scoreRect = {10, 10, scoreSurface->w, scoreSurface->h};
    SDL_RenderCopy(renderer, scoreTexture, nullptr, &scoreRect);

    SDL_FreeSurface(scoreSurface);
    SDL_DestroyTexture(scoreTexture);
    TTF_CloseFont(font);
}

void Hub::saveScoreToFile(const std::string& name, int score) {
    std::ofstream file("scores.txt", std::ios::app);
    if (!file.is_open()) {
        std::cerr << "Unable to open file!" << std::endl;
        return;
    }
    if (!(file << name << " " << score << std::endl)) {
        std::cerr << "Error writing to file!" << std::endl;
        file.close();
        return;
    }

    file.close();
}



void Hub::renderTopScores(const std::vector<std::pair<std::string, int>>& scores, SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xFF, 0xFF);
    SDL_RenderClear(renderer);

    TTF_Font* font = TTF_OpenFont("assets/arial.ttf", 36);
    if (!font) {
        std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Color textColor = {255, 255, 255};
    SDL_Surface* topScoresSurface = TTF_RenderText_Solid(font, "Top Scores:", textColor);
    SDL_Texture* topScoresTexture = SDL_CreateTextureFromSurface(renderer, topScoresSurface);

    int topScoresWidth = topScoresSurface->w;
    int topScoresHeight = topScoresSurface->h;

    SDL_Rect topScoresRect = {SCREEN_WIDTH / 2 - topScoresWidth / 2, 50, topScoresWidth, topScoresHeight};
    SDL_RenderCopy(renderer, topScoresTexture, nullptr, &topScoresRect);

    SDL_FreeSurface(topScoresSurface);
    SDL_DestroyTexture(topScoresTexture);

    int y = 150;
    int rank = 1;

    for (const auto& score : scores) {
        std::string scoreText = std::to_string(rank) + ". " + score.first + ": " + std::to_string(score.second);
        SDL_Surface* surface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

        int width = surface->w;
        int height = surface->h;

        SDL_Rect rect = {SCREEN_WIDTH / 2 - width / 2, y, width, height};
        SDL_RenderCopy(renderer, texture, nullptr, &rect);

        y += height + 10; // Move to the next line
        rank++; // Increment the rank
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }

    TTF_CloseFont(font);
}


std::vector<std::pair<std::string, int>> Hub::loadTopScoresFromFile() {
    std::vector<std::pair<std::string, int>> topScores;
    std::ifstream file("scores.txt");

    if (file.is_open()) {
        std::string name;
        int score;
        while (file >> name >> score) {
            topScores.push_back({name, score});
        }
        file.close();
    }

 
    std::sort(topScores.begin(), topScores.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

    if (topScores.size() > 5) {
        topScores.resize(5);
    }

    return topScores;
}

void Hub::saveTopScoreToFile(const Score& newScore) {
    std::vector<Score> scores;
    loadTopScoresFromFile();

    scores.push_back(newScore);

    std::sort(scores.begin(), scores.end(), [](const Score& a, const Score& b) {
        return a.score > b.score;
    });

    if (scores.size() > 5) {
        scores.resize(5);
    }

    std::ofstream file("scores.txt");
    for (const auto& s : scores) {
        file << s.name << " " << s.score << "\n";
    }
    file.close();
}

std::vector<std::pair<std::string, int>> Hub::readScoresFromFile(const std::string& filename) {
    std::vector<std::pair<std::string, int>> scores;
    std::ifstream file(filename);

    if (file.is_open()) {
        std::string name;
        int score;
        while (file >> name >> score) {
            scores.push_back({name, score});
        }
        file.close();
    }

    std::sort(scores.begin(), scores.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

    return scores;
}
