#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"

#include "invaders.h"
#include "constants.h"

Invaders::Invaders() {
    initialized = windowInit();
    initialized &= hardwareInit();
    if (initialized)
        updateScreen();
}

Invaders::~Invaders() {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

bool Invaders::windowInit() {
    // SDL init functions to start video subsystems
    if(SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    // create SDL window and set some intial properties
    window = SDL_CreateWindow("Space Invaders", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH() * 2, SCREEN_HEIGHT() * 2, SDL_WINDOW_RESIZABLE);
    if(window == nullptr) {
        SDL_Log("unable to create game window: %s", SDL_GetError());
        return false;
    }
    SDL_SetWindowMinimumSize(window, SCREEN_WIDTH(), SCREEN_HEIGHT());
    SDL_ShowCursor(SDL_DISABLE);

    // create a renderer for the game graphics
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if( renderer == nullptr) {
        SDL_Log("unable to create render: %s", SDL_GetError());
        return false;
    }
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH(), SCREEN_HEIGHT());

    // log info about the selected renderer
    SDL_RendererInfo rendererInfo;
    SDL_GetRendererInfo(renderer, &rendererInfo);
    SDL_Log("using renderer: %s", rendererInfo.name);

    // create a texture to display
    texture = SDL_CreateTexture(renderer,
                                SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH(), SCREEN_HEIGHT());
    if (texture == nullptr) {
        SDL_Log("unable to create texture: %s", SDL_GetError());
        return false;
    }
    return true;
}

bool Invaders::hardwareInit() {
    // bit 3 should always be set
    cabinet.port1 = 1u << 3u;
    cabinet.port2 = 0;

    return true;
}

void Invaders::pollEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                shouldQuit = true;
                break;
            case SDL_KEYDOWN:
                handleKeyDown(event.key.keysym.scancode);
                break;
            case SDL_KEYUP:
                handleKeyUp(event.key.keysym.scancode);
                break;
            default:
                break;
        }

    }
}

void Invaders::run() {
    if(initialized) {
        timer = SDL_GetTicks();
        while (!shouldQuit) {
            pollEvents();

            if (SDL_GetTicks() - timer > (1.0f / FPS()) * 1000) {
                timer = SDL_GetTicks();
                gameUpdate();
                gpuUpdate();
            }

            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, nullptr, nullptr);
            SDL_RenderPresent(renderer);
        }
    }
}

void Invaders::gameUpdate() {
    uint32_t cycleCount = 0;
    while(cycleCount <= CYCLES_PER_FRAME()) {
        const uint32_t start_cyc = cabinet.get_cycle_count();

        // grab the next opcode for later to handle space invader special codes
        // if hte opcode is one of the special codes the cpu will treat as a NOP
        const uint8_t opcode = cabinet.memory.read_byte(cabinet.get_program_counter());
        cabinet.cpu.step();
        cycleCount += cabinet.get_cycle_count() - start_cyc;

        // now handle special codes if needed
        switch(opcode) {
            case 0xdb:  // IN
                handleIn();
                break;
            case 0xd3:  // OUT
                handleOut();
                break;
            default:
                break;
        }

        // now we look to see if we are at a half frame so we send an interrupt for the screen
        if(cabinet.get_cycle_count() >= HALF_CYCLES_PER_FRAME()) {
            cabinet.interrupt(cabinet.next_interrupt);
            cabinet.set_cycle_count(cabinet.get_cycle_count() - HALF_CYCLES_PER_FRAME());
            cabinet.next_interrupt = cabinet.next_interrupt == 0x08 ? 0x10 : 0x08;
        }
    }
}

void Invaders::gpuUpdate() {
    // one byte of VRAM contains data for 8 pixels
    for(size_t i = 0; i < SCREEN_WIDTH() * SCREEN_HEIGHT() / 8; i++) {
        const uint32_t y = i * 8 / SCREEN_HEIGHT();
        const uint32_t base_x = (i * 8) % SCREEN_HEIGHT();
        const uint8_t currentByte = cabinet.memory.read_byte(VRAM_ADDRESS() + i);

        for(uint8_t bit = 0; bit < 8; bit++) {
            uint32_t px = base_x + bit;
            uint32_t py = y;
            const bool isLit = (currentByte >> bit) & 1u;
            uint8_t red = 0, green = 0, blue = 0;
            if(isLit) {
                red = 0; green = 222; blue = 0;
            }

            // screen is rotated 90 degrees counter clockwise so compensate the pixels
            const uint32_t tempX = px;
            px = py;
            py = -tempX + SCREEN_HEIGHT() - 1;

            cabinet.screen_buffer[py][px][0] = red;
            cabinet.screen_buffer[py][px][1] = green;
            cabinet.screen_buffer[py][px][2] = blue;
        }
    }
    updateScreen();
}

