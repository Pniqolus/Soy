#ifndef TYPES_HPP
#define TYPES_HPP

#include <cstdlib>
#include <cstdint>

using i8      = int8_t;
using i16     = int16_t;
using i32     = int32_t;
using i64     = int64_t;
using imax_t  = intmax_t;

using u8      = uint8_t;
using u16     = uint16_t;
using u32     = uint32_t;
using u64     = uint64_t;

using f32     = float;
using f64     = double;
using flong   = long double;

using chr8    = char8_t;
using chr16   = char16_t;
using chr32   = char32_t;

constexpr auto i8max   = INT8_MAX;
constexpr auto i16max  = INT16_MAX;
constexpr auto i32max  = INT32_MAX;
constexpr auto i64max  = INT64_MAX;

constexpr auto u8max    = UINT8_MAX;
constexpr auto u16max   = UINT16_MAX;
constexpr auto u32max   = UINT32_MAX;
constexpr auto u64max   = UINT64_MAX;
constexpr auto size_max = SIZE_MAX;

#include <string>
#include <string_view>

using std::string;
using std::string_view;

using str     = string;
using str_vw  = string_view;
using cstr    = const char*;
using cstring = const char*;

#endif /* TYPES_HPP */