//
// Created by cpasjuste on 28/11/24.
//

#include <cstdlib>
#include "retroloader.h"

core_functions_t *load_core(const char *filename) {
    LIBHANDLE handle = LOAD_LIBRARY(filename);
    if (!handle) return nullptr;

    const auto fns = (core_functions_t *) malloc(sizeof(core_functions_t));
    fns->core_init = (core_action_fnt) LOAD_SYMBOL(handle, "retro_init");
    fns->core_deinit = (core_action_fnt) LOAD_SYMBOL(handle, "retro_deinit");
    fns->core_run = (core_action_fnt) LOAD_SYMBOL(handle, "retro_run");
    fns->core_reset = (core_action_fnt) LOAD_SYMBOL(handle, "retro_reset");
    fns->core_get_info = (core_info_fnt) LOAD_SYMBOL(handle, "retro_get_system_info");
    fns->core_load_game = (core_loadg_fnt) LOAD_SYMBOL(handle, "retro_load_game");
    fns->core_unload_game = (core_unloadg_fnt) LOAD_SYMBOL(handle, "retro_unload_game");

    fns->core_set_env_function = (core_set_environment_fnt) LOAD_SYMBOL(handle, "retro_set_environment");
    fns->core_set_video_refresh_function = (core_set_video_refresh_fnt) LOAD_SYMBOL(handle, "retro_set_video_refresh");
    fns->core_set_audio_sample_function = (core_set_audio_sample_fnt) LOAD_SYMBOL(handle, "retro_set_audio_sample");
    fns->core_set_audio_sample_batch_function = (core_set_audio_sample_batch_fnt) LOAD_SYMBOL(
        handle, "retro_set_audio_sample_batch");
    fns->core_set_input_poll_function = (core_set_input_poll_fnt) LOAD_SYMBOL(handle, "retro_set_input_poll");
    fns->core_set_input_state_function = (core_set_input_state_fnt) LOAD_SYMBOL(handle, "retro_set_input_state");

    fns->core_serialize = (core_serialize_fnt) LOAD_SYMBOL(handle, "retro_serialize");
    fns->core_serialize_size = (core_serialize_size_fnt) LOAD_SYMBOL(handle, "retro_serialize_size");
    fns->core_unserialize = (core_unserialize_fnt) LOAD_SYMBOL(handle, "retro_unserialize");
    fns->core_get_system_av_info = (core_get_system_av_info_fnt) LOAD_SYMBOL(handle, "retro_get_system_av_info");
    fns->handle = handle;

    return fns;
}

void unload_core(core_functions_t *core) {
    UNLOAD_LIBRARY(core->handle);
    free(core);
}