void Invaders::updateScreen() {
    const uint32_t pitch = sizeof(uint8_t) * 4 * SCREEN_WIDTH();
    SDL_UpdateTexture(texture, nullptr, &cabinet.screen_buffer, pitch);
}

void Invaders::handleIn() {
    byte port = cabinet.cpu.next_byte();
    switch(port) {
        case 1:
            cabinet.set_accumulator(cabinet.port1);
            break;
        case 2:
            cabinet.set_accumulator(cabinet.port2);
            break;
        case 3: {
            uint16_t shiftVal = cabinet.shift1 << 8u | cabinet.shift0;
            cabinet.set_accumulator((shiftVal >> (8 - cabinet.shiftOffset)) & 0xff);
            break;
        }
        default:
            break;
    }
}

void Invaders::handleOut() {
    uint8_t port = cabinet.memory.read_byte(cabinet.get_program_counter());
    cabinet.increment_program_counter(1);
    uint8_t value = cabinet.get_accumulator();

    switch(port) {
        case 2:
            cabinet.shiftOffset = value & 0x07;
            break;
        case 3: // this one is for sound, which I have not done any implementation for
            break;
        case 4:
            cabinet.shift0 = cabinet.shift1;
            cabinet.shift1 = value;
        case 5: // this one is for sound
        case 6: // watchdog?
        default:
            break;
    }
}

void Invaders::handleKeyUp(SDL_Scancode keyCode) {
    switch (keyCode) {
        case SDL_SCANCODE_ESCAPE:
            shouldQuit = true;
        case SDL_SCANCODE_T:       // TILT
            cabinet.port2 &= 0xfb;
            break;
        case SDL_SCANCODE_C:       // COIN
            cabinet.port1 &= 0xfe;
            break;
        case SDL_SCANCODE_A:       // player 1 LEFT
        case SDL_SCANCODE_LEFT:    // player 1 LEFT
            cabinet.port1 &= 0xdf;
            break;
        case SDL_SCANCODE_D:       // player 1 RIGHT
        case SDL_SCANCODE_RIGHT:   // player 1 RIGHT
            cabinet.port1 &= 0xbf;
            break;
        case SDL_SCANCODE_W:       // player 1 FIRE
        case SDL_SCANCODE_SPACE:   // player 1 FIRE
            cabinet.port1 &= 0xef;
            break;
        case SDL_SCANCODE_RETURN:  // START
            cabinet.port1 &= 0xfb;
        default:
            break;
    }
}

void Invaders::handleKeyDown(SDL_Scancode keyCode) {
    switch (keyCode) {
        case SDL_SCANCODE_ESCAPE:
            shouldQuit = true;
        case SDL_SCANCODE_T:       // TILT
            cabinet.port2 |= 0x04;
            break;
        case SDL_SCANCODE_C:       // COIN
            cabinet.port1 |= 0x01;
            break;
        case SDL_SCANCODE_A:       // player 1 LEFT
        case SDL_SCANCODE_LEFT:    // player 1 LEFT
            cabinet.port1 |= 0x20;
            break;
        case SDL_SCANCODE_D:       // player 1 RIGHT
        case SDL_SCANCODE_RIGHT:   // player 1 RIGHT
            cabinet.port1 |= 0x40;
            break;
        case SDL_SCANCODE_W:       // player 1 FIRE
        case SDL_SCANCODE_SPACE:   // player 1 FIRE
            cabinet.port1 |= 0x10;
            break;
        case SDL_SCANCODE_RETURN:  // START
            cabinet.port1 |= 0x04;
        default:
            break;
    }
}

void Invaders::load_rom(std::ifstream &rom_file) {
    cabinet.memory.load_rom(rom_file);
}
