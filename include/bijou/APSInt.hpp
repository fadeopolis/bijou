// APSInt.hpp - Arbitrary Precision Signed Int
//
// Part of the bijou Project, under the Apache License v2.0 with LLVM Exceptions.
//
// This file is a modified copy of the file "llvm/ADT/APSInt.h" from the LLVM Project.
// Summary of modifications:
//   * Reformatted comment style in copyright header.
//   * Changed 'llvm' to 'bijou', and 'LLVM' to 'BIJOU'.
//   * Replaced uses of LLVM helper APIs (such as StringRef, ArrayRef,
//     raw_iostream) with standard C++ APIs (string_view, span, iostream).
//   * Replaced some LLVM helper macros with (such as LLVM_NODISCARD) with
//     standard C++ attributes (such as [[nodiscard]]).
//   * Removed uses of some LLVM helper APIs (such as FoldingSetNode, DenseMap)
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

#ifndef BIJOU_ADT_APSINT_HPP
#define BIJOU_ADT_APSINT_HPP

#include <string_view> // for std::string_view

#include "bijou/bijou-config.h" // for BIJOU_USE_IOSTREAM

#ifdef BIJOU_USE_IOSTREAM
#  include <iostream>
#endif

#include "bijou/APInt.hpp"

namespace bijou {

/// An arbitrary precision integer that knows its signedness.
class [[nodiscard]] APSInt : public APInt {
  bool IsUnsigned;

public:
  /// Default constructor that creates an uninitialized APInt.
  explicit APSInt() : IsUnsigned(false) {}

  /// Create an APSInt with the specified width, default to unsigned.
  explicit APSInt(uint32_t BitWidth, bool isUnsigned = true)
   : APInt(BitWidth, 0), IsUnsigned(isUnsigned) {}

  explicit APSInt(APInt I, bool isUnsigned = true)
   : APInt(std::move(I)), IsUnsigned(isUnsigned) {}

  /// Construct an APSInt from a string representation.
  ///
  /// This constructor interprets the string @p Str using the radix of 10.
  /// The interpretation stops at the end of the string. The bit width of the
  /// constructed APSInt is determined automatically.
  ///
  /// @param Str the string to be interpreted.
  explicit APSInt(std::string_view Str);

  /// Determine sign of this APSInt.
  ///
  /// @returns true if this APSInt is negative, false otherwise
  bool isNegative() const { return isSigned() && APInt::isNegative(); }

  /// Determine if this APSInt Value is non-negative (>= 0)
  ///
  /// @returns true if this APSInt is non-negative, false otherwise
  bool isNonNegative() const { return !isNegative(); }

  /// Determine if this APSInt Value is positive.
  ///
  /// This tests if the value of this APSInt is positive (> 0). Note
  /// that 0 is not a positive value.
  ///
  /// @returns true if this APSInt is positive.
  bool isStrictlyPositive() const { return isNonNegative() && !isZero(); }

  APSInt &operator=(APInt RHS) {
    // Retain our current sign.
    APInt::operator=(std::move(RHS));
    return *this;
  }

  APSInt &operator=(uint64_t RHS) {
    // Retain our current sign.
    APInt::operator=(RHS);
    return *this;
  }

  // Query sign information.
  bool isSigned() const { return !IsUnsigned; }
  bool isUnsigned() const { return IsUnsigned; }
  void setIsUnsigned(bool Val) { IsUnsigned = Val; }
  void setIsSigned(bool Val) { IsUnsigned = !Val; }

  /// Append this APSInt to the specified SmallString.
  void toString(std::string &Str, unsigned Radix = 10) const {
    APInt::toString(Str, Radix, isSigned());
  }
  using APInt::toString;

  /// Get the correctly-extended @c int64_t value.
  int64_t getExtValue() const {
    assert(getMinSignedBits() <= 64 && "Too many bits for int64_t");
    return isSigned() ? getSExtValue() : getZExtValue();
  }

  APSInt trunc(uint32_t width) const {
    return APSInt(APInt::trunc(width), IsUnsigned);
  }

  APSInt extend(uint32_t width) const {
    if (IsUnsigned)
      return APSInt(zext(width), IsUnsigned);
    else
      return APSInt(sext(width), IsUnsigned);
  }

  APSInt extOrTrunc(uint32_t width) const {
    if (IsUnsigned)
      return APSInt(zextOrTrunc(width), IsUnsigned);
    else
      return APSInt(sextOrTrunc(width), IsUnsigned);
  }

