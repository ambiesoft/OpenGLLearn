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
        // positions         // colors
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
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
        6 * sizeof(float), // byte count of 1 vertex
        (void*)0 // offset
    ));
    GL_VERIFY(glEnableVertexAttribArray(
        0 // location in shader
    ));

    // color attribute
    GL_VERIFY(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))));
    GL_VERIFY(glEnableVertexAttribArray(1));


    // vertex shader
    const char* vertexShaderSource = R"(#version 330 core
layout (location = 0) in vec3 aPos;   // the position variable has attribute position 0
layout (location = 1) in vec3 aColor; // the color variable has attribute position 1
  
out vec3 ourColor; // output a color to the fragment shader

void main()
{
    gl_Position = vec4(aPos, 1.0);
    ourColor = aColor; // set ourColor to the input color we got from the vertex data
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
in vec3 ourColor;
  
void main()
{
    FragColor = vec4(ourColor, 1.0);
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
        //float timeValue = glfwGetTime();
        //float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
        //int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
        //assert(-1 != vertexColorLocation);
        GL_VERIFY(glUseProgram(shaderProgram));
        //GL_VERIFY(glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f));

        GL_VERIFY(glBindVertexArray(VAO)); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        GL_VERIFY(glDrawArrays(GL_TRIANGLES, 0, 3));

        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    glfwTerminate();
	return 0;
}