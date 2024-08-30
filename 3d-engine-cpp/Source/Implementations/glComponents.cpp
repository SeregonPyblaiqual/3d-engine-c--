#include "glComponents.hpp"

GlComponents::GlComponents()
{
}

void GlComponents::initialise_components()
{
    // Require glfw to initialise gl related objects...
    m_shader = std::optional<Shader>{Shader((Constants::SHADERS_LOCATION + std::string("/shader.vs")).c_str(),
                                            (Constants::SHADERS_LOCATION + std::string("/shader.fs")).c_str())};
    glPolygonMode(GL_FRONT_AND_BACK, Constants::RENDER_MODE);
    glViewport(0, 0, Constants::WINDOW_WIDTH, Constants::WINDOW_WIDTH);

    // Update viewport on window size change
    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE);
    //  glCullFace(GL_BACK);
    //  glFrontFace(GL_CCW);
}

void GlComponents::gl_init_buffers(const std::vector<GLfloat> &vertices, const std::vector<GLuint> &packs, const std::vector<GLuint> &triangles)
{

    _loadVAO(vertices, packs, triangles);
}

void GlComponents::gl_update_buffers(std::vector<glm::vec3> &data)
{
    std::array<GLuint, 2> buffer_ids = {m_buffers.back(), m_textures.back()};
    _update_buffers(m_shader.value(), data, buffer_ids);
}

GlComponents::~GlComponents()
{
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    for (auto vertex_array = m_vertex_arrays.begin(); vertex_array != m_vertex_arrays.end(); ++vertex_array)
    {
        GLuint va = *vertex_array;
        glDeleteVertexArrays(1, &va);
    }
    /*
    for (const auto vertex_array : vertex_arrays) {
        glDeleteVertexArrays(1, &vertex_array);
    }
    */
    for (auto buffer = m_buffers.begin(); buffer != m_buffers.end(); ++buffer)
    {
        GLuint b = *buffer;
        glDeleteVertexArrays(1, &b);
    }
    for (auto texture = m_textures.begin(); texture != m_textures.end(); ++texture)
    {
        GLuint t = *texture;
        glDeleteTextures(1, &t);
    }
    // glDeleteBuffers(1, &EBO);
    glDeleteProgram(m_shader.value().ID);
}

void GlComponents::_loadVAO(const std::vector<GLfloat> &vertices, const std::vector<GLuint> &packs, const std::vector<GLuint> &triangles)
{
    _loadTexture();

    GLuint VBO_vertices, VBO_packs, VAO, EBO;
    //, EBO;

    glGenVertexArrays(1, &VAO);
    m_vertex_arrays.push_back(VAO);
    glGenBuffers(1, &VBO_vertices);
    m_buffers.push_back(VBO_vertices);
    glGenBuffers(1, &VBO_packs);
    m_buffers.push_back(VBO_packs);
    glGenBuffers(1, &EBO);
    m_buffers.push_back(EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * triangles.size(), triangles.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_packs);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLuint) * packs.size(), packs.data(), GL_STATIC_DRAW);
    glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, 1 * sizeof(GLuint), (void *)(0));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

std::array<GLuint, 2> GlComponents::_init_buffers(Shader &shader, std::vector<glm::vec3> &cubePositions)
{
    // Use TBO to store cube location
    // ==============================

    std::array<GLuint, 2> res;

    // Create and fill Texture Buffer Object (TBO)
    // -------------------------------------------
    GLuint tbo;
    glGenBuffers(1, &tbo);
    res[0] = tbo;
    glBindBuffer(GL_TEXTURE_BUFFER, tbo);
    GLsizeiptr size = cubePositions.size() * sizeof(glm::vec3);
    glBufferData(GL_TEXTURE_BUFFER, size, cubePositions.data(), GL_STATIC_DRAW);

    // Create the texture and bind the TBO to it
    // -----------------------------------------
    GLuint tex;
    glGenTextures(1, &tex);
    res[1] = tex;
    glBindTexture(GL_TEXTURE_BUFFER, tex);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, tbo);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_BUFFER, tex);
    glUniform1i(glGetUniformLocation(shader.ID, "positions"), 0);
    return res;
}

void GlComponents::_update_buffers(Shader &shader, std::vector<glm::vec3> &data, std::array<GLuint, 2> &buffers)
{
    // glDeleteBuffers(1, &buffers[0]);
    // glDeleteTextures(1, &buffers[1]);
    // std::array<GLuint, 2> res;

    // Create and fill Texture Buffer Object (TBO)
    // -------------------------------------------
    glBindBuffer(GL_TEXTURE_BUFFER, buffers[0]);
    GLsizeiptr size = data.size() * sizeof(glm::vec3);
    glBufferData(GL_TEXTURE_BUFFER, size, data.data(), GL_STATIC_DRAW);

    // Create the texture and bind the TBO to it
    // -----------------------------------------
    glBindTexture(GL_TEXTURE_BUFFER, buffers[1]);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, buffers[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_BUFFER, buffers[1]);
    glUniform1i(glGetUniformLocation(shader.ID, "positions"), 0);
}

void GlComponents::render(GLuint &VAO, const glm::mat4 &view, const unsigned int &elements_ammount)
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Transformation
    // ==============

    glm::mat4 model = glm::mat4(1.0f);
    // model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
    //  ? note that we're translating the scene in the reverse direction of where we want to move
    //  glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    //  glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    //  glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);

    // glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    // glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));

    // glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

    const float radius = 10.0f;
    float camX = sin(glfwGetTime()) * radius;
    float camZ = cos(glfwGetTime()) * radius;

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), static_cast<float>(Constants::WINDOW_WIDTH) / static_cast<float>(Constants::WINDOW_HEIGHT), 0.1f, 2000.0f);
    Shader *shader = &(m_shader.value());
    shader->use();
    int modelLoc = glGetUniformLocation(shader->ID, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    int viewLoc = glGetUniformLocation(shader->ID, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]); // glm::value_ptr(view));
    int projectionLoc = glGetUniformLocation(shader->ID, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(VAO);
    // glDrawArraysInstanced(GL_TRIANGLES, 0, 36, cubePositions.size());
    //  glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, cubePositions.size());
    glDrawElements(GL_TRIANGLES, elements_ammount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    /*
    for (unsigned int i = 0; i < cubePositions.size(); i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, cubePositions[i]);
        //float angle = 20.0f * i;
        //model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    */

    // glDrawArrays(GL_TRIANGLES, 0, 3);
    // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    // glBindVertexArray(VAO);
    // glDrawArrays(GL_TRIANGLES, 0, 36);
}

void GlComponents::framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void GlComponents::_loadTexture()
{
    /*
    float texCoords[] = {
        0.0f, 0.0f,  // lower-left corner
        1.0f, 0.0f,  // lower-right corner
        0.5f, 1.0f   // top-center corner
    };*/

    unsigned int texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
    glBindTexture(GL_TEXTURE_2D, texture);

    // s, t, r <=> (x, y, z)
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    // float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
    // glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // , GL_LINEAR);

    int width, height, nrChannels;
    // unsigned char* data = stbi_load("grassx1000.jpg", &width, &height, &nrChannels, 0);
    // grass_block_top_x16 is grayscale, open it with GL_RED
    std::string texture_path = Constants::TEXTURE_LOCATION + std::string("/grass_block_top_x64.png");
    unsigned char *data = stbi_load(texture_path.c_str(), &width, &height, &nrChannels, 0);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    m_shader.value().setInt("texture0", 0);
}