  const APSInt &operator%=(const APSInt &RHS) {
    assert(IsUnsigned == RHS.IsUnsigned && "Signedness mismatch!");
    if (IsUnsigned)
      *this = urem(RHS);
    else
      *this = srem(RHS);
    return *this;
  }
  const APSInt &operator/=(const APSInt &RHS) {
    assert(IsUnsigned == RHS.IsUnsigned && "Signedness mismatch!");
    if (IsUnsigned)
      *this = udiv(RHS);
    else
      *this = sdiv(RHS);
    return *this;
  }
  APSInt operator%(const APSInt &RHS) const {
    assert(IsUnsigned == RHS.IsUnsigned && "Signedness mismatch!");
    return IsUnsigned ? APSInt(urem(RHS), true) : APSInt(srem(RHS), false);
  }
  APSInt operator/(const APSInt &RHS) const {
    assert(IsUnsigned == RHS.IsUnsigned && "Signedness mismatch!");
    return IsUnsigned ? APSInt(udiv(RHS), true) : APSInt(sdiv(RHS), false);
  }

  APSInt operator>>(unsigned Amt) const {
    return IsUnsigned ? APSInt(lshr(Amt), true) : APSInt(ashr(Amt), false);
  }
  APSInt& operator>>=(unsigned Amt) {
    if (IsUnsigned)
      lshrInPlace(Amt);
    else
      ashrInPlace(Amt);
    return *this;
  }

  inline bool operator<(const APSInt& RHS) const {
    assert(IsUnsigned == RHS.IsUnsigned && "Signedness mismatch!");
    return IsUnsigned ? ult(RHS) : slt(RHS);
  }
  inline bool operator>(const APSInt& RHS) const {
    assert(IsUnsigned == RHS.IsUnsigned && "Signedness mismatch!");
    return IsUnsigned ? ugt(RHS) : sgt(RHS);
  }
  inline bool operator<=(const APSInt& RHS) const {
    assert(IsUnsigned == RHS.IsUnsigned && "Signedness mismatch!");
    return IsUnsigned ? ule(RHS) : sle(RHS);
  }
  inline bool operator>=(const APSInt& RHS) const {
    assert(IsUnsigned == RHS.IsUnsigned && "Signedness mismatch!");
    return IsUnsigned ? uge(RHS) : sge(RHS);
  }
  inline bool operator==(const APSInt& RHS) const {
    assert(IsUnsigned == RHS.IsUnsigned && "Signedness mismatch!");
    return eq(RHS);
  }
  inline bool operator!=(const APSInt& RHS) const {
    return !((*this) == RHS);
  }

  bool operator==(int64_t RHS) const {
    return compareValues(*this, get(RHS)) == 0;
  }
  bool operator!=(int64_t RHS) const {
    return compareValues(*this, get(RHS)) != 0;
  }
  bool operator<=(int64_t RHS) const {
    return compareValues(*this, get(RHS)) <= 0;
  }
  bool operator>=(int64_t RHS) const {
    return compareValues(*this, get(RHS)) >= 0;
  }
  bool operator<(int64_t RHS) const {
    return compareValues(*this, get(RHS)) < 0;
  }
  bool operator>(int64_t RHS) const {
    return compareValues(*this, get(RHS)) > 0;
  }

  // The remaining operators just wrap the logic of APInt, but retain the
  // signedness information.

  APSInt operator<<(unsigned Bits) const {
    return APSInt(static_cast<const APInt&>(*this) << Bits, IsUnsigned);
  }
  APSInt& operator<<=(unsigned Amt) {
    static_cast<APInt&>(*this) <<= Amt;
    return *this;
  }

  APSInt& operator++() {
    ++(static_cast<APInt&>(*this));
    return *this;
  }
  APSInt& operator--() {
    --(static_cast<APInt&>(*this));
    return *this;
  }
  APSInt operator++(int) {
    return APSInt(++static_cast<APInt&>(*this), IsUnsigned);
  }
  APSInt operator--(int) {
    return APSInt(--static_cast<APInt&>(*this), IsUnsigned);
  }
  APSInt operator-() const {
    return APSInt(-static_cast<const APInt&>(*this), IsUnsigned);
  }
  APSInt& operator+=(const APSInt& RHS) {
    assert(IsUnsigned == RHS.IsUnsigned && "Signedness mismatch!");
    static_cast<APInt&>(*this) += RHS;
    return *this;
  }
  APSInt& operator-=(const APSInt& RHS) {
    assert(IsUnsigned == RHS.IsUnsigned && "Signedness mismatch!");
    static_cast<APInt&>(*this) -= RHS;
    return *this;
  }
  APSInt& operator*=(const APSInt& RHS) {
    assert(IsUnsigned == RHS.IsUnsigned && "Signedness mismatch!");
    static_cast<APInt&>(*this) *= RHS;
    return *this;
  }
  APSInt& operator&=(const APSInt& RHS) {
    assert(IsUnsigned == RHS.IsUnsigned && "Signedness mismatch!");
    static_cast<APInt&>(*this) &= RHS;
    return *this;
  }
  APSInt& operator|=(const APSInt& RHS) {
    assert(IsUnsigned == RHS.IsUnsigned && "Signedness mismatch!");
    static_cast<APInt&>(*this) |= RHS;
    return *this;
  }
  APSInt& operator^=(const APSInt& RHS) {
    assert(IsUnsigned == RHS.IsUnsigned && "Signedness mismatch!");
    static_cast<APInt&>(*this) ^= RHS;
    return *this;
  }

