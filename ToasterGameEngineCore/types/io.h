#pragma once
#include <fstream>
#include "primitive.h"
namespace toast
{
	// fstream
	template<typename TTy = c16>
	using fstream = std::basic_fstream<TTy, std::char_traits<TTy>>;
}