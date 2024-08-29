#include <vector>
#include <array>

#include <glm/glm.hpp>
#include <FastNoiseLite/FastNoiseLite.h>

#include "constants.hpp"
#include "noise.hpp"

class MapManager
{
public:
    MapManager();
    ~MapManager();
    void build_centered_on(const std::array<int, 3> &centre);
    std::array<std::array<int, 3>, 3> get_adjacent_heights(const std::array<int, 3> &centre);
    std::vector<std::vector<int>> m_height_map;
    std::vector<std::vector<int>> m_offset_map;
    std::vector<GLfloat> m_mesh_vertex;
    std::vector<GLuint> m_mesh_triangle;
    std::vector<std::array<int, 3>> m_cubes_positions;
    FastNoiseLite m_noise;

private:
    void _compute_noise(const std::array<int, 3> &centre);
};