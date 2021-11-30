#include "clock.h"
#include "platform/platform.h"

namespace toast
{
	void Clock::update()
	{
		if (startTime != 0)
		{
			elapsed = Platform::getAbsTime() - startTime;
		}
	}

	void Clock::start()
	{
		startTime = Platform::getAbsTime();
		elapsed = 0;
	}

	void Clock::stop()
	{
		startTime = 0;
	}
}