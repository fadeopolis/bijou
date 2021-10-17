// MathExtras.hpp - Useful math functions
//
// Part of the bijou Project, under the Apache License v2.0 with LLVM Exceptions.
//
// This file is a modified copy of the file "llvm/Support/MathExtras.h" from the LLVM Project.
// Summary of modifications:
//   * Reformatted comment style in copyright header.
//   * Changed 'llvm' to 'bijou', and 'LLVM' to 'BIJOU'.
//   * Replaced uses of LLVM helper APIs (such as StringRef, ArrayRef,
//     raw_iostream) with standard C++ APIs
//     (string_view, span, iostream).
//   * Replaced GCC and MSVC builtins with standard C++ APIs (std::countr_zero, ...)
//   * Replaced some LLVM helper macros with (such as LLVM_NODISCARD) with
//     standard C++ attributes (such as [[nodiscard]]).
//   * Removed functions and types not used in bijou.
//   * Removed uses of some LLVM helper APIs (such as FoldingSetNode, DenseMap)
//   * Changed doxygen comments to consistenly use '@' command prefix.
//
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file
/// @brief
/// This file contains some functions that are useful for math stuff.
///

#ifndef BIJOU_SUPPORT_MATHEXTRAS_HPP
#define BIJOU_SUPPORT_MATHEXTRAS_HPP

#include <bit>
#include <cassert>
#include <climits>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>
#include <climits>
#include <type_traits>
#include "bijou/Compiler.hpp"

