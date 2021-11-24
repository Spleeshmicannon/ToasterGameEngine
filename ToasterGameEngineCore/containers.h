#pragma once
#include "primitive.h"
#include "../memory.h"
#include "../logger.h"

namespace toast
{
	template<typename T, u64 capInc = 10>
	class LiteVector
	{
	public:
		u64 capacity;
		u64 length;
		T* data;

	public:
		LiteVector();
		LiteVector(u16 size);
		~LiteVector();

		constexpr T& operator[](u64 index);
		void resize(u64 size);
		void push(T entry);
		void pop(T entry);
	};

	template<typename T, u64 capInc>
	LiteVector<T, capInc>::LiteVector() : capacity(10), 
		length(0), data(tnew<T>(10))
	{}

	template<typename T, u64 capInc>
	LiteVector<T, capInc>::LiteVector(u16 size) : capacity(size),
		length(0), data(tnew<T>(size))
	{}

	template<typename T, u64 capInc>
	LiteVector<T, capInc>::~LiteVector()
	{
		tdelete<T>(data);
	}

	template<typename T, u64 capInc>
	constexpr T& LiteVector<T, capInc>::operator[](u64 index)
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

	template<typename T, u64 capInc>
	void LiteVector<T, capInc>::resize(u64 size)
	{
		T* temp = allocate<T>(size);
		copyMem(temp, data, size);
		deallocate(data);
		data = temp;
		length = size;
		capacity = size;
	}

	template<typename T, u64 capInc>
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

	template<typename T, u64 capInc>
	void LiteVector<T, capInc>::pop(T entry)
	{
		data[length] = 0;
		--length;
		if (length < (capacity / capInc))
		{
			capacity = capacity / capInc;
			resize(capacity);
		}
	}
}