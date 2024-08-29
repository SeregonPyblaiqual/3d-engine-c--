#include "noise.hpp"

FastNoiseLite init_noise(time_t seed)
{
    FastNoiseLite noise;

    noise.SetSeed(seed);
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    // default: 0.01f
    // noise.SetFrequency(0.03f);
    return noise;
}

std::vector<std::vector<int>> noise_generator(FastNoiseLite *noise, int base_x, int base_y, int chunk_size)
{
    // Gather noise data
    std::vector<std::vector<int>> noiseData;
    int index = 0;

    int range = 100;
    int average = (range + 1) / 2;

    for (int y = base_y; y < chunk_size + base_y; y++)
    {
        std::vector<int> line_of_noise;
        for (int x = base_x; x < chunk_size + base_x; x++)
        {
            float raw_noise = noise->GetNoise((float)x, (float)y);
            // std::cout << x << ";" << y << ";" << raw_noise << std::endl;
            int noise_at = std::max(0, std::min(static_cast<int>(raw_noise * average + average - 1), range));
            line_of_noise.push_back(noise_at);
        }
        noiseData.push_back(line_of_noise);
    }

    return noiseData;
}

std::vector<glm::vec3> noise_to_gl_locations(std::vector<std::vector<int>> *data)
{
    std::vector<glm::vec3> res;
    return res;
}

