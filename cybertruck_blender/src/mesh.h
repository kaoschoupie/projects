// 40281057 - Mykyta Onipchenko
// 40283137 - Bogdan Ivan
// 40283954 - Kelsey Pereira Costa
// 40264524 - Maxime Smith

#pragma once

#include <stdexcept>
#include <string>
#include <vector>

#include <GL/glew.h>
#include <assimp/scene.h>
#include <glm/glm.hpp>

class MeshException : public std::runtime_error {
public:
    MeshException(const std::string& message) noexcept;
};

// NOTE: Respect the byte order of this class and location of the `in` parameters in vertex shaders
class Vertex {
    friend class Mesh;

public:
    Vertex() = delete;
    explicit Vertex(const Vertex& other) = default;
    Vertex(Vertex&& other) noexcept = default;

    Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 uv);

    ~Vertex() = default;

private:
    const glm::vec3 position;
    const glm::vec3 normal;
    const glm::vec2 uv;
};

class Triangle {
public:
    Triangle() = delete;
    explicit Triangle(const Triangle& other) = default;
    Triangle(Triangle&& other) noexcept = default;

    Triangle(glm::uvec3 indices);

    ~Triangle() = default;

private:
    const glm::uvec3 indices;
};

class Mesh {
public:
    Mesh() = delete;
    explicit Mesh(const Mesh& other);
    Mesh(Mesh&& other) noexcept;

    Mesh(std::vector<Vertex> vertices, std::vector<Triangle> triangles);

    ~Mesh();

    void draw() const;

private:
    const std::vector<Vertex> vertices;
    const std::vector<Triangle> triangles;

    GLuint vao;
    GLuint vbo;
    GLuint ebo;
};

class Model {
public:
    Model() = delete;
    explicit Model(const Model& other);
    Model(Model&& other) noexcept;

    Model(std::string path);

    ~Model() = default;

    void draw() const;

private:
    std::vector<Mesh> meshes;

    void processNode(const aiScene* scene, const aiNode* node);
    Mesh processMesh(const aiMesh* mesh);
};
