#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "fileUtilities.hpp"

std::vector<glm::vec3> read_map()
{
    std::vector<glm::vec3> cubes_coordinates;

    char source_path[] = "./Assets/map.txt";
    std::ifstream map_file(source_path);
    if (!map_file.is_open())
    {
        throw std::runtime_error("Could not open map file!");
    }

    std::string cube_coordinates_str;
    std::string cube_coordinate_str;

    while (std::getline(map_file, cube_coordinates_str))
    {
        std::stringstream cube_coordinates_ss(cube_coordinates_str);
        std::vector<float> cube_coordinates;
        while (std::getline(cube_coordinates_ss, cube_coordinate_str, ','))
        {
            cube_coordinates.push_back(std::stof(cube_coordinate_str));
        }
        if (cube_coordinates.size() == 3)
            cubes_coordinates.push_back(glm::vec3(cube_coordinates[0], cube_coordinates[1], cube_coordinates[2]));
        else
        {
            throw std::runtime_error("Invalid file format!");
        }
    }
    map_file.close();
    return cubes_coordinates;
}

std::vector<glm::vec3> image_to_map(const char *source_path)
{
    std::vector<glm::vec3> cubes_coordinates;

    // char source_path[] = "outputs/test1718992648-4-0.200000-30.png";//output2D1718992648 - 8 - 2.000000 - 20.png"
    std::ifstream map_file(source_path);
    if (!map_file.is_open())
    {
        throw std::runtime_error("Could not open map file!");
    }
    int width, height, nrChannels;
    unsigned char *data = stbi_load(source_path, &width, &height, &nrChannels, 0);

    for (int linen = 0; linen < height; linen++)
    {
        for (int coln = 0; coln < width; coln++)
        {
            cubes_coordinates.push_back(glm::vec3(linen, data[(linen * width + coln) * 3], coln));
        }
    }
    stbi_image_free(data);
    return cubes_coordinates;
}

void int_matrix_to_image(std::vector<std::vector<int>> data, int size, char *target_path)
{
    // Image dimensions
    const int channels = 3; // RGB

    // Allocate memory for the image
    unsigned char *image = new unsigned char[size * size * channels];

    // Fill the image with a gradient
    for (int y = 0; y < size; ++y)
    {
        for (int x = 0; x < size; ++x)
        {
            int index = (y * size + x) * channels;
            int color = (data[y][x]);
            image[index + 0] = static_cast<unsigned char>(color); // Red channel
            image[index + 1] = static_cast<unsigned char>(color); // Green channel
            image[index + 2] = static_cast<unsigned char>(color); // Blue
        }
    }
    if (stbi_write_png(target_path, size, size, channels, image, size * channels))
    {
        // std::cout << "Image written successfully to output.png" << std::endl;
    }
    else
    {
        std::cerr << "Failed to write image" << std::endl;
    }

    // Free the allocated memory
    delete[] image;
}
