//
// Created by cpasjuste on 08/12/2021.
//

#include "app.h"

using namespace c2d;
using namespace retropico;

//#define TEST_NES 1
//#define TEST_SNES 1
//#define TEST_GB 1
//#define TEST_MD 1
//#define TEST_PCE 1
//#define TEST_GBA 1
#define TEST_LYNX 1

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
#elif TEST_PCE
#define CORE_PATH "cores/x86_64/mednafen_pce_fast_libretro.so"
#define ROM_PATH "roms/pce/rom.pce"
#elif TEST_GBA
#define CORE_PATH "cores/x86_64/mgba_libretro.so"
#define ROM_PATH "roms/gba/rom.gba"
#elif TEST_LYNX
#define CORE_PATH "cores/x86_64/handy_libretro.so"
#define ROM_PATH "roms/lynx/rom.lnx"
#endif

// for convenience...
static App *s_app;

App::App(const Vector2f &screenSize) : C2DRenderer(screenSize) {
    s_app = this;

#ifndef NDEBUG
    // debug
    App::setPrintStats(true);
#endif

    // rendering texture
    p_retro_widget = new RetroWidget(this);
    p_retro_widget->loadCore(App::getIo()->getDataPath() + CORE_PATH);
    App::add(p_retro_widget);
    p_retro_widget->loadRom(App::getIo()->getDataPath() + ROM_PATH);
}

App *App::Instance() {
    return s_app;
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
