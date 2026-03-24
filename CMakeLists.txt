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

add_executable(R1 main.cpp
        effect.cpp
        effect.h
        demo.cpp
        demo.h
        defines.h
        PlasmaEffect.cpp
        PlasmaEffect.h
        ImageEffect.cpp
        ImageEffect.h
        MusicManager.cpp
        MusicManager.h
        PaletteBuilder.cpp
        PaletteBuilder.h
        Timeline.cpp
        Timeline.h
        TransitionManager.cpp
        TransitionManager.h
        PlasmaImageEffect.cpp
        PlasmaImageEffect.h)

# Link SDL2_mixer here
target_link_libraries(R1 PRIVATE
        SDL2::SDL2
        SDL2_image::SDL2_image
        SDL2_mixer::SDL2_mixer
        OpenGL::GL
        ${GLU_LIBRARIES}
)

target_include_directories(R1 PRIVATE ${SDL2_INCLUDE_DIRS} ${GLU_INCLUDE_DIRS})