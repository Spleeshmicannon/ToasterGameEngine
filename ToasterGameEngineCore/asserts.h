#pragma once
#include "macros.h"
#include "types/primitive.h"
#include "types/string.h"
#include "platform.h"

namespace toast
{
#ifdef TOAST_ASSERT
	TINLINE static void assert(b8 expr)
	{
		if (!expr)
		{
			Platform::consoleWriteError(
				"Assertion failure: \nFile: " +
				__FILE__ + "\nLine: " + __LINE__ + "\n",
				TRED
			)
		}
	}

	TINLINE static void assert(b8 expr, str<cv> message)
	{
		if (!expr)
		{
			Platform::consoleWriteError(
				"Assertion failure: \nFile: " +
				__FILE__ + "\nLine: " + __LINE__ +
				"\nMessage: " + message,
				TRED
			)
		}
	}

#ifdef TOAST_ASSERT_DEBUG
	TINLINE static void debug_assert(b8 expr)
	{
		if (!expr)
		{
			Platform::consoleWriteError(
				"Assertion failure: \nFile: " +
				__FILE__ + "\nLine: " + __LINE__ + "\n",
				TRED
			)
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