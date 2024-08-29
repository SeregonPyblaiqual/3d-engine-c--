#pragma once

#ifndef UTILITIES_HPP_INCLUDED
#define UTILITIES_HPP_INCLUDED

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <array>
#include <optional> // C++ 17
#include <algorithm> // min
#include <cmath> // std::sqrt
#include<limits> // std::numeric_limits<float>::infinity();
#include "Shader.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void change_camera_pos(glm::vec3 new_pos);
void showFPS(GLFWwindow* window, double& lastTime,unsigned int& frameCount);
std::array<GLuint, 2> init_buffers(Shader& shader, std::vector<glm::vec3>& cubePositions);
void update_buffers(Shader& shader, std::vector<glm::vec3>& data, std::array<GLuint, 2>& buffers);
void render(Shader& shader, GLuint& VAO,const glm::mat4& view,const std::vector<glm::vec3>& cubePositions);
glm::vec3 get_location();
std::optional<glm::vec3> facing_cube(std::vector<glm::vec3> cubePositions);
std::array<GLuint, 2> loadVAO(Shader& shader);

GLFWwindow* initWindow();

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
glm::mat4 processInput(GLFWwindow* window, float& deltaTime);

#endif // !UTILITIES_HPP_INCLUDED