std::vector<glm::vec3> gl_locations_noise(FastNoiseLite *noise, int base_x, int base_z, int chunk_size)
{
    /*

    (haut
    |
    bas)

    x
    ^
    |
    |__> z (gauche - droite)

    filling:
    |___> z
    |
    x

    LC
     BN
    C: current
    L: left
    B: bottom
    N: next bottom
    */

    // Gather noise data
    std::vector<glm::vec3> noiseData;
    std::vector<glm::vec3> extras; // required to be able to query noiseData using coordinates e.g. (y*size+x)
    int index = 0;

    int range = 50;
    int average = (range + 1) / 2;

    int max_z = chunk_size + base_z;
    int max_x = chunk_size + base_x;
    // int x = base_x;

    for (int x = base_x; x < base_x + 2; x++)
    {
        // int z = base_z;

        for (int z = base_z; z < max_z; z++)
        {
            float raw_noise = noise->GetNoise((float)x, (float)z);
            // std::cout << x << ";" << y << ";" << raw_noise << std::endl;
            int noise_at = std::max(0, std::min(static_cast<int>(raw_noise * average + average - 1), range));
            noiseData.push_back(glm::vec3(x, noise_at, z));

            // Ajouter les blocs manquants entre les niveaux

            // Cas: le bloc de la colonne actuelle (axe y) est au dessus des
            // à ajouter en dessous
            int add_current = 0;
            int add_left = 0;
            int add_bottom = 0;
            int offset_bottom = 0; // commencer à ajouter de là
            int do_add_bottom = true;

            int delta_z;
            int noise_at_left;

            if (z > base_z)
            {
                noise_at_left = noiseData[(-base_x + x) * chunk_size + (z - base_z) - 1][1];
                delta_z = noise_at - noise_at_left;
            }
            else
            {
                // outside of current map
                float raw_noise_out = noise->GetNoise((float)x, (float)z - 1);
                // std::cout << x << ";" << y << ";" << raw_noise << std::endl;
                noise_at_left = std::max(0, std::min(static_cast<int>(raw_noise * average + average - 1), range));
                delta_z = noise_at - noise_at_left;
            }
            // colonne de gauche plus basse que colonne de droite
            if (delta_z > 1)
            {
                // Ajouter des blocs en dessous de la colonne actuelle (max de la difference entre les 3 colonnes)
                add_current = delta_z - 1;
            }
            else if (delta_z < -1)
            {
                // Ajouter des blocs en dessous de la colonne de gauche
                add_left = -delta_z - 1;
            }
            int delta_x;
            int noise_at_bottom;
            int noise_at_next;

            if (x > base_x)
            {
                noise_at_bottom = noiseData[(-base_x + x - 1) * chunk_size + z - base_z][1];
                delta_x = noise_at - noise_at_bottom;
            }
            else
            {
                // outside of current map
                float raw_noise_out = noise->GetNoise((float)x - 1, (float)z);
                noise_at_bottom = std::max(0, std::min(static_cast<int>(raw_noise * average + average - 1), range));
                delta_x = noise_at - noise_at_bottom;

                raw_noise_out = noise->GetNoise((float)x - 1, (float)z + 1);
                noise_at_next = std::max(0, std::min(static_cast<int>(raw_noise * average + average - 1), range));
            }
            if (delta_x > 1)
            {
                // Ajouter des blocs en dessous de la colonne actuelle
                add_current = std::max(add_current, delta_x - 1);
            }
            else if (delta_x < -1)
            {
                // Ajouter des blocs en dessous de la colonne du bas
                add_bottom = -delta_x - 1;
            }

            // Handling the fact that cubes may already have been placed under bottom
            // ou bien current est plus bas que next -> ajouter le delta
            // ou bien current est plus haut que next -> rien faire

            int delta_nz;
            // ! May cause problems when handling continuous maps
            if (z < max_z - 1)
            {
                if (x > base_x)
                {
                    noise_at_next = noiseData[(x - 1 - base_x) * chunk_size + (z - base_z) + 1][1];
                }
                delta_nz = noise_at - noise_at_next;
            }
            else
            {
                // outside of current map
                if (x > base_x)
                {
                    float raw_noise_out = noise->GetNoise((float)x, (float)z + 1);
                    noise_at_next = std::max(0, std::min(static_cast<int>(raw_noise * average + average - 1), range));
                }
                delta_nz = noise_at - noise_at_next;
            }
            if (delta_nz >= 0)
            {
                do_add_bottom = false;
            }
            else
            {
                // Si le niveau du cube actuel est plus bas que le deuxieme cube à bottom: commencer à ajouter des cubes plus
                //
                // ! ?
                int delta_bottoms_z = noiseData[(x - 1 - base_x) * chunk_size + (z - base_z) + 1][1] - noiseData[(x - 1 - base_x) * chunk_size + (z - base_z)][1];
                if (delta_bottoms_z < 1)
                    offset_bottom = delta_bottoms_z;
            }

            for (int c = 1; c <= add_current; c++)
            {
                extras.push_back(glm::vec3(x, noise_at - c, z));
            }
            for (int l = 1; l <= add_left; l++)
            {
                int noise_there = noiseData[(-base_x + x) * chunk_size + (z - base_z) - 1][1];
                extras.push_back(glm::vec3(x, noise_there - l, z - 1));
            }
            if (do_add_bottom)
            {
                for (int b = offset_bottom; b < add_bottom; b++)
                {
                    int noise_there = noiseData[(-base_x + x - 1) * chunk_size + (z - base_z)][1];
                    extras.push_back(glm::vec3(x - 1, noise_there - b - 1, z));
                }
            }

            // Cas 1
            // Déterminer où il faut ajouter des cubes
            // Déterminer combien de cubes il faut ajouter
            // ? Risque de double traitement d'une colonne (= d'une superposition de deux cubes dans extras ?)
            // Besoin d'information suplémentaire
        }
    }
    noiseData.insert(noiseData.end(), extras.begin(), extras.end());
    // DEBUG
    //  noiseData = std::vector<glm::vec3>{ glm::vec3{0,0,0} };
    // noiseData = std::vector<glm::vec3>{ noiseData[0] };
    return noiseData;
}

std::vector<std::vector<int>> get_height_map(FastNoiseLite *noise, int base_x, int base_z, int chunk_size)
{
    // Gather noise data
    std::vector<std::vector<int>> height_map;
    int index = 0;

    int range = Constants::HEIGHT_RANGE;
    int average = (range + 1) / 2;

    int max_z = chunk_size + base_z;
    int max_x = chunk_size + base_x;

    for (int x = base_x; x < max_x; x++)
    {
        for (int z = base_z; z < max_z; z++)
        {

            float raw_noise = noise->GetNoise((float)x, (float)z);
            // std::cout << x << ";" << y << ";" << raw_noise << std::endl;
            int noise_at = std::max(0, std::min(static_cast<int>(raw_noise * average + average - 1), range));
            // height_map.push_back(glm::vec3(x, noise_at, z));
        }
    }

    return height_map;
}

struct Point
{
    std::array<GLfloat, 3> coordinates;

    constexpr Point(GLfloat x, GLfloat y, GLfloat z) : coordinates{x, y, z} {}
};

struct Normal
{
    std::array<GLfloat, 3> coordinates;

