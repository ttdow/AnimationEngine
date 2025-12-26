#pragma once

#include <GLFW/glfw3.h>

class App
{
public:

	void Run()
	{
		InitializeWindow();
		InitializeVulkan();
		MainLoop();
		Cleanup();
	}

private:

	GLFWwindow* window = nullptr;
	constexpr static uint32_t WIDTH = 800;
	constexpr static uint32_t HEIGHT = 600;

	void InitializeWindow()
	{	
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(WIDTH, HEIGHT, "Application", nullptr, nullptr);
	}

	void InitializeVulkan()
	{
		// Nothing yet.
	}

	void MainLoop()
	{
		while (!glfwWindowShouldClose(window))
		{
			glfwPollEvents();
		}
	}

	void Cleanup()
	{
		glfwDestroyWindow(window);

		glfwTerminate();
	}
};