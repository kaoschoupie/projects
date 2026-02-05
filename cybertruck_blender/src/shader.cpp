// 40281057 - Mykyta Onipchenko
// 40283137 - Bogdan Ivan
// 40283954 - Kelsey Pereira Costa
// 40264524 - Maxime Smith

#include "shader.h"

#include <vector>

#include <glm/gtc/type_ptr.hpp>

ShaderException::ShaderException(const std::string& message) noexcept
    : std::runtime_error(message) { }

Shader::Shader(Shader&& other) noexcept
    : handle(other.handle) {
    // Invalidate previous object so its destructor doesn't delete shader prematurely
    other.handle = 0;
}

Shader::Shader(GLuint type, const char* source) {
    // Create
    this->handle = glCreateShader(type);
    if (!this->handle) {
        throw ShaderException("Cannot create shader");
    }

    // Compile
    glShaderSource(this->handle, 1, &source, nullptr);
    glCompileShader(this->handle);
    GLint success;
    glGetShaderiv(this->handle, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint length = 0;
        glGetShaderiv(this->handle, GL_INFO_LOG_LENGTH, &length);
        std::vector<GLchar> log(length);
        glGetShaderInfoLog(this->handle, length, &length, log.data());
        glDeleteShader(this->handle);
        throw ShaderException(std::string(std::begin(log), std::end(log)));
    }
}

Shader::~Shader() {
    glDeleteShader(this->handle);
}

GLuint Shader::getHandle() const {
    return this->handle;
}

Program::Program(Program&& other) noexcept
    : handle(other.handle) {
    // Invalidate previous object so its destructor doesn't delete program prematurely
    other.handle = 0;
}

Program::Program(const Shader shaders[], std::size_t shadersLength) {
    this->handle = glCreateProgram();
    if (!this->handle) {
        throw ShaderException("Cannot create program");
    }

    for (std::size_t i = 0; i < shadersLength; i++) {
        glAttachShader(this->handle, shaders[i].getHandle());
    }

    glLinkProgram(this->handle);
    GLint success;
    glGetProgramiv(this->handle, GL_LINK_STATUS, &success);
    if (!success) {
        GLint length = 0;
        glGetProgramiv(this->handle, GL_INFO_LOG_LENGTH, &length);
        std::vector<GLchar> log(length);
        glGetProgramInfoLog(this->handle, length, &length, log.data());
        glDeleteProgram(this->handle);
        throw ShaderException(std::string(std::begin(log), std::end(log)));
    }
}

Program& Program::use() {
    glUseProgram(this->handle);
    return *this;
}

#define SET_UNIFORM(type, glFunction, ...)                                          \
    Program& Program::setUniform(std::string label, type value) {                   \
        glFunction(glGetUniformLocation(this->handle, label.c_str()), __VA_ARGS__); \
        return *this;                                                               \
    }

SET_UNIFORM(glm::mat4, glUniformMatrix4fv, 1, GL_FALSE, glm::value_ptr(value))

#undef SET_UNIFORM

Program::~Program() {
    glDeleteProgram(this->handle);
}

GLuint Program::getHandle() const {
    return this->handle;
}
