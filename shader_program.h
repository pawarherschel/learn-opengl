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
class shader_program {
    uint32_t shader_id_;

public:
    explicit shader_program(uint32_t shader_id);

    auto use() const -> void;
    auto set_bool(const std::string& name, bool value) const -> void;
    auto set_int(const std::string& name, int value) const -> void;
    auto set_float(const std::string& name, float value) const -> void;
};

inline shader_program::shader_program(const uint32_t shader_id):
    shader_id_{ shader_id } {}

inline auto shader_program::use() const -> void {
    glUseProgram(this->shader_id_);
}

inline auto shader_program::set_bool(
    const std::string& name,
    const bool value
) const -> void {
    glUniform1i(
        glGetUniformLocation(this->shader_id_, name.c_str()),
        static_cast<GLint>(value)
    );
}

inline auto shader_program::set_int(
    const std::string& name,
    const int value
) const -> void {
    glUniform1i(glGetUniformLocation(this->shader_id_, name.c_str()), value);
}

inline auto shader_program::set_float(
    const std::string& name,
    const float value
) const -> void {
    glUniform1f(glGetUniformLocation(this->shader_id_, name.c_str()), value);
}

namespace builder {
    class program_builder {
    private:
        bool err_{ false };
        uint32_t program_id_;

    public:
        explicit program_builder():
            program_id_{ glCreateProgram() } {}

        auto add_shader(
            uint32_t shader_type,
            const std::string& shader_path
        ) -> program_builder*;
        auto set_bool(const std::string& name, bool value) const -> void;
        auto set_int(const std::string& name, int value) const -> void;
        auto set_float(const std::string& name, float value) const -> void;

        auto build() const -> shader_program;
    };
} // namespace builder
} // namespace shader_program


inline auto shader_program::builder::program_builder::add_shader(
    const uint32_t shader_type,
    const std::string& shader_path
) -> program_builder* {
    if (err_) {
        return this;
    }
    const auto file_stream = std::make_unique<std::ifstream>(shader_path);
    if (file_stream->fail()) {
        std::cout << "ERROR: could not read file \"" << shader_path;

        this->err_ = true;
        return this;
    }
    std::stringstream contents_stringstream;
    contents_stringstream << file_stream->rdbuf();

    const auto contents_string = contents_stringstream.str();
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

inline auto shader_program::builder::program_builder::set_bool(
    const std::string& name,
    const bool value) const -> void {
    glUniform1i(
        glGetUniformLocation(this->program_id_, name.c_str()),
        static_cast<GLint>(value)
    );
}

inline auto shader_program::builder::program_builder::set_int(
    const std::string& name,
    const int value) const -> void {
    glUniform1i(glGetUniformLocation(this->program_id_, name.c_str()), value);
}

inline auto shader_program::builder::program_builder::set_float(
    const std::string& name,
    const float value) const -> void {
    glUniform1f(glGetUniformLocation(this->program_id_, name.c_str()), value);
}

inline auto shader_program::builder::program_builder::build() const -> shader_program {
    assert(!this->err_);

    return shader_program{ this->program_id_ };
}


#endif