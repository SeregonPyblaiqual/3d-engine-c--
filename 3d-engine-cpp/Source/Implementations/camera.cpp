#include "camera.hpp"

Camera::Camera() : m_speed_multiplier(1.0f), m_yaw(-90.0f), m_pitch(0.0f)
{
    m_camera_pos = glm::vec3(0, 0, 0);
    m_camera_integer_pos = std::array<int, 3>{0, 0, 0};
    m_camera_floored_pos = glm::vec3(0, 0, 0);

    m_camera_front = glm::vec3(1, 0, 0);
}
Camera::~Camera() {}

glm::vec3 *Camera::get_floored_pos()
{
    return &(m_camera_floored_pos);
}

std::array<int, 3> *Camera::get_integer_pos()
{
    return &m_camera_integer_pos;
}

bool Camera::compute_camera_floored_position()
{
    glm::vec3 camera_floored;
    camera_floored[0] = std::floor(m_camera_pos[0]);
    camera_floored[1] = std::floor(m_camera_pos[1]);
    camera_floored[2] = std::floor(m_camera_pos[2]);
    if (camera_floored != m_camera_floored_pos)
    {
        m_camera_floored_pos = camera_floored;
        return true;
    }
    return false;
}

struct Ray
{
    float origin[3];
    float dir[3];
    float inv_dir[3];
};

struct AABB
{
    float mint[3];
    float maxt[3];
};

bool Camera::_intersect(std::array<int, 3> &cube_pos)
{
    // Initialise parameters

    Ray ray;

    ray.origin[0] = m_camera_pos[0];
    ray.origin[1] = m_camera_pos[1];
    ray.origin[2] = m_camera_pos[2];

    ray.dir[0] = m_camera_pos[0];
    ray.dir[1] = m_camera_pos[1];
    ray.dir[2] = m_camera_pos[2];

    ray.inv_dir[0] = 1 / ray.dir[0];
    ray.inv_dir[1] = 1 / ray.dir[1];
    ray.inv_dir[2] = 1 / ray.dir[2];

    AABB aabb;

    aabb.mint[0] = static_cast<float>(cube_pos[0]) - 0.5f;
    aabb.mint[1] = static_cast<float>(cube_pos[1]) - 0.5f;
    aabb.mint[2] = static_cast<float>(cube_pos[2]) - 0.5f;

    aabb.maxt[0] = static_cast<float>(cube_pos[0]) + 0.5f;
    aabb.maxt[1] = static_cast<float>(cube_pos[1]) + 0.5f;
    aabb.maxt[2] = static_cast<float>(cube_pos[2]) + 0.5f;

    // Calculations

    float tmin = 0.0, tmax = INFINITY;

    for (int d = 0; d < 3; ++d)
    {
        float t1 = (aabb.mint[d] - ray.origin[d]) * ray.inv_dir[d];
        float t2 = (aabb.maxt[d] - ray.origin[d]) * ray.inv_dir[d];

        tmin = std::max(tmin, std::min(t1, t2));
        tmax = std::min(tmax, std::max(t1, t2));
    }

    return tmin < tmax;
}

std::optional<glm::vec3> Camera::_facing_cube(std::vector<std::array<int, 3>> &cubePositions)
{
    float min_dist = std::numeric_limits<float>::infinity();
    glm::vec3 min_pos;
    for (auto pos : cubePositions)
    {
        if (_intersect(pos))
        {
            float dist = std::pow(pos[0], 2) + std::pow(pos[1], 2) + std::pow(pos[2], 2);
            if (dist < min_dist)
            {
                min_dist = dist;
                min_pos = glm::vec3(pos[0], pos[1], pos[2]);
            }
        }
    }
    // return std::optional<glm::vec3>{}
    if (min_dist != std::numeric_limits<float>::infinity())
    {
        return std::optional<glm::vec3>{min_pos};
    }
    return std::nullopt;
}

bool Camera::compute_facing_cube(std::vector<std::array<int, 3>> &data)
{

    std::optional<glm::vec3> cube = _facing_cube(data);
    if (m_facing_cube != cube)
    {
        m_facing_cube = cube;
        return true;
    }
    return false;
}
// update->plane
void Camera::update_front_manner(float &delta_time)
{
    m_camera_pos += M_CAMERA_SPEED * delta_time * m_camera_front * m_speed_multiplier;
}
void Camera::update_ortho_front_manner(float &delta_time)
{
    m_camera_pos += m_camera_ortho_front * M_CAMERA_SPEED * delta_time * m_speed_multiplier;
}
void Camera::update_up_manner(float &delta_time)
{
    m_camera_pos += M_CAMERA_SPEED * delta_time * M_CAMERA_UP * m_speed_multiplier;
}
void Camera::update_west_manner(float &delta_time)
{
    m_camera_pos += M_CAMERA_SPEED * delta_time * m_camera_west * m_speed_multiplier;
}
void Camera::update_north_manner(float &delta_time)
{
    m_camera_pos += M_CAMERA_SPEED * delta_time * m_camera_plane_front * m_speed_multiplier;
}

void Camera::update_angles(std::array<float, 2> &offset)
{
    m_yaw += offset[0] * M_SENSITIVITY;
    m_pitch += offset[1] * M_SENSITIVITY;

    if (m_pitch > 89.0f)
        m_pitch = 89.0f;
    if (m_pitch < -89.0f)
        m_pitch = -89.0f;

    // ! missing some delta_time here ?
    glm::vec3 direction;
    direction.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    direction.y = sin(glm::radians(m_pitch));
    direction.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

    m_camera_front = glm::normalize(direction);
    // std::cout << m_camera_front[0] << "," << m_camera_front[1] << "," << m_camera_front[2] << std::endl;
    glm::vec3 camera_plane_front = m_camera_front;
    camera_plane_front[1] = 0.0f;
    // ? over normalisation ?
    m_camera_plane_front = glm::normalize(camera_plane_front);
    // ! equal vectors...
    m_camera_ortho_front = glm::normalize(glm::cross(m_camera_front, M_CAMERA_UP));
    m_camera_west = glm::normalize(glm::cross(camera_plane_front, M_CAMERA_UP));
}

void Camera::increase_speed()
{
    m_speed_multiplier = 10.0f;
}
void Camera::lower_speed()
{
    m_speed_multiplier = 1.0f;
}

glm::mat4 Camera::compute_view_matrix()
{
    return glm::lookAt(m_camera_pos, m_camera_pos + m_camera_front, M_CAMERA_UP);
}

void Camera::set_position(glm::vec3 &new_pos)
{
    m_camera_pos = new_pos;
    glm::vec3 new_floored_pos;
    std::array<int, 3> new_int_pos;
    for (unsigned int i = 0; i < 3; i++)
    {
        new_floored_pos[i] = std::floor(m_camera_pos[i]);
        new_int_pos[i] = static_cast<int>(new_floored_pos[i]);
    }
    m_camera_floored_pos = new_floored_pos;
    m_camera_integer_pos = new_int_pos;
}