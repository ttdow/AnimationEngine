#pragma once

#ifndef VK_NO_PROTOTYPES
#define VK_NO_PROTOTYPES
#endif

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

#include <volk/volk.h>

#ifndef VMA_STATIC_VULKAN_FUNCTIONS
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#endif
#ifndef VMA_DYNAMIC_VULKAN_FUNCTIONS
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
#endif

#include <vma/vk_mem_alloc.h>

#include "Window.h"
#include "VulkanStructs.h"
#include "SwapchainManager.h"
#include "VulkanPipeline.h"

#include <cstdint>
#include <vector>
#include <iostream>
#include <cstring>
#include <memory>
#include <optional>
#include <set>
#include <fstream>

namespace Engine
{
	static inline void Check(VkResult result)
	{
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Vulkan call returned an error.");
		}
	}

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

	class VulkanContext
	{
	public:

		VulkanContext() = delete;
		VulkanContext(Window& window);
		~VulkanContext();

		void Draw();
		void WaitForIdle() const;

	private:

		const std::vector<const char*> requiredValidationLayers =
		{
			"VK_LAYER_KHRONOS_validation"
		};

		const std::vector<const char*> deviceExtensions =
		{
			"VK_KHR_swapchain"
		};

		constexpr static uint32_t MAX_FRAMES_IN_FLIGHT = 3;
		uint32_t currentFrame = 0;

		bool enableValidationLayers = false;
		std::vector<VkLayerProperties> availableValidationLayers;
		std::vector<const char*> requiredInstanceExtensions;
		std::vector<VkExtensionProperties> availableInstanceExtensions;
		std::vector<PhysicalDevice> physicalDevices;
		size_t selectedPhysicalDeviceIndex = 0;

		Window& window;
		VkInstance instance = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
		VkSurfaceKHR surface = VK_NULL_HANDLE;
		PhysicalDevice physicalDevice{};
		VkDevice device = VK_NULL_HANDLE;
		VmaAllocator allocator;
		VkQueue graphicsQueue = VK_NULL_HANDLE;
		VkQueue presentQueue = VK_NULL_HANDLE;
		std::unique_ptr<SwapchainManager> swapchainManager;
		std::unique_ptr<VulkanPipeline> pipeline;
		VkCommandPool commandPool = VK_NULL_HANDLE;
		std::vector<VkCommandBuffer> commandBuffers;
		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;

		void CreateInstance();
		void SetupDebugMessenger();
		void CreateSurface();
		void PickPhysicalDevice();
		void CreateLogicalDevice();
		void CreateAllocator();
		void CreateSwapchain();
		void CreatePipeline();
		void CreateCommandPool();
		void CreateCommandBuffers();
		void CreateSyncObjects();
		
		void RecordCommandBuffer(VkCommandBuffer& cmd, uint32_t imageIndex);
		void DrawFrame();
		void TransitionImage(VkCommandBuffer& cmd, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout, VkImageAspectFlags aspectFlags);

		static bool CheckValidationLayerSupport(std::vector<VkLayerProperties>& availableLayers, const std::vector<const char*>& requiredLayers, bool print = false);
		bool CheckInstanceExtensionSupport(std::vector<VkExtensionProperties>& availableInstanceExtensions, bool print = false);
		
		uint32_t RatePhysicalDevice(PhysicalDevice& physDevice);
		bool CheckDeviceExtensionSupport(const PhysicalDevice& physDevice) const;
		QueueFamilyIndices FindQueueFamilies(const PhysicalDevice& device) const;
		SwapChainSupportDetails QuerySwapChainSupport(PhysicalDevice& physDevice) const;

		static VkResult CreateDebugUtilsMessenger(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
		static void DestroyDebugUtilsMessenger(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
	
		static std::vector<char> ReadFile(const std::string& filename);
		VkShaderModule CreateShaderModule(const std::vector<char>& code) const;
	};
}