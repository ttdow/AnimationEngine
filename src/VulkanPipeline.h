#pragma once

#ifndef VK_NO_PROTOTYPES
#define VK_NO_PROTOTYPES
#endif

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

#include <volk/volk.h>

#include <vector>

namespace Engine
{
	class VulkanPipeline
	{
	public:

		VulkanPipeline() = delete;
		VulkanPipeline(VkDevice device);
		~VulkanPipeline();

	private:

		VkDevice device;
		VkPipeline pipeline = VK_NULL_HANDLE;
		VkPipelineLayout layout = VK_NULL_HANDLE;

		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
		VkPipelineInputAssemblyStateCreateInfo inputAssembly;
		VkPipelineRasterizationStateCreateInfo rasterizer;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineMultisampleStateCreateInfo multisampling;
		VkPipelineDepthStencilStateCreateInfo depthStencil;
		VkPipelineRenderingCreateInfo renderInfo;
		VkFormat colorAttachmentFormat;

		void Clear();
		void Build();

	};
}