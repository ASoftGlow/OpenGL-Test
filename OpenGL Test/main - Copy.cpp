#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <shader_c.h>
#include <cmath>
#include <iostream>
using namespace std;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const char* vertexShaderSource =
"#version 330 core\n"
"layout (location = 0) in vec2 aPos;\n"
"layout (location = 1) in unsigned short tile;\n"

"out vec2 TexCoord;\n"

"void main()\n"
"{\n"
//"   gl_PointSize = 3;\n"
"   gl_Position = vec4(aPos.x, aPos.y, 0, 1.0);\n"
"   TexCoord = vec2(aPos);"
"}\0";

const char* fragmentShaderSource =
"#version 330 core\n"
"out vec4 FragColor;\n"

"in vec2 TexCoord;\n"
"uniform sampler2D ourTexture;\n"

"void main()\n"
"{\n"
"   FragColor = texture(ourTexture, TexCoord);\n"
"}\0";

const char* fragmentShaderSource2 =
"#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
"}\0";


int main()
{
    const int WINDOW_WIDTH = 600;
    const int WINDOW_HEIGHT = 600;
    const short SCALE = 100;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, 1);
    
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Hello", NULL, NULL);
    if (window == NULL)
    {
        printf("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //glfwSetWindowOpacity(window, 0.5f);

    //glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD");
        return -1;
    }

    Shader shaderProgram("vertex.vert", "fragment.frag");

    // vertex shader
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);


    // setup vertex data

    const unsigned int WIDTH = 4, HEIGHT = 4;

    unsigned char tiles[HEIGHT][WIDTH] = {
        {0,0,1,0},
        {1,1,0,0},
        {0,0,0,1},
        {0,0,1,1}
    };

    float vertices[(HEIGHT + 1) * (WIDTH + 1) * 3];
    unsigned int indices[HEIGHT * WIDTH * 2 * 3];

    for (unsigned int y = 0; y != HEIGHT +1; y++) {
        for (unsigned int x = 0; x != WIDTH +1; x++) {
            vertices[(x + y * (WIDTH + 1)) * 3 + 0] = (float)x/5;
            vertices[(x + y * (WIDTH + 1)) * 3 + 1] = (float)y/5;
            vertices[(x + y * (WIDTH + 1)) * 3 + 2] = tiles[y][x];
        };
    };

    
    for (unsigned int y = 0; y != HEIGHT; y++) {
        for (unsigned int x = 0; x != WIDTH; x++) {
            indices[(x + y * WIDTH) * (3 * 2) + 0] = x + y * (WIDTH + 1);
            indices[(x + y * WIDTH) * (3 * 2) + 1] = x + (y+1) * (WIDTH+1);
            indices[(x + y * WIDTH) * (3 * 2) + 2] = x + (y+1) * (WIDTH+1) + 1;

            indices[(x + y * WIDTH) * (3 * 2) + 3] = x + y * (WIDTH+1);
            indices[(x + y * WIDTH) * (3 * 2) + 4] = x + y * (WIDTH+1) + 1;
            indices[(x + y * WIDTH) * (3 * 2) + 5] = x + (y + 1) * (WIDTH+1) + 1;
        };
    };

    


    /*for (int i = 0; i != sizeof(vertices) / sizeof(float) / 3; i++) {
        for (int j = 0; j != 3; j++) {
            cout << vertices[i*3+j] << ' ';
        };
        cout << endl;
    };

    cout << endl;

    for (int i = 0; i != sizeof(indices) / sizeof(unsigned int) / 3; i++) {
        for (int j = 0; j != 3; j++) {
            cout << indices[i * 3 + j] << ' ';
        };
        cout << endl;
    };

    
    for (int j = 0; j != 9; j++) {
        cout << endl;
        for (int k = 0; k != 3; k++) {
            cout << vertices[indices[j]+k] << ' ';
        };
    };*/



    unsigned int VBOs[1], VAOs[1], EBOs[1];
    glGenBuffers(1, VBOs);
    glGenBuffers(1, EBOs);
    glGenVertexArrays(1, VAOs);



    glBindVertexArray(VAOs[0]);

    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(float)+sizeof(unsigned int), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 1, GL_UNSIGNED_INT, GL_FALSE, 2 * sizeof(float)+ sizeof(unsigned int), (void*)(2 * sizeof(float)+sizeof(unsigned int)));
    glEnableVertexAttribArray(1);

    //glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

    // unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    // load textures
    unsigned int textures[5];
    const char textureNames[5][10] = {
        "0.png", "1.png", "2.png", "3.png", "4.png"
    };
    unsigned char* data;
    int width, height, nrChannels;

    for (unsigned short t = 0; t != 5; t++)
    {
        glGenTextures(1, &textures[t]);
        glBindTexture(GL_TEXTURE_2D, textures[t]);
        // set the texture wrapping/filtering options (on the currently bound texture object)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        // load and generate the texture
        data = stbi_load(textureNames[t], &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);
    };



    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


        //processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        shaderProgram.use();
        glBindTexture(GL_TEXTURE_2D, textures[2]);
        glBindVertexArray(VAOs[0]);
        //glDrawArrays(GL_TRIANGLES, 0, 3);6+4*4*2*3
        //glDrawArrays(GL_POINTS, 0, 4 * 4 * 2 * 3);
        glDrawElements(GL_TRIANGLES, HEIGHT * WIDTH*2*3, GL_UNSIGNED_INT, 0);


        //glBindVertexArray(0);


        glfwSwapBuffers(window);
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }


    glfwTerminate();
    return 0;
}



void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_1)) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    if (glfwGetKey(window, GLFW_KEY_2)) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
}