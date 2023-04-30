#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <iostream>
#include <Windows.h>
#include <shellapi.h>
#include <chrono>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "Game.h"
#include "resources.hpp"

#define MAX_SAVES_LIST_COUNT 7

using namespace std::chrono;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);
void showMenuBar();


int inital_window_width = 600, inital_window_height = 600;
double mouse_pos_old_x;
double mouse_pos_old_y;
double game_pos_old_x,
game_pos_old_y;
bool panning = false;

Game game;


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

	GLFWwindow* window = glfwCreateWindow(inital_window_width, inital_window_height, "Wave Function Collapse | OpenGL", NULL, NULL);
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

	//GLFWcursor* cursor = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
	//glfwSetCursor(window, cursor);
	glfwSetCursorPos(window, (double)inital_window_width / 2, (double)inital_window_height / 2);
	glfwSetWindowPos(window, 350, 100);

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

	game.window_width = inital_window_width;
	game.window_height = inital_window_height;
	game.init();

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	bool wire_mode = false;

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);
		glfwGetFramebufferSize(window, &game.window_width, &game.window_height);
		game.update();


		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::ShowDemoWindow();
		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).

		showMenuBar();

		/*ImGui::Begin(SaveManager::current.name);

		ImGui::Checkbox("Wire mode", &wire_mode);
		ImGui::Text("%.1f FPS", io.Framerate);

		ImGui::End();*/

		ImGui::Render();

		glClear(GL_COLOR_BUFFER_BIT);

		wire_mode ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		game.render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}



