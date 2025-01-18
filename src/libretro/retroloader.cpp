//
// Created by cpasjuste on 28/11/24.
//

#include <cstdlib>
#include "retroloader.h"

retro_functions_t *load_core(const char *filename) {
    LIBHANDLE handle = LOAD_LIBRARY(filename);
    if (!handle) return nullptr;

    const auto fns = (retro_functions_t *) malloc(sizeof(retro_functions_t));
    fns->retro_init = (retro_action_fnt) LOAD_SYMBOL(handle, "retro_init");
    fns->retro_deinit = (retro_action_fnt) LOAD_SYMBOL(handle, "retro_deinit");
    fns->retro_run = (retro_action_fnt) LOAD_SYMBOL(handle, "retro_run");
    fns->retro_reset = (retro_action_fnt) LOAD_SYMBOL(handle, "retro_reset");
    fns->retro_get_info = (retro_info_fnt) LOAD_SYMBOL(handle, "retro_get_system_info");
    fns->retro_load_game = (retro_loadg_fnt) LOAD_SYMBOL(handle, "retro_load_game");
    fns->retro_unload_game = (retro_unloadg_fnt) LOAD_SYMBOL(handle, "retro_unload_game");

    fns->retro_set_env_function = (retro_set_environment_fnt) LOAD_SYMBOL(handle, "retro_set_environment");
    fns->retro_set_video_refresh_function = (retro_set_video_refresh_fnt)
            LOAD_SYMBOL(handle, "retro_set_video_refresh");
    fns->retro_set_audio_sample_function = (retro_set_audio_sample_fnt) LOAD_SYMBOL(handle, "retro_set_audio_sample");
    fns->retro_set_audio_sample_batch_function = (retro_set_audio_sample_batch_fnt) LOAD_SYMBOL(
        handle, "retro_set_audio_sample_batch");
    fns->retro_set_input_poll_function = (retro_set_input_poll_fnt) LOAD_SYMBOL(handle, "retro_set_input_poll");
    fns->retro_set_input_state_function = (retro_set_input_state_fnt) LOAD_SYMBOL(handle, "retro_set_input_state");

    fns->retro_serialize = (retro_serialize_fnt) LOAD_SYMBOL(handle, "retro_serialize");
    fns->retro_serialize_size = (retro_serialize_size_fnt) LOAD_SYMBOL(handle, "retro_serialize_size");
    fns->retro_unserialize = (retro_unserialize_fnt) LOAD_SYMBOL(handle, "retro_unserialize");
    fns->retro_get_memory_data = (retro_get_memory_data_fnt) LOAD_SYMBOL(handle, "retro_get_memory_data");
    fns->retro_get_memory_size = (retro_get_memory_size_fnt) LOAD_SYMBOL(handle, "retro_get_memory_size");
    fns->retro_get_system_av_info = (retro_get_system_av_info_fnt) LOAD_SYMBOL(handle, "retro_get_system_av_info");
    fns->handle = handle;

    return fns;
}

void unload_core(retro_functions_t *core) {
    UNLOAD_LIBRARY(core->handle);
    free(core);
}
