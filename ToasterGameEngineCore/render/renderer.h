#pragma once

#include "../macros.h"
#include "../types/string.h"
#include "../platform/platform.h"

namespace toast
{
	struct static_mesh_data;
	
	struct renderPacket
	{
		f32 deltaTime;
	};

	struct vulkanContext;
	struct vulkanDevice;
	struct vulkanImage;

	class Swapchain;

	class Renderer
	{
	private:
		platformState* platState;
		vulkanContext* context;
		vulkanDevice* device;
		//Swapchain* swapchain;

	private:
		void resized(u16 width, u16 height);
		b8 beginFrame(f32 deltaTime);
		b8 endFrame(f32 deltaTime);

		b8 createSurface();
		b8 setupDevice();
		b8 createLogicalDevice();

	public:
		Renderer();
		~Renderer();

		b8 initialise(const str<cv> appName, platformState* _platState);
		b8 shutdown();

		b8 drawFrame(renderPacket* packet);
	};
}