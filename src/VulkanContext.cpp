#define VOLK_IMPLEMENTATION
#define VMA_IMPLEMENTATION

#include "VulkanContext.h"

namespace Engine
{
	VulkanContext::VulkanContext(Window& window) : window(window)
	{
		Check(volkInitialize());
		CreateInstance();
		volkLoadInstance(instance);
		CreateSurface();
		PickPhysicalDevice();
		CreateLogicalDevice();
		volkLoadDevice(device);
		CreateSwapchain();
		CreatePipeline();
	}

	VulkanContext::~VulkanContext()
	{
		if (swapchainManager)
		{
			swapchainManager.reset();
		}

		if (device != VK_NULL_HANDLE)
		{
			vkDestroyDevice(device, nullptr);
		}

		if (surface != VK_NULL_HANDLE)
		{
			vkDestroySurfaceKHR(instance, surface, nullptr);
		}

		if (instance != VK_NULL_HANDLE)
		{
			vkDestroyInstance(instance, nullptr);
		}
	}

	void VulkanContext::Draw()
	{
		// Nothing yet.
	}

	void VulkanContext::WaitForIdle() const
	{
		if (device != VK_NULL_HANDLE)
		{
			vkDeviceWaitIdle(device);
		}
	}

	void VulkanContext::CreateInstance()
	{
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pNext = nullptr;
		appInfo.pApplicationName = "Application";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_3;

		const bool enableValidationLayers = CheckValidationLayerSupport(validationLayers);

		std::vector<const char*> extensions = GetRequiredExtensions(window, enableValidationLayers);

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		if (enableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else
		{
			createInfo.enabledLayerCount = 0;
			createInfo.ppEnabledLayerNames = nullptr;
		}

		Check(vkCreateInstance(&createInfo, nullptr, &instance));

		std::cout << "Vulkan Instance created successfully.\n";

		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> extensionProperties(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensionProperties.data());

		std::cout << "Available instance extensions:\n";

		for (const VkExtensionProperties& extension : extensionProperties)
		{
			std::cout << '\t' << extension.extensionName << '\n';
		}
	}

	void VulkanContext::CreateSurface()
	{
		Check(glfwCreateWindowSurface(instance, window.GetGLFWwindow(), nullptr, &surface));
	}

	void VulkanContext::PickPhysicalDevice()
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		if (deviceCount == 0)
		{
			throw std::runtime_error("Failed to find a physical device with Vulkan support!");
		}

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		std::multimap<uint32_t, VkPhysicalDevice> candidates;

		// Rate each candidate physical device.
		for (const auto& currentDevice : devices)
		{
			uint32_t rating = RatePhysicalDevice(currentDevice);
			candidates.insert(std::make_pair(rating, currentDevice));
		}

		// Ensure the highest-rated physical device is above zero.
		if (candidates.rbegin()->first == 0)
		{
			throw std::runtime_error("Failed to find a suitable GPU!");
		}

		// Select the highest-rated physical device.
		physicalDevice = candidates.rbegin()->second;

		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

		std::cout << "Physical device selected: " << deviceProperties.deviceName << " (rating: " << candidates.rbegin()->first << ")\n";
		
		QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(physicalDevice);

		std::cout << "Graphics queue family: " << queueFamilyIndices.graphicsFamily.value() << '\n';
		std::cout << "Present queue family: " << queueFamilyIndices.presentFamily.value() << '\n';
	}

