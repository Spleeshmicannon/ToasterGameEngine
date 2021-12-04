#pragma once
#include <chrono>
#include "primitive.h"

namespace toast
{
	namespace chrono
	{
		using hrc = std::chrono::high_resolution_clock;
		using duration = std::chrono::duration<double>;
	}

	// for getting date/time
	using sysClock = std::chrono::system_clock;
}