#pragma once
#include "types/primitive.h"

namespace toast
{
	struct Clock
	{
		f64 startTime;
		f64 elapsed;

		void update();
		void start();
		void stop();
	};
}