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

#include <vector>
#include <iostream>

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

		VulkanContext();

	private:

		VkInstance instance = VK_NULL_HANDLE;

		bool CheckValidationLayerSupport(const std::vector<const char*>& layers);

	};
}