namespace bijou {

/// The behavior an operation has on an input of 0.
enum ZeroBehavior {
  /// The returned value is undefined.
  ZB_Undefined,
  /// The returned value is numeric_limits<T>::max()
  ZB_Max,
  /// The returned value is numeric_limits<T>::digits
  ZB_Width
};

/// Mathematical constants.
namespace numbers {
// TODO: Track C++20 std::numbers.
// TODO: Favor using the hexadecimal FP constants (requires C++17).
constexpr double e          = 2.7182818284590452354, // (0x1.5bf0a8b145749P+1) https://oeis.org/A001113
                 egamma     = .57721566490153286061, // (0x1.2788cfc6fb619P-1) https://oeis.org/A001620
                 ln2        = .69314718055994530942, // (0x1.62e42fefa39efP-1) https://oeis.org/A002162
                 ln10       = 2.3025850929940456840, // (0x1.24bb1bbb55516P+1) https://oeis.org/A002392
                 log2e      = 1.4426950408889634074, // (0x1.71547652b82feP+0)
                 log10e     = .43429448190325182765, // (0x1.bcb7b1526e50eP-2)
                 pi         = 3.1415926535897932385, // (0x1.921fb54442d18P+1) https://oeis.org/A000796
                 inv_pi     = .31830988618379067154, // (0x1.45f306bc9c883P-2) https://oeis.org/A049541
                 sqrtpi     = 1.7724538509055160273, // (0x1.c5bf891b4ef6bP+0) https://oeis.org/A002161
                 inv_sqrtpi = .56418958354775628695, // (0x1.20dd750429b6dP-1) https://oeis.org/A087197
                 sqrt2      = 1.4142135623730950488, // (0x1.6a09e667f3bcdP+0) https://oeis.org/A00219
                 inv_sqrt2  = .70710678118654752440, // (0x1.6a09e667f3bcdP-1)
                 sqrt3      = 1.7320508075688772935, // (0x1.bb67ae8584caaP+0) https://oeis.org/A002194
                 inv_sqrt3  = .57735026918962576451, // (0x1.279a74590331cP-1)
                 phi        = 1.6180339887498948482; // (0x1.9e3779b97f4a8P+0) https://oeis.org/A001622
constexpr float ef          = 2.71828183F, // (0x1.5bf0a8P+1) https://oeis.org/A001113
                egammaf     = .577215665F, // (0x1.2788d0P-1) https://oeis.org/A001620
                ln2f        = .693147181F, // (0x1.62e430P-1) https://oeis.org/A002162
                ln10f       = 2.30258509F, // (0x1.26bb1cP+1) https://oeis.org/A002392
                log2ef      = 1.44269504F, // (0x1.715476P+0)
                log10ef     = .434294482F, // (0x1.bcb7b2P-2)
                pif         = 3.14159265F, // (0x1.921fb6P+1) https://oeis.org/A000796
                inv_pif     = .318309886F, // (0x1.45f306P-2) https://oeis.org/A049541
                sqrtpif     = 1.77245385F, // (0x1.c5bf8aP+0) https://oeis.org/A002161
                inv_sqrtpif = .564189584F, // (0x1.20dd76P-1) https://oeis.org/A087197
                sqrt2f      = 1.41421356F, // (0x1.6a09e6P+0) https://oeis.org/A002193
                inv_sqrt2f  = .707106781F, // (0x1.6a09e6P-1)
                sqrt3f      = 1.73205081F, // (0x1.bb67aeP+0) https://oeis.org/A002194
                inv_sqrt3f  = .577350269F, // (0x1.279a74P-1)
                phif        = 1.61803399F; // (0x1.9e377aP+0) https://oeis.org/A001622
} // namespace numbers

/// Count number of 0's from the least significant bit to the most
///   stopping at the first 1.
///
/// Only unsigned integral types are allowed.
///
/// @param ZB the behavior on an input of 0. Only ZB_Width and ZB_Undefined are
///   valid arguments.
template <typename T>
unsigned countTrailingZeros(T Val, ZeroBehavior ZB = ZB_Width) {
  static_assert(std::numeric_limits<T>::is_integer &&
                    !std::numeric_limits<T>::is_signed,
                "Only unsigned integral types are allowed.");

  if (ZB != ZB_Undefined && Val == 0) {
    return sizeof(T) * CHAR_BIT;
  }

  return std::countr_zero<T>(Val);
}

/// Count number of 0's from the most significant bit to the least
///   stopping at the first 1.
///
/// Only unsigned integral types are allowed.
///
/// @param ZB the behavior on an input of 0. Only ZB_Width and ZB_Undefined are
///   valid arguments.
template <typename T>
unsigned countLeadingZeros(T Val, ZeroBehavior ZB = ZB_Width) {
  static_assert(std::numeric_limits<T>::is_integer &&
                    !std::numeric_limits<T>::is_signed,
                "Only unsigned integral types are allowed.");

  if (ZB != ZB_Undefined && Val == 0) {
    return sizeof(T) * CHAR_BIT;
  }

  return std::countl_zero<T>(Val);
}

/// Get the index of the first set bit starting from the least
///   significant bit.
///
/// Only unsigned integral types are allowed.
///
/// @param ZB the behavior on an input of 0. Only ZB_Max and ZB_Undefined are
///   valid arguments.
template <typename T> T findFirstSet(T Val, ZeroBehavior ZB = ZB_Max) {
  if (ZB == ZB_Max && Val == 0)
    return std::numeric_limits<T>::max();

  return countTrailingZeros(Val, ZB_Undefined);
}

/// Create a bitmask with the N right-most bits set to 1, and all other
/// bits set to 0.  Only unsigned types are allowed.
template <typename T> T maskTrailingOnes(unsigned N) {
  static_assert(std::is_unsigned<T>::value, "Invalid type!");
  const unsigned Bits = CHAR_BIT * sizeof(T);
  assert(N <= Bits && "Invalid bit index");
  return N == 0 ? 0 : (T(-1) >> (Bits - N));
}

/// Get the index of the last set bit starting from the least
///   significant bit.
///
/// Only unsigned integral types are allowed.
///
/// @param ZB the behavior on an input of 0. Only ZB_Max and ZB_Undefined are
///   valid arguments.
template <typename T> T findLastSet(T Val, ZeroBehavior ZB = ZB_Max) {
  if (ZB == ZB_Max && Val == 0)
    return std::numeric_limits<T>::max();

  // Use ^ instead of - because both gcc and llvm can remove the associated ^
  // in the __builtin_clz intrinsic on x86.
  return countLeadingZeros(Val, ZB_Undefined) ^
         (std::numeric_limits<T>::digits - 1);
}

/// Macro compressed bit reversal table for 256 bits.
///
/// http://graphics.stanford.edu/~seander/bithacks.html#BitReverseTable
static const unsigned char BitReverseTable256[256] = {
#define R2(n) n, n + 2 * 64, n + 1 * 64, n + 3 * 64
#define R4(n) R2(n), R2(n + 2 * 16), R2(n + 1 * 16), R2(n + 3 * 16)
#define R6(n) R4(n), R4(n + 2 * 4), R4(n + 1 * 4), R4(n + 3 * 4)
  R6(0), R6(2), R6(1), R6(3)
#undef R2
#undef R4
#undef R6
};

/// Reverse the bits in @p Val.
template <typename T>
T reverseBits(T Val) {
  unsigned char in[sizeof(Val)];
  unsigned char out[sizeof(Val)];
  std::memcpy(in, &Val, sizeof(Val));
  for (unsigned i = 0; i < sizeof(Val); ++i)
    out[(sizeof(Val) - i) - 1] = BitReverseTable256[in[i]];
  std::memcpy(&Val, out, sizeof(Val));
  return Val;
}

#if __has_builtin(__builtin_bitreverse8)
template<>
inline uint8_t reverseBits<uint8_t>(uint8_t Val) {
  return __builtin_bitreverse8(Val);
}
#endif

#if __has_builtin(__builtin_bitreverse16)
template<>
inline uint16_t reverseBits<uint16_t>(uint16_t Val) {
  return __builtin_bitreverse16(Val);
}
#endif

#if __has_builtin(__builtin_bitreverse32)
template<>
inline uint32_t reverseBits<uint32_t>(uint32_t Val) {
  return __builtin_bitreverse32(Val);
}
#endif

#if __has_builtin(__builtin_bitreverse64)
template<>
inline uint64_t reverseBits<uint64_t>(uint64_t Val) {
  return __builtin_bitreverse64(Val);
}
#endif

// NOTE: The following support functions use the _32/_64 extensions instead of
// type overloading so that signed and unsigned integers can be used without
// ambiguity.

/// Return the high 32 bits of a 64 bit value.
constexpr inline uint32_t Hi_32(uint64_t Value) {
  return static_cast<uint32_t>(Value >> 32);
}

/// Return the low 32 bits of a 64 bit value.
constexpr inline uint32_t Lo_32(uint64_t Value) {
  return static_cast<uint32_t>(Value);
}

/// Make a 64-bit integer from a high / low pair of 32-bit integers.
constexpr inline uint64_t Make_64(uint32_t High, uint32_t Low) {
  return ((uint64_t)High << 32) | (uint64_t)Low;
}

/// Return true if the argument is a non-empty sequence of ones starting at the
/// least significant bit with the remainder zero (32 bit version).
/// Ex. isMask_32(0x0000FFFFU) == true.
constexpr inline bool isMask_32(uint32_t Value) {
  return Value && ((Value + 1) & Value) == 0;
}

/// Return true if the argument is a non-empty sequence of ones starting at the
/// least significant bit with the remainder zero (64 bit version).
constexpr inline bool isMask_64(uint64_t Value) {
  return Value && ((Value + 1) & Value) == 0;
}

/// Return true if the argument contains a non-empty sequence of ones with the
/// remainder zero (32 bit version.) Ex. isShiftedMask_32(0x0000FF00U) == true.
constexpr inline bool isShiftedMask_32(uint32_t Value) {
  return Value && isMask_32((Value - 1) | Value);
}

/// Return true if the argument contains a non-empty sequence of ones with the
/// remainder zero (64 bit version.)
constexpr inline bool isShiftedMask_64(uint64_t Value) {
  return Value && isMask_64((Value - 1) | Value);
}

/// Return true if the argument is a power of two > 0.
/// Ex. isPowerOf2_32(0x00100000U) == true (32 bit edition.)
constexpr inline bool isPowerOf2_32(uint32_t Value) {
  return Value && !(Value & (Value - 1));
}

/// Return true if the argument is a power of two > 0 (64 bit edition.)
constexpr inline bool isPowerOf2_64(uint64_t Value) {
  return Value && !(Value & (Value - 1));
}

/// Count the number of ones from the most significant bit to the first
/// zero bit.
///
/// Ex. countLeadingOnes(0xFF0FFF00) == 8.
/// Only unsigned integral types are allowed.
///
/// @param ZB the behavior on an input of all ones. Only ZB_Width and
/// ZB_Undefined are valid arguments.
template <typename T>
unsigned countLeadingOnes(T Value, ZeroBehavior ZB = ZB_Width) {
  static_assert(std::numeric_limits<T>::is_integer &&
                    !std::numeric_limits<T>::is_signed,
                "Only unsigned integral types are allowed.");
  return countLeadingZeros<T>(~Value, ZB);
}

/// Count the number of ones from the least significant bit to the first
/// zero bit.
///
/// Ex. countTrailingOnes(0x00FF00FF) == 8.
/// Only unsigned integral types are allowed.
///
/// @param ZB the behavior on an input of all ones. Only ZB_Width and
/// ZB_Undefined are valid arguments.
template <typename T>
unsigned countTrailingOnes(T Value, ZeroBehavior ZB = ZB_Width) {
  static_assert(std::numeric_limits<T>::is_integer &&
                    !std::numeric_limits<T>::is_signed,
                "Only unsigned integral types are allowed.");
  return countTrailingZeros<T>(~Value, ZB);
}

/// Count the number of set bits in a value.
/// Ex. countPopulation(0xF000F000) = 8
/// Returns 0 if the word is zero.
template <typename T>
inline unsigned countPopulation(T Value) {
  static_assert(std::numeric_limits<T>::is_integer &&
                    !std::numeric_limits<T>::is_signed,
                "Only unsigned integral types are allowed.");

  return std::popcount<T>(Value);
}

/// This function takes a 64-bit integer and returns the bit equivalent double.
inline double BitsToDouble(uint64_t Bits) {
  double D;
  static_assert(sizeof(uint64_t) == sizeof(double), "Unexpected type sizes");
  memcpy(&D, &Bits, sizeof(Bits));
  return D;
}

/// This function takes a 32-bit integer and returns the bit equivalent float.
inline float BitsToFloat(uint32_t Bits) {
  float F;
  static_assert(sizeof(uint32_t) == sizeof(float), "Unexpected type sizes");
  memcpy(&F, &Bits, sizeof(Bits));
  return F;
}

/// This function takes a double and returns the bit equivalent 64-bit integer.
/// Note that copying doubles around changes the bits of NaNs on some hosts,
/// notably x86, so this routine cannot be used if these bits are needed.
inline uint64_t DoubleToBits(double Double) {
  uint64_t Bits;
  static_assert(sizeof(uint64_t) == sizeof(double), "Unexpected type sizes");
  memcpy(&Bits, &Double, sizeof(Double));
  return Bits;
}

/// This function takes a float and returns the bit equivalent 32-bit integer.
/// Note that copying floats around changes the bits of NaNs on some hosts,
/// notably x86, so this routine cannot be used if these bits are needed.
inline uint32_t FloatToBits(float Float) {
  uint32_t Bits;
  static_assert(sizeof(uint32_t) == sizeof(float), "Unexpected type sizes");
  memcpy(&Bits, &Float, sizeof(Float));
  return Bits;
}

/// Returns the next power of two (in 64-bits) that is strictly greater than A.
/// Returns zero on overflow.
inline uint64_t NextPowerOf2(uint64_t A) {
  A |= (A >> 1);
  A |= (A >> 2);
  A |= (A >> 4);
  A |= (A >> 8);
  A |= (A >> 16);
  A |= (A >> 32);
  return A + 1;
}

/// Sign-extend the number in the bottom B bits of X to a 32-bit integer.
/// Requires 0 < B <= 32.
template <unsigned B> constexpr inline int32_t SignExtend32(uint32_t X) {
  static_assert(B > 0, "Bit width can't be 0.");
  static_assert(B <= 32, "Bit width out of range.");
  return int32_t(X << (32 - B)) >> (32 - B);
}

/// Sign-extend the number in the bottom B bits of X to a 32-bit integer.
/// Requires 0 < B <= 32.
inline int32_t SignExtend32(uint32_t X, unsigned B) {
  assert(B > 0 && "Bit width can't be 0.");
  assert(B <= 32 && "Bit width out of range.");
  return int32_t(X << (32 - B)) >> (32 - B);
}

/// Sign-extend the number in the bottom B bits of X to a 64-bit integer.
/// Requires 0 < B <= 64.
template <unsigned B> constexpr inline int64_t SignExtend64(uint64_t x) {
  static_assert(B > 0, "Bit width can't be 0.");
  static_assert(B <= 64, "Bit width out of range.");
  return int64_t(x << (64 - B)) >> (64 - B);
}

/// Sign-extend the number in the bottom B bits of X to a 64-bit integer.
/// Requires 0 < B <= 64.
inline int64_t SignExtend64(uint64_t X, unsigned B) {
  assert(B > 0 && "Bit width can't be 0.");
  assert(B <= 64 && "Bit width out of range.");
  return int64_t(X << (64 - B)) >> (64 - B);
}

} // namespace bijou

#endif // BIJOU_SUPPORT_MATHEXTRAS_HPP
