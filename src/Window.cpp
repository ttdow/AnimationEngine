#include "Window.h"

namespace Engine
{
	Window::Window()
	{
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(WIDTH, HEIGHT, "Application", nullptr, nullptr);
	}

	Window::~Window()
	{
		if (window != nullptr)
		{
			glfwDestroyWindow(window);
			window = nullptr;
		}

		glfwTerminate();
	}

	void Window::Update()
	{
		glfwPollEvents();
	}

	std::vector<const char*> Window::GetRequiredExtensions() const
	{
		uint32_t extensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&extensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + extensionCount);

		return extensions;
	}
}