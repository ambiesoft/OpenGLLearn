#include <Windows.h>

#include <sstream>
#include <cassert>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#ifndef NDEBUG
#define GL_VERIFY(OP) do {                  \
    OP;                                     \
    GLenum error = glGetError();            \
    if(error != GL_NO_ERROR)                \
    {                                       \
        assert(false);                      \
    }                                       \
} while(false)
#else
#define GL_VERIFY(OP) OP
#endif

#define APPTITLE "OpenGLLearn"

int gWidth = 800;
int gHeight = 600;

void ShowFatal(const char* message)
{
    MessageBoxA(nullptr, message, APPTITLE, MB_ICONERROR);
    GL_VERIFY(glfwTerminate());
    exit(1);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void checkShaderCompile(GLuint shader)
{
    int success = 0;
    char infoLog[512];
    GL_VERIFY(glGetShaderiv(shader, GL_COMPILE_STATUS, &success));
    if (!success)
    {
        GL_VERIFY(glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog));
        std::stringstream ssMessage;
        ssMessage << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        ShowFatal(ssMessage.str().c_str());
    }
}
void checkShaderLink(GLuint program)
{
    int success = 0;
    char infoLog[512];
    GL_VERIFY(glGetProgramiv(program, GL_LINK_STATUS, &success));
    if (!success)
    {
        GL_VERIFY(glGetProgramInfoLog(program, sizeof(infoLog), nullptr, infoLog));
        std::stringstream ssMessage;
        ssMessage << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        ShowFatal(ssMessage.str().c_str());
    }
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
        ShowFatal("Failed to crate window");

    glfwMakeContextCurrent(win);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        ShowFatal("Failed to initialize GLAD");

    GL_VERIFY(glViewport(0, 0, gWidth, gHeight));

    // called when the window resized
    glfwSetFramebufferSizeCallback(win, [](GLFWwindow* window, int width, int height) {
        gWidth = width;
        gHeight = height;
        GL_VERIFY(glViewport(0, 0, width, height));
        }
    );

    // Draw preparation
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    GLuint VBO;
    GL_VERIFY(glGenBuffers(
        1, // count
        &VBO));
    GL_VERIFY(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    GL_VERIFY(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

    // VAO
    unsigned int VAO;
    GL_VERIFY(glGenVertexArrays(1, &VAO));
    GL_VERIFY(glBindVertexArray(VAO));

    // needs VBO and VAO
    GL_VERIFY(glVertexAttribPointer(
        0, // location in shader
        3, // x,y,z
        GL_FLOAT,
        GL_FALSE, // normalize
        3 * sizeof(float), // byte count of 1 vertex
        (void*)0 // offset
    ));
    GL_VERIFY(glEnableVertexAttribArray(
        0 // location in shader
    ));


    // vertex shader
    const char* vertexShaderSource = R"(#version 330 core
layout (location = 0) in vec3 aPos;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
)";
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GL_VERIFY(glShaderSource(vertexShader,
        1, // count
        &vertexShaderSource,
        nullptr // length
    ));
    GL_VERIFY(glCompileShader(vertexShader));

    // check compile error
    checkShaderCompile(vertexShader);

    // fragment shader
    const char* fragmentShaderSource = R"(#version 330 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
)";
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    GL_VERIFY(glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL));
    GL_VERIFY(glCompileShader(fragmentShader));
    checkShaderCompile(fragmentShader);

    // Prepare program
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    GL_VERIFY(glAttachShader(shaderProgram, vertexShader));
    GL_VERIFY(glAttachShader(shaderProgram, fragmentShader));
    GL_VERIFY(glLinkProgram(shaderProgram));
    checkShaderLink(shaderProgram);

    GL_VERIFY(glUseProgram(shaderProgram));

    GL_VERIFY(glDeleteShader(vertexShader));
    GL_VERIFY(glDeleteShader(fragmentShader));




    // Main loop
    while (!glfwWindowShouldClose(win))
    {
        processInput(win);

        GL_VERIFY(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
        GL_VERIFY(glClear(GL_COLOR_BUFFER_BIT));

        // draw our first triangle
        GL_VERIFY(glUseProgram(shaderProgram));
        GL_VERIFY(glBindVertexArray(VAO)); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        GL_VERIFY(glDrawArrays(GL_TRIANGLES, 0, 3));

        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    glfwTerminate();
	return 0;
}