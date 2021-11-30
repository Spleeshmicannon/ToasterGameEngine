#pragma once
#include "macros.h"
#include "types/primitive.h"
#include "types/string.h"
#include "platform/platform.h"

namespace toast
{
#ifdef TASSERT_ENABLED
	TINLINE static void assert(b8 expr)
	{
		if (!expr)
		{
			const str<cv> file = __FILE__;
			const str<cv> line = std::to_string(__LINE__);

			Platform::consoleWriteError(
				"Assertion failure: \nFile: " +
				file + "\nLine: " + line + "\n",
				color::TRED
			);
		}
	}

	TINLINE static void assert(b8 expr, str<cv> message)
	{
		if (!expr)
		{
			const str<cv> file = __FILE__;
			const str<cv> line = std::to_string(__LINE__);

			Platform::consoleWriteError(
				"Assertion failure: \nFile: " +
				file + "\nLine: " + line +
				"\nMessage: " + message,
				color::TRED
			);
		}
	}

#ifdef TASSERT_ENABLED_DEBUG
	TINLINE static void debug_assert(b8 expr)
	{
		if (!expr)
		{
			const str<cv> file = __FILE__;
			const str<cv> line = std::to_string(__LINE__);

			Platform::consoleWriteError(
				"Assertion failure: \nFile: " +
				file + "\nLine: " + line + "\n",
				color::TRED
			);
		}
	}
#else
	TINLINE static void debug_assert(b8 expr) {}
#endif
#else
	TINLINE void assert(b8 expr) {}
	TINLINE void assert(b8 expr, const str<cv> &message) {}
	TINLINE void debug_assert(b8 expr) {}
#endif
}