#include "application.hpp"

Application::Application() : m_glcomponent(),     // does nothing
                             m_game_components(), // builds the map mesh and centers the camera above ground
                             m_frame_counter(0),

                             m_first_mouse(true)
// Tries to prevent sudden movement when mouse is first detected by application
// ! Not working yet
{
    // UI related variables: only display new positions
    m_last_coordinates[0] = Constants::WINDOW_WIDTH / 2;
    m_last_coordinates[1] = Constants::WINDOW_HEIGHT / 2;
}
Application::~Application() {}
void Application::initialiseApplication()
{
    m_window = m_glcomponent.initWindow();
    m_glcomponent.initialise_components();
    m_glcomponent.gl_init_buffers(m_game_components.m_map_manager.m_mesh_vertex, m_game_components.m_map_manager.m_mesh_triangle);

    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetWindowUserPointer(m_window, this);
    glfwSetCursorPosCallback(m_window, Application::_mouse_callback_static);
}
int Application::mainloop()
{

    // Opengl initialisation
    // =====================

    // Variables for FPS calculation
    float last_time = float(glfwGetTime()), delta_time;
    int frameCount = 0;

    glm::vec3 last_cube, last_camera_pos;
    while (!glfwWindowShouldClose(m_window))
    {
        // m_data = gl_locations_noise(&noise, -Constants::MAP_SIZE / 2 + cameraPlanePos[0], -Constants::MAP_SIZE / 2 + cameraPlanePos[1], Constants::MAP_SIZE); // gradient_noise5(1718992648, Constants::MAP_SIZE, 0, 0, 16);
        //  - 30 fps ... (/ 45) 256 map size 1800x1000 screen size
        // gl_update_buffers(data)

        // buffers.push_back(buffer_ids[0]);
        // textures.push_back(buffer_ids[1]);

        float current_frame = float(glfwGetTime());
        delta_time = current_frame - last_time;
        last_time = current_frame;

        glm::mat4 view = _processInput(delta_time);
        // rendering
        // m_glcomponent.glRender(view, m_game_components.m_map_manager.m_data);
        // m_game_components.m_camera.compute_facing_cube(m_game_components.m_map_manager.m_cubes_positions);
        m_glcomponent._renderv2(m_glcomponent.m_vertex_arrays.back(), view, m_game_components.m_map_manager.m_mesh_triangle.size());
        _show_UI();

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }

    // glDeleteBuffers(1, )
    // glDeleteTextures

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwDestroyWindow(m_window);
    glfwTerminate();

    return 0;
}

void Application::_show_UI()
{
    if (m_game_components.m_camera.compute_camera_floored_position())
    {
        glm::vec3 *camera_plane_pos = m_game_components.m_camera.get_floored_pos();
        std::cout << "cameraPos: " << (*camera_plane_pos)[0] << "," << (*camera_plane_pos)[1] << "," << (*camera_plane_pos)[2] << std::endl;
    }
    std::optional<glm::vec3> fc = std::nullopt; // facing_cube(data);
    // - 400 fps (/542 fps 1800x1000 256 map_size)
    /*
    if (m_game_components.m_camera.m_facing_cube.has_value()) {
        glm::vec3 v = m_game_components.m_camera.m_facing_cube.value();
        if (v != m_game_components.m_camera.m_facing_cube) {
            //std::cout << "facing cube: " << v[0] << "," << v[1] << "," << v[2] << std::endl;
            last_cube = v;
        }
    }*/

    // Calculate delta time and FPS
    _showFPS(m_last_time, m_frame_counter);
}

glm::mat4 Application::_processInput(float &delta_time)
{
    float opposite_delta = -delta_time;
    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(m_window, true);

    if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
    {
        // int* plane_pos = m_game_components.m_camera.get_integer_pos();
        // m_data[Constants::MAP_SIZE * (plane_pos[0]+Constants::MAP_SIZE / 2)];
        m_game_components.m_camera.update_front_manner(delta_time);
    }
    if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
        m_game_components.m_camera.update_front_manner(opposite_delta);
    if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
        m_game_components.m_camera.update_ortho_front_manner(opposite_delta);
    if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
        m_game_components.m_camera.update_ortho_front_manner(delta_time);

    if (glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(m_window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
        m_game_components.m_camera.increase_speed();
    else
        m_game_components.m_camera.lower_speed();

    if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS)
        m_game_components.m_camera.update_up_manner(delta_time);
    if (glfwGetKey(m_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        m_game_components.m_camera.update_up_manner(opposite_delta);

    if (glfwGetKey(m_window, GLFW_KEY_UP) == GLFW_PRESS)
        m_game_components.m_camera.update_north_manner(delta_time);
    if (glfwGetKey(m_window, GLFW_KEY_DOWN) == GLFW_PRESS)
        m_game_components.m_camera.update_north_manner(opposite_delta);

    if (glfwGetKey(m_window, GLFW_KEY_LEFT) == GLFW_PRESS)
        m_game_components.m_camera.update_west_manner(opposite_delta);
    if (glfwGetKey(m_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        m_game_components.m_camera.update_west_manner(delta_time);

    return m_game_components.m_camera.compute_view_matrix();
}
void Application::_mouse_callback_static(GLFWwindow *window, double xpos, double ypos)
{
    Application *app = static_cast<Application *>(glfwGetWindowUserPointer(window));
    app->_mouse_callback(xpos, ypos);
}
void Application::_mouse_callback(double xpos, double ypos)
{
    std::array<float, 2> offset;

    offset[0] = xpos - m_last_coordinates[0];
    offset[1] = m_last_coordinates[1] - ypos; // reversed since y-coordinates range from bottom to top
    m_last_coordinates[0] = xpos;
    m_last_coordinates[1] = ypos;

    m_game_components.m_camera.update_angles(offset);

    if (m_first_mouse) // initially set to true
    {
        m_last_coordinates[0] = xpos;
        m_last_coordinates[1] = ypos;
        m_first_mouse = false;
    }
}

void Application::_showFPS(double &lastTime, unsigned int &frameCount)
{
    double currentTime = glfwGetTime();
    double deltaTime = currentTime - lastTime;
    frameCount++;
    std::stringstream titleStream("");
    if (deltaTime >= 1.0)
    {
        double fps = frameCount / deltaTime;
        titleStream << Constants::APPLICATION_NAME << " - FPS: " << std::fixed << std::setprecision(2) << fps;
        glfwSetWindowTitle(m_window, titleStream.str().c_str());
        frameCount = 0;
        lastTime = currentTime;
    }
}