  APSInt operator&(const APSInt& RHS) const {
    assert(IsUnsigned == RHS.IsUnsigned && "Signedness mismatch!");
    return APSInt(static_cast<const APInt&>(*this) & RHS, IsUnsigned);
  }

  APSInt operator|(const APSInt& RHS) const {
    assert(IsUnsigned == RHS.IsUnsigned && "Signedness mismatch!");
    return APSInt(static_cast<const APInt&>(*this) | RHS, IsUnsigned);
  }

  APSInt operator^(const APSInt &RHS) const {
    assert(IsUnsigned == RHS.IsUnsigned && "Signedness mismatch!");
    return APSInt(static_cast<const APInt&>(*this) ^ RHS, IsUnsigned);
  }

  APSInt operator*(const APSInt& RHS) const {
    assert(IsUnsigned == RHS.IsUnsigned && "Signedness mismatch!");
    return APSInt(static_cast<const APInt&>(*this) * RHS, IsUnsigned);
  }
  APSInt operator+(const APSInt& RHS) const {
    assert(IsUnsigned == RHS.IsUnsigned && "Signedness mismatch!");
    return APSInt(static_cast<const APInt&>(*this) + RHS, IsUnsigned);
  }
  APSInt operator-(const APSInt& RHS) const {
    assert(IsUnsigned == RHS.IsUnsigned && "Signedness mismatch!");
    return APSInt(static_cast<const APInt&>(*this) - RHS, IsUnsigned);
  }
  APSInt operator~() const {
    return APSInt(~static_cast<const APInt&>(*this), IsUnsigned);
  }

  /// Return the APSInt representing the maximum integer value with the given
  /// bit width and signedness.
  static APSInt getMaxValue(uint32_t numBits, bool Unsigned) {
    return APSInt(Unsigned ? APInt::getMaxValue(numBits)
                           : APInt::getSignedMaxValue(numBits), Unsigned);
  }

  /// Return the APSInt representing the minimum integer value with the given
  /// bit width and signedness.
  static APSInt getMinValue(uint32_t numBits, bool Unsigned) {
    return APSInt(Unsigned ? APInt::getMinValue(numBits)
                           : APInt::getSignedMinValue(numBits), Unsigned);
  }

  /// Determine if two APSInts have the same value, zero- or
  /// sign-extending as needed.
  static bool isSameValue(const APSInt &I1, const APSInt &I2) {
    return !compareValues(I1, I2);
  }

  /// Compare underlying values of two numbers.
  static int compareValues(const APSInt &I1, const APSInt &I2) {
    if (I1.getBitWidth() == I2.getBitWidth() && I1.isSigned() == I2.isSigned())
      return I1.IsUnsigned ? I1.compare(I2) : I1.compareSigned(I2);

    // Check for a bit-width mismatch.
    if (I1.getBitWidth() > I2.getBitWidth())
      return compareValues(I1, I2.extend(I1.getBitWidth()));
    if (I2.getBitWidth() > I1.getBitWidth())
      return compareValues(I1.extend(I2.getBitWidth()), I2);

    // We have a signedness mismatch. Check for negative values and do an
    // unsigned compare if both are positive.
    if (I1.isSigned()) {
      assert(!I2.isSigned() && "Expected signed mismatch");
      if (I1.isNegative())
        return -1;
    } else {
      assert(I2.isSigned() && "Expected signed mismatch");
      if (I2.isNegative())
        return 1;
    }

    return I1.compare(I2);
  }

  static APSInt get(int64_t X) { return APSInt(APInt(64, X), false); }
  static APSInt getUnsigned(uint64_t X) { return APSInt(APInt(64, X), true); }

  /// Overload to compute a hash code for an APSInt value.
  friend std::hash<APSInt>;
};

inline bool operator==(int64_t V1, const APSInt &V2) { return V2 == V1; }
inline bool operator!=(int64_t V1, const APSInt &V2) { return V2 != V1; }
inline bool operator<=(int64_t V1, const APSInt &V2) { return V2 >= V1; }
inline bool operator>=(int64_t V1, const APSInt &V2) { return V2 <= V1; }
inline bool operator<(int64_t V1, const APSInt &V2) { return V2 > V1; }
inline bool operator>(int64_t V1, const APSInt &V2) { return V2 < V1; }

#ifdef BIJOU_USE_IOSTREAM
inline std::iostream &operator<<(std::iostream &OS, const APSInt &I) {
  I.print(OS, I.isSigned());
  return OS;
}
#endif

} // namespace bijou

namespace std {

template <> struct hash<bijou::APSInt> {
  std::size_t operator()(const bijou::APSInt &Arg) const;
};

} // namespace std

#endif // BIJOU_ADT_APSINT_HPP
