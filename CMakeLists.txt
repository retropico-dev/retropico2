cmake_minimum_required(VERSION 3.5)

option(PLATFORM_RETROPICO "RetroPico target platform" OFF)

########################
# LIBCROSS2D SUBMODULE
########################
add_subdirectory(libcross2d)

########################
# PROJECT SETUP
########################
project(RetroPico)
set(VERSION_MAJOR "1")
set(VERSION_MINOR "0")
set(PROJECT_AUTHOR "cpasjuste")

########################
# SOURCES SETUP
########################
file(GLOB SOURCES
        src/*.c*
        src/utility/*.c*
        src/libretro/*.c*
)
set(INCLUDES src)
set(CFLAGS)
set(LDFLAGS)

if (PLATFORM_RETROPICO)
    list(APPEND CFLAGS "-DRETROPICO_DEVICE")
endif ()

include(FindPkgConfig)
pkg_search_module(ZIP REQUIRED libzip)
list(APPEND CFLAGS "-I${ZIP_INCLUDE_DIRS}")
list(APPEND LDFLAGS ${ZIP_LIBRARIES})

#####################
# EXECUTABLE
#####################
add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${INCLUDES})
target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE ${CFLAGS})
target_link_libraries(${CMAKE_PROJECT_NAME} cross2d ${LDFLAGS})

#####################
# targets
#####################
include(libcross2d/cmake/targets.cmake)
