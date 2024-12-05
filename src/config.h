//
// Created by cpasjuste on 03/12/24.
//

#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>

#define C2D_CONFIG_ID_SYS_PATH 0

namespace retropico {
    class Config {
    public:
        struct Core {
            std::string name;
            std::string display_name;
            std::string path;
            std::string roms_path;
        };

        explicit Config(const std::string &arch, const std::string &systemPath);

        [[nodiscard]] std::string getConfigPath() const { return m_config_path; }

        [[nodiscard]] std::string getSystemPath() const { return m_system_path; }

        [[nodiscard]] std::vector<Core> getCores() const { return m_cores; }

        [[nodiscard]] Core *getCurrentCore() const { return m_current_core; }

        void setCurrentCore(const int coreIndex) {
            m_current_core = &m_cores[coreIndex];
        }

    private:
        std::string m_arch;
        std::string m_config_path;
        std::string m_system_path;
        Core *m_current_core = nullptr;

        std::vector<Core> m_cores = {
            {"GB", "GameBoy", "gambatte_libretro", "gb"},
            {"GBA", "GameBoy Advance", "mgba_libretro", "gba"},
            {"NES", "NES", "nestopia_libretro", "nes"},
            {"SNES", "SNES", "snes9x_libretro", "snes"},
            {"GG", "GameGear", "genesis_plus_gx_libretro", "gg"},
            {"SMS", "Master System", "genesis_plus_gx_libretro", "sms"},
            {"MD", "MegaDrive", "genesis_plus_gx_libretro", "md"},
            {"PCE", "PC-Engine", "mednafen_pce_fast_libretro", "pce"},
            {"LYNX", "Lynx", "handy_libretro", "lynx"},
        };
    };
}

#endif //CONFIG_H
