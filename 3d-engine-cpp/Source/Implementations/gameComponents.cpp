#include "gameComponents.hpp"

GameComponents::GameComponents()
{
    m_map_manager.build_centered_on(*m_camera.get_integer_pos());
    std::array<int, 3> *camera_pos = m_camera.get_integer_pos();

    m_camera.set_position(
        glm::vec3(camera_pos->at(0),
                  m_map_manager.m_height_map
                          [1 + camera_pos->at(0) + Constants::MAP_SIZE / 2]
                          [1 + camera_pos->at(2) + Constants::MAP_SIZE / 2] +
                      2,
                  camera_pos->at(2)));
}

void GameComponents::initialise_game_components()
{
}

GameComponents::~GameComponents() {}