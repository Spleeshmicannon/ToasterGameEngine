#include "renderer.h"

#ifdef TLINUX
#define VK_USE_PLATFORM_XCB_KHR
#endif
#include <vulkan/vulkan.h>
#ifdef TWIN32
#include <vulkan/vulkan_win32.h>
#endif

#include "../memory.h"
#include "../logger.h"
#include "../containers.h"
#include "../types/string.h"
#include "../platform/platformState.h"

namespace toast
{
	struct vulkanContext
	{
		VkInstance instance;
		VkAllocationCallbacks* allocator;
		VkSurfaceKHR surface;
	};

	struct vulkanDevice
	{
		VkPhysicalDevice physicalDevice;
		VkDevice logicalDevice;
	};

	struct vulkanDeviceRequirements
	{
		b8 graphics, present, compute, transfer, samplerAnisotropy, discreteGpu;
	};

	struct vulkanPhysDeviceQueFamInfo
	{
		u32 graphicsIndex, presentIndex, computeIndex, transferIndex;
	};

	struct vulknSwapchainSupportInfo
	{
		VkSurfaceCapabilitiesKHR capabilites;
		u32 formatCount;
		VkSurfaceFormatKHR* formats;
		u32 presentModeCount;
		VkPresentModeKHR* presetnModes;
	};

	TINLINE void checkVulkan(VkResult sym)
	{
#ifndef TOAST_RELEASE
		if (sym != VK_SUCCESS) Logger::staticLog<logLevel::TINFO>("vulkan failed");
#endif
	}

#ifdef TWIN32
	b8 Renderer::createSurface()
	{
		VkWin32SurfaceCreateInfoKHR createInfo = { VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };
		createInfo.hinstance = platState->hinst;
		createInfo.hwnd = platState->hwnd;

		checkVulkan(vkCreateWin32SurfaceKHR(context->instance, &createInfo,
			context->allocator, &context->surface));

		return true;
	}
#elif defined(TLINUX)
	b8 Renderer::createSurface()
	{
		VkXcbSurfaceCreateInfoKHR createInfo = { VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR };
		createInfo.connection = platState->connection;
		createInfo.window = platState->window;

		checkVulkan(vkCreateXcbSurfaceKHR(context->instance, &createInfo,
			context->allocator, &context->surface));

		return true;
	}
#endif
	TINLINE b8 vulkanDeviceQuerySwapchainSupport(VkPhysicalDevice physcialDevice,
		VkSurfaceKHR surface, vulknSwapchainSupportInfo * outSuppInfo)
	{

	}


