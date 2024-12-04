//
// Created by cpasjuste on 03/12/24.
//

#include "config.h"

using namespace retropico;

Config::Config(const std::string &arch, const std::string &systemPath) {
#warning "TODO: implement config load/save"

    m_arch = arch;
    m_system_path = systemPath;
    m_config_path = systemPath + "/config.json";

    for (auto &core: m_cores) {
        core.path = m_system_path + "/cores/" + m_arch + "/" + core.path + ".so";
        core.roms_path = m_system_path + "/roms/" + core.roms_path;
        printf("%s: path: %s, roms_path: %s\n",
               core.name.c_str(), core.path.c_str(), core.roms_path.c_str());
    }

    m_current_core = &m_cores.at(0);
}
