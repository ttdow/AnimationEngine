#include "VulkanContext.h"

namespace Engine
{
	VulkanContext::VulkanContext()
	{
		Check(volkInitialize());

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pNext = nullptr;
		appInfo.pApplicationName = "Application";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_3;

		const bool enableValidationLayers = false;

		std::vector<const char*> extensions{};

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

	}

	bool VulkanContext::CheckValidationLayerSupport(const std::vector<const char*>& layers)
	{
#ifdef NDEBUG
		return false;
#endif

		uint32_t layerCount = 0;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		std::cout << "Available validation layers:\n";

		for (const VkLayerProperties& layerProperties : availableLayers)
		{
			std::cout << '\t' << layerProperties.layerName << '\n';
		}

		return false;
	}
}