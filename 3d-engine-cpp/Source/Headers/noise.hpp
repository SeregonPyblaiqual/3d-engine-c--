#pragma once

#ifndef NOISE_H
#define NOISE_H

#include <vector>
#include <array>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <FastNoiseLite/FastNoiseLite.h> // FastNoiseLite
#include <glm/glm.hpp>                   // glm::vec3
#include <cmath>                         // std::abs, std::max

#include "constants.hpp"

FastNoiseLite init_noise(time_t seed = 1718992648);

std::vector<std::vector<int>> noise_generator(int base_x = 0, int base_y = 0, int chunk_size = 256);

std::vector<glm::vec3> gl_locations_noise(FastNoiseLite *noise, int base_x = 0, int base_z = 0, int chunk_size = 256);
std::vector<std::vector<int>> get_height_map(FastNoiseLite *noise, int base_x, int base_z, int chunk_size);

#endif // NOISE_H