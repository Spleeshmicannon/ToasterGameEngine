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
#include "../platform/platformState.h"

#define TVK_CLAMP(value, min, max) (value <= min) ? min : (value >= max) ? max : value;

namespace toast
{
	struct vulknSwapchainSupportInfo
	{
		VkSurfaceCapabilitiesKHR capabilites;
		u32 formatCount;
		VkSurfaceFormatKHR* formats;
		u32 presentModeCount;
		VkPresentModeKHR* presentModes;
	};

	struct vulkanContext
	{
		VkInstance instance;
		VkAllocationCallbacks* allocator;
		VkSurfaceKHR surface;
		u16 frameBufferWidth;
		u16 frameBufferHeight;
	};

	struct vulkanDevice
	{
		// main structs
		VkPhysicalDevice physicalDevice;
		VkDevice logicalDevice;

		// logical device stuff
		VkQueue graphicsQue;
		VkQueue presentQue;
		VkQueue transferQue;

		// physical device stuff
		u32 graphicsIndex, presentIndex, computeIndex, transferIndex;
		VkPhysicalDeviceProperties props;
		VkPhysicalDeviceFeatures feat;
		VkPhysicalDeviceMemoryProperties mem;
		vulknSwapchainSupportInfo swapSupp;
	};

	struct vulkanDeviceRequirements
	{
		b8 graphics, present, compute, transfer, samplerAnisotropy, discreteGpu;
	};

	struct vulkanPhysDeviceQueFamInfo
	{
		u32 graphicsIndex, presentIndex, computeIndex, transferIndex;
	};

	TINLINE b8 vulkanDeviceQuerySwapchainSupport(VkPhysicalDevice device,
		VkSurfaceKHR surface, vulknSwapchainSupportInfo* outSuppInfo);

	TINLINE void checkVulkan(VkResult sym)
	{
#ifndef TOAST_RELEASE
		if (sym != VK_SUCCESS) Logger::staticLog<logLevel::TINFO>("vulkan failed");
#endif
	}

	struct swapchainState
	{
		VkSwapchainKHR handle;
		u8 maxFramesInFlight;
		u32 imageCount;
		VkImage* images;
		VkImageView* views;

	};

	class Swapchain
	{
	private:
		swapchainState * state;
		VkSurfaceFormatKHR imageFormat;

