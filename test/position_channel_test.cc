// Copyright 2017 Tommy Hinks
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#include <sstream>
#include <vector>

#include <gtest/gtest.h>

#include "../include/thinks/obj_io.h"

using std::begin;
using std::end;
using std::invalid_argument;
using std::stringstream;
using std::vector;
using thinks::obj_io::make_position_channel;


TEST(PositionChannelTest, CtorThrowsIfComponentCountIsZero)
{
  const auto components_per_value = uint32_t{ 3 };
  const auto indices_per_face = uint32_t{ 3 };
  try {
    const auto channel = make_position_channel(
      vector<float>{}, components_per_value,
      vector<uint32_t>{ 0, 1, 2 }, indices_per_face);
    FAIL() << "exception not thrown";
  }
  catch (const invalid_argument& ex) {
    auto ss = stringstream();
    ss << "element count (" << 0
      << ") must be a multiple of elements per object (" 
      << components_per_value << ")";
    EXPECT_STREQ(ss.str().c_str(), ex.what());
  }
  catch (...) {
    FAIL() << "incorrect exception";
  }
}

TEST(PositionChannelTest, CtorThrowsIfComponentsPerValueIsZero)
{
  const auto components_per_value = uint32_t{ 0 };
  const auto indices_per_face = uint32_t{ 3 };
  try {
    const auto channel = make_position_channel(
      vector<float>{ 0.1f, 0.1f }, components_per_value,
      vector<uint32_t>{ 0, 1, 2 }, indices_per_face);
  }
  catch (const invalid_argument& ex) {
    EXPECT_STREQ("elements per object cannot be zero", ex.what());
  }
  catch (...) {
    FAIL() << "incorrect exception";
  }
}

TEST(PositionChannelTest, CtorThrowsIfComponentCountNotMultipleOfComponentsPerValue)
{
  const auto components_per_value = uint32_t{ 3 };
  const auto indices_per_face = uint32_t{ 3 };
  try {
    const auto positions = make_position_channel(
      vector<float>{ 0.1f, 0.1f }, components_per_value,
      vector<uint32_t>{ 0, 1, 2 }, indices_per_face);
    FAIL() << "exception not thrown";
  }
  catch (const invalid_argument& ex) {
    auto ss = stringstream();
    ss << "element count (" << 2
      << ") must be a multiple of elements per object ("
      << components_per_value << ")";
    EXPECT_STREQ(ss.str().c_str(), ex.what());
  }
  catch (...) {
    FAIL() << "incorrect exception";
  }
}

TEST(PositionChannelTest, CtorThrowsIfComponentsPerValueIsNotThreeOrFour)
{
  const auto components_per_value = uint32_t{ 2 };
  const auto indices_per_face = uint32_t{ 3 };
  try {
    const auto channel = make_position_channel(
      vector<float>{ 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f }, components_per_value,
      vector<uint32_t>{ 0, 1, 2 }, indices_per_face);
    FAIL() << "exception not thrown";
  }
  catch (const invalid_argument& ex) {
    auto ss = stringstream();
    ss << "position components per value (" 
      << components_per_value << ") must be 3 or 4";
    EXPECT_STREQ(ss.str().c_str(), ex.what());
  }
  catch (...) {
    FAIL() << "incorrect exception";
  }
}

TEST(PositionChannelTest, CtorThrowsIfIndexCountIsZero)
{
  const auto components_per_value = uint32_t{ 3 };
  const auto indices_per_face = uint32_t{ 3 };
  try {
    const auto channel = make_position_channel(
      vector<float>{ 0.1f, 0.2f, 0.3f }, components_per_value,
      vector<uint32_t>{}, indices_per_face);
    FAIL() << "exception not thrown";
  }
  catch (const invalid_argument& ex) {
    auto ss = stringstream();
    ss << "element count (" << 0
      << ") must be a multiple of elements per object ("
      << indices_per_face << ")";
    EXPECT_STREQ(ss.str().c_str(), ex.what());
  }
  catch (...) {
    FAIL() << "incorrect exception";
  }
}

TEST(PositionChannelTest, CtorThrowsIfIndicesPerFaceIsLessThanThree)
{
  const auto components_per_value = uint32_t{ 3 };
  const auto indices_per_face = uint32_t{ 2 };
  try {
    const auto channel = make_position_channel(
      vector<float>{ 0.1f, 0.2f, 0.3f }, components_per_value,
      vector<uint32_t>{ 0, 1 }, indices_per_face);
    FAIL() << "exception not thrown";
  }
  catch (const invalid_argument& ex) {
    auto ss = std::stringstream();
    ss << "indices per face (" << indices_per_face
      << ") cannot be less than 3";
    EXPECT_STREQ(ss.str().c_str(), ex.what());
  }
  catch (...) {
    FAIL() << "incorrect exception";
  }
}

TEST(PositionChannelTest, CtorThrowsIfIndexCountNotMultipleOfIndicesPerFace)
{
  const auto components_per_value = uint32_t{ 3 };
  const auto indices_per_face = uint32_t{ 3 };
  try {
    const auto positions = make_position_channel(
      vector<float>{ 0.1f, 0.2f, 0.3f }, components_per_value,
      vector<uint32_t>{ 0, 0, 0, 0 }, indices_per_face);
    FAIL() << "exception not thrown";
  }
  catch (const invalid_argument& ex) {
    auto ss = stringstream();
    ss << "element count (" << 4
      << ") must be a multiple of elements per object ("
      << indices_per_face << ")";
    EXPECT_STREQ(ss.str().c_str(), ex.what());
  }
  catch (...) {
    FAIL() << "incorrect exception";
  }
}

TEST(PositionChannelTest, CtorThrowIfInvalidIndexRange_MinNotZero)
{
  const auto components_per_value = uint32_t{ 3 };
  const auto indices_per_face = uint32_t{ 3 };
  try {
    const auto channel = make_position_channel(
      vector<float>{
        0.1f, 0.2f, 0.3f,
        0.1f, 0.2f, 0.3f,
        0.1f, 0.2f, 0.3f,
        0.1f, 0.2f, 0.3f }, components_per_value,
      vector<uint32_t>{ 1, 2, 3 }, indices_per_face);
    FAIL() << "exception not thrown";
  }
  catch (const invalid_argument& ex) {
    auto ss = stringstream();
    ss << "min index (" << 1 << ") must be zero";
    EXPECT_STREQ(ss.str().c_str(), ex.what());
  }
  catch (...) {
    FAIL() << "incorrect exception";
  }
}

TEST(PositionChannelTest, CtorThrowIfInvalidIndexRange_MaxNotLessThanValueCount)
{
  const auto components_per_value = uint32_t{ 3 };
  const auto indices_per_face = uint32_t{ 3 };
  try {
    const auto channel = make_position_channel(
      vector<float>{
        0.1f, 0.2f, 0.3f,
        0.1f, 0.2f, 0.3f,
        0.1f, 0.2f, 0.3f }, components_per_value,
      vector<uint32_t>{ 0, 1, 3 }, indices_per_face);
    FAIL() << "exception not thrown";
  }
  catch (const invalid_argument& ex) {
    auto ss = stringstream();
    ss << "max index (" << 3
      << ") must be less than value count (" << 3 << ")";
    EXPECT_STREQ(ss.str().c_str(), ex.what());
  }
  catch (...) {
    FAIL() << "incorrect exception";
  }
}
