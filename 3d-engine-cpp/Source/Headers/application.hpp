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
    void initialiseApplication();
    int mainloop();

private:
    // FPS related
    // ===========
    double m_last_time;
    unsigned int m_frame_counter;

    // Application "backend" related
    // =============================

    GLFWwindow *m_window;
    GlComponents m_glcomponent;
    GameComponents m_game_components;
    bool m_first_mouse;
    glm::mat4 _processInput(float &delta_time);
    static void _mouse_callback_static(GLFWwindow *window, double xpos, double ypos);
    void _mouse_callback(double xpos, double ypos);

    // UI related
    // ==========

    std::array<float, 2> m_last_coordinates;
    void _show_UI();
    void _showFPS(double &lastTime, unsigned int &frameCount);
};