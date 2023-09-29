#include <SDL2/SDL.h>
#include "cabinet/cabinet.h"

class Invaders {
public:
    Invaders();
    ~Invaders();

    void run();
    void load_rom(std::ifstream &rom_file);

protected:
    void gpuUpdate();
    bool windowInit();
    void pollEvents();
    void gameUpdate();
    bool hardwareInit();
    void updateScreen();

    void handleIn();
    void handleOut();
    void handleKeyUp(SDL_Scancode keyCode);
    void handleKeyDown(SDL_Scancode keyCode);

private:
    uint32_t timer = 0;
    bool shouldQuit = false;
    bool initialized = false;
    Cabinet cabinet;

    SDL_Window* window = nullptr;
    SDL_Texture* texture = nullptr;
    SDL_Renderer* renderer = nullptr;
};
