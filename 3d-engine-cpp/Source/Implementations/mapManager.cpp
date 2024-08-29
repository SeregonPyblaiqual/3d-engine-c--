#include "mapManager.hpp"

MapManager::MapManager()
{
    m_noise = init_noise();
}
MapManager::~MapManager()
{
}

void MapManager::build_centered_on(const std::array<int, 3> &centre)
{
    // m_data = gl_locations_noise(&m_noise, -Constants::MAP_SIZE / 2 + centre[0], -Constants::MAP_SIZE / 2 + centre[2], Constants::MAP_SIZE); // gradient_noise5(1718992648, Constants::MAP_SIZE, 0, 0, 16);
    compute_noise_attributes_v_brute_force(&m_noise, -Constants::MAP_SIZE / 2 + centre[0], -Constants::MAP_SIZE / 2 + centre[2], Constants::MAP_SIZE, m_mesh_vertex, m_mesh_triangle, m_cubes_positions, m_height_map);
    // m_height_map = get_height_map(&m_noise, -Constants::MAP_SIZE / 2 + centre[0], -Constants::MAP_SIZE / 2 + centre[2], Constants::MAP_SIZE);
}

std::array<std::array<int, 3>, 3> MapManager::get_adjacent_heights(const std::array<int, 3> &centre)
{
    std::array<std::array<int, 3>, 3> res;
    return res;
}

void MapManager::_compute_noise(const std::array<int, 3> &centre)
{
}