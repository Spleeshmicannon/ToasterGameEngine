#include "Application.h"
#include "platform.h"
#include "logger.h"
#include "memory.h"
#include "event.h"

namespace toast
{
	struct _internals
	{
		Logger logger;
		Platform platform;
		EventManager eventManager;
	};

	Application::Application() : initialised(false),
		running(false), suspended(false), 
		internals(tnew<_internals>(1))
	{}

	Application::~Application()
	{
		tdelete<_internals>(internals);
	}

	b8 Application::create(Game& usrGame)
	{
		auto start = clock::now();

		if (initialised)
		{
			internals->logger.log<logLevel::TFATAL>(
				"application can't be created more than once");
			return false;
		}

		// intialising subsystems
		internals->logger.clearCachedLogs();
		internals->logger.open();
		internals->logger.log<logLevel::TTRACE>("Logger Opened");

		internals->logger.log<logLevel::TTRACE>("Logger trace");
		internals->logger.log<logLevel::TDEBUG>("Logger debug");
		internals->logger.log<logLevel::TINFO>("Logger info");
		internals->logger.log<logLevel::TWARN>("Logger warn");
		internals->logger.log<logLevel::TERROR>("Logger error");
		internals->logger.log<logLevel::TFATAL>("Logger fatal");
		
		// getting user defined configuration
		toast::config gameConfig = usrGame.init();

		// starting user start method and logging if unsuccessful
		if (!usrGame.start())
		{
			internals->logger.log<logLevel::TFATAL>(
				"game create failed");
			return false;
		}

		// starting platform specific window and/or context
		toast::err error = internals->platform.start(gameConfig.name, gameConfig.posX, 
			gameConfig.posY, gameConfig.width, gameConfig.height);

		switch (error)
		{
			case 0:
				internals->logger.log<toast::logLevel::TTRACE>("Window opened");
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

		running = true;

		internals->logger.log<logLevel::TINFO>("Application start time: " +
			std::to_string(duration(clock::now() - start).count()));

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

		while (running)
		{
			// if either method returns false the loop ends
			if (suspended)
			{
				if (!internals->platform.pumpMessages())
				{
					running = false;
					internals->logger.log<logLevel::TFATAL>(
						"Failed to pump messages");
					break;
				}
			}
			else
			{
				if (!internals->platform.pumpMessages())
				{
					running = false;
					internals->logger.log<logLevel::TFATAL>(
						"Failed to pump messages");
					break;
				}
					
				if (!usrGame.update())
				{
					running = false;
					internals->logger.log<logLevel::TFATAL>(
						"game update failed");
					break;
				}
			}
		}

		running = false;

		internals->platform.shutdown();

		return true;
	}
}