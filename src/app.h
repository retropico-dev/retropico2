//
// Created by cpasjuste on 08/12/2021.
//

#ifndef C2D_APP_H
#define C2D_APP_H

#include "cross2d/c2d.h"
#include "config.h"
#include "filer.h"
#include "widgets/retrowidget.h"

#define INPUT_DELAY_UI 200

class App final : public c2d::C2DRenderer {
public:
    explicit App(const c2d::Vector2f &screenSize);

    static App *Instance();

    retropico::Config *getConfig() const { return p_config; }

    retropico::Filer *getFiler() const { return p_filer; }

    retropico::RetroWidget *getRetroWidget() const { return p_retro_widget; }

    bool quit = false;

private:
    bool onInput(c2d::Input::Player *players) override;

    void onUpdate() override;

    retropico::Config *p_config;
    retropico::Filer *p_filer;
    retropico::RetroWidget *p_retro_widget;
    uint32_t m_buttons_old = 0;
    c2d::C2DClock m_clock;
};

#endif //C2D_APP_H
