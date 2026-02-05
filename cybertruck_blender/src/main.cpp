// 40281057 - Mykyta Onipchenko
// 40283137 - Bogdan Ivan
// 40283954 - Kelsey Pereira Costa
// 40264524 - Maxime Smith

#include <iostream>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "mesh.h"
#include "shader.h"
#include "window.h"

const char* vertexShader = R"glsl(
    #version 330 core

    uniform mat4 projection;
    uniform mat4 view;
    uniform mat4 transform;
    uniform mat4 normal;

    layout(location = 0) in vec3 inPosition;
    layout(location = 1) in vec3 inNormal;
    layout(location = 2) in vec2 inUv;

    out vec3 positionLocal;
    out vec3 positionWorld;
    out vec3 normalLocal;
    out vec3 normalWorld;
    out vec2 uv;

    void main() {
        gl_Position = projection * view * transform * vec4(inPosition, 1.0);
        positionLocal = inPosition;
        positionWorld = (transform * vec4(inPosition, 1.0)).xyz;
        normalLocal = inNormal;
        normalWorld = (normal * vec4(inNormal, 0.0)).xyz;
        uv = inUv;
    }
)glsl";

const char* fragmentShader = R"glsl(
    #version 330 core

    in vec3 positionLocal;
    in vec3 positionWorld;
    in vec3 normalLocal;
    in vec3 normalWorld;
    in vec2 uv;

    out vec4 fragColor;

    const vec3 objectColor = vec3(0.922, 0.569, 0.616);

    const vec3 lightPos = vec3(-2.0, 0.0, 2.0);
    const vec3 lightColor = vec3(0.988, 0.749, 0.286);

    const float ambientStrength = 0.1;
    const vec3 ambient = ambientStrength * lightColor;

    void main() {
        vec3 norm = normalize(normalWorld);
        vec3 lightDir = normalize(lightPos - positionWorld);

        float diff = max(dot(norm, lightDir),0.0);
        vec3 diffuse = diff * lightColor;

        vec3 result = (ambient + diffuse) * objectColor;
        fragColor = vec4(result, 1.0);
    }
)glsl";

const double SPEED_POSITION = 2.0;
const double SPEED_SCALE = 2.5;
const double SPEED_ROTATION = 3.0;

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;
const char* WINDOW_TITLE = "COMP 371 - Assignment 3";

int main() {
    Window window(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    Model model("./res/model/truck.obj");

    std::unique_ptr<Shader[]> shaders {new Shader[2] {
        Shader(GL_VERTEX_SHADER, vertexShader),
        Shader(GL_FRAGMENT_SHADER, fragmentShader)
    }};
    Program program(shaders.get(), 2);

    glm::vec3 transformPosition(0.0f);
    glm::vec3 transformScale(1.0f);
    glm::vec3 transformRotation(0.0f);

    double timePrevious = 0;

    while (!window.getShouldClose()) {
        double timeCurrent = glfwGetTime();
        double timeDelta = timeCurrent - timePrevious;
        timePrevious = timeCurrent;

        window.updateSize();

        glfwPollEvents();
        window
            .ifKey(GLFW_KEY_ESCAPE, GLFW_PRESS, [&]() { window.setShouldClose(true); })
            .ifKey(GLFW_KEY_W, GLFW_PRESS, [&]() { transformPosition.z += SPEED_POSITION * timeDelta; })
            .ifKey(GLFW_KEY_S, GLFW_PRESS, [&]() { transformPosition.z -= SPEED_POSITION * timeDelta; })
            .ifKey(GLFW_KEY_A, GLFW_PRESS, [&]() { transformPosition.y += SPEED_POSITION * timeDelta; })
            .ifKey(GLFW_KEY_D, GLFW_PRESS, [&]() { transformPosition.y -= SPEED_POSITION * timeDelta; })
            .ifKey(GLFW_KEY_Q, GLFW_PRESS, [&]() { transformRotation.z += SPEED_ROTATION * timeDelta; })
            .ifKey(GLFW_KEY_E, GLFW_PRESS, [&]() { transformRotation.z -= SPEED_ROTATION * timeDelta; })
            .ifKey(GLFW_KEY_R, GLFW_PRESS, [&]() { transformScale.z += SPEED_SCALE * timeDelta; })
            .ifKey(GLFW_KEY_F, GLFW_PRESS, [&]() { transformScale.z -= SPEED_SCALE * timeDelta; });

        glm::mat4 view = glm::lookAt(glm::vec3(-10.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            float(window.getWidth()) / float(window.getHeight()),
            0.1f,
            100.0f
        );

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), transformPosition)
            * glm::mat4_cast(glm::quat(transformRotation))
            * glm::scale(glm::mat4(1.0f), transformScale);
        glm::mat4 normal = glm::transpose(glm::inverse(transform));

        glClearColor(0.1, 0.1, 0.1, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        program
            .use()
            .setUniform("projection", projection)
            .setUniform("view", view)
            .setUniform("transform", transform)
            .setUniform("normal", normal);

        model.draw();

        window.swapBuffers();
    }

    return 0;
}
