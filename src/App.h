#pragma once

#include "Window.h"
#include "VulkanContext.h"

/*
#include <stdexcept>
#include <string>
#include <limits>
#include <algorithm>
#include <fstream>
*/

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

	/*
	constexpr static uint32_t MAX_FRAMES_IN_FLIGHT = 3;
	uint32_t currentFrame = 0;

	VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
	
	VkRenderPass renderPass = VK_NULL_HANDLE;
	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
	VkPipeline graphicsPipeline = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> swapChainFramebuffers;
	VkCommandPool commandPool = VK_NULL_HANDLE;
	std::vector<VkCommandBuffer> commandBuffers;
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;

	void Cleanup();
	void SetupDebugMessenger();
	void CreateRenderPass();
	void CreateGraphicsPipeline();
	void CreateFramebuffers();
	void CreateCommandPool();
	void CreateCommandBuffers();
	void CreateSyncObjects();

	void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
	void DrawFrame();

	static VkResult CreateDebugUtilsMessenger(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	static void DestroyDebugUtilsMessenger(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
	static std::vector<char> ReadFile(const std::string& filename);
	VkShaderModule CreateShaderModule(const std::vector<char>& code) const;

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
	)
	{
		(void)pUserData;
		(void)messageSeverity;
		(void)messageType;

		std::cerr << "Validation layer: " << pCallbackData->pMessage << '\n';

		return VK_FALSE;
	}
	*/
};