//
// Created by cpasjuste on 28/11/24.
//

#include "app.h"
#include "retrowidget.h"

#include <unistd.h>

#include "utility/utility.h"

using namespace c2d;
using namespace retropico;

static RetroWidget *s_retro_widget;
static char s_syspath[PATH_MAX];
static char s_savepath[PATH_MAX];
static retro_pixel_format video_fmt = RETRO_PIXEL_FORMAT_RGB565;
static std::unordered_map<std::string, std::string> env_vars = {
    {"mgba_use_bios", "1"}
    //{"nestopia_audio_type", ""}
};

const std::unordered_map<unsigned, unsigned> buttons_map = {
    {RETRO_DEVICE_ID_JOYPAD_START, Input::Button::Start},
    {RETRO_DEVICE_ID_JOYPAD_SELECT, Input::Button::Select},
    {RETRO_DEVICE_ID_JOYPAD_A, Input::Button::A},
    {RETRO_DEVICE_ID_JOYPAD_B, Input::Button::B},
    {RETRO_DEVICE_ID_JOYPAD_X, Input::Button::X},
    {RETRO_DEVICE_ID_JOYPAD_Y, Input::Button::Y},
    {RETRO_DEVICE_ID_JOYPAD_UP, Input::Button::Up},
    {RETRO_DEVICE_ID_JOYPAD_DOWN, Input::Button::Down},
    {RETRO_DEVICE_ID_JOYPAD_LEFT, Input::Button::Left},
    {RETRO_DEVICE_ID_JOYPAD_RIGHT, Input::Button::Right},
    {RETRO_DEVICE_ID_JOYPAD_L, Input::Button::LT},
    {RETRO_DEVICE_ID_JOYPAD_R, Input::Button::RT},
};

