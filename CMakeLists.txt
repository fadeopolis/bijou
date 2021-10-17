# Part of the bijou Project, under the Apache License v2.0 with LLVM Exceptions.
#
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

cmake_minimum_required(VERSION 3.19)

project(bijou LANGUAGES CXX)

if(NOT DEFINED BIJOU_VERSION_MAJOR)
  set(BIJOU_VERSION_MAJOR 0)
endif()
if(NOT DEFINED BIJOU_VERSION_MINOR)
  set(BIJOU_VERSION_MINOR 0)
endif()
if(NOT DEFINED BIJOU_VERSION_PATCH)
  set(BIJOU_VERSION_PATCH 0)
endif()
if(NOT DEFINED BIJOU_VERSION_SUFFIX)
  set(BIJOU_VERSION_SUFFIX git)
endif()

if (NOT PACKAGE_VERSION)
  set(PACKAGE_VERSION
    "${BIJOU_VERSION_MAJOR}.${BIJOU_VERSION_MINOR}.${BIJOU_VERSION_PATCH}${BIJOU_VERSION_SUFFIX}")
endif()


################################################################################

include(CheckCXXCompilerFlag)
include(CheckIncludeFileCXX)

option(BIJOU_ENABLE_WERROR   "Enable unit tests" ON)
option(BIJOU_ENABLE_IWYU     "Run include-what-you-use during build" OFF)
option(BIJOU_ENABLE_TESTS    "Enable unit tests" OFF)
option(BIJOU_ENABLE_EXAMPLES "Build example programs" OFF)
option(BIJOU_ENABLE_DOXYGEN  "Build doxygen docs" OFF)

if(BIJOU_ENABLE_WERROR)
  check_cxx_compiler_flag("-Wall -Werror" HAS_WERROR)

  if(HAS_WERROR)
    add_compile_options(-Wall -Werror)
  endif()

  check_cxx_compiler_flag("/WX /W4" HAS_WX)

  if(HAS_WX)
    add_compile_options(/WX /W4)
  endif()
endif()

if(BIJOU_ENABLE_IWYU)
  if (NOT CMAKE_CXX_INCLUDE_WHAT_YOU_USE)
    find_program(IWYU NAMES include-what-you-use iwyu REQUIRED)

    set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE "${IWYU}")
  endif()
endif()


################################################################################

check_include_file_cxx("unistd.h" HAVE_UNISTD_H)

configure_file(
  include/bijou/bijou-config.h.cmake
  ${CMAKE_BINARY_DIR}/include/bijou/bijou-config.h)

################################################################################

function(add_libijou NAME TYPE)
  set(BIJOU_HEADERS
    ${CMAKE_BINARY_DIR}/include/bijou/bijou-config.h
    include/bijou/APFixedPoint.hpp
    include/bijou/APFloat.hpp
    include/bijou/APInt.hpp
    include/bijou/APSInt.hpp
    include/bijou/Compiler.hpp
    include/bijou/Error.hpp
    include/bijou/FloatingPointMode.hpp
    include/bijou/Hashing.hpp
    include/bijou/MathExtras.hpp
    include/bijou/SwapByteOrder.hpp
  )

  set(BIJOU_SOURCES
      lib/bijou/APFixedPoint.cpp
      lib/bijou/APFloat.cpp
      lib/bijou/APInt.cpp
      lib/bijou/APSInt.cpp
      lib/bijou/Error.cpp
      lib/bijou/Hashing.cpp
      ${BIJOU_HEADERS}
  )

  add_library("${NAME}" "${TYPE}" ${BIJOU_SOURCES})

  set_property(TARGET "${NAME}" PROPERTY PUBLIC_HEADER ${BIJOU_HEADERS})

  target_include_directories("${NAME}" PUBLIC
      # The location of the headers before installation
      $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
      $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
      # The location of the headers after installation
      $<INSTALL_INTERFACE:include>
  )

  target_compile_features("${NAME}" PUBLIC cxx_std_20)
  set_property(TARGET "${NAME}" PROPERTY CXX_STANDARD_REQUIRED TRUE)

  install(TARGETS "${NAME}"
          PUBLIC_HEADER DESTINATION include/bijou)
endfunction(add_libijou)

add_libijou(bijou-static STATIC)
add_libijou(bijou-shared SHARED)

if(BUILD_SHARED_LIBS)
  add_library(bijou ALIAS bijou-shared)
else()
  add_library(bijou ALIAS bijou-static)
endif()

if(BIJOU_ENABLE_EXAMPLES)
  add_subdirectory(examples)
endif()


################################################################################
### tests

if(BIJOU_ENABLE_TESTS)
  include(FetchContent)
  include(GoogleTest)

  enable_testing()

  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        e2239ee6043f73722e7aa812a459f54a28552929 # v1.11.0
  )
  # For Windows: Prevent overriding the parent project's compiler/linker settings
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)

  add_executable(
    bijou_unittests
    unittests/APFixedPointTest.cpp
    unittests/APFloatTest.cpp
    unittests/APIntTest.cpp
    unittests/APSIntTest.cpp
    unittests/ErrorTest.cpp
    unittests/bijou_unittest_helpers.hpp
  )
  target_link_libraries(bijou_unittests bijou gtest gtest_main)

  gtest_discover_tests(bijou_unittests)
endif()


################################################################################
### documentation

add_subdirectory(docs)