void showMenuBar()
{
	static const float BASE_TEXT_WIDTH = ImGui::CalcTextSize("A").x;
	static const auto tz = current_zone();
	static const ImU32 ERROR_BG_COLOR = ImGui::GetColorU32(ImVec4(0.4f, 0.0f, 0.0f, 0.65f));

	static auto saves = SaveManager::getSavesList();
	static bool show_new_save_popup = false;

	// dir watcher
	if (SaveManager::hasSavesDirChanged())
		saves = SaveManager::getSavesList();

	// Menu bar
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save", "Ctrl+S"))
			{
				game.save();
				saves = SaveManager::getSavesList();
			}
			if (ImGui::MenuItem("Save As"))
			{
				game.save();
				saves = SaveManager::getSavesList();
			}
			if (ImGui::BeginMenu("Load"))
			{
				static ImGuiTextFilter filter;

				ImGui::SeparatorText("Saves");
				ImGui::BeginTable("Saves", 3, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerH);
				ImGui::TableSetupColumn("Name", 0, BASE_TEXT_WIDTH * 20);
				ImGui::TableSetupColumn("Time");
				ImGui::TableSetupColumn("Date");
				int count = 0;
				for (auto& save : saves)
				{
					if (++count > MAX_SAVES_LIST_COUNT)
					{
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::Text("...");
						ImGui::SameLine();
						filter.Draw("saves_list_filter", BASE_TEXT_WIDTH * 21);
						break;
					}
					if (!filter.PassFilter(save.name)) continue;

					ImGui::PushID(save.id);
					// Remove border
					if (count == 1)
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
					//ImGui::GetStyle() ImGuiStyleVar_FrameBorderSize
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);

					bool has_error = (save.version != SaveManager::version);

					if (has_error)
						ImGui::BeginDisabled();
					if (ImGui::Selectable(save.name, false, ImGuiSelectableFlags_SpanAllColumns))
					{
						game.load(save.id);
					}
					if (has_error)
						if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
						{
							ImGui::EndDisabled();
							ImGui::SetTooltip("Incompatible version (%i)", save.version);
							ImGui::BeginDisabled();
						}
					if (ImGui::BeginPopupContextItem())
					{
						//FIXME
						if (ImGui::SmallButton("Duplicate"))
						{
							SaveManager::current.id = SaveManager::newId();
							SaveManager::save();
							saves = SaveManager::getSavesList();
						}
						if (ImGui::SmallButton("Delete"))
						{
							SaveManager::deleteSave(save.id);
							saves = SaveManager::getSavesList();
						}
						ImGui::Separator();
						if (ImGui::SmallButton("Reveal Directory"))
						{
#ifdef WIN32
							ShellExecuteA(
								NULL, "open", SaveManager::getSaveDir(save.id).string().c_str(),
								NULL, NULL, SW_SHOWDEFAULT);
#else
							const std::string cmd = "open " + SaveManager::getSaveDir(save.id).string();
							std::system(cmd.c_str());
#endif
						}
						ImGui::EndPopup();
					}
					ImGui::TableSetColumnIndex(1);
					const auto local = tz->to_local(save.last_save_time);
					ImGui::Text(std::format("{0:%R}", local).c_str());
					ImGui::TableSetColumnIndex(2);
					ImGui::Text(std::format("{0:%a %D}", local).c_str());
					if (save.version != SaveManager::version) // reset bg
						ImGui::EndDisabled();
					if (count == 1) // Reset border
						ImGui::PopStyleVar();
					ImGui::PopID();
				}
				ImGui::EndTable();
				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("New", "Ctrl+N"))
				show_new_save_popup = true;

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();

		// Shortcuts
		if (!ImGui::IsPopupOpen("New World"))
		{
			if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_S, 0, ImGuiInputFlags_RouteGlobalLow))
			{
				game.save();
				saves = SaveManager::getSavesList();
			}
			else if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_N, 0, ImGuiInputFlags_RouteGlobalLow))
				show_new_save_popup = true;
		}
	}

	if (show_new_save_popup)
	{
		ImGui::OpenPopup("New World");
		show_new_save_popup = false;
	}
	if (ImGui::BeginPopupModal("New World"))
	{
		static char name[21];

		ImGui::InputText("Name", name, 21);
		if (ImGui::Button("Close"))
			ImGui::CloseCurrentPopup();
		ImGui::SameLine();
		if (ImGui::Button("Create"))
		{
			if (name[0] != '\0')
			{
				ImGui::CloseCurrentPopup();
				game.create();
				strcpy(SaveManager::current.name, name);
			}
		}
		ImGui::EndPopup();
	}
	ImGui::PopStyleVar();
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
	if (ImGui::GetIO().WantCaptureMouse) return;
	game.zoom += game.zoom / (float)yoffset / (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) ? 8 : 32 * 4);
	if (game.zoom < 0.6f)  game.zoom = 0.6f;
	if (game.zoom > 32) game.zoom = 32;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (ImGui::GetIO().WantCaptureMouse) return;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		if (!panning) {
			glfwGetCursorPos(window, &mouse_pos_old_x, &mouse_pos_old_y);
			game_pos_old_x = game.pan_x;
			game_pos_old_y = game.pan_y;
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
	if (ImGui::GetIO().WantCaptureMouse) return;
	if (panning)
	{
		double x;
		double y;
		glfwGetCursorPos(window, &x, &y);
		game.pan_x = game_pos_old_x + (float)(x - mouse_pos_old_x) / game.zoom * game.pan_speed;
		game.pan_y = game_pos_old_y + (float)(y - mouse_pos_old_y) / game.zoom * game.pan_speed;
	}
}

void processInput(GLFWwindow* window)
{
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureKeyboard || io.KeyCtrl) return;
	else {
		if (!panning)
		{
			if (glfwGetKey(window, GLFW_KEY_W))
				game.pan_y += game.pan_speed;
			if (glfwGetKey(window, GLFW_KEY_S))
				game.pan_y -= game.pan_speed;
			if (glfwGetKey(window, GLFW_KEY_D))
				game.pan_x -= game.pan_speed;
			if (glfwGetKey(window, GLFW_KEY_A))
				game.pan_x += game.pan_speed;
		}
	}
}