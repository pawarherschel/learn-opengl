#include <GLFW/glfw3.h>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>

#include <glad/glad.h>
namespace {
auto framebuffer_size_callback(GLFWwindow * /*window*/, const int32_t width,
                               const int32_t height) -> void {
  assert(width > 0);
  assert(height > 0);

  std::cout << "resizing viewport to width " << width << " height " << height
            << '\n';
  glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
}

auto process_input(GLFWwindow *window) -> void {
  __assume(window != nullptr);

  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    // ReSharper disable once CppRedundantCastExpression
    glfwSetWindowShouldClose(window, static_cast<int>(true));
  }
}

template <typename T>
constexpr auto get_length_of_buffer(const T vertices_per_cord,
                                    const T cords_per_shape, const T shapes)
    -> T {
  return
      // vertices per coordinate
      vertices_per_cord
      // number of coordinates per shape
      * cords_per_shape
      //  number of shapes
      * shapes;
}
} // namespace

// PROGRESS:
// https://learnopengl.com/Getting-started/Hello-Triangle#:~:text=you%20missed%20anything.-,Element%20Buffer%20Objects,-There%20is%20one

auto main() -> int32_t {
  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  constexpr auto width = 800;
  constexpr auto height = 600;

  GLFWwindow *window =
      glfwCreateWindow(width, height, "LearnOpenGL", nullptr, nullptr);
  if (window == nullptr) {
    std::cout << "Failed to create GLFW window" << '\n';
    glfwTerminate();
    return EXIT_FAILURE;
  }
  __assume(window != nullptr);
  glfwMakeContextCurrent(window);

  if (0 ==
      gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    std::cout << "Failed to initialize GLAD" << '\n';
    return EXIT_FAILURE;
  }

  glViewport(0, 0, width, height);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  constexpr std::array<float, 4> gl_clear_color{0.2F, 0.3F, 0.3F, 1.0F};
  glClearColor(gl_clear_color[0], gl_clear_color[1], gl_clear_color[2],
               gl_clear_color[3]);

  uint32_t vao = 0;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  constexpr auto vertices_len = get_length_of_buffer(
      static_cast<const size_t>(3), static_cast<const size_t>(3),
      static_cast<const size_t>(1));
  constexpr std::array<float, vertices_len> vertices = {
      -0.5F, -0.5F, 0.0F, // BOTTOM LEFT
      0.5F,  -0.5F, 0.0F, // BOTTOM RIGHT
      0.0F,  0.5F,  0.0F, // TOP MIDDLE
  };

  uint32_t vbo = 0;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),
               static_cast<const void *>(vertices.data()), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                        static_cast<void *>(nullptr));
  glEnableVertexAttribArray(0);

  const auto *const vertex_shader_source =
      // language=glsl
      R"(#version 330 core
layout (location = 0) in vec3 aPos;
void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
})";
  const uint32_t vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
  glCompileShader(vertex_shader);
  auto success = 0;
  constexpr auto buffer_size = 512;
  std::array<char, buffer_size> info_log{};
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
  if (0 == success) {
    glGetShaderInfoLog(vertex_shader, buffer_size, nullptr, info_log.data());
    std::cout << "ERROR: vertex_shader failed to compile\n"
              << info_log.data() << '\n';
    glfwTerminate();
    return EXIT_FAILURE;
  }

  const auto *const fragment_shader_source =
      // language=glsl
      R"(#version 330 core
out vec4 FragColor;
void main() {
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
})";
  const uint32_t fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);
  glCompileShader(fragment_shader);
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
  if (0 == success) {
    glGetShaderInfoLog(fragment_shader, buffer_size, nullptr, info_log.data());
    std::cout << "ERROR: fragmentShader failed to compile\n"
              << info_log.data() << '\n';
    glfwTerminate();
    return EXIT_FAILURE;
  }

  const uint32_t shader_program = glCreateProgram();
  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  glLinkProgram(shader_program);
  glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
  if (0 == success) {
    glGetProgramInfoLog(shader_program, buffer_size, nullptr, info_log.data());
    std::cout << "ERROR: shader_program failed to link\n"
              << info_log.data() << '\n';
    glfwTerminate();
    return EXIT_FAILURE;
  }
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  while (0 == // NOLINT(altera-id-dependent-backward-branch,
              // altera-unroll-loops)
         glfwWindowShouldClose(window)) {
    process_input(window);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader_program);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3); // NOLINT(misc-include-cleaner)

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();

  return EXIT_SUCCESS;
}
