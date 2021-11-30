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
	/// <summary>
	/// Run inside your main function and
	/// reference your instance of a 
	/// Game subclass.
	/// </summary>
	/// <param name="usrGame"></param>
	/// <returns></returns>
	int tmain(Game & usrGame)
	{
		Application app;


		// initialisation
		if (app.create(usrGame))
		{
			// game loop
			if (!app.runLoop(usrGame))
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