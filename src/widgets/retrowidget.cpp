//
// Created by cpasjuste on 28/11/24.
//

#include "app.h"
#include "retrowidget.h"

using namespace c2d;
using namespace retropico;

typedef RETRO_CALLCONV void (*core_info_function)(retro_system_info *info);

typedef RETRO_CALLCONV void (*core_action_function)();

typedef RETRO_CALLCONV void (*core_loadg_function)(retro_game_info *game);

typedef RETRO_CALLCONV void (*core_set_environment_fn)(retro_environment_t);

typedef RETRO_CALLCONV void (*core_set_video_refresh_fn)(retro_video_refresh_t);

typedef RETRO_CALLCONV void (*core_set_audio_sample_fn)(retro_audio_sample_t);

typedef RETRO_CALLCONV void (*core_set_audio_sample_batch_fn)(retro_audio_sample_batch_t);

typedef RETRO_CALLCONV void (*core_set_input_poll_fn)(retro_input_poll_t);

typedef RETRO_CALLCONV void (*core_set_input_state_fn)(retro_input_state_t);

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
        case RETRO_ENVIRONMENT_SET_GEOMETRY: {
            const auto av = static_cast<retro_system_av_info *>(data);
            printf("RETRO_ENVIRONMENT_SET_GEOMETRY: base: %ix%i, max: %ix%i\n",
                   av->geometry.base_width, av->geometry.base_height,
                   av->geometry.max_width, av->geometry.max_height);
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

void RETRO_CALLCONV video_update(const void *data, unsigned width, unsigned height, size_t pitch) {
    if (!data) return;

    const auto tex = s_retro_widget->getTexture();
    if (!tex) return;

    const auto rect = tex->getTextureRect();
    if (rect.width != width || rect.height != height) {
        tex->setTextureRect(IntRect{0, 0, static_cast<int>(width), static_cast<int>(height)});
        tex->setSize(static_cast<float>(width), static_cast<float>(height));
    }

    tex->unlock((uint8_t *) data);
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
        printf("RetroWidget::loadCore: failed to load core (file not found: %s)\n", path.c_str());
        return false;
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
        printf("RetroWidget::loadRom: failed to load core (file not found: %s)\n", path.c_str());
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
            printf("RetroWidget::loadRom: could not read file...\n");
            return false;
        }
    }

    if (!p_retro_handle->core_load_game(&game_info)) {
        printf("RetroWidget::loadRom: retro_load_game failed...\n");
        return false;
    }

    p_retro_handle->core_reset();
    p_retro_handle->core_get_system_av_info(&m_av_info);

    // setup render texture
    const auto format = video_fmt == RETRO_PIXEL_FORMAT_RGB565 ? Texture::Format::RGB565 : Texture::Format::XBGR8;
    const auto w = Utility::pow2(static_cast<int>(m_av_info.geometry.base_width));
    const auto h = Utility::pow2(static_cast<int>(m_av_info.geometry.base_height));
    delete p_texture;
    p_texture = new C2DTexture(Vector2i(w, h), format);
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

void RetroWidget::onUpdate() {
    if (p_retro_handle && m_loaded) {
        p_retro_handle->core_run();
    }

    Rectangle::onUpdate();
}
