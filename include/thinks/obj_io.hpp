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

#ifndef THINKS_OBJ_IO_HPP_INCLUDED
#define THINKS_OBJ_IO_HPP_INCLUDED

#include <exception>
#include <iterator>
#include <ostream>
#include <string>
#include <type_traits>
#include <vector>

namespace thinks {
namespace obj_io {
namespace detail {

template<typename PosIter>
void ThrowIfPositionsEmpty(
  const PosIter pos_begin, const PosIter pos_end)
{
  using namespace std;

  if (distance(pos_begin, pos_end) == 0) {
    throw runtime_error("positions cannot be empty");
  }
}

template<typename IdxIter>
void ThrowIfIndicesEmpty(
  const IdxIter idx_begin, const IdxIter idx_end)
{
  using namespace std;

  if (distance(idx_begin, idx_end) == 0) {
    throw runtime_error("positions cannot be empty");
  }
}

template<uint32_t N, typename PosIter>
void ThrowIfPositionsNotMultipleOf(
  const PosIter pos_begin, const PosIter pos_end)
{
  using namespace std;

  const auto pos_count = distance(pos_begin, pos_end);
  if (pos_count % N != 0) {
    auto ss = stringstream();
    ss << "position element count must be a multiple of " 
      << N << ", was " << pos_count;
    throw runtime_error(ss.str());
  }
}

template<uint32_t N, typename TexIter>
void ThrowIfTextureCoordinatesNotMultipleOf(
  const TexIter tex_begin, const TexIter tex_end)
{
  using namespace std;

  const auto tex_count = distance(tex_begin, tex_end);
  if (tex_count % N != 0) {
    auto ss = stringstream();
    ss << "texture coordinate element count must be a multiple of "
      << N << ", was " << tex_count;
    throw runtime_error(ss.str());
  }
}

template<uint32_t N, typename IdxIter>
void ThrowIfIndicesNotMultipleOf(
  const IdxIter idx_begin, const IdxIter idx_end)
{
  using namespace std;

  const auto idx_count = distance(idx_begin, idx_end);
  if (idx_count % N != 0) {
    auto ss = stringstream();
    ss << "index element count must be a multiple of "
      << N << ", was " << idx_count;
    throw runtime_error(ss.str());
  }
}

template<typename T>
void ThrowIfTextureCoordinateElementOutOfRange(const T t)
{
  using namespace std;

  if (!(T(0) <= t && t <= T(1))) {
    auto ss = stringstream();
    ss << "texture coordinate element must be in [0..1], was: " << t;
    throw runtime_error(ss.str());
  }
}

template<typename Iter>
uint32_t Count(
  const Iter iter_begin,
  const Iter iter_end,
  const uint32_t element_count)
{
  using namespace std;
  return static_cast<uint32_t>(distance(iter_begin, iter_end) / element_count);
}

template<uint32_t N, typename AttrIter>
uint32_t WriteAttributes(
  std::ostream& os,
  const std::string& line_start,
  const AttrIter attr_begin, const AttrIter attr_end,
  const std::string& newline)
{
  // Note: not checking that range is multiple of N here.
  auto attr_count = uint32_t{ 0 };
  for (auto attr_iter = attr_begin; attr_iter != attr_end;) {
    os << line_start;
    for (uint32_t i = 0; i < N; ++i) {
      os << *attr_iter++ << (i != N - 1 ? ", " : "");
    }
    os << newline;
    ++attr_count;
  }
  return attr_count;
}

template<uint32_t N, typename PosIdxIter, typename TexIdxIter, typename NmlIdxIter>
uint32_t WriteIndices(
  std::ostream& os,
  //const std::string& line_start,
  const PosIdxIter pos_idx_begin, const PosIdxIter pos_idx_end,
  const TexIdxIter tex_idx_begin, const TexIdxIter tex_idx_end,
  const PosIdxIter nml_idx_begin, const PosIdxIter nml_idx_end,
  const std::string& newline)
{
  auto attr_count = uint32_t{ 0 };

}

void WriteHeader(
  std::ostream& os,
  const uint32_t vertex_count,
  const uint32_t face_count,
  const std::string& newline)
{
  os << "# Generated by https://github.com/thinks/obj-io" << newline
    << "# Vertex count: " << vertex_count << newline
    << "# Face count: " << face_count << newline;
}

template<uint32_t N, typename PosIter>
int64_t WritePositions(
  std::ostream& os,
  const PosIter pos_begin, const PosIter pos_end,
  const std::string& newline)
{
  using namespace std;

  typedef typename iterator_traits<PosIter>::value_type PosType;
  static_assert(is_arithmetic<PosType>::value,
    "position elements must have arithmetic type");
  static_assert(N == 3 || N == 4, 
    "position element count must be 3 or 4");

  ThrowIfPositionsEmpty(pos_begin, pos_end);
  ThrowIfPositionsNotMultipleOf<N>(pos_begin, pos_end);
  WriteValues<N>(os, "v: ", pos_begin, pos_end, newline, 
    [](const auto x) { return x; } );

  return distance(pos_begin, pos_end) / N;
}

template<uint32_t N, typename TexIter>
int64_t WriteTextureCoordinates(
  std::ostream& os,
  const TexIter tex_begin, const TexIter tex_end,
  const std::string& newline)
{
  using namespace std;

  typedef typename iterator_traits<TexIter>::value_type TexType;
  static_assert(is_floating_point<TexType>::value,
    "texture coordinate elements must have floating point type");
  static_assert(N == 2 || N == 3,
    "texture coordinate element count must be 2 or 3");

  // Texture coordinates are optional.
  if (tex_begin == tex_end) {
    return 0;
  }

  ThrowIfTextureCoordinatesNotMultipleOf<N>(tex_begin, tex_end);
  WriteValues<N>(os, "vt: ", tex_begin, tex_end, newline,
    [](const auto x) {
      ThrowIfTextureCoordinateElementOutOfRange(x);
      return x; 
    });

  return distance(tex_begin, tex_end) / N;
}

template<uint32_t N, typename IdxIter>
int64_t WriteIndices(
  std::ostream& os,
  const IdxIter idx_begin, const IdxIter idx_end,
  const std::string& newline)
{
  using namespace std;

  typedef typename iterator_traits<IdxIter>::value_type IdxType;
  static_assert(is_integral<IdxType>::value,
    "index elements must have integer type");
  static_assert(N >= 3,
    "index element count must be greater than or equal to 3");

  ThrowIfIndicesEmpty(idx_begin, idx_end);
  ThrowIfIndicesNotMultipleOf<N>(idx_begin, idx_end);
  // Write one-based indices.
  WriteValues<N>(os, "f: ", idx_begin, idx_end, newline,
    [](const auto x) { return x + 1; });

  return distance(idx_begin, idx_end) / N;
}


template<
  typename PosIter, typename PosIdxIter, 
  typename TexIter, typename TexIdxIter,
  typename NmlIter, typename NmlIdxIter>
std::ostream& Write(
  std::ostream& os,
  const PosIter pos_begin, const PosIter pos_end, 
  const uint32_t pos_element_count,
  const PosIdxIter pos_idx_begin, const PosIdxIter pos_idx_end,
  const uint32_t idx_element_count,
  const TexIter tex_begin, const TexIter tex_end,
  const uint32_t tex_element_count,
  const TexIdxIter tex_idx_begin, const TexIdxIter tex_idx_end,
  const NmlIter nml_begin, const NmlIter nml_end,
  const NmlIdxIter nml_idx_begin, const NmlIdxIter nml_idx_end,
  const std::string& newline)
{
  using namespace std;

  typedef typename iterator_traits<PosIter>::value_type PosType;
  typedef typename iterator_traits<PosIdxIter>::value_type PosIdxType;
  typedef typename iterator_traits<TexIter>::value_type TexType;
  typedef typename iterator_traits<TexIdxIter>::value_type TexIdxType;
  typedef typename iterator_traits<NmlIter>::value_type NmlType;
  typedef typename iterator_traits<NmlIdxIter>::value_type NmlIdxType;

  static_assert(is_arithmetic<PosType>::value,
    "position elements must have arithmetic type");
  // TODO - tex coords must be floating point
  // TODO - normals must be floating point


  static_assert(is_integral<PosIdxType>::value,
    "position index elements must have integer type");
  static_assert(is_integral<TexIdxType>::value,
    "texture coordinate index elements must have integer type");
  static_assert(is_integral<NmlIdxType>::value,
    "normal index elements must have integer type");


  const uint32_t nml_element_count = 3; // Always 3!



  // Positions
  ThrowIfPositionElementsEmpty(pos_begin, pos_end);
  // TODO - pos_element_count must be 3 or 4.
  ThrowIfPositionElementCountNotMultipleOf(pos_begin, pos_end, pos_element_count);

  ThrowIfPositionIndicesEmpty(pos_idx_begin, pos_idx_end);
  // TODO - position index element count must be >= 3
  ThrowIfPositionIndexElementCountNotMultipleOf(pos_idx_begin, pos_idx_end, idx_element_count);
  // TODO - check max index in position count range, min index must be zero.

  // Texture coordinates.
  // TODO - tex_element_count must be 2 or 3.
  ThrowIfTexCoordElementCountNotMultipleOf(
    tex_begin, tex_end, tex_element_count);
  ThrowIfTexCoordCountNotZeroOrEqualToPositionCount(
    pos_begin, pos_end, pos_element_count,
    tex_begin, tex_end, tex_element_count);
  ThrowIfTexCoordIndexElementCountNotMultipleOf(
    tex_idx_begin, tex_idx_end, idx_element_count);
  ThrowIfTexCoordIndexCountNotZeroOrEqualToPositionIndexCount(
    pos_idx_begin, pos_idx_end,
    tex_idx_begin, tex_idx_end, 
    idx_element_count);
  // TODO - throw if not all elements in [0..1]
  // TODO - check max index in texcoord count range, min index must be zero.

  // Normals.
  ThrowIfNormalElementCountNotMultipleOf(
    nml_begin, nml_end, nml_element_count);
  ThrowIfNormalCountNotZeroOrEqualToPositionCount(
    pos_begin, pos_end, pos_element_count,
    nml_begin, nml_end, tex_element_count);
  ThrowIfNormalIndexElementCountNotMultipleOf(
    nml_idx_begin, nml_idx_end, idx_element_count);
  ThrowIfNormalIndexCountNotZeroOrEqualToPositionIndexCount(
    pos_idx_begin, pos_idx_end,
    tex_idx_begin, tex_idx_end,
    idx_element_count);
  // TODO - check max index in normal count range, min index must be zero.

  const auto vtx_count = AttributeCount(
    pos_begin, pos_end, pos_element_count);
  const auto face_count = AttributeCount(
    pos_idx_begin, pos_idx_end, idx_element_count);

  WriteHeader(os, vtx_count, face_count, newline);
  WritePositions(os, pos_begin, pos_end, pos_element_count, newline);
  WriteTextureCoordinates(os, tex_begin, tex_end, tex_element_count, newline);
  WriteNormals(os, nml_begin, nml_end, nml_element_count, newline);
  //WriteParamSpace(...)
  WriteFaces(
    os, 
    pos_idx_begin, pos_idx_end,
    tex_idx_begin, tex_idx_end,
    nml_idx_begin, nml_idx_end,
    idx_element_count,
    newline);

  return os;
}

} // namespace detail

/// Input indices are zero-based.
template<
  typename PosIter, typename PosIdxIter,
  typename TexIter, typename TexIdxIter,
  typename NmlIter, typename NmlIdxIter>
std::ostream& WriteTrianglesWithTexCoordsAndNormals(
  std::ostream& os,
  const PosIter pos_begin, const PosIter pos_end,
  const PosIdxIter pos_idx_begin, const PosIdxIter pos_idx_end,
  const TexIter tex_begin, const TexIter tex_end, 
  const TexIdxIter tex_idx_begin, const PosIdxIter tex_idx_end,
  const NmlIter nml_begin, const NmlIter nml_end,
  const NmlIdxIter nml_idx_begin, const NmlIdxIter nml_idx_end,
  const std::string& newline = "\n")
{
  return detail::Write(
    os,
    pos_begin, pos_end, 3,
    pos_idx_begin, pos_idx_end, 3,
    tex_begin, tex_end, 2,
    tex_idx_begin, tex_idx_end,
    nml_begin, nml_end,
    nml_idx_begin, nml_idx_end,
    newline);
}

/// Input indices are zero-based.
template<
  typename PosIter, typename PosIdxIter,
  typename TexIter, typename TexIdxIter>
std::ostream& WriteTrianglesWithTexCoords(
  std::ostream& os,
  const PosIter pos_begin, const PosIter pos_end,
  const PosIdxIter pos_idx_begin, const PosIdxIter pos_idx_end,
  const TexIter tex_begin, const TexIter tex_end,
  const TexIdxIter tex_idx_begin, const TexIdxIter tex_idx_end,
  const std::string& newline = "\n")
{
  return detail::Write(
    os,
    pos_begin, pos_end, 3,
    pos_idx_begin, pos_idx_end, 3,
    tex_begin, tex_end, 2,
    tex_idx_begin, tex_idx_end,
    pos_end, pos_end, // Empty range!
    pos_idx_end, pos_idx_end, // Empty range!
    newline);
}

/// Input indices are zero-based.
template<
  typename PosIter, typename PosIdxIter,
  typename NmlIter, typename NmlIdxIter>
  std::ostream& WriteTrianglesWithNormals(
    std::ostream& os,
    const PosIter pos_begin, const PosIter pos_end,
    const PosIdxIter pos_idx_begin, const PosIdxIter pos_idx_end,
    const NmlIter nml_begin, const NmlIter nml_end,
    const NmlIdxIter nml_idx_begin, const NmlIdxIter nml_idx_end,
    const std::string& newline = "\n")
{
  return detail::Write(
    os,
    pos_begin, pos_end, 3,
    pos_idx_begin, pos_idx_end, 3,
    pos_end, pos_end, // Empty range!
    pos_idx_end, pos_idx_end, // Empty range!
    nml_begin, nml_end,
    nml_idx_begin, nml_idx_end,
    newline);
}

/// Input indices are zero-based.
template<typename PosIter, typename PosIdxIter>
std::ostream& WriteTriangles(
  std::ostream& os,
  const PosIter pos_begin, const PosIter pos_end,
  const PosIdxIter pos_idx_begin, const PosIdxIter pos_idx_end,
  const std::string& newline = "\n")
{
  return detail::Write(
    os,
    pos_begin, pos_end, 3,
    pos_idx_begin, pos_idx_end, 3,
    pos_end, pos_end, 0, // Empty range!
    pos_idx_end, pos_idx_end, // Empty range!
    pos_end, pos_end, // Empty range!
    pos_idx_end, pos_idx_end, // Empty range!
    newline);
}

} // namespace obj_io
} // namespace thinks

#endif // THINKS_OBJ_IO_HPP_INCLUDED
