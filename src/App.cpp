#include "App.h"

App::~App()
{
	vulkanContext.reset();
	window.reset();
}

void App::Run()
{
	InitializeWindow();
	InitializeVulkan();
	MainLoop();
}

void App::InitializeWindow()
{
	window = std::make_unique<Engine::Window>();
}

void App::InitializeVulkan()
{
	vulkanContext = std::make_unique<Engine::VulkanContext>(*window);
}

void App::MainLoop()
{
	while (!window->CheckClose())
	{
		window->Update();
		vulkanContext->Draw();
	}

	vulkanContext->WaitForIdle();
}