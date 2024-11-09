#pragma once // NOLINT(llvm-header-guard)

#ifndef SHADER_H
#define SHADER_H

#include <array>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <glad/glad.h>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

namespace shader_program {
class ShaderProgram {
    uint32_t shader_id_;

public:
    explicit ShaderProgram(uint32_t shader_id);

    auto use() const -> void;
    auto set_bool(const std::string& name, bool value) const -> void;
    auto set_int(const std::string& name, int value) const -> void;
    auto set_float(const std::string& name, float value) const -> void;
};

inline ShaderProgram::ShaderProgram(const uint32_t shader_id):
    shader_id_{ shader_id } {}

inline auto ShaderProgram::use() const -> void {
    glUseProgram(this->shader_id_);
}

inline auto ShaderProgram::set_bool(
    const std::string& name,
    const bool value
) const -> void {
    glUniform1i(
        glGetUniformLocation(this->shader_id_, name.c_str()),
        static_cast<GLint>(value)
    );
}

inline auto ShaderProgram::set_int(
    const std::string& name,
    const int value
) const -> void {
    glUniform1i(glGetUniformLocation(this->shader_id_, name.c_str()), value);
}

inline auto ShaderProgram::set_float(
    const std::string& name,
    const float value
) const -> void {
    glUniform1f(glGetUniformLocation(this->shader_id_, name.c_str()), value);
}

namespace builder {
    class ProgramBuilder {
    private:
        bool err_{ false };
        uint32_t program_id_;

    public:
        explicit ProgramBuilder():
            program_id_{ glCreateProgram() } {}

        auto add_shader(uint32_t shader_type, const std::string& shader_path) -> ProgramBuilder*;
        auto set_bool(const std::string& name, bool value) const -> void;
        auto set_int(const std::string& name, int value) const -> void;
        auto set_float(const std::string& name, float value) const -> void;

        auto build() const -> ShaderProgram;
    };
} // namespace builder
} // namespace shader_program


inline auto shader_program::builder::ProgramBuilder::add_shader(
    const uint32_t shader_type,
    const std::string& shader_path
) -> ProgramBuilder* {
    if (err_) {
        return this;
    }
    const auto file_stream = std::make_unique<std::ifstream>(shader_path);
    if (file_stream->fail()) {
        std::cout << "ERROR: could not read file \"" << shader_path;

        this->err_ = true;
        return this;
    }
    std::stringstream contents_string_stream;
    contents_string_stream << file_stream->rdbuf();

    const auto contents_string = contents_string_stream.str();
    const auto contents = std::make_unique<const char*>(contents_string.c_str());

    auto success = 0;
    constexpr auto info_log_buffer_size = 512;
    std::array<char, info_log_buffer_size> info_log{};

    const auto shader_id = glCreateShader(shader_type);
    glShaderSource(shader_id, 1, &*contents, nullptr);
    glCompileShader(shader_id);
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);

    if (0 == success) {
        glGetShaderInfoLog(shader_id, info_log_buffer_size, nullptr, info_log.data());
        std::cout << "ERROR: shader failed to compile\n" << info_log.data() << '\n';
        this->err_ = true;
        return this;
    }

    glAttachShader(this->program_id_, shader_id);
    glLinkProgram(this->program_id_);
    glGetProgramiv(this->program_id_, GL_LINK_STATUS, &success);
    if (0 == success) {
        glGetProgramInfoLog(this->program_id_, info_log_buffer_size, nullptr, info_log.data());
        std::cout << "ERROR: linking shader failed\n" << info_log.data() << '\n';
        this->err_ = true;
        return this;
    }

    glDeleteShader(shader_id);

    return this;
}

inline auto shader_program::builder::ProgramBuilder::set_bool(
    const std::string& name,
    const bool value
) const -> void {
    glUniform1i(
        glGetUniformLocation(this->program_id_, name.c_str()),
        static_cast<GLint>(value)
    );
}

inline auto shader_program::builder::ProgramBuilder::set_int(
    const std::string& name,
    const int value
) const -> void {
    glUniform1i(glGetUniformLocation(this->program_id_, name.c_str()), value);
}

inline auto shader_program::builder::ProgramBuilder::set_float(
    const std::string& name,
    const float value
) const -> void {
    glUniform1f(glGetUniformLocation(this->program_id_, name.c_str()), value);
}

inline auto shader_program::builder::ProgramBuilder::build() const -> ShaderProgram {
    assert(!this->err_);

    return ShaderProgram{ this->program_id_ };
}

#endif