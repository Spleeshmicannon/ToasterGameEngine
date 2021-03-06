#pragma once
#include <string>
#include "primitive.h"

namespace toast
{
	template<class TTy = c16, class TAlloc = std::allocator<TTy>>
	using str = std::basic_string <TTy, std::char_traits<TTy>, TAlloc>;
	using str8 = std::basic_string <c8, std::char_traits<c8>, std::allocator<c8>>;
	using str16 = std::basic_string<c16, std::char_traits<c16>, std::allocator<c16>>;
	using str32 = std::basic_string<c32, std::char_traits<c32>, std::allocator<c32>>;
	
	template<u64 size>
	b8 cStrEqual(const cv str0[size], const cv str1[size])
	{
		for (i32 i = 0; i < size; ++i)
		{
			if (str0[i] != str1[i]) return false;
		}

		return true;
	}
}