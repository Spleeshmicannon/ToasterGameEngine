#pragma once
#include "macros.h"
#include "types/primitive.h"
#include "platform.h"

namespace toast
{
	template<class T>
	TINLINE T* tnew(u64 size)
	{
		void* buffer = Platform::allocate(size * sizeof(T), false);
		return reinterpret_cast<T*>(new(buffer) T());
	}

	template<class T>
	TINLINE void tdelete(T* block)
	{
		block->~T();
		Platform::deallocate(block, false);
	}

	template<typename T>
	TINLINE T* allocate(u64 size)
	{
		return reinterpret_cast<T*>(Platform::allocate(size*sizeof(T), false));
	}

	template<typename T>
	TINLINE void deallocate(T* block)
	{
		Platform::deallocate(block, false);
	}

	template<typename T>
	TINLINE void zeroMem(T* block, u64 size)
	{
		Platform::zeroMem(block, size * sizeof(T));
	}

	template<typename T>
	TINLINE void copyMem(T* dest, const T* source, u64 size)
	{
		Platform::copyMem(dest, source, size * sizeof(T));
	}

	template<typename T>
	TINLINE void setMem(T* dest, i32 value, u64 size)
	{
		Platform::setMem(dest, value, size * sizeof(T));
	}
}