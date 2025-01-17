//
// Created by cpasjuste on 08/12/2021.
//

#include "app.h"

using namespace c2d;
using namespace retropico;

// for convenience...
static App *s_app;

#if defined(__GLES2__) && !defined(__GLES3__)
extern const char *libretro_xrgb_shader;
#endif

App::App(const Vector2f &screenSize, const bool rotate) : C2DRenderer(screenSize) {
    s_app = this;
    m_rotate = rotate;

    if (m_rotate) {
        App::setRotation(90);
        App::setPosition(screenSize.x, 0);
    }

#ifndef NDEBUG
    // debug
    App::setPrintStats(true);
#endif

    // load config
    p_config = new Config(App::getIo());

    // add gles2 custom "xrgb" shader
#if defined(__GLES2__) && !defined(__GLES3__)
    const auto shader = new GLShader("libretro_xrgb_shader", libretro_xrgb_shader, 0, "100");
    App::getShaderList()->add(shader);
#endif

    // custom fonts
    p_font = new C2DFont();
    p_font->loadFromFile(App::getIo()->getDataPath() + "res/fonts/default.ttf");
    App::setFont(p_font);

    // rendering texture
    p_retro_widget = new RetroWidget(this);
    p_retro_widget->setVisibility(Visibility::Hidden);
    App::add(p_retro_widget);

    p_filer = new Filer();
    p_filer->load();
    App::add(p_filer);

    const auto bounds = App::getLocalBounds();
    p_menu = new Menu({
        bounds.left + bounds.width + 1, bounds.height / 2,
        bounds.width * 0.32f, bounds.height * 0.8f
    });
    p_menu->setOrigin(Origin::Right);
    p_menu->setVisibility(Visibility::Hidden);
    App::add(p_menu);

    // set default joystick mapping
    const std::vector<Input::ButtonMapping> mapping = {
        {Input::Button::Up, KEY_JOY_UP_DEFAULT},
        {Input::Button::Down, KEY_JOY_DOWN_DEFAULT},
        {Input::Button::Left, KEY_JOY_LEFT_DEFAULT},
        {Input::Button::Right, KEY_JOY_RIGHT_DEFAULT},
        {Input::Button::Select, KEY_JOY_SELECT_DEFAULT},
        {Input::Button::Start, KEY_JOY_START_DEFAULT},
        {Input::Button::A, KEY_JOY_A_DEFAULT},
        {Input::Button::B, KEY_JOY_B_DEFAULT},
        {Input::Button::X, KEY_JOY_X_DEFAULT},
        {Input::Button::Y, KEY_JOY_Y_DEFAULT},
        {Input::Button::LT, KEY_JOY_LT_DEFAULT},
        {Input::Button::RT, KEY_JOY_RT_DEFAULT},
        {Input::Button::LB, -1},
        {Input::Button::RB, -1},
        {Input::Button::LS, -1},
        {Input::Button::RS, -1},
        {Input::Button::Menu1, KEY_JOY_MENU1_DEFAULT},
        {Input::Button::Menu2, -1}
    };

    App::getInput()->setJoystickMapping(
        0, mapping,
        {KEY_JOY_AXIS_LX, KEY_JOY_AXIS_LY},
        {KEY_JOY_AXIS_RX, KEY_JOY_AXIS_RY}, 8000);

#ifndef NO_KEYBOARD
    // set default keyboard mapping
    const std::vector<Input::ButtonMapping> kb_mapping = {
        {Input::Button::Up, KEY_KB_UP_DEFAULT},
        {Input::Button::Down, KEY_KB_DOWN_DEFAULT},
        {Input::Button::Left, KEY_KB_LEFT_DEFAULT},
        {Input::Button::Right, KEY_KB_RIGHT_DEFAULT},
        {Input::Button::Select, KEY_KB_SELECT_DEFAULT},
        {Input::Button::Start, KEY_KB_START_DEFAULT},
        {Input::Button::A, KEY_KB_A_DEFAULT},
        {Input::Button::B, KEY_KB_B_DEFAULT},
        {Input::Button::X, KEY_KB_X_DEFAULT},
        {Input::Button::Y, KEY_KB_Y_DEFAULT},
        {Input::Button::LT, KEY_KB_LT_DEFAULT},
        {Input::Button::RT, KEY_KB_RT_DEFAULT},
        {Input::Button::LB, KEY_KB_LB_DEFAULT},
        {Input::Button::RB, KEY_KB_RB_DEFAULT},
        {Input::Button::LS, KEY_KB_LS_DEFAULT},
        {Input::Button::RS, KEY_KB_RS_DEFAULT},
        {Input::Button::Menu1, SDL_SCANCODE_ESCAPE},
        {Input::Button::Menu2, -1}
    };

    App::getInput()->setKeyboardMapping(kb_mapping);
#endif

    App::getInput()->setRepeatDelay(INPUT_DELAY_UI);
}

App *App::Instance() {
    return s_app;
}

Vector2f App::getSize() {
    return m_rotate ? Vector2f{320, 240} : Renderer::getSize();
}

FloatRect App::getLocalBounds() const {
    return m_rotate
               ? FloatRect{0.f, 0.f, m_size.y, m_size.x}
               : SDL2Renderer::getLocalBounds();
}

// onInput is only called when a key is pressed
bool App::onInput(Input::Player *players) {
    const uint32_t buttons = players[0].buttons;

    // quit app (enter and space on a keyboard)
    if (buttons & Input::Button::Quit) {
        quit = true;
    }

    return Renderer::onInput(players);
}

// onUpdate is called every frames
void App::onUpdate() {
    if (p_retro_widget->isVisible() && !p_menu->isVisible()) {
        return Renderer::onUpdate();
    }

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

    Renderer::onUpdate();
}
