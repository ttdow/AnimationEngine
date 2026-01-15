#pragma once

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif

#include <GLFW/glfw3.h>

#include <cstdint>
#include <vector>

namespace Engine
{
	class Window
	{
	public:

		bool framebufferResized = false;

		Window();
		~Window();
		void Update();

		std::vector<const char*> GetRequiredExtensions() const;
		GLFWwindow* GetGLFWwindow() const { return window; }
		int CheckClose() const { return glfwWindowShouldClose(window); }

	private:

		GLFWwindow* window = nullptr;
		constexpr static uint32_t WIDTH = 800;
		constexpr static uint32_t HEIGHT = 600;
	
		static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
	};
}