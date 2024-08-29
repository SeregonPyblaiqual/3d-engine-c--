#include <vector>
#include <array>
#include <iomanip> // std::setprecision

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <FastNoiseLite/FastNoiseLite.h>

#include "noise.hpp"
#include "glComponents.hpp"
#include "gameComponents.hpp"

class Application
{
public:
    Application();
    ~Application();

    int mainloop();

private:
    double m_last_time;
    GLFWwindow *m_window;
    GlComponents m_glcomponent;
    GameComponents m_game_components;
    unsigned int m_frame_count;
    std::array<float, 2> m_last_coordinates;
    bool m_first_mouse;
    void _setup_spawn_point();
    void _show_UI();

    glm::mat4 _processInput(float &delta_time);
    static void _mouse_callback_static(GLFWwindow *window, double xpos, double ypos);
    void _mouse_callback(double xpos, double ypos);
    void _showFPS(double &lastTime, unsigned int &frameCount);
};