// APFixedPoint.hpp - Fixed point constant handling
//
// Part of the bijou Project, under the Apache License v2.0 with LLVM Exceptions.
//
// This file is a modified copy of the file "include/llvm/ADT/APFixedPoint.h" from the LLVM Project.
// Summary of modifications:
//   * Reformatted comment style in copyright header.
//   * Changed 'llvm' to 'bijou', and 'LLVM' to 'BIJOU'.
//   * Replaced uses of LLVM helper APIs (such as StringRef, ArrayRef,
//     raw_iostream) with standard C++ APIs (string_view, span, iostream).
//   * Replaced some LLVM helper macros with (such as LLVM_NODISCARD) with
//     standard C++ attributes (such as [[nodiscard]]).
//   * Removed unused LLVM helper APIs (such as FoldingSetNode, DenseMap)
//   * Added APIs to print classes defined in this file with C stdio routines.
//   * Changed doxygen comments to consistenly use '@' command prefix.
//
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file
/// @brief
/// Defines the fixed point number interface.
/// This is a class for abstracting various operations performed on fixed point
/// types.
///

#ifndef BIJOU_ADT_APFIXEDPOINT_HPP
#define BIJOU_ADT_APFIXEDPOINT_HPP

#include "bijou/bijou-config.h" // for BIJOU_USE_IOSTREAM

#ifdef BIJOU_USE_IOSTREAM
#include <iostream>
#endif

#include <cassert>           // for assert
#include <cstdint>           // for uint64_t
#include <string>            // for basic_string
#include "bijou/APInt.hpp"   // for APInt
#include "bijou/APSInt.hpp"  // for APSInt

namespace bijou {

class APFloat;
struct fltSemantics;

/// The fixed point semantics work similarly to fltSemantics. The width
/// specifies the whole bit width of the underlying scaled integer (with padding
/// if any). The scale represents the number of fractional bits in this type.
/// When HasUnsignedPadding is true and this type is unsigned, the first bit
/// in the value this represents is treated as padding.
class FixedPointSemantics {
public:
  FixedPointSemantics(unsigned Width, unsigned Scale, bool IsSigned,
                      bool IsSaturated, bool HasUnsignedPadding)
      : Width(Width), Scale(Scale), IsSigned(IsSigned),
        IsSaturated(IsSaturated), HasUnsignedPadding(HasUnsignedPadding) {
    assert(Width >= Scale && "Not enough room for the scale");
    assert(!(IsSigned && HasUnsignedPadding) &&
           "Cannot have unsigned padding on a signed type.");
  }

  unsigned getWidth() const { return Width; }
  unsigned getScale() const { return Scale; }
  bool isSigned() const { return IsSigned; }
  bool isSaturated() const { return IsSaturated; }
  bool hasUnsignedPadding() const { return HasUnsignedPadding; }

  void setSaturated(bool Saturated) { IsSaturated = Saturated; }

  /// Return the number of integral bits represented by these semantics. These
  /// are separate from the fractional bits and do not include the sign or
  /// padding bit.
  unsigned getIntegralBits() const {
    if (IsSigned || (!IsSigned && HasUnsignedPadding))
      return Width - Scale - 1;
    else
      return Width - Scale;
  }

  /// Return the FixedPointSemantics that allows for calculating the full
  /// precision semantic that can precisely represent the precision and ranges
  /// of both input values. This does not compute the resulting semantics for a
  /// given binary operation.
  FixedPointSemantics
  getCommonSemantics(const FixedPointSemantics &Other) const;

  /// Returns true if this fixed-point semantic with its value bits interpreted
  /// as an integer can fit in the given floating point semantic without
  /// overflowing to infinity.
  /// For example, a signed 8-bit fixed-point semantic has a maximum and
  /// minimum integer representation of 127 and -128, respectively. If both of
  /// these values can be represented (possibly inexactly) in the floating
  /// point semantic without overflowing, this returns true.
  bool fitsInFloatSemantics(const fltSemantics &FloatSema) const;

  /// Return the FixedPointSemantics for an integer type.
  static FixedPointSemantics GetIntegerSemantics(unsigned Width,
                                                 bool IsSigned) {
    return FixedPointSemantics(Width, /*Scale=*/0, IsSigned,
                               /*IsSaturated=*/false,
                               /*HasUnsignedPadding=*/false);
  }

private:
  unsigned Width          : 16;
  unsigned Scale          : 13;
  unsigned IsSigned       : 1;
  unsigned IsSaturated    : 1;
  unsigned HasUnsignedPadding : 1;
};

/// The APFixedPoint class works similarly to APInt/APSInt in that it is a
/// functional replacement for a scaled integer. It is meant to replicate the
/// fixed point types proposed in ISO/IEC JTC1 SC22 WG14 N1169. The class carries
/// info about the fixed point type's width, sign, scale, and saturation, and
/// provides different operations that would normally be performed on fixed point
/// types.
class APFixedPoint {
public:
  APFixedPoint(const APInt &Val, const FixedPointSemantics &Sema)
      : Val(Val, !Sema.isSigned()), Sema(Sema) {
    assert(Val.getBitWidth() == Sema.getWidth() &&
           "The value should have a bit width that matches the Sema width");
  }

  APFixedPoint(uint64_t Val, const FixedPointSemantics &Sema)
      : APFixedPoint(APInt(Sema.getWidth(), Val, Sema.isSigned()), Sema) {}

