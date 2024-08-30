#include <vector>
#include <array>
#include <algorithm> // std::max, std::min
#include <glm/glm.hpp>
#include <FastNoiseLite/FastNoiseLite.h>

#include "constants.hpp"
#include "noise.hpp"

enum class faceID : uint_fast8_t
{
    SOUTH = 0,
    TOP = 1,
    EAST = 2,
    BOTTOM = 3,
    WEST = 4,
    NORTH = 5
};

class MapManager
{
public:
    MapManager();
    ~MapManager();
    void build_centered_on(const std::array<int, 3> &centre);
    std::array<std::array<int, 3>, 3> get_adjacent_heights(const std::array<int, 3> &centre);
    // offset include extraborder info (to compute borderline faces): north, south, west, east (+1 per direction)
    std::vector<std::vector<int>> m_height_map_with_offset;
    std::vector<std::vector<int>> m_offset_map;
    std::vector<GLfloat> m_mesh_vertex;
    std::vector<GLuint> m_mesh_triangle;
    std::vector<std::array<int, 3>> m_cubes_positions;
    FastNoiseLite m_noise;

private:
    void _compute_noise(const std::array<int, 3> &centre);
    int _get_noise(const int &average, const int &x, const int &z);
    void _add_face(const std::array<GLint, 3> &vertice, const faceID &face_id);
};