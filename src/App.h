#pragma once

#include "Window.h"
#include "VulkanContext.h"

class App
{
public:

	App() = default;
	~App();

	void Run();

private:

	void InitializeWindow();
	void InitializeVulkan();
	void MainLoop();

	std::unique_ptr<Engine::Window> window;
	std::unique_ptr<Engine::VulkanContext> vulkanContext;
};