// SwapByteOrder.hpp - Generic and optimized byte swaps
//
// Part of the bijou Project, under the Apache License v2.0 with LLVM Exceptions.
//
// This file is a modified copy of the file "include/llvm/Support/SwapByteOrder.h" from the LLVM Project.
// Summary of modifications:
//   * Reformatted comment style in copyright header.
//   * Changed 'llvm' to 'bijou', and 'LLVM' to 'BIJOU'.
//   * Removed unused APIs.
//
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file
/// @brief
/// This file declares generic and optimized functions to swap the byte order of
/// an integral type.
///
/// @TODO: use c++23 std::byteswap instead of builtins.

#ifndef BIJOU_SUPPORT_SWAPBYTEORDER_HPP
#define BIJOU_SUPPORT_SWAPBYTEORDER_HPP

#include <cstddef>
#include <cstdint>
#include <type_traits>
#if defined(_MSC_VER) && !defined(_DEBUG)
#include <stdlib.h> // for _byteswap_ushort, _byteswap_ulong, _byteswap_uint64
#endif

namespace bijou {

/// ByteSwap_16 - This function returns a byte-swapped representation of
/// the 16-bit argument.
inline uint16_t ByteSwap_16(uint16_t value) {
#if defined(_MSC_VER) && !defined(_DEBUG)
  // The DLL version of the runtime lacks these functions (bug!?), but in a
  // release build they're replaced with BSWAP instructions anyway.
  return _byteswap_ushort(value);
#else
  uint16_t Hi = value << 8;
  uint16_t Lo = value >> 8;
  return Hi | Lo;
#endif
}

/// This function returns a byte-swapped representation of the 32-bit argument.
inline uint32_t ByteSwap_32(uint32_t value) {
#if defined(__llvm__) || (defined(__GNUC__) && !defined(__ICC))
  return __builtin_bswap32(value);
#elif defined(_MSC_VER) && !defined(_DEBUG)
  return _byteswap_ulong(value);
#else
  uint32_t Byte0 = value & 0x000000FF;
  uint32_t Byte1 = value & 0x0000FF00;
  uint32_t Byte2 = value & 0x00FF0000;
  uint32_t Byte3 = value & 0xFF000000;
  return (Byte0 << 24) | (Byte1 << 8) | (Byte2 >> 8) | (Byte3 >> 24);
#endif
}

/// This function returns a byte-swapped representation of the 64-bit argument.
inline uint64_t ByteSwap_64(uint64_t value) {
#if defined(__llvm__) || (defined(__GNUC__) && !defined(__ICC))
  return __builtin_bswap64(value);
#elif defined(_MSC_VER) && !defined(_DEBUG)
  return _byteswap_uint64(value);
#else
  uint64_t Hi = ByteSwap_32(uint32_t(value));
  uint32_t Lo = ByteSwap_32(uint32_t(value >> 32));
  return (Hi << 32) | Lo;
#endif
}

} // namespace bijou

#endif // BIJOU_SUPPORT_SWAPBYTEORDER_HPP
