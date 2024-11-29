//
// Created by cpasjuste on 08/12/2021.
//

#include "app.h"

using namespace c2d;
using namespace retropico;

#if 1
#define CORE_PATH "cores/x86_64/nestopia_libretro.so"
#define ROM_PATH "roms/nes/rom.nes"
#else
#define CORE_PATH "cores/x86_64/gambatte_libretro.so"
#define ROM_PATH "roms/gb/rom.gb"
#endif

App::App(const Vector2f &screenSize) : C2DRenderer(screenSize) {
    // debug
    App::setPrintStats(true);

    // rendering texture
    p_retro_widget = new RetroWidget(this);
    p_retro_widget->loadCore(App::getIo()->getRomFsPath() + CORE_PATH);
    App::add(p_retro_widget);
    p_retro_widget->loadRom(App::getIo()->getRomFsPath() + ROM_PATH);
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
