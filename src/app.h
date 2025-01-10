//
// Created by cpasjuste on 08/12/2021.
//

#ifndef C2D_APP_H
#define C2D_APP_H

#include "cross2d/c2d.h"
#include "config.h"
#include "filer.h"
#include "menu.h"
#include "retrowidget.h"

#define INPUT_DELAY_UI 200

class App final : public c2d::C2DRenderer {
public:
    explicit App(const c2d::Vector2f &screenSize, bool rotate = false);

    static App *Instance();

    retropico::Config *getConfig() const { return p_config; }

    retropico::Filer *getFiler() const { return p_filer; }

    retropico::Menu *getMenu() const { return p_menu; }

    retropico::RetroWidget *getRetroWidget() const { return p_retro_widget; }

    c2d::Vector2f getSize() override;

    c2d::FloatRect getLocalBounds() const override;

    bool quit = false;

private:
    bool onInput(c2d::Input::Player *players) override;

    void onUpdate() override;

    retropico::Config *p_config;
    retropico::Filer *p_filer;
    retropico::Menu *p_menu;
    retropico::RetroWidget *p_retro_widget;
    uint32_t m_buttons_old = 0;
    c2d::C2DClock m_clock;
    bool m_rotate = false;
};

#endif //C2D_APP_H