    constexpr Normal(GLfloat x, GLfloat y, GLfloat z) : coordinates{x, y, z} {}
};
namespace Points
{
    constexpr Point A(-0.5f, 0.5f, 0.5f);
    constexpr Point B(0.5f, 0.5f, 0.5f);
    constexpr Point C(-0.5f, -0.5f, 0.5f);
    constexpr Point D(0.5f, -0.5f, 0.5f);
    constexpr Point E(-0.5f, 0.5f, -0.5f);
    constexpr Point F(0.5f, 0.5f, -0.5f);
    constexpr Point G(-0.5f, -0.5f, -0.5f);
    constexpr Point H(0.5f, -0.5f, -0.5f);
}

namespace Normals
{
    // -x
    constexpr Normal Front(-1.0f, 0.0f, 0.0f);
    // +x
    constexpr Normal Back(1.0f, 0.0f, 0.0f);
    // -z
    constexpr Normal Left(0.0f, 0.0f, -1.0f);
    // +z
    constexpr Normal Right(0.0f, 0.0f, 1.0f);
    // +y
    constexpr Normal Top(0.0f, 1.0f, 0.0f);
    // -y
    constexpr Normal Bottom(0.0f, -1.0f, 0.0f);
}

std::array<GLfloat, 27> construct_triangle(Point &first, Point &second, Point &third, Normal &normal, bool &upper_triangle, float &lighting)
{
    // counter clockwise

    // tex coordinates switch
    if (upper_triangle)
    {
        /*
        F T
        S
         */
        return std::array<GLfloat, 27>{
            first.coordinates[0],
            first.coordinates[1],
            first.coordinates[2],
            normal.coordinates[0],
            normal.coordinates[1],
            normal.coordinates[2],
            1.0f,
            0.0f,
            lighting,

            second.coordinates[0],
            second.coordinates[1],
            second.coordinates[2],
            normal.coordinates[0],
            normal.coordinates[1],
            normal.coordinates[2],
            0.0f,
            0.0f,
            lighting,

            third.coordinates[0],
            third.coordinates[1],
            third.coordinates[2],
            normal.coordinates[0],
            normal.coordinates[1],
            normal.coordinates[2],
            1.0f,
            1.0f,
            lighting,
        };
    }
    else
    {
        /*
          F
        S T
         */
        return std::array<GLfloat, 27>{
            first.coordinates[0],
            first.coordinates[1],
            first.coordinates[2],
            normal.coordinates[0],
            normal.coordinates[1],
            normal.coordinates[2],
            1.0f,
            1.0f,
            lighting,

            second.coordinates[0],
            second.coordinates[1],
            second.coordinates[2],
            normal.coordinates[0],
            normal.coordinates[1],
            normal.coordinates[2],
            0.0f,
            0.0f,
            lighting,

            third.coordinates[0],
            third.coordinates[1],
            third.coordinates[2],
            normal.coordinates[0],
            normal.coordinates[1],
            normal.coordinates[2],
            1.0f,
            0.0f,
            lighting,
        };
    }
}

struct Faces
{
    // std::array<GLfloat, 9> back_face(Points::F[0], Points::F[1], Points::F[2]);
};

/*
GLfloat vertices[] = {
    // back face
    0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.8f,   // F
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.8f,  // H
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.8f, // G

    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.8f,  // E
    0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.8f,   // F
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.8f, // G

    // front face
    0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.8f,   // B
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.8f, // C
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.8f,  // D

    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.8f,  // A
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.8f, // C
    0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.8f,   // B

    // left face
    -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.6f,   // A
    -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.6f,  // E
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.6f, // G

    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.6f, // G
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.6f,  // C
    -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.6f,   // A

    // right face
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.6f,   // B
    0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.6f, // H
    0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.6f,  // F

    0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.6f,   // B
    0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.6f,  // D
    0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.6f, // H

    // bottom face
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.2f, // G
    0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.2f,  // H
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.2f,   // D

    0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.2f,   // D
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.2f,  // C
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.2f, // G

    // top face
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 1.0f, // E
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f,   // B
    0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f,  // F

    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 1.0f, // E
    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,  // A
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f,   // B
};
*/
// struct Face

