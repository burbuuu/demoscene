cmake_minimum_required(VERSION 4.1)
project(R1)

set(CMAKE_CXX_STANDARD 20)

# Find all SDL2 components
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(OpenGL REQUIRED)

if(WIN32)
    set(GLU_LIBRARIES glu32)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GLU REQUIRED glu)
endif()

add_executable(R1 src/main.cpp
        src/effect.cpp
        src/effect.h
        src/demo.cpp
        src/demo.h
        src/defines.h
        src/effects/PlasmaEffect.cpp
        src/effects/PlasmaEffect.h
        src/effects/ImageEffect.cpp
        src/effects/ImageEffect.h
        src/MusicManager.cpp
        src/MusicManager.h
        src/PaletteBuilder.cpp
        src/PaletteBuilder.h
        src/Timeline.cpp
        src/Timeline.h
        src/TransitionManager.cpp
        src/TransitionManager.h
        src/effects/PlasmaImageEffect.cpp
        src/effects/PlasmaImageEffect.h)

# Link SDL2_mixer here
target_link_libraries(R1 PRIVATE
        SDL2::SDL2
        SDL2_image::SDL2_image
        SDL2_mixer::SDL2_mixer
        OpenGL::GL
        ${GLU_LIBRARIES}
)

target_include_directories(R1 PRIVATE ${SDL2_INCLUDE_DIRS} ${GLU_INCLUDE_DIRS} src/ src/effects/)