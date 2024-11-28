//
// Created by cpasjuste on 08/12/2021.
//

#ifndef C2D_APP_H
#define C2D_APP_H

#include "cross2d/c2d.h"
#include "widgets/retrowidget.h"

class App final : public c2d::C2DRenderer {
public:
    explicit App(const c2d::Vector2f &screenSize);

    bool quit = false;

private:
    bool onInput(c2d::Input::Player *players) override;

    void onUpdate() override;

    retropico::RetroWidget *p_retro_widget;
};

#endif //C2D_APP_H
