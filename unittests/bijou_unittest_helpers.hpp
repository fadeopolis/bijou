// bijou_unittest_helpers.hpp - Helper functions for unittests
//
// Part of the bijou Project, under the Apache License v2.0 with LLVM Exceptions.
//
// This file contains modified copies code of the files "include/llvm/ADT/StringExtras.h", and
// "include/llvm/ADT/STLExtras.h" from the LLVM Project.
// Summary of modifications:
//   * Copied definitions of llvm::array_lengthof, llvm::utostr, and llvm::utohexstr.
//   * Changed 'LLVM' to 'BIJOU'.
//   * Changed doxygen comments to consistenly use '@' command prefix.
//
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <string>  // for std::string
#include <cstddef> // for std::size_t

namespace bijou {

/// Find the length of an array.
template <class T, std::size_t N>
constexpr inline size_t array_lengthof(T (&)[N]) {
  return N;
}

/// hexdigit - Return the hexadecimal character for the
/// given number @p X (which should be less than 16).
inline char hexdigit(unsigned X, bool LowerCase = false) {
  const char HexChar = LowerCase ? 'a' : 'A';
  return X < 10 ? '0' + X : HexChar + X - 10;
}

/// Convert an unsigned int to a string.
inline std::string utostr(uint64_t X, bool isNeg = false) {
  char Buffer[21];
  char *BufPtr = std::end(Buffer);

  if (X == 0) *--BufPtr = '0';  // Handle special case...

  while (X) {
    *--BufPtr = '0' + char(X % 10);
    X /= 10;
  }

  if (isNeg) *--BufPtr = '-';   // Add negative sign...
  return std::string(BufPtr, std::end(Buffer));
}

inline std::string utohexstr(uint64_t X, bool LowerCase = false) {
  char Buffer[17];
  char *BufPtr = std::end(Buffer);

  if (X == 0) *--BufPtr = '0';

  while (X) {
    unsigned char Mod = static_cast<unsigned char>(X) & 15;
    *--BufPtr = hexdigit(Mod, LowerCase);
    X >>= 4;
  }

  return std::string(BufPtr, std::end(Buffer));
}

} // end namespace bijou
