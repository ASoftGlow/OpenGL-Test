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

int WINDOW_WIDTH = 600, WINDOW_HEIGHT = 600;


int main()
{	
	// setup window

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



	srand(1);
	game.init();


	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);


	while (!glfwWindowShouldClose(window))
	{
		processInput(window);
		glfwGetFramebufferSize(window, &WINDOW_WIDTH, &WINDOW_HEIGHT);

		glClear(GL_COLOR_BUFFER_BIT);
		

		game.render();


		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	glfwTerminate();
	return 0;
}

//timeValue = glfwGetTime();



void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	game.scale += static_cast<float>(yoffset) * 4;
	//if (game.scale < 0) game.scale = 0;
	//if (game.scale > 32) game.scale = 32;
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