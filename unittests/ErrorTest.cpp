// ErrorTest.cpp - Error and Expected unit tests
//
// Part of the bijou Project, under the Apache License v2.0 with LLVM Exceptions.
//
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <bijou/Error.hpp>
#include <gtest/gtest.h>

using bijou::Error;
using bijou::Expected;

namespace {

static Error createError(std::string_view Err) {
  return Error(Err);
}

static Expected<int> createExpectedValue(int Val) {
  return Val;
}

static Expected<int> createExpectedError(Error &&Err) {
  return std::move(Err);
}

TEST(ErrorTest, DefaultCtor) {
  Error Err;

  EXPECT_FALSE(Err);
}

TEST(ErrorTest, ErrorCtor) {
  Error Err("an error occurred");

  EXPECT_TRUE(Err);
}

TEST(ErrorTest, Success) {
  Error Err = Error::success();

  EXPECT_FALSE(Err);
}

TEST(ErrorTest, Take) {
  Error Err1("ERROR!");
  Error Err2(Err1.take());

  EXPECT_FALSE(Err1);
  EXPECT_TRUE(Err2);
}

TEST(ExpectedTest, ValueCtor) {
  Expected<int> Exp = createExpectedValue(42);

  EXPECT_TRUE(Exp);
}

TEST(ExpectedTest, ErrorCtor) {
  Expected<int> Exp = createExpectedError(createError("ERROR!"));

  EXPECT_FALSE(Exp);
}

TEST(ExpectedTest, TakeErr) {
  Expected<int> Exp = createExpectedError(createError("ERROR!"));

  EXPECT_FALSE(Exp);

  Error Err = Exp.takeError();

  EXPECT_TRUE(Err);
}

TEST(ExpectedTest, TakeVal) {
  Expected<int> Exp = createExpectedValue(42);

  EXPECT_TRUE(Exp);

  Error Err = Exp.takeError();

  EXPECT_FALSE(Err);
}

} // end anonymous namespace
