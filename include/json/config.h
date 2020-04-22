// Copyright 2007-2010 Baptiste Lepilleur and The JsonCpp Authors
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.
// See file LICENSE for detail or copy at http://jsoncpp.sourceforge.net/LICENSE

#ifndef JSON_CONFIG_H_INCLUDED
#define JSON_CONFIG_H_INCLUDED

#include <istream>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>

#if JSONCPP_VER_11
#include <cstddef>   // typedef ptrdiff_t
#include <cstdint>   // typedef int64_t, uint64_t
#else
#include <stddef.h>
#include <stdint.h>
#endif

// If non-zero, the library uses exceptions to report bad input instead of C
// assertion macros. The default is to use exceptions.
#ifndef JSON_USE_EXCEPTION
#define JSON_USE_EXCEPTION 1
#endif

// Temporary, tracked for removal with issue #982.
#ifndef JSON_USE_NULLREF
#define JSON_USE_NULLREF 1
#endif

/// If defined, indicates that the source file is amalgamated
/// to prevent private header inclusion.
/// Remarks: it is automatically defined in the generated amalgamated header.
// #define JSON_IS_AMALGAMATION

// Export macros for DLL visibility
#if defined(JSON_DLL_BUILD)
#if defined(_MSC_VER) || defined(__MINGW32__)
#define JSON_API __declspec(dllexport)
#define JSONCPP_DISABLE_DLL_INTERFACE_WARNING
#elif defined(__GNUC__) || defined(__clang__)
#define JSON_API __attribute__((visibility("default")))
#endif // if defined(_MSC_VER)

#elif defined(JSON_DLL)
#if defined(_MSC_VER) || defined(__MINGW32__)
#define JSON_API __declspec(dllimport)
#define JSONCPP_DISABLE_DLL_INTERFACE_WARNING
#endif // if defined(_MSC_VER)
#endif // ifdef JSON_DLL_BUILD

#if !defined(JSON_API)
#define JSON_API
#endif

#if defined(_MSC_VER) && _MSC_VER < 1800
#error                                                                         \
    "ERROR:  Visual Studio 12 (2013) with _MSC_VER=1800 is the oldest supported compiler with sufficient C++11 capabilities"
#endif

#if defined(_MSC_VER) && _MSC_VER < 1900
// As recommended at
// https://stackoverflow.com/questions/2915672/snprintf-and-visual-studio-2010
extern JSON_API int msvc_pre1900_c99_snprintf(char* outBuf, size_t size,
                                              const char* format, ...);
#define jsoncpp_snprintf msvc_pre1900_c99_snprintf
#else
#define jsoncpp_snprintf std::snprintf
#endif

// If JSON_NO_INT64 is defined, then Json only support C++ "int" type for
// integer
// Storages, and 64 bits integer support is disabled.
// #define JSON_NO_INT64 1

// These Macros are maintained for backwards compatibility of external tools.
#if (defined(_MSC_VER) && _MSC_VER >= 1900) ||                                \
    (defined(__GNUC__) && __cplusplus >= 201103L) ||                          \
    (defined(__clang__) && __clang_major__ == 3 && __clang_minor__ > 3 )

#define JSONCPP_VER_11 1
#define JSONCPP_NULL nullptr
#define JSONCPP_CONST constexpr
#define JSONCPP_CTOR_DEFAULT = default
#define JSONCPP_CTOR_DELETE = delete
#define JSONCPP_NOEXCEPT noexcept
#define JSONCPP_OP_EXPLICIT explicit
#define JSONCPP_OVERRIDE override
#define JSONCPP_MOVE(value) std::move(value)
#else
#define JSONCPP_VER_11 0
#define JSONCPP_NULL NULL
#define JSONCPP_CONST const
#define JSONCPP_CTOR_DEFAULT
#define JSONCPP_CTOR_DELETE
#define JSONCPP_NOEXCEPT throw()
#define JSONCPP_OP_EXPLICIT
#define JSONCPP_OVERRIDE
#define JSONCPP_MOVE(value) value
#endif

// Define deprecated attribute
// [[deprecated]] is in C++14 or in Visual Studio 2015 and later
// For compatibility, [[deprecated]] is not used
// and there is no preformance improvement for this usage, so we don't need to
// use it.
#ifdef __clang__
#if __has_extension(attribute_deprecated_with_message)
#define JSONCPP_DEPRECATED(message) __attribute__((deprecated(message)))
#endif
#elif defined(__GNUC__) // not clang (gcc comes later since clang emulates gcc)
#if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5))
#define JSONCPP_DEPRECATED(message) __attribute__((deprecated(message)))
#elif (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1))
#define JSONCPP_DEPRECATED(message) __attribute__((__deprecated__))
#endif                  // GNUC version
#elif defined(_MSC_VER) // MSVC (after clang because clang on Windows emulates
                        // MSVC)
#define JSONCPP_DEPRECATED(message) __declspec(deprecated(message))
#endif // __clang__ || __GNUC__ || _MSC_VER

#if !defined(JSONCPP_DEPRECATED)
#define JSONCPP_DEPRECATED(message)
#endif // if !defined(JSONCPP_DEPRECATED)

// Define int64 double voncersion in Clang platform
#if defined(__clang__) || (defined(__GNUC__) && (__GNUC__ >= 6))
#define JSON_USE_INT64_DOUBLE_CONVERSION 1
#endif
// Define AMALGMATION macro
#if !defined(JSON_IS_AMALGAMATION)
#if JSONCPP_VER_11
#include "allocator.h"
#endif
#include "version.h"

#endif // if !defined(JSON_IS_AMALGAMATION)

namespace Json {
typedef int Int;
typedef unsigned int UInt;
#if defined(JSON_NO_INT64)
typedef int LargestInt;
typedef unsigned int LargestUInt;
#undef JSON_HAS_INT64
#else                 // if defined(JSON_NO_INT64)
// For Microsoft Visual use specific types as long long is not supported
#if defined(_MSC_VER) // Microsoft Visual Studio
typedef __int64 Int64;
typedef unsigned __int64 UInt64;
#else                 // if defined(_MSC_VER) // Other platforms, use long long
typedef __int64_t Int64;
typedef __uint64_t UInt64;
#endif                // if defined(_MSC_VER)
typedef Int64 LargestInt;
typedef UInt64 LargestUInt;
#define JSON_HAS_INT64
#endif // if defined(JSON_NO_INT64)

#if JSONCPP_VER_11 // CXX_Stdandard >= C++11
template <typename T>
using Allocator =
    typename std::conditional<JSONCPP_USING_SECURE_MEMORY, SecureAllocator<T>,
                              std::allocator<T>>::type;
using String = std::basic_string<char, std::char_traits<char>, Allocator<char>>;
using IStringStream =
    std::basic_istringstream<String::value_type, String::traits_type,
                             String::allocator_type>;
using OStringStream =
    std::basic_ostringstream<String::value_type, String::traits_type,
                             String::allocator_type>;
using IStream = std::istream;
using OStream = std::ostream;
#else
typedef std::string String;
typedef std::ostringstream OStringStream;
typedef std::ostream OStream;
typedef std::istringstream IStringStream;
typedef std::istream IStream;
#endif // JSONCPP_VER_11
} // namespace Json

// Legacy names (formerly macros).
typedef Json::String JSONCPP_STRING;
typedef Json::IStringStream JSONCPP_ISTRINGSTREAM;
typedef Json::OStringStream JSONCPP_OSTRINGSTREAM;
typedef Json::IStream JSONCPP_ISTREAM;
typedef Json::OStream JSONCPP_OSTREAM;

#endif // JSON_CONFIG_H_INCLUDED
