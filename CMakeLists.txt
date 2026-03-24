cmake_minimum_required(VERSION 4.1)
project(R1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configure Debug/Release flags
# For GCC/Clang:
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(COMMON_FLAGS "-Wall -Wextra")
    set(CMAKE_CXX_FLAGS_DEBUG "${COMMON_FLAGS} -g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "${COMMON_FLAGS} -O3")
elseif(MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "/W4 /Zi /Od")
    set(CMAKE_CXX_FLAGS_RELEASE "/W4 /O2")
endif()

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
        src/effects/PlasmaImageEffect.h
        src/effects/RotateImage.cpp
        src/effects/RotateImage.h
        src/effects/LetteringEffect.cpp
        src/effects/LetteringEffect.h)

# Link SDL2_mixer here
target_link_libraries(R1 PRIVATE
        SDL2::SDL2
        SDL2_image::SDL2_image
        SDL2_mixer::SDL2_mixer
        OpenGL::GL
        ${GLU_LIBRARIES}
)

# Use target_compile_definitions for configuration-specific macros
target_compile_definitions(R1 PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)

target_include_directories(R1 PRIVATE ${SDL2_INCLUDE_DIRS} ${GLU_INCLUDE_DIRS} src/ src/effects/)