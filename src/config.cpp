//
// Created by cpasjuste on 03/12/24.
//

#include "config.h"

using namespace retropico;

#warning "TODO: implement config load/save"

Config::Config(c2d::Io *io) {
    std::string cores_path;

#ifdef RETROPICO_DEVICE
    io->setDataPath("/usr/share/retropico/");
    m_system_path = "/home/retropico";
    cores_path = "/usr/lib/libretro";
#else
    m_system_path = getenv("HOME");
    m_system_path += "/.retropico";
    cores_path = "/usr/lib/x86_64-linux-gnu/libretro";
#endif

    io->create(m_system_path);

    for (auto &core: m_cores) {
        core.path = cores_path + "/" + core.path;
        core.roms_path = m_system_path + "/roms/" + core.roms_path;
        io->create(core.roms_path);
        printf("%s: path: %s, roms_path: %s\n",
               core.name.c_str(), core.path.c_str(), core.roms_path.c_str());
    }

    m_current_core = &m_cores.at(0);
}
