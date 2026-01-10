#include "VulkanPipeline.h"

namespace Engine
{
	VulkanPipeline::VulkanPipeline(VkDevice device) : device(device)
	{
		Clear();
	}

	VulkanPipeline::~VulkanPipeline()
	{
		Clear();
	}

	void VulkanPipeline::Clear()
	{
		inputAssembly = {};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.pNext = nullptr;

		rasterizer = {};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.pNext = nullptr;

		colorBlendAttachment = {};

		multisampling = {};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.pNext = nullptr;

		depthStencil = {};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.pNext = nullptr;

		renderInfo = {};
		renderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
		renderInfo.pNext = nullptr;

		shaderStages.clear();

		if (pipeline != VK_NULL_HANDLE)
		{
			vkDestroyPipeline(device, pipeline, nullptr);
		}

		pipeline = VK_NULL_HANDLE;

		if (layout != VK_NULL_HANDLE)
		{
			vkDestroyPipelineLayout(device, layout, nullptr);
		}

		layout = VK_NULL_HANDLE;
	}

	void VulkanPipeline::Build()
	{
		VkPipelineViewportStateCreateInfo viewportState = {};
	}
}