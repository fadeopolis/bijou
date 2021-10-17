// Compiler.hpp - Compiler abstraction support
//
// Part of the bijou Project, under the Apache License v2.0 with LLVM Exceptions.
//
// This file is a modified copy of the file "llvm/Support/Compiler.h" from the LLVM Project.
// Summary of modifications:
//   * Reformatted comment style in copyright header.
//   * Changed 'llvm' to 'bijou', and 'LLVM' to 'BIJOU'.
//   * Removed unused defines and most tests for specific compiler versions.
//   * Changed doxygen comments to consistenly use '@' command prefix.
//
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file
/// @brief
/// This file defines several macros, based on the current compiler.  This allows
/// use of compiler-specific features in a way that remains portable. This header
/// can be included from either C or C++.
///

#ifndef BIJOU_SUPPORT_COMPILER_HPP
#define BIJOU_SUPPORT_COMPILER_HPP

#ifndef __has_attribute
# define __has_attribute(x) 0
#endif

#ifndef __has_builtin
# define __has_builtin(x) 0
#endif

// Prior to clang 3.2, clang did not accept any spelling of
// __has_attribute(const), so assume it is supported.
#if defined(__clang__) || defined(__GNUC__)
// aka 'CONST' but following bijou Conventions.
#define BIJOU_READNONE __attribute__((__const__))
#else
#define BIJOU_READNONE
#endif

#if __has_attribute(pure) || defined(__GNUC__)
// aka 'PURE' but following BIJOU Conventions.
#define BIJOU_READONLY __attribute__((__pure__))
#else
#define BIJOU_READONLY
#endif

#if __has_builtin(__builtin_expect)
#define BIJOU_LIKELY(EXPR) __builtin_expect((bool)(EXPR), true)
#define BIJOU_UNLIKELY(EXPR) __builtin_expect((bool)(EXPR), false)
#else
#define BIJOU_LIKELY(EXPR) (EXPR)
#define BIJOU_UNLIKELY(EXPR) (EXPR)
#endif

/// BIJOU_BUILTIN_UNREACHABLE - On compilers which support it, expands
/// to an expression which states that it is undefined behavior for the
/// compiler to reach this point.  Otherwise is not defined.
#if __has_builtin(__builtin_unreachable)
# define BIJOU_BUILTIN_UNREACHABLE __builtin_unreachable()
#elif defined(_MSC_VER)
# define BIJOU_BUILTIN_UNREACHABLE __assume(false)
#endif

#if __has_attribute(used)
#define BIJOU_ATTRIBUTE_USED __attribute__((__used__))
#else
#define BIJOU_ATTRIBUTE_USED
#endif

/// BIJOU_ATTRIBUTE_NOINLINE - On compilers where we have a directive to do so,
/// mark a method "not for inlining".
#if __has_attribute(noinline)
#define BIJOU_ATTRIBUTE_NOINLINE __attribute__((noinline))
#elif defined(_MSC_VER)
#define BIJOU_ATTRIBUTE_NOINLINE __declspec(noinline)
#else
#define BIJOU_ATTRIBUTE_NOINLINE
#endif

/// Mark debug helper function definitions like dump() that should not be
/// stripped from builds.
#define BIJOU_DUMP_METHOD BIJOU_ATTRIBUTE_NOINLINE BIJOU_ATTRIBUTE_USED

#endif // BIJOU_SUPPORT_COMPILER_HPP
