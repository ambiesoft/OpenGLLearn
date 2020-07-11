#include <Windows.h>

#include <sstream>
#include <cassert>

#include <glad/glad.h>
// run 'git submodule update -i'
#include <GLFW/glfw3.h>

#include "../stb/stb_image.h"

#include "shader.h"

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
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
    };
    const int ELEMENT_COUNT = 8;
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    GLuint VBO;
    {
        GL_VERIFY(glGenBuffers(
            1, // count
            &VBO));
        GL_VERIFY(glBindBuffer(GL_ARRAY_BUFFER, VBO));
        GL_VERIFY(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));
    }

    // VAO
    GLuint VAO;
    {
        GL_VERIFY(glGenVertexArrays(1, &VAO));
        GL_VERIFY(glBindVertexArray(VAO));
    }

    // EBO
    GLuint EBO;
    {
        GL_VERIFY(glGenBuffers(1, &EBO));
        GL_VERIFY(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
        GL_VERIFY(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));
    }

    // needs VBO and VAO
    GL_VERIFY(glVertexAttribPointer(
        0, // location in shader
        3, // x,y,z
        GL_FLOAT,
        GL_FALSE, // normalize
        ELEMENT_COUNT * sizeof(float), // byte count of 1 vertex
        (void*)0 // offset
    ));
    GL_VERIFY(glEnableVertexAttribArray(
        0 // location in shader
    ));
    // color attribute
    GL_VERIFY(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, ELEMENT_COUNT * sizeof(float), (void*)(3 * sizeof(float))));
    GL_VERIFY(glEnableVertexAttribArray(1));

    GL_VERIFY(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, ELEMENT_COUNT * sizeof(float), (void*)(6 * sizeof(float))));
    GL_VERIFY(glEnableVertexAttribArray(2));


    Shader ourShader("3.3.shader.vs", "3.3.shader.fs"); // you can name your shader files however you like


    GLuint texture1;
    {
        int width, height, nrChannels;
        unsigned char* data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
        assert(data);

        GL_VERIFY(glGenTextures(1, &texture1));
        GL_VERIFY(glBindTexture(GL_TEXTURE_2D, texture1));

        // set the texture wrapping/filtering options (on the currently bound texture object)
        GL_VERIFY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
        GL_VERIFY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
        GL_VERIFY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GL_VERIFY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

        // attach image to texture
        GL_VERIFY(glTexImage2D(
            GL_TEXTURE_2D,
            0, // mipmap level
            GL_RGB,
            width, height,
            0, // reserved
            GL_RGB, // source format
            GL_UNSIGNED_BYTE, // source format
            data));
        GL_VERIFY(glGenerateMipmap(GL_TEXTURE_2D));

        stbi_image_free(data);
    }
    GLuint texture2;
    {
        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load("awesomeface.png", &width, &height, &nrChannels, 0);
        assert(data);

        GL_VERIFY(glGenTextures(1, &texture2));
        GL_VERIFY(glBindTexture(GL_TEXTURE_2D, texture2));

        // set the texture wrapping/filtering options (on the currently bound texture object)
        GL_VERIFY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
        GL_VERIFY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
        GL_VERIFY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GL_VERIFY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

        // attach image to texture
        GL_VERIFY(glTexImage2D(
            GL_TEXTURE_2D,
            0, // mipmap level
            GL_RGBA,
            width, height,
            0, // reserved
            GL_RGBA, // source format
            GL_UNSIGNED_BYTE, // source format
            data));
        GL_VERIFY(glGenerateMipmap(GL_TEXTURE_2D));

        stbi_image_free(data);
    }


    ourShader.use(); // don't forget to activate the shader before setting uniforms!  
    glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0); // set it manually
    glUniform1i(glGetUniformLocation(ourShader.ID, "texture2"), 1); // set it manually

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
        //GL_VERIFY(glUseProgram(shaderProgram));
        //GL_VERIFY(glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f));

        ourShader.use();

        GL_VERIFY(glActiveTexture(GL_TEXTURE0));
        GL_VERIFY(glBindTexture(GL_TEXTURE_2D, texture1));
        GL_VERIFY(glActiveTexture(GL_TEXTURE1));
        GL_VERIFY(glBindTexture(GL_TEXTURE_2D, texture2));

        GL_VERIFY(glBindVertexArray(VAO)); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        // GL_VERIFY(glDrawArrays(GL_TRIANGLES, 0, 3));
        GL_VERIFY(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    glfwTerminate();
	return 0;
}