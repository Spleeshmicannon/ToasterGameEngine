#pragma once
#include <cstdint>
#include <chrono>
#include <string>
#include <vector>
#include <fstream>

namespace toast
{
	// unsigned int types
	typedef std::uint8_t  u8;
	typedef std::uint16_t u16;
	typedef std::uint32_t u32;
	typedef std::uint64_t u64;

	typedef std::uint_fast8_t  ufast8;
	typedef std::uint_fast16_t ufast6;
	typedef std::uint_fast32_t ufast32;
	typedef std::uint_fast64_t ufast64;

	// other int types
	typedef u8 err;
	typedef u8 bitmask;

	// signed int types
	typedef std::int8_t  i8;
	typedef std::int16_t i16;
	typedef std::int32_t i32;
	typedef std::int64_t i64;

	typedef std::int_fast8_t  ifast8;
	typedef std::int_fast16_t ifast6;
	typedef std::int_fast32_t ifast32;
	typedef std::int_fast64_t ifast64;

	// floating point types
	typedef float  f32;
	typedef double f64;
	typedef long double f96; // note, has variable implementations

	// chars
#ifndef TENABLE_CHAR8
	typedef u8 c8; // UTF-8?
#else
	typedef char8_t c8 // has possibly undefined behaviour on some systems
#endif
	typedef char16_t c16; // UTF-16
	typedef char32_t c32; // UTF-32
	typedef char cv; // variable size char

	// bool
	typedef bool b8;
	
	// other
	typedef void* ptr;
}