void RETRO_CALLCONV logging_callback(retro_log_level level, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

bool RETRO_CALLCONV env_callback(unsigned cmd, void *data) {
    const auto r_vars = static_cast<retro_variable *>(data);

    switch (cmd) {
        case RETRO_ENVIRONMENT_GET_CAN_DUPE:
            printf("RETRO_ENVIRONMENT_GET_CAN_DUPE\n");
            strcpy(static_cast<char *>(data), "1");
            return true;
        case RETRO_ENVIRONMENT_SET_PIXEL_FORMAT:
            video_fmt = *static_cast<retro_pixel_format *>(data);
            printf("RETRO_ENVIRONMENT_SET_PIXEL_FORMAT: %i\n", video_fmt);
            return true;
        case RETRO_ENVIRONMENT_SET_GEOMETRY:
        case RETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO: {
            const auto av = static_cast<retro_system_av_info *>(data);
            s_retro_widget->setAvInfo(av);
            printf("RETRO_ENVIRONMENT_SET_GEOMETRY: base: %ix%i, max: %ix%i, ratio: %f, fps: %f, rate: %f\n",
                   av->geometry.base_width, av->geometry.base_height,
                   av->geometry.max_width, av->geometry.max_height,
                   av->geometry.aspect_ratio, av->timing.fps, av->timing.sample_rate);
            return true;
        }
        case RETRO_ENVIRONMENT_GET_VARIABLE:
            printf("RETRO_ENVIRONMENT_GET_VARIABLE: %s\n", r_vars->key);
            if (env_vars.count(r_vars->key)) {
                r_vars->value = env_vars.at(r_vars->key).c_str();
                printf("RETRO_ENVIRONMENT_GET_VARIABLE: %s => %s\n", r_vars->key, r_vars->value);
                return true;
            }
            return false;
        case RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY: {
            strncpy(s_syspath, (s_retro_widget->getApp()->getConfig()->getSystemPath()
                                + "/system").c_str(), PATH_MAX - 1);
            s_retro_widget->getApp()->getIo()->create(s_syspath);
            *static_cast<const char **>(data) = s_syspath;
            printf("RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY: %s\n", s_syspath);
            return true;
        }
        case RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY: {
            *static_cast<const char **>(data) = s_savepath;
            printf("RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY: %s\n", s_savepath);
            return true;
        }
        case RETRO_ENVIRONMENT_SET_MINIMUM_AUDIO_LATENCY:
            return true;
        case RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE:
            //printf("RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE\n");
            return false;
        case RETRO_ENVIRONMENT_GET_LOG_INTERFACE:
            static_cast<retro_log_callback *>(data)->log = &logging_callback;
            printf("RETRO_ENVIRONMENT_GET_LOG_INTERFACE\n");
            return true;
        case RETRO_ENVIRONMENT_GET_MESSAGE_INTERFACE_VERSION:
            printf("RETRO_ENVIRONMENT_GET_MESSAGE_INTERFACE_VERSION\n");
            *static_cast<unsigned *>(data) = 1;
            return true;
        case RETRO_ENVIRONMENT_SET_MESSAGE:
        case RETRO_ENVIRONMENT_SET_MESSAGE_EXT:
            printf("[CORE] %s\n", static_cast<retro_message *>(data)->msg);
            return true;
        default:
            return false;
    }
}

void RETRO_CALLCONV video_update(const void *data, const unsigned width, const unsigned height, const size_t pitch) {
    if (!data) return;

    const auto tex = s_retro_widget->getTexture();
    if (!tex) return;

    const auto rect = tex->getTextureRect();
    if (rect.width != static_cast<int>(width) || rect.height != static_cast<int>(height)) {
        printf("video_update: buffer: %ix%i, pitch: %lu\n", width, height, pitch);
        tex->setUnpackRowLength(static_cast<int>(pitch) / tex->m_bpp);
        tex->setTextureRect(IntRect{0, 0, static_cast<int>(width), static_cast<int>(height)});
        tex->setSize(static_cast<float>(width), static_cast<float>(height));
        s_retro_widget->setScaling();

        printf("tex: %.0fx%.0f, scale: %.2fx%.2f\n", tex->getSize().x, tex->getSize().y,
               tex->getScale().x, tex->getScale().y);
    }

    tex->unlock(static_cast<const uint8_t *>(data));
}

void RETRO_CALLCONV single_sample(int16_t left, int16_t right) {
    // TODO ?
}

size_t RETRO_CALLCONV audio_buffer(const int16_t *data, const size_t frames) {
    //printf("audio_buffer: %p, frames: %lu\n", data, frames);
    s_retro_widget->getAudio()->play(data, static_cast<int>(frames), Audio::SyncMode::LowLatency);
    return frames;
}

void RETRO_CALLCONV input_poll() {
    // TODO ?
}

int16_t RETRO_CALLCONV input_state(unsigned port, unsigned device, unsigned index, unsigned id) {
    if (port != 0 || !buttons_map.count(id)) return 0;

    const auto buttons = s_retro_widget->getApp()->getInput()->getButtons();
    return static_cast<int16_t>(buttons & buttons_map.at(id));
}

RetroWidget::RetroWidget(App *app) : Rectangle(app->getSize()) {
    s_retro_widget = this;
    p_app = app;
}

bool RetroWidget::loadCore(const std::string &path) {
    printf("RetroWidget::loadCore: %s\n", path.c_str());

    if (m_core_path == path) {
        // assume core is already loaded...
        printf("RetroWidget::loadCore: already loaded...\n");
        return true;
    }

    if (!p_app->getIo()->exist(path)) {
        printf("RetroWidget::loadCore: failed to load core (file not found: %s)\n", path.c_str());
        return false;
    }

    p_retro_handle = load_core(path.c_str());
    if (!p_retro_handle) {
        printf("RetroWidget::loadCore: failed to load core (could not get an handle)\n");
        return false;
    }

    // get core info
    p_retro_handle->retro_get_info(&m_core_info);
    printf("RetroWidget::loadCore: loaded %s version %s (need_fullpath: %i)\n",
           m_core_info.library_name, m_core_info.library_version, m_core_info.need_fullpath);

    // set callbacks
    p_retro_handle->retro_set_env_function(&env_callback);
    p_retro_handle->retro_set_video_refresh_function(&video_update);
    p_retro_handle->retro_set_audio_sample_function(&single_sample);
    p_retro_handle->retro_set_audio_sample_batch_function(&audio_buffer);
    p_retro_handle->retro_set_input_poll_function(&input_poll);
    p_retro_handle->retro_set_input_state_function(&input_state);

    // init core
    p_retro_handle->retro_init();
    m_core_path = path;

    // set save (sram) path
    strncpy(s_savepath, (p_app->getConfig()->getSystemPath()
                         + "/system/" + m_core_info.library_name + "/").c_str(),PATH_MAX - 1);
    p_app->getIo()->create(s_savepath);

    return true;
}

bool RetroWidget::loadRom(const std::string &path) {
    printf("RetroWidget::loadRom: %s\n", path.c_str());

    if (!p_app->getIo()->exist(path)) {
        printf("RetroWidget::loadRom: file not found: %s\n", path.c_str());
        return false;
    }

    if (!p_retro_handle) {
        printf("RetroWidget::loadRom: no core loaded...\n");
        return false;
    }

    // load rom file in memory
    m_game_info = {.path = path.c_str(), .data = nullptr, .size = 0, .meta = nullptr};
    if (!m_core_info.need_fullpath) {
        const size_t size = p_app->getIo()->getSize(path);
        m_game_info.data = malloc(size);
        m_game_info.size = p_app->getIo()->read(path, (char *) m_game_info.data);
        if (!m_game_info.size) {
            if (m_game_info.data) {
                free(const_cast<void *>(m_game_info.data));
                m_game_info.data = nullptr;
            }
            printf("RetroWidget::loadRom: could not read file: %s\n", path.c_str());
            return false;
        }
    }

    if (!p_retro_handle->retro_load_game(&m_game_info)) {
        printf("RetroWidget::loadRom: retro_load_game failed...\n");
        if (m_game_info.data) {
            free((void *) m_game_info.data);
            m_game_info.data = nullptr;
            m_game_info.size = 0;
        }
        return false;
    }

    // set game/rom name
    m_rom_name = c2d::Utility::baseName(m_game_info.path);
    m_rom_name = c2d::Utility::removeExt(m_rom_name);

    // load sram
    loadSram(RETRO_MEMORY_SAVE_RAM);
    loadSram(RETRO_MEMORY_RTC);

    //p_retro_handle->retro_reset();

    p_retro_handle->retro_get_system_av_info(&m_av_info);
    if (m_av_info.geometry.aspect_ratio == 0.0f) {
        m_av_info.geometry.aspect_ratio =
                static_cast<float>(m_av_info.geometry.base_width) /
                static_cast<float>(m_av_info.geometry.base_height);
    }
    printf("RetroWidget::loadRom: base: %ix%i, max: %ix%i, ratio: %f, fps: %f, rate: %f\n",
           m_av_info.geometry.base_width, m_av_info.geometry.base_height,
           m_av_info.geometry.max_width, m_av_info.geometry.max_height,
           m_av_info.geometry.aspect_ratio, m_av_info.timing.fps, m_av_info.timing.sample_rate);

    // setup render texture
#if defined(__GLES2__) && !defined(__GLES3__)
    const auto format = video_fmt == RETRO_PIXEL_FORMAT_RGB565 ? Texture::Format::RGB565 : Texture::Format::RGBA8;
#else
    const auto format = video_fmt == RETRO_PIXEL_FORMAT_RGB565 ? Texture::Format::RGB565 : Texture::Format::XBGR8;
#endif
    const auto w = c2d::Utility::pow2(static_cast<int>(m_av_info.geometry.max_width));
    const auto h = c2d::Utility::pow2(static_cast<int>(m_av_info.geometry.max_height));
    delete p_texture;
    p_texture = new C2DTexture(Vector2i(w, h), format);
#if defined(__GLES2__) && !defined(__GLES3__)
    if (format == Texture::Format::RGBA8) p_texture->setShader("libretro_xrgb_shader");
#endif
    p_texture->setFilter(Texture::Filter::Linear);
    p_texture->setOrigin(Origin::Center);
    p_texture->setPosition(m_size.x / 2, m_size.y / 2);
    RetroWidget::add(p_texture);

    // setup audio
    delete p_audio;
    p_audio = new C2DAudio(static_cast<int>(m_av_info.timing.sample_rate),
                           Audio::toSamples(static_cast<int>(m_av_info.timing.sample_rate),
                                            static_cast<float>(m_av_info.timing.fps)));
    p_audio->pause(0);

    // set loaded state
    m_loaded = true;

    return true;
}

void RetroWidget::unloadRom() {
    printf("RetroWidget::unloadRom\n");

    if (!p_retro_handle) return;

    // save sram if any
    saveSram(RETRO_MEMORY_SAVE_RAM);
    saveSram(RETRO_MEMORY_RTC);

    p_retro_handle->retro_unload_game();
    if (m_game_info.data) {
        free(const_cast<void *>(m_game_info.data));
        m_game_info.data = nullptr;
        m_game_info.size = 0;
    }

    // set loaded state
    m_loaded = false;
}

void RetroWidget::unloadCore() {
    printf("RetroWidget::unloadCore\n");

    if (!p_retro_handle) return;

    unload_core(p_retro_handle);
    p_retro_handle = nullptr;

    // set loaded state
    m_loaded = false;
    m_core_path.clear();
}

void RetroWidget::saveSram(uint32_t type) const {
    if (!p_retro_handle) return;

    const std::string path = s_savepath + m_rom_name + (type == RETRO_MEMORY_SAVE_RAM ? ".sav" : ".rtc");
    printf("RetroWidget::loadSram: path: %s\n", path.c_str());

    const auto size = p_retro_handle->retro_get_memory_size(type);
    if (!size) {
        printf("RetroWidget::saveSram: retro_get_memory_size failed\n");
        return;
    }

    const auto sram = p_retro_handle->retro_get_memory_data(type);
    if (!sram) {
        printf("RetroWidget::saveSram: retro_get_memory_data failed\n");
        return;
    }

    if (!p_app->getIo()->write(path, static_cast<const char *>(sram), size)) {
        printf("RetroWidget::saveSram: write failed\n");
    }
}

void RetroWidget::loadSram(uint32_t type) const {
    if (!p_retro_handle) return;

    const std::string path = s_savepath + m_rom_name + (type == RETRO_MEMORY_SAVE_RAM ? ".sav" : ".rtc");
    printf("RetroWidget::loadSram: %s\n", path.c_str());

    const auto size = p_retro_handle->retro_get_memory_size(type);
    if (!size) {
        printf("RetroWidget::loadSram: retro_get_memory_size failed\n");
        return;
    }

    const auto sram = p_retro_handle->retro_get_memory_data(type);
    if (!sram) {
        printf("RetroWidget::loadSram: retro_get_memory_data failed\n");
        return;
    }

    if (!p_app->getIo()->read(path, static_cast<char *>(sram), size)) {
        printf("RetroWidget::loadSram: read failed\n");
    }
}

void RetroWidget::setScaling() {
    const Vector2f tex_size = getTexture()->getSize();
    const Vector2f screen = getSize();
    Vector2f scale = {1, 1};
    const Vector2f scale_max = {
        screen.x / p_texture->getSize().x,
        screen.y / p_texture->getSize().y
    };

    if (m_scale_mode == ScaleMode::Full) {
        scale.x = scale_max.x;
        scale.y = scale_max.y;
    } else if (m_scale_mode == ScaleMode::Fit) {
        const float aspect_ratio = m_av_info.geometry.aspect_ratio;
        const float scale_y = scale_max.y;
        const float scale_x = (tex_size.y * scale_y * aspect_ratio) / tex_size.x;
        if (scale_x > scale_max.x) {
            scale.x = scale_max.x;
            scale.y = (tex_size.x * scale.x) / (tex_size.y * aspect_ratio);
        } else {
            scale.x = scale_x;
            scale.y = scale_y;
        }
    }

#ifndef NDEBUG
    printf("RetroWidget::setScaling: %ix%i => %ix%i (%.2fx%.2f, ratio: %.2f)\n",
           (int) tex_size.x, (int) tex_size.y, (int) (tex_size.x * scale.x), (int) (tex_size.y * scale.y),
           scale.x, scale.y, m_av_info.geometry.aspect_ratio);
    p_texture->setScale(scale);
#endif
}

bool RetroWidget::onInput(Input::Player *players) {
    if (App::Instance()->getMenu()->isVisible()) {
        return false;
    }

    const auto buttons = players->buttons;
    if (buttons & Input::Button::Start && buttons & Input::Button::Select) {
        App::Instance()->getInput()->setRepeatDelay(INPUT_DELAY_UI);
        App::Instance()->getInput()->clear();
        App::Instance()->getFiler()->setVisibility(Visibility::Visible);
        return true;
    }

    return Rectangle::onInput(players);
}

void RetroWidget::onUpdate() {
    if (!App::Instance()->getFiler()->isVisible()
        && !App::Instance()->getMenu()->isVisible() && p_retro_handle && m_loaded) {
        p_retro_handle->retro_run();
    }

    Rectangle::onUpdate();
}
