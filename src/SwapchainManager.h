#pragma once

#ifndef VK_NO_PROTOTYPES
#define VK_NO_PROTOTYPES
#endif

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

#include <volk/volk.h>

#include "VulkanStructs.h"
#include "Window.h"

#include <iostream>
#include <vector>
#include <algorithm>

namespace Engine
{
	class SwapchainManager
	{
	public:

		SwapchainManager() = delete;
		SwapchainManager(Window& window, PhysicalDevice physicalDevice, VkSurfaceKHR surface, VkDevice device);
		~SwapchainManager();

		void RecreateSwapchain(VkSurfaceKHR surface, PhysicalDevice physicalDevice);

		VkSwapchainKHR& GetSwapchain() { return swapchain; }
		VkImage GetImage(size_t i) { return swapchainImages[i]; }
		VkImageView GetImageView(size_t i) { return swapchainImageViews[i]; }
		VkExtent2D GetExtent() { return swapchainExtent; }

	private:

		Window& window;
		VkSurfaceKHR surface = VK_NULL_HANDLE;
		VkDevice device = VK_NULL_HANDLE;
		PhysicalDevice physicalDevice{};
		VkSwapchainKHR swapchain = VK_NULL_HANDLE;
		VkFormat swapchainImageFormat = VK_FORMAT_UNDEFINED;
		std::vector<VkImage> swapchainImages;
		std::vector<VkImageView> swapchainImageViews;
		VkExtent2D swapchainExtent{};

		void CreateSwapchain();
		void CreateImages();
		void CreateViews();
		void Cleanup();

		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice physDevice, VkSurfaceKHR surface) const;
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		void PrintSurfaceFormat(const VkSurfaceFormatKHR& format);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	};
}