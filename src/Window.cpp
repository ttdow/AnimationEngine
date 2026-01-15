#include "Window.h"

namespace Engine
{
	Window::Window()
	{
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(WIDTH, HEIGHT, "Application", nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, FramebufferResizeCallback);
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

	void Window::FramebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		Window* win = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		win->framebufferResized = true;
	}
}