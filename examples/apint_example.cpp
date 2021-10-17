// apint_example.cpp - APInt example
//
// Part of the bijou Project, under the Apache License v2.0 with LLVM Exceptions.
//
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <bijou/APInt.hpp>
#include <cstdio>

using namespace bijou;

int main() {
  const APInt A{6, 52}; // 6-bit int holding the value 52
  const APInt B{6, 13}; // 6-bit int holding the value 13

  {
    APInt C = A + B;

    printf("(%s + %s) %% 64 = %s\n",
           A.toStringUnsigned().c_str(),
           B.toStringUnsigned().c_str(),
           C.toStringUnsigned().c_str());
  }

  {
    APInt C = A - B;

    printf("(%s - %s) %% 64 = %s\n",
           A.toStringUnsigned().c_str(),
           B.toStringUnsigned().c_str(),
           C.toStringUnsigned().c_str());
  }

  {
    APInt C = A * B;

    printf("(%s * %s) %% 64 = %s\n",
           A.toStringUnsigned().c_str(),
           B.toStringUnsigned().c_str(),
           C.toStringUnsigned().c_str());
  }

  {
    APInt C = APIntOps::RoundingUDiv(A, B, APInt::Rounding::DOWN);

    printf("(%s / %s) %% 64 = %s\n",
           A.toStringUnsigned().c_str(),
           B.toStringUnsigned().c_str(),
           C.toStringUnsigned().c_str());
  }


  {
    APInt C = A - (B * APIntOps::RoundingUDiv(A, B, APInt::Rounding::DOWN));

    printf("(%s %% %s) %% 64 = %s\n",
           A.toStringUnsigned().c_str(),
           B.toStringUnsigned().c_str(),
           C.toStringUnsigned().c_str());
  }
}
