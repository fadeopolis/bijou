// Error.cpp - Error handling
//
// Part of the bijou Project, under the Apache License v2.0 with LLVM Exceptions.
//
// This file contains modified copies code of the files "lib/Support/Error.cpp", and
// "lib/Support/ErrorHandling.cpp" from the LLVM Project.
// Summary of modifications:
//   * Reformatted comment style in copyright header.
//   * Copied stripped down definitions of report_fatal_error, and llvm_unreachable_internal.
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

#include "bijou/Error.hpp"
#include <cassert> // for assert
#include <string>  // for std::string
#include <cstdlib>

#include "bijou/bijou-config.h" // for HAVE_UNISTD_H

#if defined(HAVE_UNISTD_H)
# include <unistd.h> // for write
#endif
#if defined(_MSC_VER)
# include <io.h>     // for write
# include <fcntl.h>
#endif

using namespace bijou;

void bijou::report_fatal_error(const char *Reason, bool GenCrashDiag) {
  report_fatal_error(std::string_view(Reason), GenCrashDiag);
}

void bijou::report_fatal_error(const std::string &Reason, bool GenCrashDiag) {
  report_fatal_error(std::string_view(Reason), GenCrashDiag);
}

void bijou::report_fatal_error(std::string_view Reason, bool GenCrashDiag) {
  // Blast the result out to stderr.  We don't try hard to make sure this
  // succeeds (e.g. handling EINTR) and we can't use errs() here because
  // raw ostreams can call report_fatal_error.
  std::string Buffer;
  Buffer += "BIJOU ERROR: ";
  Buffer += Reason;
  Buffer += "\n";
  ssize_t written = ::write(2, Buffer.data(), Buffer.size());
  (void) written; // If something went wrong, we deliberately just give up.

  abort();
}

void bijou::bijou_unreachable_internal(const char *msg, const char *file,
                                     unsigned line) {
  // This code intentionally doesn't call the ErrorHandler callback, because
  // bijou_unreachable is intended to be used to indicate "impossible"
  // situations, and not legitimate runtime errors.
  std::string Buffer;
  if (msg) {
    Buffer += msg;
    Buffer += "\n";
  }
  Buffer += "UNREACHABLE executed";
  if (file) {
    Buffer += " at ";
    Buffer += file;
    Buffer += ":";
    Buffer += line;
  }
  Buffer += "!\n";
  ssize_t written = ::write(2, Buffer.data(), Buffer.size());
  (void)written; // If something went wrong, we deliberately just give up.

  abort();
#ifdef BIJOU_BUILTIN_UNREACHABLE
  // Windows systems and possibly others don't declare abort() to be noreturn,
  // so use the unreachable builtin to avoid a Clang self-host warning.
  BIJOU_BUILTIN_UNREACHABLE;
#endif
}
