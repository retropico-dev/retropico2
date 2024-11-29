//
// Created by cpasjuste on 08/12/2021.
//

#include "app.h"

using namespace c2d;

int main() {
    // create main app/renderer
#if RETROPICO
    const auto app = new App(Vector2f(240, 320));
    app->setRotation(90);
    app->setPosition(240, 0);
#else
    const auto app = new App(Vector2f(320, 240));
#endif

    while (!app->quit) {
        // renderer everything
        app->flip();
    }

    // will delete all child's (textures, shapes, text..)
    delete (app);

    return 0;
}
