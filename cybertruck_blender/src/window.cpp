// 40281057 - Mykyta Onipchenko
// 40283137 - Bogdan Ivan
// 40283954 - Kelsey Pereira Costa
// 40264524 - Maxime Smith

#include "window.h"

#include <sstream>

WindowException::WindowException(const std::string& message) noexcept
    : std::runtime_error(message) { }

Window::Window(int width, int height, const char* title) {
    if (!glfwInit()) {
        const char* description;
        int code = glfwGetError(&description);
        std::stringstream message;
        message << "GLFW init error (" << code << ") " << description;
        throw WindowException(message.str());
    }

    this->handle = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!this->handle) {
        glfwTerminate();

        const char* description;
        int code = glfwGetError(&description);
        std::stringstream message;
        message << "Cannot create window (" << code << ") " << description;

        throw WindowException(message.str());
    }
    glfwMakeContextCurrent(handle);

    GLenum err = glewInit();
    // HACK(Mykyta Onipchenko): On Wayland (Linux), GLEW always returns `NO_GLX_DISPLAY`.
    // This error is not fatal and the application still works.
    // [Issue](https://github.com/nigels-com/glew/issues/172)
    if (err != GLEW_OK && err != GLEW_ERROR_NO_GLX_DISPLAY) {
        glfwTerminate();

        std::stringstream message;
        message << "GLEW init error " << glewGetErrorString(err);

        throw WindowException(message.str());
    }
}

Window::~Window() {
    glfwDestroyWindow(this->handle);
    glfwTerminate();
}

GLFWwindow* Window::getHandle() const {
    return this->handle;
}

bool Window::getShouldClose() {
    return glfwWindowShouldClose(this->handle);
}

void Window::setShouldClose(bool value) {
    glfwSetWindowShouldClose(this->handle, value);
}

Window& Window::ifKey(int keycode, int state, std::function<void()> callback) {
    if (glfwGetKey(this->handle, keycode) == state) {
        callback();
    }
    return *this;
}

void Window::updateSize() {
    glfwGetWindowSize(this->handle, &this->width, &this->height);
    glViewport(0, 0, this->width, this->height);
}

void Window::swapBuffers() {
    glfwSwapBuffers(this->handle);
}

int Window::getWidth() const {
    return this->width;
}

int Window::getHeight() const {
    return this->height;
}
