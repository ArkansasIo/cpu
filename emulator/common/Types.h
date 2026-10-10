// types.h - shared primitive types for the PS3 clone skeleton.
#pragma once

#include <cstdint>
#include <string>

namespace ps3clone {

using u8  = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using s8  = std::int8_t;
using s16 = std::int16_t;
using s32 = std::int32_t;
using s64 = std::int64_t;

// A PowerPC/Cell guest address. The Cell uses 64-bit effective addresses.
using GuestAddr = u64;

// Result code returned by every emulated syscall / library function.
// CELL_OK == 0 matches the real SDK convention.
enum class CellResult : s32 {
    CELL_OK       = 0,
    CELL_EINVAL   = 0x80010002,
    CELL_ENOMEM   = 0x80010004,
    CELL_EFAULT   = 0x8001000F,
    CELL_ENOSYS   = 0x80010003,
    NOT_IMPLEMENTED = -1,
};

const char* ToString(CellResult r);

} // namespace ps3clone
