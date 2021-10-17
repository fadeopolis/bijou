// Error.hpp - Error handling
//
// Part of the bijou Project, under the Apache License v2.0 with LLVM Exceptions.
//
// This file contains modified copies code of the files "include/llvm/Support/Error.h", and
// "include/llvm/Support/ErrorHandling.h" from the LLVM Project.
// Summary of modifications:
//   * Reformatted comment style in copyright header.
//   * Copied declarations of report_fatal_error, and llvm_unreachable_internal,
//     and the definition of llvm_unreachable.
//     Copied a very stripped down version of llvm::Error and llvm::Expect.
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
/// This file defines APIs used to handle recoverable errors and report fatal errors.
///
/// @TODO use std::source_location instead of __FILE__ and __LINE__.

#ifndef BIJOU_SUPPORT_ERROR_HPP
#define BIJOU_SUPPORT_ERROR_HPP

#include <bit>                // for std::bit_cast
#include <cassert>            // for assert
#include <optional>           // for std::optional
#include <string>             // for std::string
#include <string_view>        // for std::string_view
#include <type_traits>        // for std::aligned_union_t
#include <variant>            // for std::variant, std::monostate
#include "bijou/Compiler.hpp" // for BIJOU_BUILTIN_UNREACHABLE

namespace bijou {

/// Reports a serious error, calling any installed error handler. These
/// functions are intended to be used for error conditions which are outside
/// the control of the compiler (I/O errors, invalid user input, etc.)
///
/// If no error handler is installed the default is to print the message to
/// standard error, followed by a newline.
/// After the error handler is called this function will call exit(1), it
/// does not return.
void report_fatal_error[[noreturn]](const char *reason,
                                    bool gen_crash_diag = true);
void report_fatal_error[[noreturn]](const std::string &reason,
                                    bool gen_crash_diag = true);
void report_fatal_error[[noreturn]](std::string_view reason,
                                    bool gen_crash_diag = true);

/// This function calls abort(), and prints the optional message to stderr.
/// Use the bijou_unreachable macro (that adds location info), instead of
/// calling this function directly.
void bijou_unreachable_internal[[noreturn]](const char *msg = nullptr,
                                           const char *file = nullptr,
                                           unsigned line = 0);

////////////////////////////////////////////////////////////////////////////////
/// very simplified version of llvm/Support/Error.h

struct [[nodiscard]] Error {
  Error() {}

  explicit Error(std::optional<std::string_view> Err) : Err{Err} {}

  Error(Error &&Other) {
    std::swap(this->Err, Other.Err);
  }

  Error(const Error &Err) = delete;

  static Error success() { return Error(); }

  bool has_error() const {
    return Err.has_value();
  }

  explicit operator bool() const {
    return has_error();
  }

  std::optional<std::string_view> take() {
    auto Out = Err;
    Err.reset();
    return Out;
  }
private:
  std::optional<std::string_view> Err;
};

template<typename T>
struct [[nodiscard]] Expected {
private:
  using error_type = std::string_view;
public:
  Expected(const T &t) : Data{t} {
  }
  Expected(Error &&Err) {
    assert(Err && "Cannot create Expected<T> from Error success value.");
    Data = *Err.take();
  }

  Expected(Expected &&Other) : Data{std::move(Other.Data)} {
    Other.Data = std::monostate();
  }

  Expected(const Expected &) = delete;

  const T &operator *() const {
    assert(std::holds_alternative<T>(Data));
    return get<T>(Data);
  }

  Error takeError() {
    assert(!std::holds_alternative<std::monostate>(Data) && "Already taken");

    if (const std::string_view *Ptr = std::get_if<std::string_view>(&Data)) {
      std::string_view Out = *Ptr;
      Data = std::monostate();
      return Error(Out);
    } else {
      return Error::success();
    }
  }

  bool has_value() const {
    return bool(*this);
  }

  bool has_error() const {
    return !bool(*this);
  }

  explicit operator bool() const {
    return std::holds_alternative<T>(Data);
  }
private:
  std::variant<
    std::monostate,
    T,
    std::string_view
  > Data;
};

static inline void consumeError(Error &&Err) {
  Err.take();
}

} // namespace bijou

/// Marks that the current location is not supposed to be reachable.
/// In !NDEBUG builds, prints the message and location info to stderr.
/// In NDEBUG builds, becomes an optimizer hint that the current location
/// is not supposed to be reachable.  On compilers that don't support
/// such hints, prints a reduced message instead.
///
/// Use this instead of assert(0).  It conveys intent more clearly and
/// allows compilers to omit some unnecessary code.
#ifndef NDEBUG
#define bijou_unreachable(msg) ::bijou::bijou_unreachable_internal(msg, __FILE__, __LINE__)
#elif defined(BIJOU_BUILTIN_UNREACHABLE)
#define bijou_unreachable(msg) BIJOU_BUILTIN_UNREACHABLE
#else
#define bijou_unreachable(msg) ::bijou::bijou_unreachable_internal()
#endif

#endif // BIJOU_SUPPORT_ERROR_HPP
