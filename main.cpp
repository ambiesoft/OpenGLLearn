#include <Windows.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define APPTITLE "OpenGLLearn"
#define APPTITLEW L"OpenGLLearn"

int gWidth = 800;
int gHeight = 600;

void ShowFatal(const wchar_t* message)
{
    MessageBox(nullptr, message, APPTITLEW, MB_ICONERROR);
    glfwTerminate();
    exit(1);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#ifndef __MAC__
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#else
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Creating a Window
    GLFWwindow* win = glfwCreateWindow(
        gWidth,
        gHeight,
        APPTITLE,
        nullptr, // monitor
        nullptr  // share
    );
    if (!win)
        ShowFatal(L"Failed to crate window");

    glfwMakeContextCurrent(win);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        ShowFatal(L"Failed to initialize GLAD");

    glViewport(0, 0, gWidth, gHeight);

    // called when the window resized
    glfwSetFramebufferSizeCallback(win, [](GLFWwindow* window, int width, int height) {
        gWidth = width;
        gHeight = height;
        glViewport(0, 0, width, height);
        }
    );

    // Main loop
    while (!glfwWindowShouldClose(win))
    {
        processInput(win);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    glfwTerminate();
	return 0;
}