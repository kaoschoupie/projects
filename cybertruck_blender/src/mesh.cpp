// 40281057 - Mykyta Onipchenko
// 40283137 - Bogdan Ivan
// 40283954 - Kelsey Pereira Costa
// 40264524 - Maxime Smith

#include "mesh.h"

#include <sstream>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

MeshException::MeshException(const std::string& message) noexcept
    : std::runtime_error(message) { }

Vertex::Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 uv)
    : position(position)
    , normal(normal)
    , uv(uv) { }

Triangle::Triangle(glm::vec<3, GLuint> indices)
    : indices(indices) { }

Mesh::Mesh(const Mesh& other)
    : Mesh(other.vertices, other.triangles) { }

Mesh::Mesh(Mesh&& other) noexcept
    : vertices(std::move(other.vertices))
    , triangles(std::move(other.triangles))
    , vao(other.vao)
    , vbo(other.vbo)
    , ebo(other.ebo) {
    // Invalidate previous object so its destructor doesn't delete buffers prematurely
    other.vao = 0;
    other.vbo = 0;
    other.ebo = 0;
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<Triangle> triangles)
    : vertices(std::move(vertices))
    , triangles(std::move(triangles))

{
    // Initialize
    glGenVertexArrays(1, &this->vao);
    glGenBuffers(1, &this->vbo);
    glGenBuffers(1, &this->ebo);

    glBindVertexArray(this->vao);

    // Populate VBO
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        this->vertices.size() * sizeof(Vertex),
        this->vertices.data(),
        GL_STATIC_DRAW
    );

    // Populate EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        this->triangles.size() * sizeof(Triangle),
        this->triangles.data(),
        GL_STATIC_DRAW
    );

    // Populate VAO
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

    glBindVertexArray(0);
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &this->vao);
    glDeleteBuffers(1, &this->vbo);
    glDeleteBuffers(1, &this->ebo);
}

void Mesh::draw() const {
    glBindVertexArray(this->vao);
    glDrawElements(GL_TRIANGLES, this->triangles.size() * 3, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

Model::Model(const Model& other)
    : meshes(other.meshes) {
}

Model::Model(Model&& other) noexcept
    : meshes(std::move(other.meshes)) {
}

Model::Model(std::string path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        path,
        aiProcess_Triangulate
            | aiProcess_GenNormals
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::stringstream message;
        message << "Cannot import model " << importer.GetErrorString();
        throw MeshException(message.str());
    }

    this->processNode(scene, scene->mRootNode);
}

void Model::draw() const {
    for (const Mesh& mesh : this->meshes) {
        mesh.draw();
    }
}

void Model::processNode(const aiScene* scene, const aiNode* node) {
    for (size_t i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        this->meshes.push_back(processMesh(mesh));
    }
    for (size_t i = 0; i < node->mNumChildren; i++) {
        processNode(scene, node->mChildren[i]);
    }
}

Mesh Model::processMesh(const aiMesh* mesh) {
    std::vector<Vertex> vertices;
    vertices.reserve(mesh->mNumVertices);
    std::vector<Triangle> triangles;
    triangles.reserve(mesh->mNumFaces);

    for (size_t i = 0; i < mesh->mNumVertices; i++) {
        if (!mesh->HasPositions()) {
            throw MeshException("Cannot import model without positions");
        }
        if (!mesh->HasNormals()) {
            throw MeshException("Cannot import model without normals");
        }
        if (!mesh->HasTextureCoords(0)) {
            throw MeshException("Cannot import model without texture coordinates");
        }
        vertices.push_back({
            {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z},
            {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z},
            {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y},
        });
    }
    for (size_t i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        if (face.mNumIndices != 3) {
            throw MeshException("Cannot import model with non-triangular faces");
        }
        triangles.push_back({
            {face.mIndices[0], face.mIndices[1], face.mIndices[2]},
        });
    }

    return Mesh(std::move(vertices), std::move(triangles));
}
