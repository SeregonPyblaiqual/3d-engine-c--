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

/* Responsible for handling inputs and displaying information */
class Application
{
public:
    Application();
    ~Application();
    int mainloop();

private:

    // Application "backend" related
    // =============================

    GLFWwindow *m_window;
    GlComponents m_glcomponent;
    GameComponents m_game_components;
    bool m_first_mouse = true;
    bool m_first_input = true;
    // Tries to prevent sudden movement when mouse is first detected by application
    // ! Not working yet

    glm::mat4 _processInput(const float &delta_time);
    static void _mouse_callback_static(GLFWwindow *window, double xpos, double ypos);
    void _mouse_callback(double xpos, double ypos);

    // UI related
    // ==========

    std::array<float, 2> m_last_coordinates;
    void _show_UI(float &last_time, unsigned int &frame_counter);
    std::optional<float> _compute_fps(float &last_time, unsigned int &frame_counter);
};