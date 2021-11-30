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

		constexpr u8 extCount = 2;
		const char* extNames[extCount] =
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