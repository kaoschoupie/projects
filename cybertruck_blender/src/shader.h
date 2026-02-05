// 40281057 - Mykyta Onipchenko
// 40283137 - Bogdan Ivan
// 40283954 - Kelsey Pereira Costa
// 40264524 - Maxime Smith

#pragma once

#include <array>
#include <span>
#include <stdexcept>

#include <GL/glew.h>
#include <glm/glm.hpp>

class ShaderException : public std::runtime_error {
public:
    ShaderException(const std::string& message) noexcept;
};

class Shader {
public:
    Shader() = delete;
    Shader(Shader& other) = delete;
    Shader(Shader&& other) noexcept;

    Shader(GLuint type, const char* source);

    ~Shader();

    GLuint getHandle() const;

private:
    GLuint handle;
};

#define SET_UNIFORM(type) Program& setUniform(std::string label, type value)

class Program {
public:
    Program() = delete;
    Program(Program& other) = delete;
    Program(Program&& other) noexcept;

    Program(const Shader* shaders, std::size_t shadersLength);

    ~Program();

    Program& use();
    SET_UNIFORM(glm::mat4);

    GLuint getHandle() const;

private:
    GLuint handle;
};

#undef SET_UNIFORM