	void VulkanContext::CreateLogicalDevice()
	{
		QueueFamilyIndices indices = FindQueueFamilies(physicalDevice);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies =
		{
			indices.graphicsFamily.value(),
			indices.presentFamily.value()
		};

		float queuePriority = 1.0f;

		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.pNext = nullptr;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures features{};
		features.samplerAnisotropy = true;

		VkPhysicalDeviceVulkan12Features features2{};
		features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
		features2.pNext = nullptr;
		features2.descriptorIndexing = true;
		features2.descriptorBindingVariableDescriptorCount = true;
		features2.runtimeDescriptorArray = true;
		features2.bufferDeviceAddress = true;

		VkPhysicalDeviceVulkan13Features features3{};
		features3.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
		features3.pNext = &features2;
		features3.synchronization2 = true;
		features3.dynamicRendering = true;

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pNext = &features3;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.pEnabledFeatures = &features;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

#if NDEBUG
		createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledLayerNames = nullptr;
#else
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
#endif

		VkResult result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &device);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create logical device!");
		}

		vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
		vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
	}

	void VulkanContext::CreateSwapchain()
	{
		swapchainManager = std::make_unique<SwapchainManager>(window, physicalDevice, surface, device);
	}

	void VulkanContext::CreatePipeline()
	{
		pipeline = std::make_unique<VulkanPipeline>(device);
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

		for (const char* layerName : layers)
		{
			bool layerFound = false;

			for (const VkLayerProperties& layerProperties : availableLayers)
			{
				if (std::strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			if (!layerFound)
			{
				return false;
			}
		}

		return true;
	}

	std::vector<const char*> VulkanContext::GetRequiredExtensions(const Window& window, bool enableValidationLayers)
	{
		std::vector<const char*> extensions = window.GetRequiredExtensions();

		if (enableValidationLayers)
		{
			extensions.push_back("VK_EXT_debug_utils");
		}

		return extensions;
	}

	uint32_t VulkanContext::RatePhysicalDevice(VkPhysicalDevice physDevice) const
	{
		uint32_t score = 0;

		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(physDevice, &deviceProperties);

		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(physDevice, &deviceFeatures);

		std::cout << "Physical device candidate:\n";
		std::cout << '\t' << deviceProperties.deviceName << '\n';

		if (!deviceFeatures.geometryShader)
		{
			return 0;
		}

		const QueueFamilyIndices indices = FindQueueFamilies(physDevice);
		if (!indices.IsComplete())
		{
			return 0;
		}

		const bool extensionsSupported = CheckDeviceExtensionSupport(physDevice);
		if (!extensionsSupported)
		{
			return 0;
		}

		const SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(physDevice);
		const bool swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		if (!swapChainAdequate)
		{
			return 0;
		}

		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			score += 1000;
		}
		else if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
		{
			score += 100;
		}

		score += deviceProperties.limits.maxImageDimension2D;

		std::cout << "\tScore: " << score << '\n';

		return score;
	}

	bool VulkanContext::CheckDeviceExtensionSupport(VkPhysicalDevice physDevice) const
	{
		// Get the number of available device extensions.
		uint32_t extensionCount = 0;
		vkEnumerateDeviceExtensionProperties(physDevice, nullptr, &extensionCount, nullptr);

		// Get the available device extensions.
		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(physDevice, nullptr, &extensionCount, availableExtensions.data());

		// Create a set of required extensions.
		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		// Check off the required extensions that are available.
		for (const auto& extension : availableExtensions)
		{
			requiredExtensions.erase(extension.extensionName);
		}

		// If the set is empty, all required extensions are supported.
		return requiredExtensions.empty();
	}

	QueueFamilyIndices VulkanContext::FindQueueFamilies(VkPhysicalDevice physDevice) const
	{
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physDevice, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physDevice, &queueFamilyCount, queueFamilies.data());

		for (size_t i = 0; i < queueFamilies.size(); i++)
		{
			const VkQueueFamilyProperties& queueFamily = queueFamilies[i];

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(physDevice, i, surface, &presentSupport);
			if (presentSupport)
			{
				indices.presentFamily = i;
			}

			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.graphicsFamily = i;
			}

			// Early out.
			if (indices.IsComplete())
			{
				break;
			}
		}

		return indices;
	}

	SwapChainSupportDetails VulkanContext::QuerySwapChainSupport(VkPhysicalDevice physDevice) const
	{
		SwapChainSupportDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physDevice, surface, &details.capabilities);

		uint32_t formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physDevice, surface, &formatCount, nullptr);

		if (formatCount != 0)
		{
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(physDevice, surface, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(physDevice, surface, &presentModeCount, nullptr);

		if (presentModeCount != 0)
		{
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(physDevice, surface, &presentModeCount, details.presentModes.data());
		}

		return details;
	}
}