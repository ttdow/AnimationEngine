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
		SetupDebugMessenger();
		CreateSurface();
		PickPhysicalDevice();
		CreateLogicalDevice();
		volkLoadDevice(device);
		CreateAllocator();
		CreateSwapchain();
		CreatePipeline();
		CreateCommandPool();
		CreateCommandBuffers();
		CreateSyncObjects();
	}

	VulkanContext::~VulkanContext()
	{
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			if (renderFinishedSemaphores[i] != VK_NULL_HANDLE)
			{
				vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
			}

			if (imageAvailableSemaphores[i] != VK_NULL_HANDLE)
			{
				vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
			}

			if (inFlightFences[i] != VK_NULL_HANDLE)
			{
				vkDestroyFence(device, inFlightFences[i], nullptr);
			}
		}

		if (commandPool != VK_NULL_HANDLE)
		{
			vkDestroyCommandPool(device, commandPool, nullptr);
		}

		if (pipeline)
		{
			pipeline.reset();
		}

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

		if (debugMessenger != VK_NULL_HANDLE)
		{
			vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		}

		if (instance != VK_NULL_HANDLE)
		{
			vkDestroyInstance(instance, nullptr);
		}
	}

	void VulkanContext::Draw()
	{
		DrawFrame();
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

		// Verify that the required validation layers are available.
		enableValidationLayers = CheckValidationLayerSupport(availableValidationLayers, requiredValidationLayers, true);

		// Determine the required instance extensions and verify they are available.
		const bool haveRequiredExtensions = CheckInstanceExtensionSupport(availableInstanceExtensions, true);
		if (!haveRequiredExtensions)
		{
			throw std::runtime_error("The host device does not support the required windowing extensions!");
		}

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredInstanceExtensions.size());
		createInfo.ppEnabledExtensionNames = requiredInstanceExtensions.data();

		if (enableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(requiredValidationLayers.size());
			createInfo.ppEnabledLayerNames = requiredValidationLayers.data();
		}
		else
		{
			createInfo.enabledLayerCount = 0;
			createInfo.ppEnabledLayerNames = nullptr;
		}

		VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create the Vulkan instance! " + result);
		}
	}

	void VulkanContext::SetupDebugMessenger()
	{
		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.pNext = nullptr;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = DebugCallback;
		createInfo.pUserData = nullptr;

		VkResult result = CreateDebugUtilsMessenger(instance, &createInfo, nullptr, &debugMessenger);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create Vulkan debug utility messenger!");
		}
	}

	void VulkanContext::CreateSurface()
	{
		VkResult result = glfwCreateWindowSurface(instance, window.GetGLFWwindow(), nullptr, &surface);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create a window surface using GLFW!");
		}
	}

	void VulkanContext::PickPhysicalDevice()
	{
		// Get the number of physical devices available.
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		// If there are no supported physical devices, abort. 
		if (deviceCount < 1)
		{
			throw std::runtime_error("Failed to find a physical device with Vulkan support!");
		}

		// Save a reference to each available physical device.
		std::vector<VkPhysicalDevice> availablePhysicalDevices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, availablePhysicalDevices.data());
		
		// Initialize memory for our list of available physical devices and their details.
		physicalDevices.resize(deviceCount);

		// Rate each candidate physical device to select the best.
		uint32_t highestScore = 0;
		std::cout << "Physical device candidates:\n";
		for (size_t i = 0; i < availablePhysicalDevices.size(); i++)
		{
			// Populate the physical device details.
			PhysicalDevice& currentDevice = physicalDevices[i];
			currentDevice.device = availablePhysicalDevices[i];
			vkGetPhysicalDeviceProperties(currentDevice.device, &currentDevice.properties);
			vkGetPhysicalDeviceFeatures(currentDevice.device, &currentDevice.features);

			// Rate this device.
			currentDevice.rating = RatePhysicalDevice(currentDevice);
			if (currentDevice.rating > highestScore)
			{
				selectedPhysicalDeviceIndex = i;
				highestScore = currentDevice.rating;
			}
		}

		if (highestScore == 0)
		{
			throw std::runtime_error("Failed to find a suitable physical device (GPU)!");
		}

		physicalDevice = physicalDevices[selectedPhysicalDeviceIndex];
		const PhysicalDevice& physDevice = physicalDevice;

		std::cout << '\n';
		std::cout << "Physical device selected:\n";
		std::cout << "\t[" << selectedPhysicalDeviceIndex << "] " << physDevice.properties.deviceName << '\n';
		std::cout << "\tRating: " << physDevice.rating << '\n';
		std::cout << "\tGraphics queue family : " << physDevice.queueFamilyIndices.graphicsFamily.value() << '\n';
		std::cout << "\tPresent queue family: " << physDevice.queueFamilyIndices.presentFamily.value() << "\n\n";
	}

	void VulkanContext::CreateLogicalDevice()
	{
		const QueueFamilyIndices& indices = physicalDevices[selectedPhysicalDeviceIndex].queueFamilyIndices;

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies =
		{
			indices.graphicsFamily.value(),
			indices.presentFamily.value()
		};

		float queuePriority = 1.0f;

		// Create a device queue for each queue family.
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

		// Enable our required features.
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

		if (enableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(requiredValidationLayers.size());
			createInfo.ppEnabledLayerNames = requiredValidationLayers.data();
		}
		else
		{
			createInfo.enabledLayerCount = 0;
			createInfo.ppEnabledLayerNames = nullptr;
		}

		VkResult result = vkCreateDevice(physicalDevice.device, &createInfo, nullptr, &device);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create the logical device!");
		}

		vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
		vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
	}

	void VulkanContext::CreateAllocator()
	{
		VmaVulkanFunctions vmaFuncs{};
		vmaFuncs.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
		vmaFuncs.vkGetDeviceProcAddr = vkGetDeviceProcAddr;

		VmaAllocatorCreateInfo info{};
		info.physicalDevice = physicalDevice.device;
		info.device = device;
		info.instance = instance;
		info.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
		info.pVulkanFunctions = &vmaFuncs;

		VkResult result = vmaCreateAllocator(&info, &allocator);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create Vulkan Memory Allocator (VMA)!");
		}
	}

	void VulkanContext::CreateSwapchain()
	{
		swapchainManager = std::make_unique<SwapchainManager>(window, physicalDevice, surface, device);
	}

	void VulkanContext::CreatePipeline()
	{
		pipeline = std::make_unique<VulkanPipeline>(device);

		std::vector<char> vertShaderCode = ReadFile("shaders/vert.spv");
		std::vector<char> fragShaderCode = ReadFile("shaders/frag.spv");

		VkShaderModule vertShaderModule = CreateShaderModule(vertShaderCode);
		VkShaderModule fragShaderModule = CreateShaderModule(fragShaderCode);

		pipeline->SetLayout();
		pipeline->SetShaders(vertShaderModule, fragShaderModule);
		pipeline->SetInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
		pipeline->SetPolygonMode(VK_POLYGON_MODE_FILL);
		pipeline->SetCullMode(VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_CLOCKWISE);
		pipeline->SetMultisamplingNone();
		pipeline->DisableBlending();
		pipeline->DisableDepthTest();
		pipeline->SetColorAttachmentFormat(VK_FORMAT_R8G8B8A8_SRGB);
		pipeline->Build();

		vkDestroyShaderModule(device, vertShaderModule, nullptr);
		vkDestroyShaderModule(device, fragShaderModule, nullptr);
	}

	void VulkanContext::CreateCommandPool()
	{
		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.pNext = nullptr;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = physicalDevice.queueFamilyIndices.graphicsFamily.value();

		VkResult result = vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create command pool!");
		}
	}

	void VulkanContext::CreateCommandBuffers()
	{
		commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.pNext = nullptr;
		allocInfo.commandPool = commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		VkResult result = vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data());
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate command buffers!");
		}
	}

	void VulkanContext::CreateSyncObjects()
	{
		imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		semaphoreInfo.pNext = nullptr;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.pNext = nullptr;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
				vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
				vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create synchronization objects for a frame!");
			}
		}
	}

	void VulkanContext::RecordCommandBuffer(VkCommandBuffer& cmd, uint32_t imageIndex)
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.pNext = nullptr;
		beginInfo.flags = 0; //VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
		beginInfo.pInheritanceInfo = nullptr;

		VkResult result = vkBeginCommandBuffer(cmd, &beginInfo);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to begin recording command buffer!");
		}

		TransitionImage(cmd, swapchainManager->GetImage(imageIndex), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);

		VkRenderingAttachmentInfo colorAttachment = {};
		colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		colorAttachment.pNext = nullptr;
		colorAttachment.imageView = swapchainManager->GetImageView(imageIndex);
		colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		colorAttachment.clearValue.color = { {1.0f, 0.0f, 1.0f, 1.0f } };
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		VkRenderingInfo renderInfo = {};
		renderInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
		renderInfo.pNext = nullptr;
		renderInfo.renderArea = VkRect2D{ VkOffset2D { 0, 0 }, swapchainManager->GetExtent()};
		renderInfo.layerCount = 1;
		renderInfo.colorAttachmentCount = 1;
		renderInfo.pColorAttachments = &colorAttachment;
		renderInfo.pDepthAttachment = nullptr; // &depthAttachment;
		renderInfo.pStencilAttachment = nullptr;

		vkCmdBeginRendering(cmd, &renderInfo);

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(swapchainManager->GetExtent().width);
		viewport.height = static_cast<float>(swapchainManager->GetExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		vkCmdSetViewport(cmd, 0, 1, &viewport);

		VkRect2D scissor = {};
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		scissor.extent.width = swapchainManager->GetExtent().width;
		scissor.extent.height = swapchainManager->GetExtent().height;

		vkCmdSetScissor(cmd, 0, 1, &scissor);

		vkCmdSetLineWidth(cmd, 1.0f);

		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipeline());

		vkCmdDraw(cmd, 3, 1, 0, 0);

		vkCmdEndRendering(cmd);

		TransitionImage(cmd, swapchainManager->GetImage(imageIndex), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_ASPECT_COLOR_BIT);

		result = vkEndCommandBuffer(cmd);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to end recording frame command buffer!");
		}
	}

	void VulkanContext::DrawFrame()
	{
		VkResult result = vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to wait for in flight fence!");
		}

		uint32_t imageIndex = 0;
		result = vkAcquireNextImageKHR(device, swapchainManager->GetSwapchain(), UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			physicalDevice.swapChainSupportDetails = QuerySwapChainSupport(physicalDevice);
			vkDestroySurfaceKHR(instance, surface, nullptr);
			CreateSurface();
			swapchainManager->RecreateSwapchain(surface, physicalDevice);
			return;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("Failed to acquire next swapchain image!");
		}

		result = vkResetFences(device, 1, &inFlightFences[currentFrame]);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to reset in flight fence!");
		}

		result = vkResetCommandBuffer(commandBuffers[currentFrame], 0);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to reset command buffer!");
		}

		RecordCommandBuffer(commandBuffers[currentFrame], imageIndex);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = nullptr;

		VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame]};
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

		VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		result = vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to submit draw command buffer!");
		}

		VkSwapchainKHR swapChains[] = { swapchainManager->GetSwapchain() };

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = nullptr;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr;

		result = vkQueuePresentKHR(presentQueue, &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.framebufferResized)
		{
			window.framebufferResized = false;
			physicalDevice.swapChainSupportDetails = QuerySwapChainSupport(physicalDevice);
			vkDestroySurfaceKHR(instance, surface, nullptr);
			CreateSurface();
			swapchainManager->RecreateSwapchain(surface, physicalDevice);
		}
		else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to submit to present queue!");
		}

		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	void VulkanContext::TransitionImage(VkCommandBuffer& cmd, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout, VkImageAspectFlags aspectFlags)
	{
		VkImageMemoryBarrier2 imageBarrier = {};
		imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
		imageBarrier.pNext = nullptr;
		imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
		imageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
		imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
		imageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;
		imageBarrier.oldLayout = currentLayout;
		imageBarrier.newLayout = newLayout;
		imageBarrier.subresourceRange.aspectMask = aspectFlags;
		imageBarrier.subresourceRange.baseMipLevel = 0;
		imageBarrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
		imageBarrier.subresourceRange.baseArrayLayer = 0;
		imageBarrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
		imageBarrier.image = image;

		VkDependencyInfo depInfo = {};
		depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
		depInfo.pNext = nullptr;
		depInfo.imageMemoryBarrierCount = 1;
		depInfo.pImageMemoryBarriers = &imageBarrier;

		vkCmdPipelineBarrier2(cmd, &depInfo);
	}

	bool VulkanContext::CheckValidationLayerSupport(std::vector<VkLayerProperties>& availableLayers, const std::vector<const char*>& requiredLayers, bool print)
	{
#ifdef NDEBUG
		return false;
#endif

		// Query the host device's available validation layers.
		uint32_t layerCount = 0;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		availableLayers.resize(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		// Print the available validation layers to the console if requested.
		if (print)
		{
			std::cout << "Available validation layers:\n";
			for (const VkLayerProperties& layerProperties : availableLayers)
			{
				std::cout << '\t' << layerProperties.layerName << '\n';
			}

			std::cout << '\n';
		}

		// Iterate through each required validation layer to verify it is present in the 
		// available validation layers.
		for (const char* layerName : requiredLayers)
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

			// If we didn't find a required validation layer, return false.
			if (!layerFound)
			{
				return false;
			}
		}

		return true;
	}

	bool VulkanContext::CheckInstanceExtensionSupport(std::vector<VkExtensionProperties>& availableExtensions, bool print)
	{
		// Query the host device's available instance extensions.
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		availableExtensions.resize(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());

		if (print)
		{
			std::cout << "Available instance extensions:\n";

			for (const VkExtensionProperties& extension : availableExtensions)
			{
				std::cout << '\t' << extension.extensionName << '\n';
			}

			std::cout << '\n';
		}

		// Query the Window abstraction for the required windowing extensions.
		requiredInstanceExtensions = window.GetRequiredExtensions();

		// If we have enabled validation layers, query the required relevant extensions.
		if (enableValidationLayers)
		{
			requiredInstanceExtensions.push_back("VK_EXT_debug_utils");
		}

		for (const VkExtensionProperties& extensionProperties : availableInstanceExtensions)
		{
			if (std::strcmp("VK_EXT_swapchain_colorspace", extensionProperties.extensionName) == 0)
			{
				requiredInstanceExtensions.push_back("VK_EXT_swapchain_colorspace");
			}
		}

		if (print)
		{
			std::cout << "Required instance extensions:\n";

			for (const char* extension : requiredInstanceExtensions)
			{
				std::cout << '\t' << extension << '\n';
			}

			std::cout << '\n';
		}

		// Iterate through each required instance extension to verify it is present in the 
		// available instance extensions.
		for (const char* extensionName : requiredInstanceExtensions)
		{
			bool extensionFound = false;

			for (const VkExtensionProperties& extensionProperties : availableInstanceExtensions)
			{
				if (std::strcmp(extensionName, extensionProperties.extensionName) == 0)
				{
					extensionFound = true;
					break;
				}
			}

			// If we didn't find a required instance extension, return false.
			if (!extensionFound)
			{
				return false;
			}
		}

		return true;
	}

	uint32_t VulkanContext::RatePhysicalDevice(PhysicalDevice& physDevice)
	{
		uint32_t score = 0;

		std::cout << '\t' << physDevice.properties.deviceName;

		if (physDevice.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			score += 1000;
		}
		else if (physDevice.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
		{
			score += 100;
		}

		score += physDevice.properties.limits.maxImageDimension2D;

		physDevice.queueFamilyIndices = FindQueueFamilies(physDevice);
		if (!physDevice.queueFamilyIndices.IsComplete())
		{
			score = 0;
		}

		const bool extensionsSupported = CheckDeviceExtensionSupport(physDevice);
		if (!extensionsSupported)
		{
			score = 0;
		}

		physDevice.swapChainSupportDetails = QuerySwapChainSupport(physDevice);
		const bool swapChainAdequate = !physDevice.swapChainSupportDetails.formats.empty() &&
			!physDevice.swapChainSupportDetails.presentModes.empty();
		if (!swapChainAdequate)
		{
			score = 0;
		}

		if (physDevice.properties.apiVersion < VK_API_VERSION_1_3)
		{
			score = 0;
		}

		std::cout << " - Score: " << score << '\n';

		return score;
	}

	bool VulkanContext::CheckDeviceExtensionSupport(const PhysicalDevice& physDevice) const
	{
		// Get the number of available device extensions.
		uint32_t extensionCount = 0;
		vkEnumerateDeviceExtensionProperties(physDevice.device, nullptr, &extensionCount, nullptr);

		// Get the available device extensions.
		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(physDevice.device, nullptr, &extensionCount, availableExtensions.data());

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

	QueueFamilyIndices VulkanContext::FindQueueFamilies(const PhysicalDevice& physDevice) const
	{
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physDevice.device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physDevice.device, &queueFamilyCount, queueFamilies.data());

		for (size_t i = 0; i < queueFamilies.size(); i++)
		{
			const VkQueueFamilyProperties& queueFamily = queueFamilies[i];

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(physDevice.device, i, surface, &presentSupport);
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

	SwapChainSupportDetails VulkanContext::QuerySwapChainSupport(PhysicalDevice& physDevice) const
	{
		SwapChainSupportDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physDevice.device, surface, &details.capabilities);

		uint32_t formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physDevice.device, surface, &formatCount, nullptr);

		if (formatCount != 0)
		{
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(physDevice.device, surface, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(physDevice.device, surface, &presentModeCount, nullptr);

		if (presentModeCount != 0)
		{
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(physDevice.device, surface, &presentModeCount, details.presentModes.data());
		}

		return details;
	}

	VkResult VulkanContext::CreateDebugUtilsMessenger(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		auto f = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (f != nullptr)
		{
			return f(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else
		{
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	void VulkanContext::DestroyDebugUtilsMessenger(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
	{
		auto f = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (f != nullptr)
		{
			f(instance, debugMessenger, pAllocator);
		}
	}

	std::vector<char> VulkanContext::ReadFile(const std::string& filename)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			throw std::runtime_error("Failed to open file!");
		}

		size_t filesize = static_cast<size_t>(file.tellg());
		std::vector<char> buffer(filesize);

		file.seekg(0);
		file.read(buffer.data(), filesize);

		file.close();

		return buffer;
	}

	VkShaderModule VulkanContext::CreateShaderModule(const std::vector<char>& code) const
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule shaderModule;
		VkResult result = vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create shader module!");
		}

		return shaderModule;
	}
}