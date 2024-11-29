//
// Created by cpasjuste on 08/12/2021.
//

#include "app.h"

using namespace c2d;
using namespace retropico;

//#define TEST_NES 1
#define TEST_SNES 1
//#define TEST_GB 1
//#define TEST_MD 1

#if TEST_NES
#define CORE_PATH "cores/x86_64/nestopia_libretro.so"
#define ROM_PATH "roms/nes/rom.nes"
#elif TEST_SNES
#define CORE_PATH "cores/x86_64/snes9x_libretro.so"
#define ROM_PATH "roms/snes/rom.sfc"
#elif TEST_GB
#define CORE_PATH "cores/x86_64/gambatte_libretro.so"
#define ROM_PATH "roms/gb/rom.gb"
#elif TEST_MD
#define CORE_PATH "cores/x86_64/genesis_plus_gx_libretro.so"
#define ROM_PATH "roms/megadrive/rom.gen"
//#define ROM_PATH "roms/sms/rom.sms"
//#define ROM_PATH "roms/gg/rom.gg"
#endif

App::App(const Vector2f &screenSize) : C2DRenderer(screenSize) {
    // debug
    App::setPrintStats(true);

    // rendering texture
    p_retro_widget = new RetroWidget(this);
    p_retro_widget->loadCore(App::getIo()->getDataPath() + CORE_PATH);
    App::add(p_retro_widget);
    p_retro_widget->loadRom(App::getIo()->getDataPath() + ROM_PATH);
}

// onInput is only called when a key is pressed
bool App::onInput(Input::Player *players) {
    const uint32_t buttons = players[0].buttons;

    // quit app (enter and space on a keyboard)
    if (buttons & Input::Button::Quit) {
        quit = true;
    }

    return C2DRenderer::onInput(players);
}

// onUpdate is called every frames
void App::onUpdate() {
    C2DRenderer::onUpdate();
}
