#include <iostream>
#include <sstream>
#include <iomanip>

// image
#include "stb_image.h"

// Matrix

#include "Shader.hpp"

#include "utilities.hpp"

#include "Constants.hpp"

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::mat4 view = glm::mat4(1.0f);
bool firstMouse = true;

float lastX = 400, lastY = 300;
auto yaw = -90.0f;
auto pitch = 0.0f;
auto multiplier = 1.0f;

/*
std::vector<glm::vec3> cubePositions = {
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(2.0f, 5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f, 3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f, 2.0f, -2.5f),
	glm::vec3(1.5f, 0.2f, -1.5f),
	glm::vec3(-1.3f, 1.0f, -1.5f)};
;
*/

void change_camera_pos(glm::vec3 new_pos)
{
	cameraPos = new_pos;
}