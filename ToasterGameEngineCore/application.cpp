#include "Application.h"
#include "platform/platform.h"
#include "logger.h"
#include "memory.h"
#include "clock.h"
#include "render/renderer.h"

namespace toast
{
	b8 Application::running;
	b8 Application::suspended;

	applicationInternals* Application::internals;

	struct applicationInternals
	{
		Logger logger;
		Platform platform;
		EventManager eventManager;
		Clock clock;
		f64 lastTime;
		Renderer renderer;
	};

	b8 Application::create(Game& usrGame)
	{
		// setup variables
		running = false;
		suspended = false;

		// sets up a bunch subsystems implicitly here
		internals = tnew<applicationInternals>();

		/// intialising subsystems
		internals->logger.clearCachedLogs();
		internals->logger.open();
		internals->logger.log<logLevel::TTRACE>("Logger initialised successfully");

#ifdef LOGGER_TEST
		// testing logs
		internals->logger.log<logLevel::TTRACE>("Logger trace");
		internals->logger.log<logLevel::TDEBUG>("Logger debug");
		internals->logger.log<logLevel::TINFO>("Logger info");
		internals->logger.log<logLevel::TWARN>("Logger warn");
		internals->logger.log<logLevel::TERROR>("Logger error");
		internals->logger.log<logLevel::TFATAL>("Logger fatal");
#endif

		// Input init
		if (!Input::initialise(&internals->eventManager))
		{
			internals->logger.log<logLevel::TFATAL>(
				"Input initialisation failed");
			return false;
		}
		else
		{
			internals->logger.log<logLevel::TTRACE>(
				"Input initialised successfully");
		}

		internals->eventManager.registerEvent(eventCode::APPLICATION_QUIT,
			0, onEvent);
		internals->eventManager.registerEvent(eventCode::KEY_PRESSED,
			0, onKey);
		internals->eventManager.registerEvent(eventCode::KEY_RELEASED,
			0, onKey);

		// getting user defined configuration
		toast::config gameConfig = usrGame.init();

		// starting platform specific window and/or context
		toast::err error = internals->platform.start(gameConfig.name, gameConfig.posX, 
			gameConfig.posY, gameConfig.width, gameConfig.height);

		switch (error)
		{
			case 0:
				internals->logger.log<toast::logLevel::TTRACE>(
					"Platform intialised successfully");
				break;
#ifdef TWIN32
			case 1:
				internals->logger.log<toast::logLevel::TFATAL>(
					"Failed to register window class");
				return false;
			case 2:
				internals->logger.log<toast::logLevel::TFATAL>(
					"window creation failed");
				return false;
#elif defined(TLINUX)
			case 1:
				internals->logger.log<toast::logLevel::TERROR>("Failed to flush stream");
				return false;
			case 2:
				internals->logger.log<toast::logLevel::TFATAL>("Invalid X server display");
				return false;
			case 3:
				internals->logger.log<toast::logLevel::TFATAL>("Could not connect to X server");
				return false;
#endif
			default: break;
		}

		// starting renderer before user code but after platform
		if (!internals->renderer.initialise(gameConfig.name, internals->platform.state))
		{
			internals->logger.log<logLevel::TFATAL>(
				"renderer couldn't be initialised");
			return false;
		}
		else
		{
			internals->logger.log<logLevel::TTRACE>(
				"renderer intialised successfully");
		}

		// starting user start method and logging if unsuccessful
		if (!usrGame.start())
		{
			internals->logger.log<logLevel::TFATAL>(
				"game create failed");
			return false;
		}

		running = true;

		return true;
	}

	b8 Application::runLoop(Game& usrGame)
	{
		if (!running)
		{
			internals->logger.log<logLevel::TERROR>(
				"Cannot run application that wasn't created successfully!");
			return false;
		}

		internals->clock.start();
		internals->clock.update();
		internals->lastTime = internals->clock.elapsed;
		f64 runTime = 0;
		f64 targetFrameSec = 1.0f / 144;
		u8 frameCount = 0;

		while (running)
		{
			if (!internals->platform.pumpMessages())
			{
				running = false;
				internals->logger.log<logLevel::TFATAL>(
					"Failed to pump messages");
				break;
			}

			// if either method returns false the loop ends
			if(!suspended)
			{
				internals->clock.update();
				const f64 currTime = internals->clock.elapsed;
				const f64 delta = (currTime - internals->lastTime);
				const f64 frameStartTime = Platform::getAbsTime();

				if (!internals->platform.pumpMessages())
				{
					running = false;
					internals->logger.log<logLevel::TFATAL>(
						"Failed to pump messages");
					break;
				}
					
				if (!usrGame.update((f32)delta))
				{
					running = false;
					internals->logger.log<logLevel::TFATAL>(
						"game update failed");
					break;
				}

				renderPacket packet = { delta };
				internals->renderer.drawFrame(&packet);

				const f64 frameEndTime = Platform::getAbsTime();
				const f64 frameElapTime = frameEndTime - frameStartTime;
				runTime += frameElapTime;
				const f64 remainingSecs = targetFrameSec - frameElapTime;

				if (remainingSecs > 0 && (remainingSecs * 1000) > 0)
				{
					Platform::sleep((remainingSecs * 1000));

					frameCount++;
				}

				Input::update(delta);
			}
		}

		running = false;

		// calling shutdown functions
		internals->platform.shutdown();

		// deallocating memory and calling destructors
		tdelete<applicationInternals>(internals);

		// shutting down static components
		if (!Input::shutdown())
		{
			internals->logger.log<logLevel::TFATAL>(
				"Input shutdown failed");
		}

		return true;
	}

	b8 Application::onEvent(eventCode code, ptr sender, ptr listener, eventContext context)
	{
		switch (code) {
			case eventCode::APPLICATION_QUIT:
				internals->logger.log<logLevel::TINFO>(
					"Application quit recieved, shutting down");
				running = false;
				return true;
		}
	}

	b8 Application::onKey(eventCode code, ptr sender, ptr listener, eventContext context)
	{
		switch (code)
		{
			case eventCode::KEY_PRESSED:
				switch (static_cast<keys>(context._u16[0]))
				{
					case keys::ESCAPE:
						internals->eventManager.fireEvent(
							eventCode::APPLICATION_QUIT, 0, {});
						return true;
					default: return true;
				}
			case eventCode::KEY_RELEASED:
				switch (static_cast<keys>(context._u16[0]))
				{
					default: return true;
				}
				break;
		}

		return false;
	}
}