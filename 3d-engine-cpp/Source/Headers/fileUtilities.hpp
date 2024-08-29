#pragma once

#ifndef FILE_UTILITIES_H
#define FILE_UTILITIES_H

#include <vector>      // std::vector
#include <glm/glm.hpp> // glm::vec3
#include <iostream>
#include <fstream>   // std::ifstream
#include <sstream>   // std::stringstream
#include <string>    // std::string, std::stof
#include <stdexcept> // std::runtime_error

#include <stb/stb_image_write.h> // stbi_write_png
#include <stb_image.h>

std::vector<glm::vec3> read_map();
std::vector<glm::vec3> image_to_map(const char *source_path);
void int_matrix_to_image(std::vector<std::vector<int>> data, int size, char *target_path);

#endif // FILE_UTILITIES_H