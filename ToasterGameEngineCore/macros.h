#pragma once

// dll management
#ifdef TOAST_EXPORT
#ifdef _MSC_VER
#define TAPI __declspec(dllexport)
#else
#define TAPI __attribute__((visibility("default")))
#endif
#else
#ifdef _MSC_VER
#define TAPI __declspec(dllimport)
#else
#define TAPI
#endif
#endif

// hopefully I can replace my uses of this with the much more elegant
// version above but I don't think that will be possible
#ifdef _MSC_VER
#define TEXPORT __declspec(dllexport)
#define TIMPORT __declspec(dllimport)
#else
#define TEXPORT __attribute__((visibility("default")))
#define TIMPORT
#endif

// Getting debugbreak/builtintrap for errors
#define TASSERT_ENABLED
#ifdef TASSERT_ENABLED
#ifdef _MSC_VER
#include <intrin.h>
#define debugBreak() __debugbreak()
#else
#define debugBreak() __builtin_trap()
#endif
#endif

// inlining
#ifdef __GNUC__
#define TINLINE __attribute__((always_inline)) inline
#define TNOINLINE __attribute__((noinline))
#elif defined(_MSC_VER)
#define TINLINE __forceinline
#define TNOINLINE __declspec(noinline)
#else
#define TINLINE inline
#define TNOINLINE
#endif

// platform
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))
#define TWIN32
#elif __APPLE__
#define TAPPLE
#elif __linux__
#define TLINUX
#endif 

// fuck off clang
#if defined(__clang__)
#error "won't compile on clang!"
#endif