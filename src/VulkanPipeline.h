#pragma once

#ifndef VK_NO_PROTOTYPES
#define VK_NO_PROTOTYPES
#endif

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

#include <volk/volk.h>

#include <vector>
#include <iostream>

namespace Engine
{
	class VulkanPipeline
	{
	public:

		VulkanPipeline() = delete;
		VulkanPipeline(VkDevice& device);
		~VulkanPipeline();

		void Clear();
		void SetLayout();
		void SetShaders(VkShaderModule& vertexShader, VkShaderModule& fragmentShader);
		void SetInputTopology(VkPrimitiveTopology topology);
		void SetPolygonMode(VkPolygonMode polygonMode);
		void SetCullMode(VkCullModeFlags cullModeFlags, VkFrontFace frontFace);
		void SetMultisamplingNone();
		void DisableBlending();
		void DisableDepthTest();
		void SetColorAttachmentFormat(VkFormat format);
		void Build();

		VkPipeline& GetPipeline() { return pipeline; }
		VkPipelineLayout& GetLayout() { return layout; }


	private:

		VkDevice& device;
		VkPipelineLayout layout = VK_NULL_HANDLE;
		VkPipeline pipeline = VK_NULL_HANDLE;

		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
		VkPipelineInputAssemblyStateCreateInfo inputAssembly;
		VkPipelineRasterizationStateCreateInfo rasterizer;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineMultisampleStateCreateInfo multisampling;
		VkPipelineDepthStencilStateCreateInfo depthStencil;
		VkPipelineRenderingCreateInfo renderInfo;
		VkFormat colorAttachmentFormat;
	};
}