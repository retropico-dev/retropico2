//
// Created by cpasjuste on 08/12/2021.
//

#include <unistd.h>
#include "app.h"

using namespace c2d;

int main(const int argc, char *argv[]) {
    // parse command line arguments
    int opt;
    std::string core, rom;
    while ((opt = getopt(argc, argv, "c:r:")) != -1) {
        switch (opt) {
            case 'c':
                core = optarg;
                printf("core: %s\n", optarg);
                break;
            case 'r':
                rom = optarg;
                printf("rom: %s\n", optarg);
                break;
            default:
                break;
        }
    }

    // create main app/renderer
#if RETROPICO_DEVICE
    const auto app = new App(Vector2f(240, 320));
    app->setRotation(90);
    app->setPosition(240, 0);
#else
    const auto app = new App(Vector2f(320, 240));
#endif

    // load core and rom from command line if requested
    if (!core.empty() && !rom.empty()) {
        if (app->getRetroWidget()->loadCore(core)) {
            if (app->getRetroWidget()->loadRom(rom)) {
                app->getFiler()->setVisibility(Visibility::Hidden);
                app->getMenu()->setVisibility(Visibility::Hidden);
                app->getRetroWidget()->setVisibility(Visibility::Visible);
            }
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
