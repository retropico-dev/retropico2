//
// Created by cpasjuste on 28/11/24.
//

#ifndef RETROPICO_LOADER_HH_
#define RETROPICO_LOADER_HH_

#include "libretro.h"

#ifdef WIN32
  #include <windows.h>
  #define LIBHANDLE HMODULE
  #define LOAD_LIBRARY(path) LoadLibraryA(path)
  #define UNLOAD_LIBRARY(lib) FreeLibrary(lib)
  #define LOAD_SYMBOL(lib, name) GetProcAddress(lib, name)
#else
#include <dlfcn.h>
#define LIBHANDLE void*
#define LOAD_LIBRARY(path) dlopen(path, RTLD_LAZY | RTLD_LOCAL)
#define UNLOAD_LIBRARY(lib) dlclose(lib)
#define LOAD_SYMBOL(lib, name) dlsym(lib, name)
#endif

// Function types
typedef RETRO_CALLCONV void (*retro_info_fnt)(retro_system_info *info);

typedef RETRO_CALLCONV void (*retro_action_fnt)();

typedef RETRO_CALLCONV bool (*retro_loadg_fnt)(const retro_game_info *game);

typedef RETRO_CALLCONV void (*retro_unloadg_fnt)();

typedef RETRO_CALLCONV void (*retro_set_environment_fnt)(retro_environment_t);

typedef RETRO_CALLCONV void (*retro_set_video_refresh_fnt)(retro_video_refresh_t);

typedef RETRO_CALLCONV void (*retro_set_audio_sample_fnt)(retro_audio_sample_t);

typedef RETRO_CALLCONV void (*retro_set_audio_sample_batch_fnt)(retro_audio_sample_batch_t);

typedef RETRO_CALLCONV void (*retro_set_input_poll_fnt)(retro_input_poll_t);

typedef RETRO_CALLCONV void (*retro_set_input_state_fnt)(retro_input_state_t);

typedef RETRO_CALLCONV bool (*retro_serialize_fnt)(void *data, size_t size);

typedef RETRO_CALLCONV size_t (*retro_serialize_size_fnt)();

typedef RETRO_CALLCONV bool (*retro_unserialize_fnt)(const void *data, size_t size);

typedef RETRO_CALLCONV void *(*retro_get_memory_data_fnt)(unsigned id);

typedef RETRO_CALLCONV size_t (*retro_get_memory_size_fnt)(unsigned id);

typedef RETRO_CALLCONV void (*retro_get_system_av_info_fnt)(retro_system_av_info *info);

typedef struct {
    retro_action_fnt retro_init;
    retro_action_fnt retro_deinit;
    retro_action_fnt retro_run;
    retro_action_fnt retro_reset;
    retro_info_fnt retro_get_info;
    retro_loadg_fnt retro_load_game;
    retro_unloadg_fnt retro_unload_game;
    retro_set_environment_fnt retro_set_env_function;
    retro_set_video_refresh_fnt retro_set_video_refresh_function;
    retro_set_audio_sample_fnt retro_set_audio_sample_function;
    retro_set_audio_sample_batch_fnt retro_set_audio_sample_batch_function;
    retro_set_input_poll_fnt retro_set_input_poll_function;
    retro_set_input_state_fnt retro_set_input_state_function;
    retro_serialize_fnt retro_serialize;
    retro_serialize_size_fnt retro_serialize_size;
    retro_unserialize_fnt retro_unserialize;
    retro_get_memory_data_fnt retro_get_memory_data;
    retro_get_memory_size_fnt retro_get_memory_size;
    retro_get_system_av_info_fnt retro_get_system_av_info;

    LIBHANDLE handle;
} retro_functions_t;

// Loads the core and gets a handler to its functions
retro_functions_t *load_core(const char *filename);

// Unloads and frees the core
void unload_core(retro_functions_t *core);

#endif
