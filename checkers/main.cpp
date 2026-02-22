
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include "world.h"
#include "SheetCheckers.h"



int main()
{
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW\n";
        return -1;
    }
    int width, height, channels;
    unsigned char* pixels = stbi_load("icon.png", &width, &height, &channels, 4);

    GLFWwindow* window = glfwCreateWindow(windowW, windowH, "Checkers", NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }
    if (pixels) {
        GLFWimage icon{ width, height, pixels };
        glfwSetWindowIcon(window, 1, &icon);
        stbi_image_free(pixels);
    }

    glfwMakeContextCurrent(window);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(minX,maxX, minY, maxY, minZ, maxZ);
    glMatrixMode(GL_MODELVIEW);

    Checkers sheet;

    while (!glfwWindowShouldClose(window))
    {
        sheet.processInput(window);

        glClear(GL_COLOR_BUFFER_BIT);

        sheet.drawSheet();
        sheet.drawChecker();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

