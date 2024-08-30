#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::lookAt
#include <vector>
#include <array>
#include <optional>

#include <cmath> // std::floor
#include <iostream>

class Camera
{
public:
    Camera();
    ~Camera();

    glm::vec3 *get_floored_pos();
    std::array<int, 3> *get_integer_pos();

    void set_position(glm::vec3 &new_position);

    /*true if updated*/
    bool compute_camera_floored_position();
    bool compute_facing_cube(std::vector<std::array<int, 3>> &data);

    // update->plane

    void update_front_manner(const float &delta_time);
    void update_ortho_front_manner(const float &delta_time);
    void update_up_manner(const float &delta_time);
    void update_west_manner(const float &delta_time);
    void update_north_manner(const float &delta_time);
    void update_angles(const std::array<float, 2> &offset);

    void increase_speed();
    void lower_speed();

    glm::mat4 compute_view_matrix();

private:
    const float M_CAMERA_SPEED = 2.5f;
    float m_pitch;
    float m_yaw;
    const float M_SENSITIVITY = 0.1f;
    glm::vec3 m_camera_front;
    glm::vec3 m_camera_plane_front;
    glm::vec3 m_camera_ortho_front;
    glm::vec3 m_camera_pos;
    glm::vec3 m_camera_floored_pos;
    std::array<int, 3> m_camera_integer_pos;
    const glm::vec3 M_CAMERA_UP = {0, 1, 0};
    glm::vec3 m_camera_west;
    glm::mat4 view;
    float yaw;
    std::optional<glm::vec3> m_facing_cube;
    float m_speed_multiplier;

    bool _intersect(std::array<int, 3> &cube_pos);
    std::optional<glm::vec3> _facing_cube(std::vector<std::array<int, 3>> &cubePositions);
};