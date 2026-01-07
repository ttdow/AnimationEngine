#pragma once

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif

#include <GLFW/glfw3.h>

namespace Engine
{
	class Window
	{
	public:

		Window();

	private:

		GLFWwindow* window = nullptr;
		constexpr static uint32_t WIDTH = 800;
		constexpr static uint32_t HEIGHT = 600;
	};
}