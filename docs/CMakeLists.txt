# Part of the bijou Project, under the Apache License v2.0 with LLVM Exceptions.
#
# This file is a modified copy of the file "docs/CMakeLists.txt" from the LLVM Project.
# Summary of modifications:
#   * Added copyright header.
#   * Changed 'llvm' to 'bijou', and 'LLVM' to 'BIJOU'.
#   * Removed unused config flags and code, like ocamldoc and Sphinx support.
#
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

if(BIJOU_ENABLE_DOXYGEN)
  message(STATUS "Doxygen enabled.")
  find_package(Doxygen REQUIRED)

  set(abs_top_srcdir   ${CMAKE_CURRENT_SOURCE_DIR})
  set(abs_top_builddir ${CMAKE_CURRENT_BINARY_DIR})

  find_program(DOT dot REQUIRED)

  if (BIJOU_DOXYGEN_EXTERNAL_SEARCH)
    set(enable_searchengine "YES")
    set(searchengine_url "${BIJOU_DOXYGEN_SEARCHENGINE_URL}")
    set(enable_server_based_search "YES")
    set(enable_external_search "YES")
    set(extra_search_mappings "${BIJOU_DOXYGEN_SEARCH_MAPPINGS}")
  else()
    set(enable_searchengine "NO")
    set(searchengine_url "")
    set(enable_server_based_search "NO")
    set(enable_external_search "NO")
    set(extra_search_mappings "")
  endif()

  option(BIJOU_DOXYGEN_SVG
    "Use svg instead of png files for doxygen graphs." OFF)
  if (BIJOU_DOXYGEN_SVG)
    set(DOT_IMAGE_FORMAT "svg")
  else()
    set(DOT_IMAGE_FORMAT "png")
  endif()

  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)

  add_custom_target(doxygen
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating bijou doxygen documentation." VERBATIM)

  if (NOT BIJOU_INSTALL_TOOLCHAIN_ONLY)
    # ./ suffix is needed to copy the contents of html directory without
    # appending html/ into BIJOU_INSTALL_DOXYGEN_HTML_DIR.
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doxygen/html/.
      COMPONENT doxygen-html
      DESTINATION "${BIJOU_INSTALL_DOXYGEN_HTML_DIR}")
  endif()
endif()
