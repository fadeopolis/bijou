//===- llvm/Support/FloatingPointMode.h -------------------------*- C++ -*-===//
//
// Part of the bijou Project, under the Apache License v2.0 with LLVM Exceptions.
//
// This file is a modified copy of the file "include/llvm/Support/FloatingPointMode.h" from the LLVM Project.
// Summary of modifications:
//   * Reformatted comment style in copyright header.
//   * Changed 'llvm' to 'bijou', and 'LLVM' to 'BIJOU'.
//   * Replaced uses of LLVM helper APIs (such as StringRef, ArrayRef,
//     raw_iostream) with standard C++ APIs
//     (string_view, span, iostream).
//   * Replaced some LLVM helper macros with (such as LLVM_NODISCARD) with
//     standard C++ attributes (such as [[nodiscard]]).
//   * Removed uses of some LLVM helper APIs (such as FoldingSetNode, DenseMap)
//   * Changed doxygen comments to consistenly use '@' command prefix.
//
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file
/// @brief
/// Utilities for dealing with flags related to floating point mode controls.
///

#ifndef BIJOU_ADT_FLOATINGPOINTMODE_HPP
#define BIJOU_ADT_FLOATINGPOINTMODE_HPP

#include <cstdio>      // for FILE
#include <string_view> // for std::string_view
#include <tuple>       // for std::tie

#include "bijou/bijou-config.h" // for BIJOU_USE_IOSTREAM

#ifdef BIJOU_USE_IOSTREAM
#include <iostream>
#endif

namespace bijou {

/// Rounding mode.
///
/// Enumerates supported rounding modes, as well as some special values. The set
/// of the modes must agree with IEEE-754, 4.3.1 and 4.3.2. The constants
/// assigned to the IEEE rounding modes must agree with the values used by
/// FLT_ROUNDS (C11, 5.2.4.2.2p8).
///
/// This value is packed into bitfield in some cases, including @c FPOptions, so
/// the rounding mode values and the special value @c Dynamic must fit into the
/// the bit field (now - 3 bits). The value @c Invalid is used only in values
/// returned by intrinsics to indicate errors, it should never be stored as
/// rounding mode value, so it does not need to fit the bit fields.
///
enum class RoundingMode : int8_t {
  // Rounding mode defined in IEEE-754.
  TowardZero        = 0,    ///< roundTowardZero.
  NearestTiesToEven = 1,    ///< roundTiesToEven.
  TowardPositive    = 2,    ///< roundTowardPositive.
  TowardNegative    = 3,    ///< roundTowardNegative.
  NearestTiesToAway = 4,    ///< roundTiesToAway.

  // Special values.
  Dynamic = 7,    ///< Denotes mode unknown at compile time.
  Invalid = -1    ///< Denotes invalid value.
};

/// Returns text representation of the given rounding mode.
inline std::string_view spell(RoundingMode RM) {
  switch (RM) {
  case RoundingMode::TowardZero: return "towardzero";
  case RoundingMode::NearestTiesToEven: return "tonearest";
  case RoundingMode::TowardPositive: return "upward";
  case RoundingMode::TowardNegative: return "downward";
  case RoundingMode::NearestTiesToAway: return "tonearestaway";
  case RoundingMode::Dynamic: return "dynamic";
  default: return "invalid";
  }
}

#ifdef BIJOU_USE_IOSTREAM
inline std::ostream &operator<<(std::ostream &OS, RoundingMode RM) {
  OS << spell(RM);
  return OS;
}
#endif

/// Represent subnormal handling kind for floating point instruction inputs and
/// outputs.
struct DenormalMode {
  /// Represent handled modes for denormal (aka subnormal) modes in the floating
  /// point environment.
  enum DenormalModeKind : int8_t {
    Invalid = -1,

    /// IEEE-754 denormal numbers preserved.
    IEEE,

    /// The sign of a flushed-to-zero number is preserved in the sign of 0
    PreserveSign,

    /// Denormals are flushed to positive zero.
    PositiveZero
  };

  /// Denormal flushing mode for floating point instruction results in the
  /// default floating point environment.
  DenormalModeKind Output = DenormalModeKind::Invalid;

  /// Denormal treatment kind for floating point instruction inputs in the
  /// default floating-point environment. If this is not DenormalModeKind::IEEE,
  /// floating-point instructions implicitly treat the input value as 0.
  DenormalModeKind Input = DenormalModeKind::Invalid;