		TINLINE VkSurfaceFormatKHR _getFormat(VkFormat format, VkColorSpaceKHR colorSpace, 
			vulkanDevice* device)
		{
			for (u32 i = 0; i < device->swapSupp.formatCount; ++i)
			{
				if (device->swapSupp.formats[i].format == VK_FORMAT_B8G8R8A8_UNORM &&
					device->swapSupp.formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				{
					return device->swapSupp.formats[i];
				}
			}

			return device->swapSupp.formats[0];
		}

		TINLINE void _create(vulkanContext* context, vulkanDevice* device, 
			int &width, int &height)
		{
			VkExtent2D swapExtent = { width, height };

			if (imageFormat.format == VK_FORMAT_UNDEFINED)
			{
				imageFormat = _getFormat(VK_FORMAT_B8G8R8A8_UNORM,
					VK_COLOR_SPACE_SRGB_NONLINEAR_KHR, device);
			}

			state = allocate<swapchainState>();
			state->maxFramesInFlight = 2;

			VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
			for (u32 i = 0; i < device->swapSupp.presentModeCount; ++i)
			{
				if (device->swapSupp.presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
				{
					presentMode = device->swapSupp.presentModes[i];
					break;
				}
			}

			vulkanDeviceQuerySwapchainSupport(
				device->physicalDevice,
				context->surface,
				&device->swapSupp
			);

			if (device->swapSupp.capabilites.currentExtent.width != UINT32_MAX)
			{
				swapExtent = device->swapSupp.capabilites.currentExtent;
			}

			VkExtent2D min = device->swapSupp.capabilites.minImageExtent;
			VkExtent2D max = device->swapSupp.capabilites.maxImageExtent;

			swapExtent.width = TVK_CLAMP(swapExtent.width, min.width, max.width);
			swapExtent.height = TVK_CLAMP(swapExtent.height, min.height, max.height);
		}

		TINLINE void _destroy()
		{
			zeroMem(state, sizeof(state));
			deallocate<swapchainState>(state);
		}

	public:
		Swapchain()
		{
			state = nullptr;
			imageFormat.format = VK_FORMAT_UNDEFINED;
		}

		void create(vulkanContext* context, vulkanDevice* device, i32 width, i32 height)
		{
			_create(context, device, width, height);
		}

		void recreate(vulkanContext* context, vulkanDevice* device, i32 width, i32 height)
		{
			_destroy();
			_create(context, device, width, height);
		}

		b8 acquireNextImageIndex(vulkanContext* context, vulkanDevice* device, u64 timeoutNS,
			VkSemaphore imageAvaiSemaphore, VkFence fence, u32*outImageIndex)
		{
			VkResult result = vkAcquireNextImageKHR(
				device->logicalDevice,
				state->handle,
				timeoutNS,
				imageAvaiSemaphore,
				fence,
				outImageIndex
			);

			if (result == VK_ERROR_OUT_OF_DATE_KHR)
			{
				recreate(context, device, context->frameBufferWidth, 
					context->frameBufferHeight);
				return false;
			}
			else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
			{
				Logger::staticLog<logLevel::TFATAL>("ACQUIRE NEXT IMAGE FAILED");
				return false;
			}

			return true;
		}

		b8 present(vulkanContext* context, vulkanDevice* device, VkQueue graphicsQue, 
			VkQueue presentQue, VkSemaphore renderCompleteSemaphore, u32 presentImageIndex)
		{
			VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
			presentInfo.waitSemaphoreCount = 1;
			presentInfo.pWaitSemaphores = &renderCompleteSemaphore;
			presentInfo.swapchainCount = 1;
			presentInfo.pSwapchains = &state->handle;
			presentInfo.pImageIndices = &presentImageIndex;
			presentInfo.pResults = 0;

			VkResult result = vkQueuePresentKHR(presentQue, &presentInfo);

			if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
			{
				recreate(context, device, context->frameBufferWidth,
					context->frameBufferHeight);
				return false;
			}
			else if (result != VK_SUCCESS)
			{
				Logger::staticLog<logLevel::TFATAL>("failed to present swapchain image");
				return false;
			}


			return true;
		}

		~Swapchain()
		{
			if (state != nullptr) _destroy();
		}
	};

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

	TINLINE b8 vulkanDeviceQuerySwapchainSupport(VkPhysicalDevice device,
		VkSurfaceKHR surface, vulknSwapchainSupportInfo * outSuppInfo)
	{
		checkVulkan(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface,
			&outSuppInfo->capabilites));

		checkVulkan(vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface,
			&outSuppInfo->formatCount, 0));

		if (outSuppInfo->formatCount != 0)
		{
			if (outSuppInfo->formats == nullptr)
			{
				outSuppInfo->formats = allocate<VkSurfaceFormatKHR>();
			}

			checkVulkan(vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface,
				&outSuppInfo->formatCount, outSuppInfo->formats));
		}

