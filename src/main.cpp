//
// Created by cpasjuste on 08/12/2021.
//

#include <unistd.h>
#include "app.h"

using namespace c2d;

static bool rotate = false;

int main(const int argc, char *argv[]) {
    App *app;

    // parse command line arguments
    int opt;
    std::string core, rom;
    while ((opt = getopt(argc, argv, "c:g:r")) != -1) {
        switch (opt) {
            case 'c':
                core = optarg;
                printf("core: %s\n", optarg);
                break;
            case 'g':
                rom = optarg;
                printf("game: %s\n", optarg);
                break;
            case 'r':
                printf("rotation enabled\n");
                rotate = true;
                break;
            default:
                break;
        }
    }

    // create main app/renderer
    if (rotate) {
        app = new App(Vector2f(240, 320), true);
    } else {
        app = new App(Vector2f(320, 240));
    }

    // load core and rom from command line if requested
    if (!core.empty() && !rom.empty()) {
        if (app->getRetroWidget()->loadCore(core) && app->getRetroWidget()->loadRom(rom)) {
            app->getFiler()->setVisibility(Visibility::Hidden);
            app->getMenu()->setVisibility(Visibility::Hidden);
            app->getRetroWidget()->setVisibility(Visibility::Visible);
        }
    }

    while (!app->quit) {
        // renderer everything
        app->flip();
    }

    // will delete all child's (textures, shapes, text..)
    delete (app);

    return 0;
}
