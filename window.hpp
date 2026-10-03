#ifndef WINDOW_H
#define WINDOW_H

#include "glad/glad.h"
#include <GLFW/glfw3.h>


// Just a helper function for creating glfw windows
GLFWwindow *createGLFWWindow(int windowWidth, int windowHeight, const char *title, bool debugMode);


#endif