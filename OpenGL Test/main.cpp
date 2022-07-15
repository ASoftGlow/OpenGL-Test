#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
using namespace std;

//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"

//#include "resources.hpp"
//#include "tile.h"
#include "Game.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

Game game;

const unsigned int WIDTH = 16, HEIGHT = 16;

int WINDOW_WIDTH = 600;
int WINDOW_HEIGHT = 600;


int main()
{



    /*const unsigned char vr_rots[4][3][2] = {
        0, 0, // 0
        0, 1,
        1, 1,

        0, 1, // 1
        1, 1,
        1, 0,

        0, 0, // 2
        0, 1,
        1, 1,

        0, 1, // 3
        1, 1,
        1, 0,
    };

    const unsigned char tx_rots[4][3][2] = {
        0, 0, // 0
        0, 1,
        1, 1,

        0, 0, // 1
        0, 1,
        1, 1,

        1, 1, // 2
        1, 0,
        0, 0,

        1, 1, // 3
        1, 0,
        0, 0,
    };

    Tile tiles[HEIGHT][WIDTH] = {
        {{4,1},{3,1},{5,1},{0,1},{0,0},{2,0},{2,2},{0,3},{0,1},{0,0},{0,2},{5,0},{2,0},{4,1},{2,1},{2,1}},
        {{3,1},{3,3},{3,2},{0,2},{5,3},{2,0},{2,2},{3,3},{2,3},{2,3},{2,3},{3,2},{3,0},{3,1},{3,3},{2,3}},
        {{3,3},{4,3},{2,2},{0,1},{5,1},{2,0},{4,2},{4,3},{1,2},{1,3},{1,0},{2,2},{5,1},{3,3},{4,3},{1,2}},
        {{4,3},{1,3},{2,2},{0,2},{0,3},{2,0},{1,0},{1,2},{1,1},{1,0},{1,2},{4,2},{2,3},{4,3},{1,2},{1,3}},
        {{1,3},{1,1},{2,2},{0,3},{5,3},{3,0},{2,1},{2,1},{2,1},{2,1},{4,0},{1,3},{1,3},{1,1},{1,1},{1,3}},
        {{2,1},{4,0},{2,2},{3,3},{3,2},{0,2},{0,1},{5,2},{5,1},{5,0},{3,0},{2,1},{4,0},{1,1},{1,3},{1,1}},
        {{3,2},{2,0},{4,2},{4,3},{4,2},{3,2},{5,2},{0,2},{0,2},{5,2},{5,1},{0,0},{3,0},{4,0},{4,1},{2,1}},
        {{2,2},{3,0},{4,0},{1,0},{1,3},{4,2},{3,2},{5,0},{5,3},{0,2},{5,3},{0,2},{5,3},{3,0},{3,1},{3,3}},
        {{2,2},{0,0},{3,0},{2,1},{4,0},{1,2},{2,2},{0,0},{0,1},{5,0},{3,3},{2,3},{3,2},{5,0},{3,3},{4,3}},
        {{4,2},{2,3},{3,2},{5,0},{3,0},{4,0},{4,2},{2,3},{2,3},{2,3},{4,3},{1,2},{2,2},{3,3},{4,3},{1,2}},
        {{1,3},{1,2},{2,2},{0,1},{5,1},{2,0},{1,0},{1,2},{1,0},{1,2},{1,0},{1,0},{4,2},{4,3},{1,2},{1,1}},
        {{1,3},{1,3},{2,2},{5,2},{0,3},{3,0},{2,1},{4,0},{4,1},{4,0},{1,2},{1,1},{1,2},{1,2},{1,2},{1,1}},
        {{1,2},{1,3},{4,2},{2,3},{3,2},{5,1},{5,1},{3,0},{3,1},{2,0},{1,1},{1,0},{1,1},{1,3},{1,0},{1,2}},
        {{1,3},{1,2},{1,0},{1,1},{4,2},{2,3},{2,3},{2,3},{3,2},{3,0},{4,0},{1,0},{1,0},{1,2},{1,0},{4,1}},
        {{4,0},{4,1},{4,0},{1,0},{1,1},{1,3},{4,1},{4,0},{2,2},{5,3},{2,0},{1,3},{4,1},{4,0},{4,1},{3,1}},
        {{3,0},{3,1},{2,0},{1,3},{1,0},{4,1},{3,1},{2,0},{4,2},{3,2},{3,0},{2,1},{3,1},{3,0},{3,1},{3,3}}
    };

    unsigned int ind;
    unsigned char rot;
    const unsigned short TOTAL_TILES = 6;
    float vertices[HEIGHT * WIDTH * 2 * 3 * 5]{};
    float scale_x = 1.0f / (WIDTH*8);
    float scale_y = 1.0f / (HEIGHT*8);
    float offset_x = WIDTH / -2.0f;
    float offset_y = HEIGHT / -2.0f;

    // setup vertex data

    for (unsigned int y = 0; y != HEIGHT; y++) {
        for (unsigned int x = 0; x != WIDTH; x++) 
        {
            for (unsigned char j = 0; j != 2; j++) {

                ind = (x + y * WIDTH) * 2*3*5 + j * 3*5;

                rot = tiles[y][x].rotation;


                vertices[ind + 0 + 0] = (x + vr_rots[rot][0][0] + offset_x) * scale_x;
                vertices[ind + 0 + 1] = (y + vr_rots[rot][0][1] + offset_y) * scale_y;
                vertices[ind + 0 + 2] = (tx_rots[rot][0][0] + tiles[y][x].type) / float (TOTAL_TILES);
                vertices[ind + 0 + 3] = tx_rots[rot][0][1];
                vertices[ind + 0 + 4] = 0;
                
                vertices[ind + 5 + 0] = (x + (j ? 1-vr_rots[rot][1][0]: vr_rots[rot][1][0]) + offset_x) * scale_x;
                vertices[ind + 5 + 1] = (y + (j ? 1-vr_rots[rot][1][1]: vr_rots[rot][1][1]) + offset_y) * scale_y;
                vertices[ind + 5 + 2] = ((j ? 1-tx_rots[rot][1][0]: tx_rots[rot][1][0]) + tiles[y][x].type) / float (TOTAL_TILES);
                vertices[ind + 5 + 3] = (j ? 1.0f-tx_rots[rot][1][1]: tx_rots[rot][1][1]);  
                vertices[ind + 5 + 4] = 0;

                vertices[ind + 10 + 0] = (x + vr_rots[rot][2][0] + offset_x) * scale_x;
                vertices[ind + 10 + 1] = (y + vr_rots[rot][2][1] + offset_y) * scale_y;
                vertices[ind + 10 + 2] = (tx_rots[rot][2][0] + tiles[y][x].type) / float (TOTAL_TILES);
                vertices[ind + 10 + 3] = tx_rots[rot][2][1];  
                vertices[ind + 10 + 4] = 0;
            };
        };
    };
    
    



    */
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, 1);
    //glfwWindowHint(GLFW_DECORATED, 0);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Wave Function Collapse | OpenGL", NULL, NULL);
    if (window == NULL)
    {
        printf("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSwapInterval(1); // vsync

    //glfwSetWindowOpacity(window, 0.5f);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD");
        return -1;
    }

    game.init();

    //Shader shaderProgram{ "vertex.vert", "fragment.frag" };

    /*Shader shaderProgram = Resources::loadShader("vertex.vert", "fragment.frag", "tiles");
    

    unsigned int VBOs[1], VAOs[1];
    glGenBuffers(1, VBOs);
    glGenVertexArrays(1, VAOs);



    glBindVertexArray(VAOs[0]);

    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 4, GL_FLOAT, 0, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // pos & tex

    glVertexAttribPointer(1, 1, GL_FLOAT, 0, 5 * sizeof(float), (void*)(4 * sizeof(float)));
    glEnableVertexAttribArray(1); // tile type

    //glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

    // unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    // load textures

    Resources::flipTextures(true);
    Texture atlas_texture = Resources::loadTexture("atlas.png", true, "atlas");

    glActiveTexture(GL_TEXTURE0);
    atlas_texture.bind();



    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //double timeValue;

    shaderProgram.use();

    shaderProgram.setVector2f("scale", game.scale* (600.0f / WINDOW_WIDTH), game.scale* (600.0f / WINDOW_HEIGHT));*/


    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);



        glfwGetFramebufferSize(window, &WINDOW_WIDTH, &WINDOW_HEIGHT);
        //timeValue = glfwGetTime();
        //SCALE = 5*sin(timeValue/10)+5+1;
        
        
        /*glBindVertexArray(VAOs[0]);
        glDrawArrays(GL_TRIANGLES, 0, HEIGHT * WIDTH * 2 * 3);*/
        //glDrawArrays(GL_POINTS, 0, 3);


        //glBindVertexArray(0);

        game.render();


        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glfwTerminate();
    return 0;
}



void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    game.scale += static_cast<float>(yoffset) * 4;
    //if (game.scale < 0) game.scale = 0;
    //if (game.scale > 32) game.scale = 32;

    //Shader s = Resources::getShader("tiles");
    //s.setVector2f("scale", game.scale * (600.0f / WINDOW_WIDTH), game.scale * (600.0f / WINDOW_HEIGHT));
}


void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_1)) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    else if (glfwGetKey(window, GLFW_KEY_2)) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
}