struct Cube
{
    std::array<float, 57> upper_face;
};
/*
void compute_noise_attributes(FastNoiseLite *noise, int base_x, int base_z, int chunk_size)
{
    /*

    (haut
    |
    bas)

    x
    ^
    |
    |__> z (gauche - droite)

    filling:
    |___> z
    |
    x

    LC
     BN
    C: current
    L: left
    B: bottom
    N: next bottom


// Gather noise data
std::vector<std::vector<int>> height_map_extended;

std::vector<float> mesh_vertex_buffer;   // std::vector<std::array<float, 9>>
std::vector<float> mesh_triangle_buffer; // std::vector<std::array<float, 3>>

int range = Constants::HEIGHT_RANGE;
int average = (range + 1) / 2;

int max_z = chunk_size + base_z;
int max_x = chunk_size + base_x;
// int x = base_x;

for (int x = base_x - 1; x <= max_x; x++)
{
    std::vector<int> z_axis_noise;
    for (int z = base_z - 1; z <= max_z; z++)
    {
        float raw_noise = noise->GetNoise((float)x, (float)z);
        int noise_at = std::max(0, std::min(static_cast<int>(raw_noise * average + average - 1), range));
        z_axis_noise.push_back(noise_at);
    }
    height_map_extended.push_back(z_axis_noise);
}

std::vector<std::vector<std::vector<unsigned int>>> vertex_location;

/*
Traitement des coins d'abord:

x
^
|
|
--x - x -->
 / - / -/
x - x > z



for (int x = base_x; x < max_x; x++)
{
    for (int z = base_z; z < max_z; z++)
    {
        int current_noise = height_map_extended[x + 1][z + 1];

        int north_noise = height_map_extended[x + 2][z + 1];
        mesh_vertex_buffer.size();
        mesh_vertex_buffer.push_back(glm::vec3(x - 0.5f, current_noise + 0.5f, z - 0.5f))
            mesh_vertex_buffer.push_back(glm::vec3(x - 0.5f, current_noise + 0.5f, z - 0.5f))
                mesh_vertex_buffer.push_back(glm::vec3(x - 0.5f, current_noise + 0.5f, z - 0.5f))
    }
}

// DEBUG
//  noiseData = std::vector<glm::vec3>{ glm::vec3{0,0,0} };
// noiseData = std::vector<glm::vec3>{ noiseData[0] };
return noiseData;
}
*/

void _add_west(std::vector<GLfloat> &mesh_vertex_buffer, std::vector<GLuint> &mesh_triangle_buffer, std::array<GLint, 3> vertice, std::array<GLfloat, 3> normal, GLfloat lighting)
{
    unsigned int starting_index = mesh_vertex_buffer.size() / 9;

    // top north

    // vertice coordinates
    mesh_vertex_buffer.push_back(vertice[0] + 0.5f); // x - 0.5f);
    mesh_vertex_buffer.push_back(vertice[1] + 0.5f); // current_noise + 0.5f);
    mesh_vertex_buffer.push_back(vertice[2] - 0.5f); // z - 0.5f);
    // normal
    mesh_vertex_buffer.push_back(normal[0]); //-1.0f);
    mesh_vertex_buffer.push_back(normal[1]);
    mesh_vertex_buffer.push_back(normal[2]);
    // tex coordinates
    mesh_vertex_buffer.push_back(0.0f);
    mesh_vertex_buffer.push_back(1.0f);
    // lighting
    mesh_vertex_buffer.push_back(lighting);

    // top south

    // vertice coordinates
    mesh_vertex_buffer.push_back(vertice[0] - 0.5f);
    mesh_vertex_buffer.push_back(vertice[1] + 0.5f);
    mesh_vertex_buffer.push_back(vertice[2] - 0.5f);
    // normal
    mesh_vertex_buffer.push_back(normal[0]);
    mesh_vertex_buffer.push_back(normal[1]);
    mesh_vertex_buffer.push_back(normal[2]);
    // tex coordinates
    mesh_vertex_buffer.push_back(1.0f);
    mesh_vertex_buffer.push_back(1.0f);
    // lighting
    mesh_vertex_buffer.push_back(lighting);

    // bottom north

    // vertice coordinates
    mesh_vertex_buffer.push_back(vertice[0] + 0.5f);
    mesh_vertex_buffer.push_back(vertice[1] - 0.5f);
    mesh_vertex_buffer.push_back(vertice[2] - 0.5f);
    // normal
    mesh_vertex_buffer.push_back(normal[0]);
    mesh_vertex_buffer.push_back(normal[1]);
    mesh_vertex_buffer.push_back(normal[2]);
    // tex coordinates
    mesh_vertex_buffer.push_back(0.0f);
    mesh_vertex_buffer.push_back(0.0f);
    // lighting
    mesh_vertex_buffer.push_back(lighting);

    // bottom south

    // vertice coordinates
    mesh_vertex_buffer.push_back(vertice[0] - 0.5f);
    mesh_vertex_buffer.push_back(vertice[1] - 0.5f);
    mesh_vertex_buffer.push_back(vertice[2] - 0.5f);
    // normal
    mesh_vertex_buffer.push_back(normal[0]);
    mesh_vertex_buffer.push_back(normal[1]);
    mesh_vertex_buffer.push_back(normal[2]);
    // tex coordinates
    mesh_vertex_buffer.push_back(1.0f);
    mesh_vertex_buffer.push_back(0.0f);
    // lighting
    mesh_vertex_buffer.push_back(lighting);

    mesh_triangle_buffer.push_back(starting_index + 0);
    mesh_triangle_buffer.push_back(starting_index + 2);
    mesh_triangle_buffer.push_back(starting_index + 1);

    mesh_triangle_buffer.push_back(starting_index + 1);
    mesh_triangle_buffer.push_back(starting_index + 2);
    mesh_triangle_buffer.push_back(starting_index + 3);
}

