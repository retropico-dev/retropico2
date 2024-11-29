//
// Created by cpasjuste on 29/11/24.
//

#include <zip.h>
#include "app.h"
#include "utility.h"

using namespace retropico;

bool Utility::Unzip(const std::string &src, const std::string &dst) {
    if (!App::Instance()->getIo()->exist(src)) {
        printf("Utility::Unzip: cannot open file '%s'\n", src.c_str());
        return false;
    }

    zip_t *za;
    int err = 0;
    if ((za = zip_open(src.c_str(), 0, &err)) == nullptr) {
        zip_error_t error;
        zip_error_init_with_code(&error, err);
        printf("Utility::Unzip: cannot open zip archive '%s': %s\n",
               src.c_str(), zip_error_strerror(&error));
        zip_error_fini(&error);
        return false;
    }

    const auto count = zip_get_num_entries(za, 0);
    if (count <= 0) {
        printf("Utility::Unzip: cannot find any file in zip archive '%s'\n", src.c_str());
        zip_close(za);
        return false;
    }

    // extract first file only
    struct zip_stat st{};
    zip_stat_init(&st);
    if (zip_stat_index(za, 0, 0, &st) != 0) {
        printf("Utility::Unzip: cannot stat file in zip archive '%s'\n", src.c_str());
        zip_close(za);
        return false;
    }

    // read to buffer
    const auto data = new char[st.size];
    zip_file *f = zip_fopen_index(za, 0, 0);
    zip_fread(f, data, st.size);
    zip_fclose(f);
    zip_close(za);

    // write to file
    App::Instance()->getIo()->write(dst, data, st.size);

    delete[] data;

    return true;
}
