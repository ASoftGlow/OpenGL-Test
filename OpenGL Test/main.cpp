#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <iostream>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <shader_c.h>
using namespace std;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);



int main()
{
    int WINDOW_WIDTH  = 600;
    int WINDOW_HEIGHT = 600;
    float SCALE = 9;




    // setup vertex data

    const unsigned int WIDTH = 4, HEIGHT = 4;

    const unsigned char vr_rots[4][3][2] = {
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

    unsigned char tiles[HEIGHT][WIDTH][2] = {
        {{2,2},{3,3},{4,3},{1,0}},
        {{3,1},{2,0},{1,0},{1,0}},
        {{0,0},{3,0},{4,0},{1,0}},
        {{0,0},{0,0},{2,0},{1,0}}
    };

    unsigned int ind;
    unsigned char rot;
    const unsigned short TOTAL_TILES = 5;
    float vertices[HEIGHT * WIDTH * 2 * 3 * 5]{};
    float scale_x = 1.0f / (WIDTH*8);
    float scale_y = 1.0f / (HEIGHT*8);
    float offset_x = WIDTH / -2.0f;
    float offset_y = HEIGHT / -2.0f;


    for (unsigned int y = 0; y != HEIGHT; y++) {
        for (unsigned int x = 0; x != WIDTH; x++) 
        {
            for (unsigned char j = 0; j != 2; j++) {

                ind = (x + y * WIDTH) * 2*3*5 + j * 3*5;

                rot = tiles[y][x][1];


                vertices[ind + 0 + 0] = (x + vr_rots[rot][0][0] + offset_x) * scale_x;
                vertices[ind + 0 + 1] = (y + vr_rots[rot][0][1] + offset_y) * scale_y;
                vertices[ind + 0 + 2] = (tx_rots[rot][0][0] + tiles[y][x][0]) / float (TOTAL_TILES);
                vertices[ind + 0 + 3] = tx_rots[rot][0][1];
                vertices[ind + 0 + 4] = 0;
                
                vertices[ind + 5 + 0] = (x + (j ? 1-vr_rots[rot][1][0]: vr_rots[rot][1][0]) + offset_x) * scale_x;
                vertices[ind + 5 + 1] = (y + (j ? 1-vr_rots[rot][1][1]: vr_rots[rot][1][1]) + offset_y) * scale_y;
                vertices[ind + 5 + 2] = ((j ? 1-tx_rots[rot][1][0]: tx_rots[rot][1][0]) + tiles[y][x][0]) / float (TOTAL_TILES);
                vertices[ind + 5 + 3] = (j ? 1.0f-tx_rots[rot][1][1]: tx_rots[rot][1][1]);  
                vertices[ind + 5 + 4] = 0;

                vertices[ind + 10 + 0] = (x + vr_rots[rot][2][0] + offset_x) * scale_x;
                vertices[ind + 10 + 1] = (y + vr_rots[rot][2][1] + offset_y) * scale_y;
                vertices[ind + 10 + 2] = (tx_rots[rot][2][0] + tiles[y][x][0]) / float (TOTAL_TILES);
                vertices[ind + 10 + 3] = tx_rots[rot][2][1];  
                vertices[ind + 10 + 4] = 0;
            };
        };
    };
    

    // print vertices
    /*for (int i = 0; i != sizeof(vertices) / sizeof(float) / 5; i++) {
        for (int j = 0; j != 5; j++) {
            cout << vertices[i*5+j] << ' ';
        };
        cout << endl;
    };*/

    float vertices2[2*3*5] = {
        0, 0,    0, 0,  0,
        0, 0.2f,  0, 1,  0,
        1, 0.2f,  1, 1,  0,

        1, 0.2f,  1, 1,  0,
        1, 0,    1, 0,  0,
        0, 0,    0, 0,  0
    };

    /*unsigned char r = 3;

    float vertices3[5 * 3 * 5] = {
        0.5f*(r%2), 0.0f, r%3!=0, r%3!=0, 9.0f,
        0.0f, 0.5f,       1<r, r%3==0,    9.0f,
        0.5f, 0.5f,       r%3==0, r<2,    9.0f,
    };

    

    for (int i = 0; i != 5 * 3; i++) {
        cout << vertices3[i] << " ";
    };*/


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
    glfwSwapInterval(1); // vsync

    //glfwSetWindowOpacity(window, 0.5f);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD");
        return -1;
    }

    Shader shaderProgram("vertex.vert", "fragment.frag");




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
    unsigned int textures;
    unsigned char* data;
    int width, height, nrChannels;

    stbi_set_flip_vertically_on_load(true);


    glGenTextures(1, &textures);
    glBindTexture(GL_TEXTURE_2D, textures);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);//GL_LINEAR_MIPMAP_LINEAR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // load and generate the texture
    data = stbi_load("atlas.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures);




    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    double timeValue = glfwGetTime(), pastTime = timeValue;
    int frames = 0, fps;

    shaderProgram.use();


    while (!glfwWindowShouldClose(window))
    {
        /*frames += 1;
        if ((timeValue - pastTime) > 1.0)
        {
            fps = round(frames * 1000 / (timeValue - pastTime));
            pastTime = timeValue;
            frames = 0;
            cout << fps << endl;
        }*/

        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);



        glfwGetFramebufferSize(window, &WINDOW_WIDTH, &WINDOW_HEIGHT);
        //timeValue = glfwGetTime();
        //SCALE = 5*sin(timeValue/10)+5+1;
        shaderProgram.setFloat("scale_x", SCALE * (600.0f / WINDOW_WIDTH));
        shaderProgram.setFloat("scale_y", SCALE * (600.0f / WINDOW_HEIGHT));

        // TODO: only set when fb size changed
        
        
        glBindVertexArray(VAOs[0]);
        glDrawArrays(GL_TRIANGLES, 0, HEIGHT* WIDTH * 2 * 3);
        //glDrawArrays(GL_POINTS, 0, 3);


        //glBindVertexArray(0);

        


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


void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_1)) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    else if (glfwGetKey(window, GLFW_KEY_2)) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
}