#pragma once
#include "macros.h"
#include "types/primitive.h"
#include "platform/platform.h"

namespace toast
{
	template<class T>
	TINLINE T* tnew(u64 length)
	{
		void* buffer = Platform::allocate(length * sizeof(T), false);
		return reinterpret_cast<T*>(new(buffer) T());
	}

	template<class T>
	TINLINE T* tnew()
	{
		void* buffer = Platform::allocate(sizeof(T), false);
		return reinterpret_cast<T*>(new(buffer) T());
	}

	template<class T>
	TINLINE T* tnew(T &inst)
	{
		void* buffer = Platform::allocate(sizeof(T), false);
		return reinterpret_cast<T*>(new(buffer) T(inst));
	}

	template<class T>
	TINLINE T* tnew(T&& inst)
	{
		void* buffer = Platform::allocate(sizeof(T), false);
		return reinterpret_cast<T*>(new(buffer) T(inst));
	}


	template<class T>
	TINLINE void tdelete(T* block)
	{
		block->~T();
		Platform::deallocate(block, false);
	}

	template<typename T>
	TINLINE T* allocate(u64 length)
	{
		return reinterpret_cast<T*>(Platform::allocate(length *sizeof(T), false));
	}

	template<typename T>
	TINLINE T* allocate()
	{
		return reinterpret_cast<T*>(Platform::allocate(sizeof(T), false));
	}

	template<typename T>
	TINLINE void deallocate(T* block)
	{
		Platform::deallocate(block, false);
	}

	template<typename T>
	TINLINE void zeroMem(T* block, u64 length)
	{
		Platform::zeroMem(block, length * sizeof(T));
	}

	template<typename T>
	TINLINE void copyMem(T* dest, T* source, u64 length)
	{
		Platform::copyMem(dest, source, length * sizeof(T));
	}

	template<typename T>
	TINLINE void setMem(T* dest, i32 value, u64 length)
	{
		Platform::setMem(dest, value, length * sizeof(T));
	}
}