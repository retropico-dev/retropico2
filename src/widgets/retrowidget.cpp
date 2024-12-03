//
// Created by cpasjuste on 28/11/24.
//

#include "app.h"
#include "retrowidget.h"
#include "utility/utility.h"

using namespace c2d;
using namespace retropico;

RetroWidget *s_retro_widget;
retro_pixel_format video_fmt = RETRO_PIXEL_FORMAT_RGB565;
std::unordered_map<std::string, std::string> env_vars = {
    //{"nestopia_audio_type", ""}
};

const std::unordered_map<std::string, unsigned> buttons_map = {
    {"start", RETRO_DEVICE_ID_JOYPAD_START},
    {"select", RETRO_DEVICE_ID_JOYPAD_SELECT},
    {"a", RETRO_DEVICE_ID_JOYPAD_A},
    {"b", RETRO_DEVICE_ID_JOYPAD_B},
    {"x", RETRO_DEVICE_ID_JOYPAD_X},
    {"y", RETRO_DEVICE_ID_JOYPAD_Y},
    {"up", RETRO_DEVICE_ID_JOYPAD_UP},
    {"down", RETRO_DEVICE_ID_JOYPAD_DOWN},
    {"left", RETRO_DEVICE_ID_JOYPAD_LEFT},
    {"right", RETRO_DEVICE_ID_JOYPAD_RIGHT},
    {"l", RETRO_DEVICE_ID_JOYPAD_L},
    {"r", RETRO_DEVICE_ID_JOYPAD_R},
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
        case RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY:
        case RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY:
            printf("RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY\n");
            *static_cast<const char **>(data) = "./retrosys";
            return true;
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
            printf("[Core message] %s\n", static_cast<retro_message *>(data)->msg);
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

void RETRO_CALLCONV input_poll() {
}

void RETRO_CALLCONV single_sample(int16_t left, int16_t right) {
    //printf("single_sample\n");
}

size_t RETRO_CALLCONV audio_buffer(const int16_t *data, size_t frames) {
    //printf("audio_buffer: %p, frames: %lu\n", data, frames);
    s_retro_widget->getAudio()->play(data, static_cast<int>(frames), Audio::SyncMode::LowLatency);
    return frames;
}

int16_t RETRO_CALLCONV input_state(unsigned port, unsigned device, unsigned index, unsigned id) {
    if (port != 0) return 0;

    uint32_t buttons = s_retro_widget->getApp()->getInput()->getButtons();

    switch (id) {
        case 2:
            return buttons & Input::Button::Select ? static_cast<short>(1) : static_cast<short>(0);
        case 3:
            return buttons & Input::Button::Start ? static_cast<short>(1) : static_cast<short>(0);
        case 0: // B
            return buttons & Input::Button::B ? static_cast<short>(1) : static_cast<short>(0);
        case 8: // A
            return buttons & Input::Button::A ? static_cast<short>(1) : static_cast<short>(0);
        default:
            return 0;
    }
}

RetroWidget::RetroWidget(App *app) : Rectangle(app->getSize()) {
    p_app = app;
    s_retro_widget = this;
}

bool RetroWidget::loadCore(const std::string &path) {
    if (!p_app->getIo()->exist(path)) {
        // check if zipped core exists
        const std::string zipped = c2d::Utility::removeExt(path) + ".zip";
        if (!p_app->getIo()->exist(zipped)) {
            printf("RetroWidget::loadCore: failed to load core (file not found: %s)\n", path.c_str());
            return false;
        }

        printf("RetroWidget::loadCore: found zipped core, extracting... (%s)\n", zipped.c_str());
        if (!Utility::Unzip(zipped, path)) {
            printf("RetroWidget::loadCore: failed to load core (unzip failed: %s)\n", path.c_str());
            return false;
        }
    }

    p_retro_handle = load_core(path.c_str());
    if (!p_retro_handle) {
        printf("RetroWidget::loadCore: failed to load core (could not get an handle)\n");
        return false;
    }

    // get core info
    p_retro_handle->core_get_info(&m_core_info);
    printf("RetroWidget::loadCore: loaded %s version %s (need_fullpath: %i)\n",
           m_core_info.library_name, m_core_info.library_version, m_core_info.need_fullpath);

    // set callbacks
    p_retro_handle->core_set_env_function(&env_callback);
    p_retro_handle->core_set_video_refresh_function(&video_update);
    p_retro_handle->core_set_audio_sample_function(&single_sample);
    p_retro_handle->core_set_audio_sample_batch_function(&audio_buffer);
    p_retro_handle->core_set_input_poll_function(&input_poll);
    p_retro_handle->core_set_input_state_function(&input_state);

    // init core
    p_retro_handle->core_init();

    return true;
}

bool RetroWidget::loadRom(const std::string &path) {
    if (!p_app->getIo()->exist(path)) {
        printf("RetroWidget::loadRom: file not found: %s\n", path.c_str());
        return false;
    }

    if (!p_retro_handle) {
        printf("RetroWidget::loadRom: no core loaded...\n");
        return false;
    }

    // load rom file in memory
    retro_game_info game_info = {.path = path.c_str(), .data = nullptr, .size = 0, .meta = nullptr};
    if (!m_core_info.need_fullpath) {
        game_info.size = p_app->getIo()->read(path, (char **) &game_info.data);
        if (!game_info.size) {
            printf("RetroWidget::loadRom: could not read file: %s\n", path.c_str());
            return false;
        }
    }

    if (!p_retro_handle->core_load_game(&game_info)) {
        printf("RetroWidget::loadRom: retro_load_game failed...\n");
        return false;
    }

    p_retro_handle->core_reset();
    p_retro_handle->core_get_system_av_info(&m_av_info);
    printf("RetroWidget::loadRom: base: %ix%i, max: %ix%i, ratio: %f, fps: %f, rate: %f\n",
           m_av_info.geometry.base_width, m_av_info.geometry.base_height,
           m_av_info.geometry.max_width, m_av_info.geometry.max_height,
           m_av_info.geometry.aspect_ratio, m_av_info.timing.fps, m_av_info.timing.sample_rate);

    // setup render texture
    const auto format = video_fmt == RETRO_PIXEL_FORMAT_RGB565 ? Texture::Format::RGB565 : Texture::Format::XBGR8;
    const auto w = c2d::Utility::pow2(static_cast<int>(m_av_info.geometry.max_width));
    const auto h = c2d::Utility::pow2(static_cast<int>(m_av_info.geometry.max_height));
    delete p_texture;
    p_texture = new C2DTexture(Vector2i(w, h), format);
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

void RetroWidget::onUpdate() {
    if (p_retro_handle && m_loaded) {
        p_retro_handle->core_run();
    }

    Rectangle::onUpdate();
}
