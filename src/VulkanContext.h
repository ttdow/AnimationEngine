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
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 0
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
#include <map>

namespace Engine
{
	static inline void Check(VkResult result)
	{
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Vulkan call returned an error.");
		}
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

		const std::vector<const char*> validationLayers =
		{
			"VK_LAYER_KHRONOS_validation"
		};

		const std::vector<const char*> deviceExtensions =
		{
			"VK_KHR_swapchain"
		};

		Window& window;
		VkInstance instance = VK_NULL_HANDLE;
		VkSurfaceKHR surface = VK_NULL_HANDLE;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkDevice device = VK_NULL_HANDLE;
		VkQueue graphicsQueue = VK_NULL_HANDLE;
		VkQueue presentQueue = VK_NULL_HANDLE;
		std::unique_ptr<SwapchainManager> swapchainManager;
		std::unique_ptr<VulkanPipeline> pipeline;

		void CreateInstance();
		void CreateSurface();
		void PickPhysicalDevice();
		void CreateLogicalDevice();
		void CreateSwapchain();
		void CreatePipeline();

		static bool CheckValidationLayerSupport(const std::vector<const char*>& layers);
		static std::vector<const char*> GetRequiredExtensions(const Window& window, bool enableValidationLayers);
		uint32_t RatePhysicalDevice(VkPhysicalDevice physDevice) const;
		bool CheckDeviceExtensionSupport(VkPhysicalDevice physDevice) const;
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device) const;
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice physDevice) const;
	};
}