  constexpr DenormalMode() = default;
  constexpr DenormalMode(DenormalModeKind Out, DenormalModeKind In) :
    Output(Out), Input(In) {}


  static constexpr DenormalMode getInvalid() {
    return DenormalMode(DenormalModeKind::Invalid, DenormalModeKind::Invalid);
  }

  static constexpr DenormalMode getIEEE() {
    return DenormalMode(DenormalModeKind::IEEE, DenormalModeKind::IEEE);
  }

  static constexpr DenormalMode getPreserveSign() {
    return DenormalMode(DenormalModeKind::PreserveSign,
                        DenormalModeKind::PreserveSign);
  }

  static constexpr DenormalMode getPositiveZero() {
    return DenormalMode(DenormalModeKind::PositiveZero,
                        DenormalModeKind::PositiveZero);
  }

  bool operator==(DenormalMode Other) const {
    return Output == Other.Output && Input == Other.Input;
  }

  bool operator!=(DenormalMode Other) const {
    return !(*this == Other);
  }

  bool isSimple() const {
    return Input == Output;
  }

  bool isValid() const {
    return Output != DenormalModeKind::Invalid &&
           Input != DenormalModeKind::Invalid;
  }

#ifdef BIJOU_USE_IOSTREAM
  inline void print(std::iostream &OS) const;
#endif
  inline void print(FILE *O) const;

  inline std::string str() const;
};

#ifdef BIJOU_USE_IOSTREAM
inline std::ostream& operator<<(std::ostream &OS, DenormalMode Mode) {
  Mode.print(OS);
  return OS;
}
#endif

/// Parse the expected names from the denormal-fp-math attribute.
inline DenormalMode::DenormalModeKind
parseDenormalFPAttributeComponent(std::string_view Str) {
  // Assume ieee on unspecified attribute.
  if ((Str == "") || (Str == "ieee")) { return DenormalMode::IEEE; }
  if (Str == "preserve-sign") { return DenormalMode::PreserveSign; }
  if (Str == "positive-zero") { return DenormalMode::PositiveZero; }

  return DenormalMode::Invalid;
}

/// Return the name used for the denormal handling mode used by the the
/// expected names from the denormal-fp-math attribute.
inline std::string_view denormalModeKindName(DenormalMode::DenormalModeKind Mode) {
  switch (Mode) {
  case DenormalMode::IEEE:
    return "ieee";
  case DenormalMode::PreserveSign:
    return "preserve-sign";
  case DenormalMode::PositiveZero:
    return "positive-zero";
  default:
    return "";
  }
}

/// Returns the denormal mode to use for inputs and outputs.
inline DenormalMode parseDenormalFPAttribute(std::string_view Str) {
  std::string_view OutputStr, InputStr;

  const size_t Idx = Str.find_first_of(',');
  if (Idx == std::string_view::npos) {
    OutputStr = Str;
  } else {
    OutputStr = Str.substr(0, Idx);
    InputStr  = Str.substr(Idx + 1);
  }

  DenormalMode Mode;
  Mode.Output = parseDenormalFPAttributeComponent(OutputStr);

  // Maintain compatability with old form of the attribute which only specified
  // one component.
  Mode.Input = InputStr.empty() ? Mode.Output  :
               parseDenormalFPAttributeComponent(InputStr);

  return Mode;
}

#ifdef BIJOU_USE_IOSTREAM
void DenormalMode::print(std::ostream &OS) const {
  OS << denormalModeKindName(Output) << ',' << denormalModeKindName(Input);
}
#endif

void DenormalMode::print(FILE *O) const {
  const std::string_view OutputName = denormalModeKindName(Output);
  const std::string_view InputName  = denormalModeKindName(Input);

  fwrite(OutputName.data(), 1, OutputName.size(), O);
  fwrite(",", 1, 1, O);
  fwrite(InputName.data(), 1, InputName.size(), O);
}

inline std::string DenormalMode::str() const {
  std::string Buf;
  Buf += denormalModeKindName(Output);
  Buf += ',';
  Buf += denormalModeKindName(Input);
  return Buf;
}

} // namespace bijou

#endif // BIJOU_ADT_FLOATINGPOINTMODE_HPP
