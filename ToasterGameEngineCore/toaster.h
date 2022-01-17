#pragma once
#include "application.h"
#include "macros.h"
#include "types/types.h"
#include "game.h"
#include "types/time.h"
#include "logger.h"
#include "memory.h"
#include "input.h"

namespace toast
{
	TINLINE void debugWrite(const char * message)
	{
		Logger::staticLog<logLevel::TUSER_DEBUG>(message);
	}

	/// <summary>
	/// Run inside your main function and
	/// reference your instance of a 
	/// Game subclass.
	/// </summary>
	/// <param name="usrGame"></param>
	/// <returns></returns>
	int tmain(Game & usrGame)
	{
		// initialisation
		if (Application::create(usrGame))
		{
			// game loop
			if (!Application::runLoop(usrGame))
			{
				return 2;
			}
		}
		else
		{
			return 1;
		}

		return 0;
	}
}