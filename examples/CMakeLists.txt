# Part of the bijou Project, under the Apache License v2.0 with LLVM Exceptions.
#
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

cmake_minimum_required(VERSION 3.19)

################################################################################

function(add_example NAME)
  add_executable("${NAME}" "${NAME}.cpp")
  target_link_libraries("${NAME}" bijou)

  install(TARGETS "${NAME}"
          RUNTIME DESTINATION bijou/examples)
endfunction(add_example)

add_example(apint_example)