void _add_south(std::vector<GLfloat> &mesh_vertex_buffer, std::vector<GLuint> &mesh_triangle_buffer, std::array<GLint, 3> vertice, std::array<GLfloat, 3> normal, GLfloat lighting)
{
    unsigned int starting_index = mesh_vertex_buffer.size() / 9;

    // top left

    // vertice coordinates
    mesh_vertex_buffer.push_back(vertice[0] - 0.5f); // x - 0.5f);
    mesh_vertex_buffer.push_back(vertice[1] + 0.5f); // current_noise + 0.5f);
    mesh_vertex_buffer.push_back(vertice[2] - 0.5f); // z - 0.5f);
    // normal
    mesh_vertex_buffer.push_back(normal[0]); //-1.0f);
    mesh_vertex_buffer.push_back(normal[1]);
    mesh_vertex_buffer.push_back(normal[2]);
    // tex coordinates
    mesh_vertex_buffer.push_back(0.0f);
    mesh_vertex_buffer.push_back(1.0f);
    // lighting
    mesh_vertex_buffer.push_back(lighting);

    // top right

    // vertice coordinates
    mesh_vertex_buffer.push_back(vertice[0] - 0.5f);
    mesh_vertex_buffer.push_back(vertice[1] + 0.5f);
    mesh_vertex_buffer.push_back(vertice[2] + 0.5f);
    // normal
    mesh_vertex_buffer.push_back(normal[0]);
    mesh_vertex_buffer.push_back(normal[1]);
    mesh_vertex_buffer.push_back(normal[2]);
    // tex coordinates
    mesh_vertex_buffer.push_back(1.0f);
    mesh_vertex_buffer.push_back(1.0f);
    // lighting
    mesh_vertex_buffer.push_back(lighting);

    // bottom left

    // vertice coordinates
    mesh_vertex_buffer.push_back(vertice[0] - 0.5f);
    mesh_vertex_buffer.push_back(vertice[1] - 0.5f);
    mesh_vertex_buffer.push_back(vertice[2] - 0.5f);
    // normal
    mesh_vertex_buffer.push_back(normal[0]);
    mesh_vertex_buffer.push_back(normal[1]);
    mesh_vertex_buffer.push_back(normal[2]);
    // tex coordinates
    mesh_vertex_buffer.push_back(0.0f);
    mesh_vertex_buffer.push_back(0.0f);
    // lighting
    mesh_vertex_buffer.push_back(lighting);

    // bottom right

    // vertice coordinates
    mesh_vertex_buffer.push_back(vertice[0] - 0.5f);
    mesh_vertex_buffer.push_back(vertice[1] - 0.5f);
    mesh_vertex_buffer.push_back(vertice[2] + 0.5f);
    // normal
    mesh_vertex_buffer.push_back(normal[0]);
    mesh_vertex_buffer.push_back(normal[1]);
    mesh_vertex_buffer.push_back(normal[2]);
    // tex coordinates
    mesh_vertex_buffer.push_back(1.0f);
    mesh_vertex_buffer.push_back(0.0f);
    // lighting
    mesh_vertex_buffer.push_back(lighting);

    mesh_triangle_buffer.push_back(starting_index + 0);
    mesh_triangle_buffer.push_back(starting_index + 2);
    mesh_triangle_buffer.push_back(starting_index + 1);

    mesh_triangle_buffer.push_back(starting_index + 1);
    mesh_triangle_buffer.push_back(starting_index + 2);
    mesh_triangle_buffer.push_back(starting_index + 3);
}

