#pragma once
#include "../asserts.h"
#include "../types/string.h"
#include "../macros.h"

#ifdef TWIN32
#include <windows.h>
#include <windowsx.h>
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
	//struct memoryState;

	struct TEXPORT platformInfo
	{
		u8 processCount;
		u16 processArch;
		u64 processType;
	};

	/// <summary>
	/// Class for static use only. Essentially a glorified namespace,
	/// with some encapsulation to enforce proper usage.
	/// </summary>
	class Platform final
	{
	public:
		// the internal state of the platform
		platformState* state;

		// useful info
		static platformInfo info;

	private:
		// memory allocator variables
		//static memoryState * memState;

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

		TEXPORT static b8 createHeap();
		TEXPORT static ptr allocate(u64 size, b8 aligned);
		TEXPORT static b8 deallocate(void* block, b8 aligned);
		TEXPORT static ptr zeroMem(void* block, u64 size);
		TEXPORT static ptr copyMem(void* dest, void* source, u64 size);
		TEXPORT static ptr setMem(void* dest, i32 value, u64 size);
		TEXPORT static b8 destroyHeap();

		TEXPORT static void consoleWrite(const str<cv> &message, const color color);
		TEXPORT static void consoleWriteError(const str<cv> &message, const color color);

		static f64 getAbsTime();

		static void sleep(u64 ms);

		void shutdown();

		~Platform();
	};
}