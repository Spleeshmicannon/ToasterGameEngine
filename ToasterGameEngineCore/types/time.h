#pragma once
#include <chrono>
#include "primitive.h"

namespace toast
{
	/// chrono
	// for timing operations
	using clock = std::chrono::high_resolution_clock;
	using duration = std::chrono::duration<double>;
	using timePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;

	// for getting date/time
	using sysClock = std::chrono::system_clock;
}