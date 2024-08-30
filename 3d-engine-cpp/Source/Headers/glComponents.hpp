#include <vector>
#include <array>
#include <optional>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image.h>

#include "constants.hpp"
#include "shader.hpp"

/* Handles OpenGl related rendering features */
class GlComponents
{
public:
    GlComponents();

    void initialise_components();

    void gl_init_buffers(const std::vector<GLfloat> &vertices, const std::vector<GLuint> &packs, const std::vector<GLuint> &triangles);

    void gl_update_buffers(std::vector<glm::vec3> &data);

    ~GlComponents();
    GLFWwindow *initWindow();
    void render(GLuint &VAO, const glm::mat4 &view, const unsigned int &elements_ammount);
    std::vector<GLuint> m_vertex_arrays;
    static void framebuffer_size_callback(GLFWwindow *window, int width, int height);

private:
    std::vector<GLuint> m_buffers;
    std::vector<GLuint> m_textures;
    std::optional<Shader> m_shader;
    void _loadVAO(const std::vector<GLfloat> &vertices, const std::vector<GLuint> &packs, const std::vector<GLuint> &triangles);
    std::array<GLuint, 2> _init_buffers(Shader &shader, std::vector<glm::vec3> &cubePositions);
    void _update_buffers(Shader &shader, std::vector<glm::vec3> &data, std::array<GLuint, 2> &buffers);

    void _loadTexture();
};
