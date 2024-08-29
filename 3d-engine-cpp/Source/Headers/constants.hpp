#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <glad/glad.h>
#include <string>

#define DEBUG false

namespace Constants
{
    const std::string APPLICATION_NAME = "Polyworld";
    // Window dimensions
    constexpr int WINDOW_WIDTH = 1800;  // 900;  // 1800;  // 1920;
    constexpr int WINDOW_HEIGHT = 1000; // 500; // 1000; // 1080;
    constexpr int MAP_SIZE = 2048;      // 64;       // 256;       // 2048;
    constexpr int HEIGHT_RANGE = 50;
    // 2048 = 14 fps (pluged-in) // 4,194,304
    // 4096 = 3 fps (pluged-in) // 16,777,216
    // 8192 = 1 fps (pluged-in: max, rather 0.1 fps...) // 67,108,864
#if DEBUG
    constexpr auto RENDER_MODE = GL_LINE;
#else
    constexpr auto RENDER_MODE = GL_FILL;
#endif
    const std::string TEXTURE_LOCATION = "./3d-engine-cpp/Assets/Textures";
    const std::string SHADERS_LOCATION = "./3d-engine-cpp/Source/Shaders";
} // namespace Constants

#endif // CONSTANTS_H