void _add_top(std::vector<GLfloat> &mesh_vertex_buffer, std::vector<GLuint> &mesh_triangle_buffer, std::array<GLint, 3> vertice, std::array<GLfloat, 3> normal, GLfloat lighting)
{
    unsigned int starting_index = mesh_vertex_buffer.size() / 9;

    // north-west

    // vertice coordinates
    mesh_vertex_buffer.push_back(vertice[0] + 0.5f); // x - 0.5f);
    mesh_vertex_buffer.push_back(vertice[1] + 0.5f); // current_noise + 0.5f);
    mesh_vertex_buffer.push_back(vertice[2] - 0.5f); // z - 0.5f);
    // normal
    mesh_vertex_buffer.push_back(normal[0]); //-1.0f);
    mesh_vertex_buffer.push_back(normal[1]);
    mesh_vertex_buffer.push_back(normal[2]);
    // tex coordinates
    mesh_vertex_buffer.push_back(0.0f);
    mesh_vertex_buffer.push_back(1.0f);
    // lighting
    mesh_vertex_buffer.push_back(lighting);

    // north-east

    // vertice coordinates
    mesh_vertex_buffer.push_back(vertice[0] + 0.5f);
    mesh_vertex_buffer.push_back(vertice[1] + 0.5f);
    mesh_vertex_buffer.push_back(vertice[2] + 0.5f);
    // normal
    mesh_vertex_buffer.push_back(normal[0]);
    mesh_vertex_buffer.push_back(normal[1]);
    mesh_vertex_buffer.push_back(normal[2]);
    // tex coordinates
    mesh_vertex_buffer.push_back(1.0f);
    mesh_vertex_buffer.push_back(1.0f);
    // lighting
    mesh_vertex_buffer.push_back(lighting);

    // south-west

    // vertice coordinates
    mesh_vertex_buffer.push_back(vertice[0] - 0.5f);
    mesh_vertex_buffer.push_back(vertice[1] + 0.5f);
    mesh_vertex_buffer.push_back(vertice[2] - 0.5f);
    // normal
    mesh_vertex_buffer.push_back(normal[0]);
    mesh_vertex_buffer.push_back(normal[1]);
    mesh_vertex_buffer.push_back(normal[2]);
    // tex coordinates
    mesh_vertex_buffer.push_back(0.0f);
    mesh_vertex_buffer.push_back(0.0f);
    // lighting
    mesh_vertex_buffer.push_back(lighting);

    // south-east

    // vertice coordinates
    mesh_vertex_buffer.push_back(vertice[0] - 0.5f);
    mesh_vertex_buffer.push_back(vertice[1] + 0.5f);
    mesh_vertex_buffer.push_back(vertice[2] + 0.5f);
    // normal
    mesh_vertex_buffer.push_back(normal[0]);
    mesh_vertex_buffer.push_back(normal[1]);
    mesh_vertex_buffer.push_back(normal[2]);
    // tex coordinates
    mesh_vertex_buffer.push_back(1.0f);
    mesh_vertex_buffer.push_back(0.0f);
    // lighting
    mesh_vertex_buffer.push_back(lighting);

    mesh_triangle_buffer.push_back(starting_index + 0);
    mesh_triangle_buffer.push_back(starting_index + 2);
    mesh_triangle_buffer.push_back(starting_index + 1);

    mesh_triangle_buffer.push_back(starting_index + 1);
    mesh_triangle_buffer.push_back(starting_index + 2);
    mesh_triangle_buffer.push_back(starting_index + 3);
}

