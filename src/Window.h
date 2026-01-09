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
	};
}