		checkVulkan(vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, 
			&outSuppInfo->presentModeCount, 0));

		if (outSuppInfo->presentModeCount != 0)
		{
			if (outSuppInfo->presentModes == nullptr)
			{
				outSuppInfo->presentModes = allocate<VkPresentModeKHR>();
			}

			checkVulkan(vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface,
				&outSuppInfo->presentModeCount, outSuppInfo->presentModes));
		}


		return true;
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
			if (vulkanDeviceQuerySwapchainSupport(device, surface, outSInfo))
			{
				if (outSInfo->formatCount < 0 || outSInfo->presentModeCount < 0)
				{
					if (outSInfo->formats != nullptr)
					{
						deallocate<VkSurfaceFormatKHR>(outSInfo->formats);
					}

					if (outSInfo->presentModes != nullptr)
					{
						deallocate<VkPresentModeKHR>(outSInfo->presentModes);
					}

					Logger::staticLog<logLevel::TINFO>("swapchain support not present, not using device");
					return false;
				}
			}

			// device related extensions
			constexpr u8 extCount = 1;
			const cv* extNames[extCount] =
			{
				VK_KHR_SWAPCHAIN_EXTENSION_NAME
			};

			u32 avaExtCount = 0;
			checkVulkan(vkEnumerateDeviceExtensionProperties(device, NULL, 
				&avaExtCount, NULL));

			if (avaExtCount != 0)
			{
				VkExtensionProperties* availableExts = allocate<VkExtensionProperties>(avaExtCount);
				checkVulkan(vkEnumerateDeviceExtensionProperties(device, NULL,
					&avaExtCount, availableExts));

				for (u32 i = 0; i < extCount; ++i)
				{
					b8 found = false;
					for (u32 j = 0; j < avaExtCount; ++j)
					{
						if (cStrEqual<sizeof(extNames[i])>(extNames[i], availableExts[j].extensionName))
						{
							found = true;
							break;
						}
					}

					if (!found)
					{
						Logger::staticLog<logLevel::TINFO>("Could not find required extensions");
						deallocate<VkExtensionProperties>(availableExts);
						return false;
					}
				}

				deallocate<VkExtensionProperties>(availableExts);

				return true;
			}
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

			vulknSwapchainSupportInfo swapSupInfo = {}; // zeroing out memory
			vulkanPhysDeviceQueFamInfo queFamInfo = {};

			if (physicalDeviceMeetsRequirements(physicalDevices[i], context->surface, props, feat, req,
				&swapSupInfo, &queFamInfo))
			{
				const str<cv> deviceName = props.deviceName;
				Logger::staticLog<logLevel::TINFO>("chosen device: " + deviceName);

				Logger::staticLog<logLevel::TINFO>("GPU driver version: " +
					std::to_string(VK_VERSION_MAJOR(props.driverVersion)) + "." +
					std::to_string(VK_VERSION_MINOR(props.driverVersion)) + "." +
					std::to_string(VK_VERSION_PATCH(props.driverVersion)));

				Logger::staticLog<logLevel::TINFO>("Vulkan API version: " +
					std::to_string(VK_VERSION_MAJOR(props.apiVersion)) + "." +
					std::to_string(VK_VERSION_MINOR(props.apiVersion)) + "." +
					std::to_string(VK_VERSION_PATCH(props.apiVersion)));
				
				for (u32 j = 0; j < memProps.memoryHeapCount; ++j)
				{
					f32 memSizeGb = (((f32)memProps.memoryHeaps[j].size) 
						/ 1024.0f / 1024.0f / 1024.0f);
					if (memProps.memoryHeaps[j].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
					{
						Logger::staticLog<logLevel::TINFO>("Local GPU memory:  " + 
							std::to_string(memSizeGb) + " GiB");
					}
					else
					{
						Logger::staticLog<logLevel::TINFO>("Shared Sys memory: " +
							std::to_string(memSizeGb) + " GiB");
					}
				}
				
				// storing physical device data
				device->physicalDevice = physicalDevices[i];
				device->graphicsIndex = queFamInfo.graphicsIndex;
				device->presentIndex = queFamInfo.presentIndex;
				device->transferIndex = queFamInfo.transferIndex;
				device->props = props;
				device->feat = feat;
				device->mem = memProps;
				device->swapSupp = swapSupInfo;

				break;
			}
		}

		return true;
	}

	b8 Renderer::createLogicalDevice()
	{
		const b8 presentSharesGraphicsQue  = device->graphicsIndex == device->presentIndex;
		const b8 transferSharesGraphicsQue = device->graphicsIndex == device->transferIndex;
		const b8 transferSharesPresentQue  = device->presentIndex  == device->transferIndex;

		u32 indexCount = 1;

		if (!presentSharesGraphicsQue)
		{
			++indexCount;
		}

		if (!transferSharesGraphicsQue)
		{
			++indexCount;
		}

		u32* indices = allocate<u32>(indexCount);
		u8 index = 0;

		indices[index++] = device->graphicsIndex;

		if (!presentSharesGraphicsQue)
		{
			indices[index++] = device->presentIndex;
		}

		if (!transferSharesGraphicsQue)
		{
			indices[index++] = device->transferIndex;
		}

		VkDeviceQueueCreateInfo* queCreateInfos = allocate<VkDeviceQueueCreateInfo>(indexCount);
		for (u32 i = 0; i < indexCount; ++i)
		{
			queCreateInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queCreateInfos[i].queueFamilyIndex = indices[i];
			queCreateInfos[i].queueCount = 1;
			
			if (indices[i] == device->graphicsIndex)
			{
				queCreateInfos[i].queueCount = 2;
			}

			queCreateInfos[i].flags = 0;
			queCreateInfos[i].pNext = 0;

			f32 quePriority = 1.0f;

			queCreateInfos[i].pQueuePriorities = &quePriority;
		}

		VkPhysicalDeviceFeatures deviceFeat = {};
		deviceFeat.samplerAnisotropy = VK_TRUE;
		
		VkDeviceCreateInfo deviceCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
		deviceCreateInfo.queueCreateInfoCount = indexCount;
		deviceCreateInfo.pQueueCreateInfos = queCreateInfos;
		deviceCreateInfo.pEnabledFeatures = &deviceFeat;
		deviceCreateInfo.enabledExtensionCount = 1;
		const char* extensionNames = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
		deviceCreateInfo.ppEnabledExtensionNames = &extensionNames;

		// actually create the logical device
		checkVulkan(vkCreateDevice(
			device->physicalDevice,
			&deviceCreateInfo,
			context->allocator,
			&device->logicalDevice
		));

		Logger::staticLog<logLevel::TDEBUG>("Logical Device created");

		// get queues
		vkGetDeviceQueue(
			device->logicalDevice,
			device->graphicsIndex,
			0,
			&device->graphicsQue
		);

		vkGetDeviceQueue(
			device->logicalDevice,
			device->presentIndex,
			0,
			&device->presentQue
		);

		vkGetDeviceQueue(
			device->logicalDevice,
			device->transferIndex,
			0,
			&device->transferQue
		);

		Logger::staticLog<logLevel::TDEBUG>("Device Queues obtained");

		deallocate<VkDeviceQueueCreateInfo>(queCreateInfos);
		deallocate<u32>(indices);

		return true;
	}

	Renderer::Renderer()
	{
		platState = nullptr;
		context = allocate<vulkanContext>();
		device = allocate<vulkanDevice>();
	}

	Renderer::~Renderer()
	{
		deallocate<vulkanContext>(context);

		if (device->swapSupp.formats != nullptr)
		{
			deallocate<VkSurfaceFormatKHR>(device->swapSupp.formats);
		}

		if (device->swapSupp.presentModes != nullptr)
		{
			deallocate<VkPresentModeKHR>(device->swapSupp.presentModes);
		}

		deallocate<vulkanDevice>(device);
	}

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

		// creating the logical device
		if (!createLogicalDevice()) return false;

		return true;
	}

	b8 Renderer::shutdown()
	{
		device->graphicsQue = 0;
		device->presentQue = 0;
		device->transferQue = 0;

		if (device->logicalDevice != nullptr)
		{
			vkDestroyDevice(device->logicalDevice, context->allocator);
			device->logicalDevice = 0;
		}

		if (context->surface != nullptr)
		{
			vkDestroySurfaceKHR(context->instance, context->surface, context->allocator);
			context->surface = 0;
		}

		device->physicalDevice = 0;
	
		vkDestroyInstance(context->instance, context->allocator);

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