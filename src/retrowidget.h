//
// Created by cpasjuste on 28/11/24.
//

#ifndef RETRO_WIDGET_H
#define RETRO_WIDGET_H

#include "cross2d/c2d.h"
#include "libretro/retroloader.h"

class App;

namespace retropico {
    class RetroWidget final : public c2d::Rectangle {
    public:
        enum class ScaleMode {
            None,
            Fit,
            Full
        };

        explicit RetroWidget(App *app);

        bool loadCore(const std::string &path);

        bool loadRom(const std::string &path);

        void unloadRom();

        void unloadCore();

        App *getApp() const { return p_app; }

        c2d::Texture *getTexture() const { return p_texture; }

        c2d::Audio *getAudio() const { return p_audio; }

        retro_system_av_info getAvInfo() const { return m_av_info; }

        void setAvInfo(const retro_system_av_info &info) { m_av_info = info; }

        void setAvInfo(const retro_system_av_info *info) {
            m_av_info.geometry = info->geometry;
            m_av_info.timing = info->timing;
        }

        void setScaling();

        bool onInput(c2d::Input::Player *players) override;

        void onUpdate() override;

    private:
        App *p_app = nullptr;
        c2d::C2DAudio *p_audio = nullptr;
        c2d::C2DTexture *p_texture = nullptr;
        core_functions_t *p_retro_handle = nullptr;
        retro_game_info m_game_info{};
        retro_system_info m_core_info{};
        retro_system_av_info m_av_info{};
        ScaleMode m_scale_mode = ScaleMode::Fit;
        bool m_loaded = false;
    };
}

#endif //RETRO_WIDGET_H
