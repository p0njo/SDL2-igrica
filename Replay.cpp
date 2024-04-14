#include "vse.h"

void Replay::saveMovementsToFile(const std::vector<SDL_Event>& events) {
    std::ofstream file("movements.txt", std::ios::out | std::ios::app);
    if (file.is_open()) {
        for (const auto& event : events) {
            file << event.type << " " << event.key.keysym.sym << "\n";
        }
        file.close();
    } else {
        std::cerr << "Unable to open file movements.txt for writing!" << std::endl;
    }
}


std::vector<SDL_Event> Replay::loadMovementsFromFile() {
    std::ifstream inputFile("movements.txt");
    std::vector<SDL_Event> movements;
    std::string line;

    while (std::getline(inputFile, line)) {
        std::istringstream iss(line);
        int type, keyCode;
        SDL_Event event;

        iss >> type;
        event.type = static_cast<Uint32>(type);

        if (type == SDL_KEYDOWN || type == SDL_KEYUP) {
            iss >> keyCode;
            event.key.keysym.sym = static_cast<SDL_Keycode>(keyCode);
        }

        movements.push_back(event);
    }

    return movements;
}

