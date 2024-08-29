#include "mapManager.hpp"

MapManager::MapManager()
{
    m_noise = init_noise();
}
MapManager::~MapManager()
{
}

int MapManager::_get_noise(const int &average, const int &x, const int &z)
{
    float raw_noise = m_noise.GetNoise((float)x, (float)z);
    int noise_at = std::max(0, std::min(static_cast<int>(raw_noise * average + average - 1), Constants::HEIGHT_RANGE));
    return noise_at;
}

std::array<std::array<float, 3>, 8> vertices_offset = {{
    {{-0.5f, +0.5f, -0.5f}},
    {{-0.5f, +0.5f, +0.5f}},
    {{-0.5f, -0.5f, -0.5f}},
    {{-0.5f, -0.5f, +0.5f}},
    {{+0.5f, +0.5f, -0.5f}},
    {{+0.5f, +0.5f, +0.5f}},
    {{+0.5f, -0.5f, -0.5f}},
    {{+0.5f, -0.5f, +0.5f}},
}};
namespace face_to_vertices
{
    std::array<GLuint, 4> South = {{0, 1, 2, 3}};
    std::array<GLuint, 4> North = {{5, 4, 7, 6}};
    std::array<GLuint, 4> Top = {{4, 5, 0, 1}};
    std::array<GLuint, 4> Bottom = {{3, 2, 7, 6}};
    std::array<GLuint, 4> West = {{4, 0, 6, 2}};
    std::array<GLuint, 4> East = {{2, 5, 3, 7}};
};

std::array<std::array<float, 2>, 4> face_to_tex_coord = {{
    {{0.0f, 1.0f}},
    {{1.0f, 1.0f}},
    {{0.0f, 0.0f}},
    {{1.0f, 0.0f}},
}};

void MapManager::_add_face(const std::array<GLint, 3> &vertice, const std::array<GLuint, 4> &face_vertices)
{
    unsigned int starting_index = m_mesh_vertex.size() / 5;

    for (unsigned int i_face = 0; i_face < 4; i_face++)
    {
        for (unsigned int i_vertice = 0; i_vertice < 3; i_vertice++)
        {
            m_mesh_vertex.push_back(vertice[i_vertice] + vertices_offset[face_vertices[i_face]][i_vertice]);
        }
        m_mesh_vertex.push_back(face_to_tex_coord[i_face][0]);
        m_mesh_vertex.push_back(face_to_tex_coord[i_face][1]);
    }

    // use triangles for now because end purpose is to use triangles instead of quads

    m_mesh_triangle.push_back(starting_index + 0);
    m_mesh_triangle.push_back(starting_index + 2);
    m_mesh_triangle.push_back(starting_index + 1);

    m_mesh_triangle.push_back(starting_index + 1);
    m_mesh_triangle.push_back(starting_index + 2);
    m_mesh_triangle.push_back(starting_index + 3);
}

void MapManager::build_centered_on(const std::array<int, 3> &centre)
{
    // Reset vectors
    // Vector usage justifies as different locations have no reasons to be similar (different vertices location in vector)
    m_mesh_vertex = std::vector<GLfloat>();
    m_mesh_triangle = std::vector<GLuint>();

    // location info
    const int chunk_size = Constants::MAP_SIZE;
    const int base_x = -Constants::MAP_SIZE / 2 + centre[0]; // ! untested for now...
    const int base_z = -Constants::MAP_SIZE / 2 + centre[2]; // !
    const int max_z = chunk_size + base_z;
    const int max_x = chunk_size + base_x;
    const int halved_chunk_size = chunk_size / 2;

    // noise config

    int average = (Constants::HEIGHT_RANGE + 1) / 2;

    // implementation choice: avoiding a if for each x, and z afterwards
    std::vector<int> line_of_z_noise;
    for (int z = base_z - 1; z < max_z + 1; z++)
    {

        line_of_z_noise.push_back(_get_noise(average, base_x - 1, z));
    }
    m_height_map_with_offset.push_back(line_of_z_noise);

    for (int x = base_x; x < max_x; x++) // max_x
    {
        std::vector<int> line_of_z_noise;
        line_of_z_noise.push_back(_get_noise(average, x, base_z - 1));

        for (int z = base_z; z < max_z; z++) // max_z
        {
            line_of_z_noise.push_back(_get_noise(average, x, z));
            int current_noise = line_of_z_noise[z + 1 + halved_chunk_size];

            int south_noise = m_height_map_with_offset[x + 0 + halved_chunk_size][z + 1 + halved_chunk_size];
            int west_noise = line_of_z_noise[z + 0 + halved_chunk_size];
            unsigned int starting_index = m_mesh_vertex.size() / 5;

            m_cubes_positions.push_back({x, current_noise, z});

            // Get where the highest face should be and fill if needed
            int delta_noise = current_noise - south_noise;
            _add_face(std::array<GLint, 3>{x, current_noise, z}, face_to_vertices::Top);
            if (delta_noise == 0)
            {
            }
            else
            {
                if (delta_noise > 0)
                {
                    for (int i = 0; i < delta_noise; i++)
                    {
                        _add_face(std::array<GLint, 3>{x, current_noise - i, z}, face_to_vertices::South);
                    }
                }
                else // delta_noise < 0
                {
                    for (int i = 0; i < -delta_noise; i++)
                    {
                        _add_face(std::array<GLint, 3>{x, current_noise + 1 + i, z}, face_to_vertices::South);
                    }
                }
            }
            delta_noise = current_noise - west_noise;
            if (delta_noise == 0)
            {
            }
            else
            {
                if (delta_noise > 0)
                {
                    for (int i = 0; i < delta_noise; i++)
                    {
                        _add_face(std::array<GLint, 3>{x, current_noise - i, z}, face_to_vertices::West);
                    }
                }
                else // delta_noise < 0
                {
                    for (int i = 0; i < -delta_noise; i++)
                    {
                        _add_face(std::array<GLint, 3>{x, current_noise + 1 + i, z}, face_to_vertices::West);
                    }
                }
            }
        }
        // ! Handling last differently

        ; // ....

        m_height_map_with_offset.push_back(line_of_z_noise);
    }
}

std::array<std::array<int, 3>, 3> MapManager::get_adjacent_heights(const std::array<int, 3> &centre)
{
    std::array<std::array<int, 3>, 3> res;
    return res;
}

void MapManager::_compute_noise(const std::array<int, 3> &centre)
{
}