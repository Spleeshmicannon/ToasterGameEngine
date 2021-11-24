#pragma once
#include "types/primitive.h"
#include "memory.h"
#include "logger.h"

namespace toast
{
	template<typename T, u32 capInc = 10>
	class LiteVector
	{
	public:
		u32 capacity;
		u32 length;
		T* data;

	public:
		LiteVector();
		LiteVector(u32 size);
		~LiteVector();

		T& safeGet(u32 index);
		void resize(u32 size);
		void push(T entry);
		void pop();
		void unsafePopAt(u32 index);
		void safePopAt(u32 index);
	};

	template<typename T, u32 capInc>
	LiteVector<T, capInc>::LiteVector() : capacity(10), 
		length(0), data(allocate<T>(10))
	{
		zeroMem(data, capacity);
	}

	template<typename T, u32 capInc>
	LiteVector<T, capInc>::LiteVector(u32 size) : capacity(size),
		length(0), data(allocate<T>(size))
	{
		zeroMem(data, capacity);
	}

	template<typename T, u32 capInc>
	LiteVector<T, capInc>::~LiteVector()
	{
		tdelete<T>(data);
	}

	template<typename T, u32 capInc>
	T& LiteVector<T, capInc>::safeGet(u32 index)
	{
		if (index <= length)
		{
			return data[index];
		} 
		else
		{
			Logger::staticLog<logLevel::TERROR>(
				"Can't access LiteVec member, index larger than length!");
			return nullptr;
		}
	}

	template<typename T, u32 capInc>
	void LiteVector<T, capInc>::resize(u32 size)
	{
		T* temp = allocate<T>(size);
		copyMem(temp, data, size);
		deallocate(data);
		data = temp;
		length = size;
		capacity = size;
	}

	template<typename T, u32 capInc>
	void LiteVector<T, capInc>::push(T entry)
	{
		if (length < capacity)
		{
			data[++length] = entry;
		}
		else
		{
			capacity += capacity * capInc;
			resize(capacity);
			data[++length] = entry;
		}
	}

	template<typename T, u32 capInc>
	void LiteVector<T, capInc>::pop()
	{
		data[length] = 0;
		--length;
		if (length < (capacity / capInc))
		{
			capacity = capacity / capInc;
			resize(capacity);
		}
	}

	template<typename T, u32 capInc>
	void LiteVector<T, capInc>::unsafePopAt(u32 index)
	{
		--length;
		for (u32 i = index; i < length; ++i)
		{
			data[i] = data[i + 1];
		}
		
		const u32 downCap = capacity / capInc;
		if (length <= downCap)
		{
			capacity = downCap;
			resize(capacity);
		}
	}

	template<typename T, u32 capInc>
	void LiteVector<T, capInc>::safePopAt(u32 index)
	{
		if (index <= length)
		{
			--length;
			for (u32 i = index; i < length; ++i)
			{
				data[i] = data[i + 1];
			}

			const u32 downCap = capacity / capInc;
			if (length <= downCap)
			{
				capacity = downCap;
				resize(capacity);
			}
		}
		else
		{
			Logger::staticLog<logLevel::TERROR>(
				"Can't access LiteVec member, index larger than length!");
		}
	}
}