	TINLINE b8 physicalDeviceMeetsRequirements(
		VkPhysicalDevice device,
		VkSurfaceKHR surface,
		VkPhysicalDeviceProperties props,
		VkPhysicalDeviceFeatures feat,
		vulkanDeviceRequirements dreq, 
		vulknSwapchainSupportInfo *outSInfo,
		vulkanPhysDeviceQueFamInfo *outQue)
	{
		outQue->graphicsIndex = -1;
		outQue->presentIndex = -1;
		outQue->computeIndex = -1;
		outQue->transferIndex = -1;

		if (dreq.discreteGpu) 
		{
			if (props.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			{
				Logger::staticLog<logLevel::TINFO>("Device is not discrete GPU");
				return false;
			}
		}

		u32 queFamCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queFamCount, 0);
		VkQueueFamilyProperties * queFams = allocate<VkQueueFamilyProperties>(queFamCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queFamCount, queFams);

		Logger::staticLog<logLevel::TDEBUG>(
			"| Graphics | Present | Compute | Transfer |            Name            |");
		u8 minTransScore = 255;
		for (u32 i = 0; i < queFamCount; ++i)
		{
			u8 currentTransScore = 0;

			// graphics que?
			if (queFams[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				outQue->graphicsIndex = i;
				++currentTransScore;
			}

			// compute que?
			if (queFams[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
			{
				outQue->computeIndex = i;
				++currentTransScore;
			}

			// Transfer que?
			if ((queFams[i].queueFlags & VK_QUEUE_TRANSFER_BIT) && 
				currentTransScore <= minTransScore)
			{
				minTransScore = currentTransScore;
				outQue->transferIndex = i;
			}

			// Present Que?
			VkBool32 supportsPresent = VK_FALSE;
			checkVulkan(vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, 
				&supportsPresent));
			if (supportsPresent)
			{
				outQue->presentIndex = i;
			}
		}

		deallocate<VkQueueFamilyProperties>(queFams);

		Logger::staticLog<logLevel::TDEBUG>(
			"|    "			+ std::to_string(outQue->graphicsIndex) +
			"     |    "	+ std::to_string(outQue->presentIndex) +
			"    |    "		+ std::to_string(outQue->computeIndex) +
			"    |     "	+ std::to_string(outQue->transferIndex) +
			"    | "		+ props.deviceName +
			" |");

		if ((!dreq.graphics || (dreq.graphics && outQue->graphicsIndex != -1)) &&
			(!dreq.present || (dreq.present && outQue->presentIndex != -1)) &&
			(!dreq.compute || (dreq.compute && outQue->computeIndex != -1)) &&
			(!dreq.transfer || (dreq.transfer && outQue->transferIndex != -1)))
		{
			Logger::staticLog<logLevel::TINFO>("Device meets queue requirements");
			return true;
		}

		return false;
	}

	b8 Renderer::setupDevice()
	{
		// query what gpu's are on the system
		u32 physicalDeviceCount = 0;
		checkVulkan(vkEnumeratePhysicalDevices(context->instance, &physicalDeviceCount, 0));
		
		// if no devices then renderer won't work
		if (physicalDeviceCount == 0)
		{
			Logger::staticLog<logLevel::TFATAL>("No devices that suppport vulkan");
			return false;
		}

		// reading in device data for each device
		VkPhysicalDevice* physicalDevices = allocate<VkPhysicalDevice>(physicalDeviceCount);
		checkVulkan(vkEnumeratePhysicalDevices(context->instance, &physicalDeviceCount, 
			physicalDevices));

		// processing device data
		for (u32 i = 0; i < physicalDeviceCount; ++i)
		{
			VkPhysicalDeviceProperties props;
			vkGetPhysicalDeviceProperties(physicalDevices[i], &props);

			VkPhysicalDeviceFeatures feat;
			vkGetPhysicalDeviceFeatures(physicalDevices[i], &feat);

			VkPhysicalDeviceMemoryProperties memProps;
			vkGetPhysicalDeviceMemoryProperties(physicalDevices[i], &memProps);

			// a clear way of setting device requirements
			vulkanDeviceRequirements req = {};
			req.graphics = true;
			req.present = true;
			req.transfer = true;
			req.compute = true;
			req.samplerAnisotropy = true;
			req.discreteGpu = true;

			// device related extensions
			constexpr u8 extCount = 1;
			const cv* extNames[extCount] =
			{
				VK_KHR_SWAPCHAIN_EXTENSION_NAME
			};

			vulknSwapchainSupportInfo swapSupInfo;
			vulkanPhysDeviceQueFamInfo queFamInfo;

			physicalDeviceMeetsRequirements(physicalDevices[i], context->surface, props, feat, req,
				&swapSupInfo, &queFamInfo);
		}

		return true;
	}

	Renderer::Renderer()
	{
		
		platState = nullptr;
		context = allocate<vulkanContext>(1);
		device = allocate<vulkanDevice>(1);
	}

	Renderer::~Renderer()
	{}

	b8 Renderer::initialise(const str<cv> appName, platformState* _platState)
	{
		platState = _platState;
		context->allocator = 0;

		// setup vulkan instance
		// info about the application using vulkan and how we'll use vulkan
		VkApplicationInfo appInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
		appInfo.apiVersion = VK_API_VERSION_1_2;
		appInfo.pApplicationName = appName.c_str();
		appInfo.pEngineName = "Toaster Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);

		// creation boilerplate
		VkInstanceCreateInfo createInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
		createInfo.pApplicationInfo = &appInfo;

		// defining the extensions being used
		constexpr u8 extCount = 2;
		const cv* extNames[extCount] =
		{
			VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef TWIN32
			"VK_KHR_win32_surface" 
#elif defined(TLINUX)
			"VK_KHR_xcb_surface"
#endif
		};

		createInfo.enabledExtensionCount = extCount;
		createInfo.ppEnabledExtensionNames = extNames;
		createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledLayerNames = 0;

		// creating the vulkan instance and checking for errors
		checkVulkan(vkCreateInstance(&createInfo, context->allocator, &context->instance));

		// creating surface
		if (!createSurface()) return false;

		// setting up device related stuff (i.e. selecting a GPU device)
		if (!setupDevice()) return false;

		return true;
	}

	b8 Renderer::shutdown()
	{
		vkDestroyInstance(context->instance, context->allocator);
		tdelete<vulkanContext>(context);
		return true;
	}

	b8 Renderer::beginFrame(f32 deltaTime)
	{
		return true;
	}

	b8 Renderer::endFrame(f32 deltaTime)
	{
		return true;
	}

	void resized(u16 width, u16 height)
	{

	}

	b8 Renderer::drawFrame(renderPacket* packet)
	{
		if (beginFrame(packet->deltaTime)) 
		{
			if (!endFrame(packet->deltaTime))
			{
				Logger::staticLog<logLevel::TERROR>("end frame failed");
				return false;
			}
		}

		return true;
	}
}