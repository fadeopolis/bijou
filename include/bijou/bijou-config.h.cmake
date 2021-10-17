// bijou-config.h - bijou configuration
//
// Part of the bijou Project, under the Apache License v2.0 with LLVM Exceptions.
//
// This file is a modified copy of the file "include/llvm/Config/llvm-config.h.cmake" from the LLVM Project.
// Summary of modifications:
//   * Reformatted comment style in copyright header.
//   * Changed 'LLVM' to 'BIJOU'.
//   * Removed unused config flags.
//   * Changed C style comments to C++/doxygen style line comments.
//
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// This file enumerates variables from the BIJOU configuration so that they
// can be in exported headers and won't override package specific directives.

#ifndef BIJOU_CONFIG_H
#define BIJOU_CONFIG_H

/// Major version of the BIJOU API
#define BIJOU_VERSION_MAJOR ${BIJOU_VERSION_MAJOR}

/// Minor version of the BIJOU API
#define BIJOU_VERSION_MINOR ${BIJOU_VERSION_MINOR}

/// Patch version of the BIJOU API
#define BIJOU_VERSION_PATCH ${BIJOU_VERSION_PATCH}

/// BIJOU version string
#define BIJOU_VERSION_STRING "${PACKAGE_VERSION}"

/// Whether the header unistd.h is available.
#cmakedefine01 HAVE_UNISTD_H

#endif // BIJOU_CONFIG_H
