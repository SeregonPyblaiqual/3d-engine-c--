#include <array>

#include <glm/glm.hpp>

#include "camera.hpp"
#include "mapManager.hpp"

/* Handles game elements */
class GameComponents
{
public:
    GameComponents();
    ~GameComponents();
    void initialise_game_components();
    glm::vec3 m_last_facing_cube_pos;
    Camera m_camera;
    MapManager m_map_manager;

private:
};
