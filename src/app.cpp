//
// Created by cpasjuste on 08/12/2021.
//

#include "app.h"

using namespace c2d;
using namespace retropico;

// for convenience...
static App *s_app;

App::App(const Vector2f &screenSize) : C2DRenderer(screenSize) {
    s_app = this;

#ifndef NDEBUG
    // debug
    App::setPrintStats(true);
#endif

    p_config = new Config("x86_64", App::getIo()->getDataPath());

    p_filer = new Filer();
    p_filer->load();
    App::add(p_filer);

    // rendering texture
    p_retro_widget = new RetroWidget(this);
    p_retro_widget->setVisibility(Visibility::Hidden);
    App::add(p_retro_widget);
    //p_retro_widget->loadCore(App::getIo()->getDataPath() + CORE_PATH);
    //p_retro_widget->loadRom(App::getIo()->getDataPath() + ROM_PATH);
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
    // handle auto-repeat speed
    const auto buttons = getInput()->getButtons();
    if (buttons != Input::Button::Delay) {
        const bool changed = (m_buttons_old ^ buttons) != 0;
        m_buttons_old = buttons;
        if (!changed) {
            if (m_clock.getElapsedTime().asSeconds() > 3) {
                getInput()->setRepeatDelay(INPUT_DELAY_UI / 20);
            } else if (m_clock.getElapsedTime().asSeconds() > 2) {
                getInput()->setRepeatDelay(INPUT_DELAY_UI / 8);
            } else if (m_clock.getElapsedTime().asSeconds() > 1) {
                getInput()->setRepeatDelay(INPUT_DELAY_UI / 4);
            }
        } else {
            getInput()->setRepeatDelay(INPUT_DELAY_UI);
            m_clock.restart();
        }
    }

    C2DRenderer::onUpdate();
}
