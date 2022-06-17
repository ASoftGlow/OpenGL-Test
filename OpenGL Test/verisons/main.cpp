#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <shader_c.h>
#include <cmath>
#include <iostream>
#include <string>
using namespace std;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);



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
    
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Wave Function Collapse | OpenGL", NULL, NULL);
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



    // setup vertex data

    const unsigned int WIDTH = 4, HEIGHT = 4;

    unsigned char tiles[HEIGHT][WIDTH][2] = {
        {{1,0},{0,0},{1,0},{1,0}},
        {{0,2},{1,0},{0,1},{0,2}},
        {{1,0},{1,3},{0,1},{0,3}},
        {{0,0},{0,0},{1,0},{0,0}}
    };

    float vertices[HEIGHT * WIDTH * 2*3*5];

    for (unsigned int y = 0; y != HEIGHT; y++) {
        for (unsigned int x = 0; x != WIDTH; x++) 
        {
            for (unsigned short j = 0; j != 2; j++) {

                int ind = (x + y * WIDTH) * 2*3*5 + j * 3*5;

                vertices[ind + 0 + 0] = x / 5.0f;
                vertices[ind + 0 + 1] = y / 5.0f;
                vertices[ind + 0 + 2] = 0;
                vertices[ind + 0 + 3] = 0;
                vertices[ind + 0 + 4] = tiles[y][x][0];

                vertices[ind + 5 + 0] = (x+j) / 5.0f;
                vertices[ind + 5 + 1] = (y+(1-j)) / 5.0f;
                vertices[ind + 5 + 2] = j;
                vertices[ind + 5 + 3] = 1-j;
                vertices[ind + 5 + 4] = tiles[y][x][0];

                vertices[ind + 10 + 0] = (x+1) / 5.0f;
                vertices[ind + 10 + 1] = (y+1) / 5.0f;
                vertices[ind + 10 + 2] = 1;
                vertices[ind + 10 + 3] = 1;
                vertices[ind + 10 + 4] = tiles[y][x][0];
            };
        };
    };
    

    // print vertices
    for (int i = 0; i != sizeof(vertices) / sizeof(float) / 5; i++) {
        for (int j = 0; j != 5; j++) {
            cout << vertices[i*5+j] << ' ';
        };
        cout << endl;
    };
    /*
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



    unsigned int VBOs[1], VAOs[1];
    glGenBuffers(1, VBOs);
    glGenVertexArrays(1, VAOs);



    glBindVertexArray(VAOs[0]);

    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, 0, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // pos

    glVertexAttribPointer(1, 2, GL_FLOAT, 0, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1); // texture coords

    glVertexAttribPointer(2, 1, GL_FLOAT, 0, 5 * sizeof(float), (void*)(4 * sizeof(float)));
    glEnableVertexAttribArray(2); // tile type

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

    stbi_set_flip_vertically_on_load(true);

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

        glActiveTexture(GL_TEXTURE0+t);
        glBindTexture(GL_TEXTURE_2D, textures[t]);
    };





    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


        //processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        shaderProgram.use();

        for (unsigned short t = 0; t != 5; t++)
        {
            shaderProgram.setInt("tile" + to_string(t), t);
        }
            

        glBindVertexArray(VAOs[0]);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        //glDrawArrays(GL_POINTS, 0, 3);
        //glDrawElements(GL_TRIANGLES, HEIGHT * WIDTH*2*3, GL_UNSIGNED_INT, 0);


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