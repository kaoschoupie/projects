// 40281057 - Mykyta Onipchenko
// 40283137 - Bogdan Ivan
// 40283954 - Kelsey Pereira Costa
// 40264524 - Maxime Smith

#pragma once

#include <functional>
#include <stdexcept>

// NOTE: Make sure to keep `glew` included before `glfw`
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class WindowException : public std::runtime_error {
public:
    WindowException(const std::string& message) noexcept;
};

class Window {
public:
    Window() = delete;
    Window(const Window& other) = delete;
    Window(Window&& other) = delete;

    Window(int width, int height, const char* title);

    ~Window();

    GLFWwindow* getHandle() const;

    int getWidth() const;
    int getHeight() const;

    void updateSize();

    void swapBuffers();

    bool getShouldClose();
    void setShouldClose(bool value);

    Window& ifKey(int keycode, int state, std::function<void()> callback);

private:
    GLFWwindow* handle;
    int width;
    int height;
};
