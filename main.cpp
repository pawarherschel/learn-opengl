#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>

#include "shader_program.h"
#include "stb_image.h"

// #define REMAP(value, min1, max1, min2, max2)\
//     ((min2) + ((value) - (min1)) * ((max2) - (min2)) / ((max1) - (min1)))

namespace {
    auto framebuffer_size_callback(
        GLFWwindow* /*window*/,
        const int32_t width,
        const int32_t height
    ) -> void {
        std::cout << "resizing viewport to width " << width << " height " << height
            << '\n';
        glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
    }

    auto process_input(GLFWwindow* window) -> void {
        __assume(window != nullptr);

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            // ReSharper disable once CppRedundantCastExpression
            glfwSetWindowShouldClose(window, static_cast<int>(true));
        }
    }
} // namespace

// PROGRESS:
// https://learnopengl.com/Getting-started/Transformations

auto main() -> int32_t {
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    constexpr auto window_width = 800;
    constexpr auto window_height = 600;

    GLFWwindow* window = glfwCreateWindow(
        window_width,
        window_height,
        "LearnOpenGL",
        nullptr,
        nullptr
    );
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << '\n';
        glfwTerminate();
        return EXIT_FAILURE;
    }
    __assume(window != nullptr);
    glfwMakeContextCurrent(window);

    if (0 == gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cout << "Failed to initialize GLAD" << '\n';
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glViewport(0, 0, window_width, window_height);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    constexpr std::array<float, 4> gl_clear_color { 0.2F, 0.3F, 0.3F, 1.0F };
    glClearColor(
        gl_clear_color[0],
        gl_clear_color[1],
        gl_clear_color[2],
        gl_clear_color[3]
    );

    uint32_t vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    constexpr auto stride = 8;
    constexpr auto vertices_len = static_cast<size_t>(stride) * static_cast<size_t>(3);
    constexpr std::array<float, vertices_len> vertices = {
        // bottom right
        // v
        0.5F,
        -0.5F,
        0.0F,
        // c
        1.0F,
        0.0F,
        0.0F,
        // t
        1.0F,
        0.0F,
        // bottom left
        // v
        -0.5F,
        -0.5F,
        0.0F,
        // c
        0.0F,
        1.0F,
        0.0F,
        // t
        0.0F,
        0.0F,
        // top
        // v
        0.0F,
        0.5F,
        0.0F,
        // c
        0.0F,
        0.0F,
        1.0F,
        // t
        0.5F,
        1.0F
    };

    uint32_t vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        static_cast<const void*>(vertices.data()),
        GL_STATIC_DRAW
    );
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        stride * sizeof(float),
        static_cast<void*>(nullptr)
    );
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        stride * sizeof(float),
        reinterpret_cast<void*>(3 * sizeof(float))
    );
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        stride * sizeof(float),
        reinterpret_cast<void*>(6 * sizeof(float))
    );
    glEnableVertexAttribArray(2);

    stbi_set_flip_vertically_on_load(true);

    const std::string container_jpg_path = "container.jpg";
    int32_t container_jpg_width {};
    int32_t container_jpg_height {};
    int32_t container_jpg_nr_channels {};
    const auto container_jpg_sanity = stbi_info(
        container_jpg_path.c_str(),
        &container_jpg_width,
        &container_jpg_height,
        &container_jpg_nr_channels
    );
    if (1 != container_jpg_sanity) {
        std::cout << "failed to load container.jpg" << '\n';
        glfwTerminate();
        return EXIT_FAILURE;
    }
    auto* const container_jpg_data = stbi_load(
        container_jpg_path.c_str(),
        &container_jpg_width,
        &container_jpg_height,
        &container_jpg_nr_channels,
        0
    );
    uint32_t container_jpg_texture {};
    glGenTextures(1, &container_jpg_texture);
    glBindTexture(GL_TEXTURE_2D, container_jpg_texture);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        container_jpg_width,
        container_jpg_height,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        container_jpg_data
    );
    stbi_image_free(container_jpg_data);
    glGenerateMipmap(GL_TEXTURE_2D);

    const std::string awesomeface_png_path = "awesomeface.png";
    int32_t awesomeface_png_width {};
    int32_t awesomeface_png_height {};
    int32_t awesomeface_png_nr_channels {};
    const auto awesomeface_png_sanity = stbi_info(
        awesomeface_png_path.c_str(),
        &awesomeface_png_width,
        &awesomeface_png_height,
        &awesomeface_png_nr_channels
    );
    if (1 != awesomeface_png_sanity) {
        std::cout << "failed to load awesomeface.png" << '\n';
        glfwTerminate();
        return EXIT_FAILURE;
    }
    auto* const awesomeface_png_data = stbi_load(
        awesomeface_png_path.c_str(),
        &awesomeface_png_width,
        &awesomeface_png_height,
        &awesomeface_png_nr_channels,
        0
    );
    uint32_t awesomeface_png_texture {};
    glGenTextures(1, &awesomeface_png_texture);
    glBindTexture(GL_TEXTURE_2D, awesomeface_png_texture);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        awesomeface_png_width,
        awesomeface_png_height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        awesomeface_png_data
    );
    stbi_image_free(awesomeface_png_data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    const auto program =
        shader_program::builder::ProgramBuilder {}
        .add_shader(
            GL_VERTEX_SHADER,
            "shaders/shader.vs.glsl"
        )->add_shader(
            GL_FRAGMENT_SHADER,
            "shaders/shader.fs.glsl"
        )->build();
    program.use();
    program.set_int("texture2", 1);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    while (0 == glfwWindowShouldClose(window)) {
        process_input(window);
        glClear(GL_COLOR_BUFFER_BIT);

        program.use();

        glBindVertexArray(vao);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, container_jpg_texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, awesomeface_png_texture);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return EXIT_SUCCESS;
}