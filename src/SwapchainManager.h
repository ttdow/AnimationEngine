#pragma once

#ifndef VK_NO_PROTOTYPES
#define VK_NO_PROTOTYPES
#endif

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

#include <volk/volk.h>

#include "VulkanStructs.h"

#include <iostream>
#include <vector>
#include <algorithm>

namespace Engine
{
	class SwapchainManager
	{
	public:

		SwapchainManager(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkDevice device);

	private:

		VkSwapchainKHR swapChain = VK_NULL_HANDLE;
		VkFormat swapChainImageFormat = VK_FORMAT_UNDEFINED;
		VkExtent2D swapChainExtent{};
		std::vector<VkImage> swapChainImages;
		std::vector<VkImageView> swapChainImageViews;

		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice physDevice, VkSurfaceKHR surface) const;
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		void PrintSurfaceFormat(const VkSurfaceFormatKHR& format);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	};
}