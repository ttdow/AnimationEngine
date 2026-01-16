#pragma once

#ifndef VK_NO_PROTOTYPES
#define VK_NO_PROTOTYPES
#endif

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

#include <volk/volk.h>

#include <glm/glm.hpp>

#include <cstdint>
#include <vector>
#include <optional>
#include <array>
#include <string>

namespace Engine
{
	struct Vertex
	{
		glm::vec4 pos;
		glm::vec4 color;

		static VkVertexInputBindingDescription GetBindingDescription()
		{
			VkVertexInputBindingDescription bindingDescription{};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescription()
		{
			std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, pos);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, color);

			return attributeDescriptions;
		}
	};

	struct Texture
	{
		uint32_t id;
		std::string type;
	};

	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool IsComplete() const
		{
			return graphicsFamily.has_value() &&
				presentFamily.has_value();
		}
	};

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	struct PhysicalDevice
	{
		VkPhysicalDevice device = VK_NULL_HANDLE;
		VkPhysicalDeviceProperties properties{};
		VkPhysicalDeviceFeatures features{};
		uint32_t rating = 0;

		QueueFamilyIndices queueFamilyIndices;
		SwapChainSupportDetails swapChainSupportDetails;
	};
}