  // Zero initialization.
  APFixedPoint(const FixedPointSemantics &Sema) : APFixedPoint(0, Sema) {}

  APSInt getValue() const { return APSInt(Val, !Sema.isSigned()); }
  inline unsigned getWidth() const { return Sema.getWidth(); }
  inline unsigned getScale() const { return Sema.getScale(); }
  inline bool isSaturated() const { return Sema.isSaturated(); }
  inline bool isSigned() const { return Sema.isSigned(); }
  inline bool hasPadding() const { return Sema.hasUnsignedPadding(); }
  FixedPointSemantics getSemantics() const { return Sema; }

  bool getBoolValue() const { return Val.getBoolValue(); }

  // Convert this number to match the semantics provided. If the overflow
  // parameter is provided, set this value to true or false to indicate if this
  // operation results in an overflow.
  APFixedPoint convert(const FixedPointSemantics &DstSema,
                       bool *Overflow = nullptr) const;

  // Perform binary operations on a fixed point type. The resulting fixed point
  // value will be in the common, full precision semantics that can represent
  // the precision and ranges of both input values. See convert() for an
  // explanation of the Overflow parameter.
  APFixedPoint add(const APFixedPoint &Other, bool *Overflow = nullptr) const;
  APFixedPoint sub(const APFixedPoint &Other, bool *Overflow = nullptr) const;
  APFixedPoint mul(const APFixedPoint &Other, bool *Overflow = nullptr) const;
  APFixedPoint div(const APFixedPoint &Other, bool *Overflow = nullptr) const;

  // Perform shift operations on a fixed point type. Unlike the other binary
  // operations, the resulting fixed point value will be in the original
  // semantic.
  APFixedPoint shl(unsigned Amt, bool *Overflow = nullptr) const;
  APFixedPoint shr(unsigned Amt, bool *Overflow = nullptr) const {
    // Right shift cannot overflow.
    if (Overflow)
      *Overflow = false;
    return APFixedPoint(Val >> Amt, Sema);
  }

  /// Perform a unary negation (-X) on this fixed point type, taking into
  /// account saturation if applicable.
  APFixedPoint negate(bool *Overflow = nullptr) const;

  /// Return the integral part of this fixed point number, rounded towards
  /// zero. (-2.5k -> -2)
  APSInt getIntPart() const {
    if (Val < 0 && Val != -Val) // Cover the case when we have the min val
      return -(-Val >> getScale());
    else
      return Val >> getScale();
  }

  /// Return the integral part of this fixed point number, rounded towards
  /// zero. The value is stored into an APSInt with the provided width and sign.
  /// If the overflow parameter is provided, and the integral value is not able
  /// to be fully stored in the provided width and sign, the overflow parameter
  /// is set to true.
  APSInt convertToInt(unsigned DstWidth, bool DstSign,
                      bool *Overflow = nullptr) const;

  /// Convert this fixed point number to a floating point value with the
  /// provided semantics.
  APFloat convertToFloat(const fltSemantics &FloatSema) const;

  void toString(std::string &Str) const;
  std::string toString() const {
    std::string S;
    toString(S);
    return S;
  }

  // If LHS > RHS, return 1. If LHS == RHS, return 0. If LHS < RHS, return -1.
  int compare(const APFixedPoint &Other) const;
  bool operator==(const APFixedPoint &Other) const {
    return compare(Other) == 0;
  }
  bool operator!=(const APFixedPoint &Other) const {
    return compare(Other) != 0;
  }
  bool operator>(const APFixedPoint &Other) const { return compare(Other) > 0; }
  bool operator<(const APFixedPoint &Other) const { return compare(Other) < 0; }
  bool operator>=(const APFixedPoint &Other) const {
    return compare(Other) >= 0;
  }
  bool operator<=(const APFixedPoint &Other) const {
    return compare(Other) <= 0;
  }

  static APFixedPoint getMax(const FixedPointSemantics &Sema);
  static APFixedPoint getMin(const FixedPointSemantics &Sema);

  /// Given a floating point semantic, return the next floating point semantic
  /// with a larger exponent and larger or equal mantissa.
  static const fltSemantics *promoteFloatSemantics(const fltSemantics *S);

  /// Create an APFixedPoint with a value equal to that of the provided integer,
  /// and in the same semantics as the provided target semantics. If the value
  /// is not able to fit in the specified fixed point semantics, and the
  /// overflow parameter is provided, it is set to true.
  static APFixedPoint getFromIntValue(const APSInt &Value,
                                      const FixedPointSemantics &DstFXSema,
                                      bool *Overflow = nullptr);

  /// Create an APFixedPoint with a value equal to that of the provided
  /// floating point value, in the provided target semantics. If the value is
  /// not able to fit in the specified fixed point semantics and the overflow
  /// parameter is specified, it is set to true.
  /// For NaN, the Overflow flag is always set. For +inf and -inf, if the
  /// semantic is saturating, the value saturates. Otherwise, the Overflow flag
  /// is set.
  static APFixedPoint getFromFloatValue(const APFloat &Value,
                                        const FixedPointSemantics &DstFXSema,
                                        bool *Overflow = nullptr);

private:
  APSInt Val;
  FixedPointSemantics Sema;
};

#ifdef BIJOU_USE_IOSTREAM
inline std::ostream &operator<<(std::ostream &OS, const APFixedPoint &FX) {
  OS << FX.toString();
  return OS;
}
#endif

} // namespace bijou

#endif // BIJOU_ADT_APFIXEDPOINT_HPP
