// APSInt.cpp - Arbitrary Precision Signed Int
//
// Part of the bijou Project, under the Apache License v2.0 with LLVM Exceptions.
//
// This file is a modified copy of the file "lib/Support/APSInt.cpp" from the LLVM Project.
// Summary of modifications:
//   * Reformatted comment style in copyright header.
//   * Changed 'llvm' to 'bijou', and 'LLVM' to 'BIJOU'.
//   * Replaced uses of LLVM helper APIs (such as StringRef, ArrayRef,
//     raw_iostream) with standard C++ APIs (string_view, span, iostream).
//   * Removed unused LLVM helper APIs (such as FoldingSetNode, DenseMap)
//   * Added APIs to print classes defined in this file with C stdio routines.
//   * Changed doxygen comments to consistenly use '@' command prefix.
//
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file
/// @brief
/// This file implements the APSInt class, which is a simple class that
/// represents an arbitrary sized integer that knows its signedness.
///

#include "bijou/APSInt.hpp"

#include <cassert>
#include <string_view> // for std::string_view

using namespace bijou;

APSInt::APSInt(std::string_view Str) {
  assert(!Str.empty() && "Invalid string length");

  // (Over-)estimate the required number of bits.
  unsigned NumBits = ((Str.size() * 64) / 19) + 2;
  APInt Tmp(NumBits, Str, /*radix=*/10);
  if (Str[0] == '-') {
    unsigned MinBits = Tmp.getMinSignedBits();
    if (MinBits < NumBits)
      Tmp = Tmp.trunc(std::max<unsigned>(1, MinBits));
    *this = APSInt(Tmp, /*isUnsigned=*/false);
    return;
  }
  unsigned ActiveBits = Tmp.getActiveBits();
  if (ActiveBits < NumBits)
    Tmp = Tmp.trunc(std::max<unsigned>(1, ActiveBits));
  *this = APSInt(Tmp, /*isUnsigned=*/true);
}
