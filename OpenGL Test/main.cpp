#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Game.h"
#include "resources.hpp"
#include <iomanip>
#include <random>
#include <string>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);


Game game;

int WINDOW_WIDTH = 600, WINDOW_HEIGHT = 600;
bool press = false;
double mouse_pos_old_x;
double mouse_pos_old_y;
float game_pos_old_x;
float game_pos_old_y;
bool panning = false;


int main()
{
	// setup window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, 0);
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
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSwapInterval(1); // vsync
	//glfwSetWindowOpacity(window, 0.5f);


	// icon
	GLFWimage images[3] = {
		Resources::textureFromFile("assets/icons/64.png", false),
		Resources::textureFromFile("assets/icons/32.png", false),
		Resources::textureFromFile("assets/icons/16.png", false)
	};

	glfwSetWindowIcon(window, 3, images);

	for (char i = 0; i != 3; i++)  Resources::freeImageData(images[i].pixels);


	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("Failed to initialize GLAD");
		return -1;
	}

	srand((unsigned)time(0));
	game.init();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);


	while (!glfwWindowShouldClose(window))
	{
		processInput(window);
		//glfwGetFramebufferSize(window, &WINDOW_WIDTH, &WINDOW_HEIGHT);

		glClear(GL_COLOR_BUFFER_BIT);


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

	glClear(GL_COLOR_BUFFER_BIT);

	game.render();

	glfwSwapBuffers(window);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	game.scale += static_cast<float>(yoffset);
	if (game.scale < 1)  game.scale = 1;
	if (game.scale > 32) game.scale = 32;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		if (!panning) {
			glfwGetCursorPos(window, &mouse_pos_old_x, &mouse_pos_old_y);
			game_pos_old_x = game.x;
			game_pos_old_y = game.y;
			panning = true;
		}
	}
	else if (action == 0 && panning)
	{
		panning = false;
	}
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (panning)
	{
		double x;
		double y;
		glfwGetCursorPos(window, &x, &y);
		game.x = game_pos_old_x + (float)(x - mouse_pos_old_x) / game.scale * game.pan_speed;
		game.y = game_pos_old_y + (float)(y - mouse_pos_old_y) / game.scale * game.pan_speed;
	}
}

void processInput(GLFWwindow* window)
{
	// triangle render mode
	if (glfwGetKey(window, GLFW_KEY_1)) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else if (glfwGetKey(window, GLFW_KEY_2)) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	// regen
	else if (glfwGetKey(window, GLFW_KEY_3)) {
		if (press) return;
		press = true;

		game.update();
	}
	else if (glfwGetKey(window, GLFW_KEY_4)) {
		if (press) return;
		press = true;

		game.save();

	}
	else if (glfwGetKey(window, GLFW_KEY_5)) {
		if (press) return;
		press = true;

		game.load();
	}
	else {
		press = false;

		if (!panning)
		{
			if (glfwGetKey(window, GLFW_KEY_W))
				game.y += game.pan_speed;
			if (glfwGetKey(window, GLFW_KEY_S))
				game.y -= game.pan_speed;
			if (glfwGetKey(window, GLFW_KEY_D))
				game.x -= game.pan_speed;
			if (glfwGetKey(window, GLFW_KEY_A))
				game.x += game.pan_speed;
		}
	}
}