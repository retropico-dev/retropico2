//
// Created by cpasjuste on 08/12/2021.
//

#include "app.h"

using namespace c2d;

int main() {
    // create main app/renderer
    const auto app = new App(Vector2f(240, 320));

    while (!app->quit) {
        // renderer everything
        app->flip();
    }

    // will delete all child's (textures, shapes, text..)
    delete (app);

    return 0;
}