void compute_noise_attributes_v_brute_force(FastNoiseLite *noise, int base_x, int base_z, int chunk_size, std::vector<GLfloat> &mesh_vertex_buffer, std::vector<GLuint> &mesh_triangle_buffer, std::vector<std::array<int, 3>> &cubes_positions, std::vector<std::vector<int>> &height_map_extended)
{
    /*

    (haut
    |
    bas)

    x
    ^
    |
    |__> z (gauche - droite)

    filling:
    |___> z
    |
    x

    LC
     BN
    C: current
    L: left
    B: bottom
    N: next bottom
    */

    // Gather noise data

    // std::vector<float> mesh_vertex_buffer;   // std::vector<std::array<float, 9>>
    // std::vector<float> mesh_triangle_buffer; // std::vector<std::array<float, 3>>
    // std::array<std::vector<float>, 2> res;
    mesh_vertex_buffer = std::vector<GLfloat>();
    mesh_triangle_buffer = std::vector<GLuint>();
    int range = Constants::HEIGHT_RANGE;
    int average = (range + 1) / 2;

    int max_z = chunk_size + base_z;
    int max_x = chunk_size + base_x;
    // int x = base_x;

    for (int x = base_x - 1; x <= max_x; x++)
    {
        std::vector<int> z_axis_noise;
        for (int z = base_z - 1; z <= max_z; z++)
        {
            float raw_noise = noise->GetNoise((float)x, (float)z);
            int noise_at = std::max(0, std::min(static_cast<int>(raw_noise * average + average - 1), range));
            z_axis_noise.push_back(noise_at);
        }
        height_map_extended.push_back(z_axis_noise);
    }

    std::vector<std::vector<std::vector<unsigned int>>> vertex_location;

    /*
    Traitement des coins d'abord:

    x
    ^
    |
    |
    --x - x -->
     / - / -/
    x - x > z

    */

    for (int x = base_x; x < max_x; x++) // max_x
    {
        for (int z = base_z; z < max_z; z++) // max_z
        {
            int current_noise = height_map_extended[x + 1 + chunk_size / 2][z + 1 + chunk_size / 2];

            int south_noise = height_map_extended[x + 0 + chunk_size / 2][z + 1 + chunk_size / 2];
            int west_noise = height_map_extended[x + 1 + chunk_size / 2][z + 0 + chunk_size / 2];
            unsigned int starting_index = mesh_vertex_buffer.size() / 9;

            cubes_positions.push_back({x, current_noise, z});

            // Get where the highest face should be and fill if needed
            int delta_noise = current_noise - south_noise;
            _add_top(mesh_vertex_buffer, mesh_triangle_buffer, std::array<GLint, 3>{x, current_noise, z}, std::array<GLfloat, 3>{0.0f, 1.0f, 0.0f}, 1.0f);
            if (delta_noise == 0)
            {
            }
            else
            {
                if (delta_noise > 0)
                {
                    for (int i = 0; i < delta_noise; i++)
                    {
                        _add_south(mesh_vertex_buffer, mesh_triangle_buffer, std::array<GLint, 3>{x, current_noise - i, z}, std::array<GLfloat, 3>{-1.0f, 0.0f, 0.0f}, 0.8f);
                    }
                }
                else // delta_noise < 0
                {
                    for (int i = 0; i < -delta_noise; i++)
                    {
                        _add_south(mesh_vertex_buffer, mesh_triangle_buffer, std::array<GLint, 3>{x, current_noise + 1 + i, z}, std::array<GLfloat, 3>{1.0f, 0.0f, 0.0f}, 0.8f);
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
                        _add_west(mesh_vertex_buffer, mesh_triangle_buffer, std::array<GLint, 3>{x, current_noise - i, z}, std::array<GLfloat, 3>{0.0f, 0.0f, -1.0f}, 0.6f);
                    }
                }
                else // delta_noise < 0
                {
                    for (int i = 0; i < -delta_noise; i++)
                    {
                        _add_west(mesh_vertex_buffer, mesh_triangle_buffer, std::array<GLint, 3>{x, current_noise + 1 + i, z}, std::array<GLfloat, 3>{0.0f, 0.0f, 1.0f}, 0.6f);
                    }
                }
            }
        }
    }
}
// DEBUG
// noiseData = std::vector<glm::vec3>{ glm::vec3{0,0,0} };
// noiseData = std::vector<glm::vec3>{ noiseData[0] };

/*
std::array<GLfloat, 3> Points::G()
{
    return std::array<GLfloat, 3>();
}
*/