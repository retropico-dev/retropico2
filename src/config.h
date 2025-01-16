//
// Created by cpasjuste on 03/12/24.
//

#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>
#include "cross2d/skeleton/io.h"

namespace retropico {
    class Config {
    public:
        struct Core {
            std::string name;
            std::string display_name;
            std::string path;
            std::string roms_path;
        };

        explicit Config(c2d::Io *io);

        [[nodiscard]] std::string getSystemPath() const { return m_system_path; }

        [[nodiscard]] std::string getConfigPath() const { return m_system_path + "/config.json"; }

        [[nodiscard]] std::vector<Core> getCores() const { return m_cores; }

        [[nodiscard]] Core *getCurrentCore() const { return m_current_core; }

        void setCurrentCore(const int coreIndex) {
            m_current_core = &m_cores[coreIndex];
        }

    private:
        std::string m_system_path;
        Core *m_current_core = nullptr;

        std::vector<Core> m_cores = {
            {"GB", "GameBoy", "gambatte_libretro.so", "gb"},
            {"GBA", "GameBoy Advance", "mgba_libretro.so", "gba"},
            {"NES", "NES", "nestopia_libretro.so", "nes"},
            {"SNES", "SNES", "snes9x_libretro.so", "snes"},
            {"GG", "GameGear", "genesis_plus_gx_libretro.so", "gg"},
            {"SMS", "Master System", "genesis_plus_gx_libretro.so", "sms"},
            {"MD", "MegaDrive", "genesis_plus_gx_libretro.so", "md"},
            {"PCE", "PC-Engine", "mednafen_pce_libretro.so", "pce"},
            {"LYNX", "Lynx", "handy_libretro.so", "lynx"},
        };
    };
}

#endif //CONFIG_H
