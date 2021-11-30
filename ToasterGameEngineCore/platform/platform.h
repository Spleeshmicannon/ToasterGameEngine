#pragma once
#include "../asserts.h"
#include "../types/string.h"
#include "../macros.h"

#ifdef TWIN32
#include <windows.h>
#include <windowsx.h>
#elif defined(TLINUX)

#elif defined(TAPPLE)
#error "no apple support, sorry :("
#else
#error "unkown/unsupported platform"
#endif

namespace toast
{
	enum class color 
	{
		TRED_BKG = 0,
		TRED = 1,
		TYELLOW = 2,
		TBLUE = 3,
		TGREEN = 4,
		TGREY = 5
	};

	struct platformState;

	/// <summary>
	/// Class for static use only. Essentially a glorified namespace,
	/// with some encapsulation to enforce proper usage.
	/// </summary>
	class Platform final
	{
	public:
		// the internal state of the platform
		platformState* state;

		// time based variables
#ifdef TWIN32
		static f64 clockFreq;
		static LARGE_INTEGER startTime;
#endif
	public:
		Platform();

		err start(const str<cv> &name,
			i32 x, i32 y, i32 width, i32 height);

		b8 pumpMessages();

		// TODO: assm regex
		/* template<int size>
		static bitmask regex(bitmask params,
			const char str1[size], const char str2[size]);*/

		TEXPORT static void* allocate(u64 size, b8 aligned);
		TEXPORT static void deallocate(void* block, b8 aligned);
		TEXPORT static void* zeroMem(void* block, u64 size);
		TEXPORT static void* copyMem(void* dest, void* source, u64 size);
		TEXPORT static void* setMem(void* dest, i32 value, u64 size);

		TEXPORT static void consoleWrite(const str<cv> &message, const color color);
		TEXPORT static void consoleWriteError(const str<cv> &message, const color color);

		static f64 getAbsTime();

		static void sleep(u64 ms);

		void shutdown();

		~Platform();
	};
}