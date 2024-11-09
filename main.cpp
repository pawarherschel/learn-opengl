#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>

#include "shader_program.h"

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
// https://learnopengl.com/Getting-started/Textures

auto main() -> int32_t {
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    constexpr auto width = 800;
    constexpr auto height = 600;

    GLFWwindow* window = glfwCreateWindow(
        width,
        height,
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
        return EXIT_FAILURE;
    }

    glViewport(0, 0, width, height);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    constexpr std::array<float, 4> gl_clear_color{ 0.2F, 0.3F, 0.3F, 1.0F };
    glClearColor(
        gl_clear_color[0],
        gl_clear_color[1],
        gl_clear_color[2],
        gl_clear_color[3]
    );

    uint32_t vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    constexpr auto stride = 6;
    constexpr auto vertices_len =
        static_cast<size_t>(stride)
        * static_cast<size_t>(3);
    constexpr std::array<float, vertices_len> vertices = {
        // bottom right
        // v
        0.5F, -0.5F, 0.0F,
        // c
        1.0F, 0.0F, 0.0F,
        // bottom left
        // v
        -0.5F, -0.5F, 0.0F,
        // c
        0.0F, 1.0F, 0.0F,
        // top
        // v
        0.0F, 0.5F, 0.0F,
        // c
        0.0F, 0.0F, 1.0F
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

    const auto program =
        shader_program::builder::program_builder{}
        .add_shader(
            GL_VERTEX_SHADER,
            "shaders/shader.vs.glsl")
        ->add_shader(
            GL_FRAGMENT_SHADER,
            "shaders/shader.fs.glsl")
        ->build();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    while (0 == glfwWindowShouldClose(window)) {
        process_input(window);
        glClear(GL_COLOR_BUFFER_BIT);

        program.use();

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3); // NOLINT(misc-include-cleaner)

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return EXIT_SUCCESS;
}