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

#ifndef THINKS_OBJ_IO_H_INCLUDED
#define THINKS_OBJ_IO_H_INCLUDED

#include <algorithm>
#include <exception>
#include <iterator>
#include <ostream>
#include <string>
#include <type_traits>
#include <vector>

namespace thinks {
namespace obj_io {
#if 0
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
uint32_t AttributeCount(
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
    "position elements must be arithmetic");
  static_assert(is_floating_point<TexType>::value,
    "texture coordinate elements must be floating point");
  static_assert(is_floating_point<NmlType>::value,
    "normal elements must be floating point");

  // TODO - tex coords must be floating point
  // TODO - normals must be floating point

  static_assert(is_integral<PosIdxType>::value,
    "position index elements must be integer");
  static_assert(is_integral<TexIdxType>::value,
    "texture coordinate index elements must be integer");
  static_assert(is_integral<NmlIdxType>::value,
    "normal index elements must be integer");

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
  const uint32_t nml_element_count = 3; // Always 3!
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
#endif

template<typename ElementIter, typename IndexIter>
class Positions
{
public:
  Positions(
    const ElementIter elements_begin,
    const ElementIter elements_end,
    const IndexIter indices_begin,
    const IndexIter indices_end,
    const uint32_t elements_per_vertex,
    const uint32_t indices_per_face)
    : elements_begin(elements_begin)
    , elements_end(elements_end)
    , indices_begin(indices_begin)
    , indices_end(indices_end)
    , elements_per_vertex(elements_per_vertex)
    , indices_per_face(indices_per_face)
  {
    using namespace std;

    typedef typename iterator_traits<ElementIter>::value_type ElementType;
    typedef typename iterator_traits<IndexIter>::value_type IndexType;

    static_assert(is_arithmetic<ElementType>::value,
      "position elements must be arithmetic");
    static_assert(is_integral<IndexType>::value,
      "position index elements must be integral");

    // Elements.
    ThrowIfElementsEmpty_(elements_begin, elements_end);
    ThrowIfElementsPerVertexIsNotThreeOrFour_(elements_per_vertex);
    ThrowIfElementCountNotMultipleOfElementsPerVertex_(
      elements_begin, elements_end, elements_per_vertex);

    // Indices.
    ThrowIfIndicesEmpty_(indices_begin, indices_end);
    ThrowIfIndicesPerFaceIsLessThanThree_(indices_per_face);
    ThrowIfIndexCountNotMultipleOfIndicesPerFace_(
      indices_begin, indices_end, indices_per_face);
    ThrowIfInvalidIndexRange_(
      indices_begin, indices_end,
      elements_begin, elements_end, elements_per_vertex);
  }

  const ElementIter elements_begin;
  const ElementIter elements_end;
  const IndexIter indices_begin;
  const IndexIter indices_end;
  const uint32_t elements_per_vertex;
  const uint32_t indices_per_face;

private:
  static void ThrowIfElementsEmpty_(
    const ElementIter elements_begin, 
    const ElementIter elements_end)
  {
    using namespace std;
    if (elements_begin == elements_end) {
      throw runtime_error("position elements cannot be empty");
    }
  }

  static void ThrowIfElementsPerVertexIsNotThreeOrFour_(
    const uint32_t elements_per_vertex)
  {
    using namespace std;
    if (!(elements_per_vertex == 3 || elements_per_vertex == 4)) {
      auto ss = stringstream();
      ss << "position elements per vertex must be 3 or 4, was " 
        << elements_per_vertex;
      throw runtime_error(ss.str());
    }
  }

  static void ThrowIfElementCountNotMultipleOfElementsPerVertex_(
    const ElementIter elements_begin,
    const ElementIter elements_end,
    const uint32_t elements_per_vertex)
  {
    using namespace std;
    const auto element_count = distance(elements_begin, elements_end);
    if (element_count % elements_per_vertex != 0) {
      auto ss = stringstream();
      ss << "position element count (" << element_count 
        << ") must be a multiple of " << elements_per_vertex;
      throw runtime_error(ss.str());
    }
  }

  static void ThrowIfIndicesEmpty_(
    const IndexIter indices_begin,
    const IndexIter indices_end)
  {
    using namespace std;
    if (indices_begin == indices_end) {
      throw runtime_error("position indices cannot be empty");
    }
  }

  static void ThrowIfIndicesPerFaceIsLessThanThree_(
    const uint32_t indices_per_face)
  {
    using namespace std;
    if (!(indices_per_face >= 3)) {
      auto ss = stringstream();
      ss << "position indices per face must be " 
        << "greater than or equal to 3, was "
        << indices_per_face;
      throw runtime_error(ss.str());
    }
  }

  static void ThrowIfIndexCountNotMultipleOfIndicesPerFace_(
    const IndexIter indices_begin,
    const IndexIter indices_end,
    const uint32_t indices_per_face)
  {
    using namespace std;
    const auto index_count = distance(indices_begin, indices_end);
    if (index_count % indices_per_face != 0) {
      auto ss = stringstream();
      ss << "position index count (" << index_count
        << ") must be a multiple of " << indices_per_face;
      throw runtime_error(ss.str());
    }
  }

  static void ThrowIfInvalidIndexRange_(
    const IndexIter indices_begin,
    const IndexIter indices_end,
    const ElementIter elements_begin,
    const ElementIter elements_end,
    const uint32_t elements_per_vertex)
  {
    using namespace std;
    ThrowIfIndicesEmpty_(indices_begin, indices_end);
    const auto min_index = *min_element(indices_begin, indices_end);
    const auto max_index = *max_element(indices_begin, indices_end);
    if (min_index != 0) {
      auto ss = stringstream();
      ss << "min position index must be zero, was " << min_index;
      throw runtime_error(ss.str());
    }

    const auto vertex_count = 
      distance(elements_begin, elements_end) / elements_per_vertex;
    if (max_index >= vertex_count) {
      auto ss = stringstream();
      ss << "max position index must be less than vertex count ("
        << vertex_count << "), was " << max_index;
      throw runtime_error(ss.str());
    }
  }
};

/// Named constructor to help with template type deduction.
template<typename ElementIter, typename IndexIter> inline
Positions<ElementIter, IndexIter> make_positions(
  const ElementIter elements_begin,
  const ElementIter elements_end,
  const IndexIter indices_begin,
  const IndexIter indices_end,
  const uint32_t elements_per_vertex,
  const uint32_t indices_per_face)
{
  return Positions<ElementIter, IndexIter>(
    elements_begin, elements_end, 
    indices_begin, indices_end, 
    elements_per_vertex, indices_per_face);
}


template<typename ElementIter, typename IndexIter>
class TexCoords
{
public:
  TexCoords(
    const ElementIter elements_begin,
    const ElementIter elements_end,
    const IndexIter indices_begin,
    const IndexIter indices_end,
    const uint32_t elements_per_vertex,
    const uint32_t indices_per_face)
    : elements_begin(elements_begin)
    , elements_end(elements_end)
    , indices_begin(indices_begin)
    , indices_end(indices_end)
    , elements_per_vertex(elements_per_vertex)
    , indices_per_face(indices_per_face)
  {
    using namespace std;

    typedef typename iterator_traits<ElementIter>::value_type ElementType;
    typedef typename iterator_traits<IndexIter>::value_type IndexType;

    static_assert(is_floating_point<ElementType>::value,
      "tex coord elements must be floating point");
    static_assert(is_integral<IndexType>::value,
      "tex coord index elements must be integral");

    // Elements.
    //ThrowIfElementsEmpty_(elements_begin, elements_end);
    ThrowIfElementsPerVertexIsNotTwoOrThree_(elements_per_vertex);
    ThrowIfElementCountNotMultipleOfElementsPerVertex_(
      elements_begin, elements_end, elements_per_vertex);
    ThrowIfElementsNotNormalized_(
      elements_begin, elements_end);

    // Indices.
    //ThrowIfIndicesEmpty_(indices_begin, indices_end);
    ThrowIfIndicesPerFaceIsLessThanThree_(indices_per_face);
    ThrowIfIndexCountNotMultipleOfIndicesPerFace_(
      indices_begin, indices_end, indices_per_face);
    ThrowIfInvalidIndexRange_(
      indices_begin, indices_end,
      elements_begin, elements_end, elements_per_vertex);
  }

  const ElementIter elements_begin;
  const ElementIter elements_end;
  const IndexIter indices_begin;
  const IndexIter indices_end;
  const uint32_t elements_per_vertex;
  const uint32_t indices_per_face;

private:
  static void ThrowIfElementsPerVertexIsNotTwoOrThree_(
    const uint32_t elements_per_vertex)
  {
    using namespace std;
    if (!(elements_per_vertex == 2 || elements_per_vertex == 3)) {
      auto ss = stringstream();
      ss << "tex coord elements per vertex must be 2 or 3, was "
        << elements_per_vertex;
      throw runtime_error(ss.str());
    }
  }

  static void ThrowIfElementCountNotMultipleOfElementsPerVertex_(
    const ElementIter elements_begin,
    const ElementIter elements_end,
    const uint32_t elements_per_vertex)
  {
    using namespace std;
    const auto element_count = distance(elements_begin, elements_end);
    if (!(element_count % elements_per_vertex == 0)) {
      auto ss = stringstream();
      ss << "tex coord element count (" << element_count
        << ") must be a multiple of " << elements_per_vertex;
      throw runtime_error(ss.str());
    }
  }

  static void ThrowIfElementsNotNormalized_(
    const ElementIter elements_begin,
    const ElementIter elements_end)
  {
    using namespace std;
    typedef typename iterator_traits<ElementIter>::value_type ElementType;
    for_each(elements_begin, elements_end,
      [](const auto& e) {
        if (!(ElementType(0) <= e && e <= ElementType(1))) {
          auto ss = stringstream();
          ss << "tex coord elements must be in range [0, 1], found " << e;
          throw runtime_error(ss.str());
        }
      });
  }

  static void ThrowIfIndicesPerFaceIsLessThanThree_(
    const uint32_t indices_per_face)
  {
    using namespace std;
    if (!(indices_per_face >= 3)) {
      auto ss = stringstream();
      ss << "tex coord indices per face must be "
        << "greater than or equal to 3, was "
        << indices_per_face;
      throw runtime_error(ss.str());
    }
  }

  static void ThrowIfIndexCountNotMultipleOfIndicesPerFace_(
    const IndexIter indices_begin,
    const IndexIter indices_end,
    const uint32_t indices_per_face)
  {
    using namespace std;
    const auto index_count = distance(indices_begin, indices_end);
    if (index_count % indices_per_face != 0) {
      auto ss = stringstream();
      ss << "tex coord index count (" << index_count
        << ") must be a multiple of " << indices_per_face;
      throw runtime_error(ss.str());
    }
  }

  static void ThrowIfInvalidIndexRange_(
    const IndexIter indices_begin,
    const IndexIter indices_end,
    const ElementIter elements_begin,
    const ElementIter elements_end,
    const uint32_t elements_per_vertex)
  {
    using namespace std;
    //ThrowIfIndicesEmpty_(indices_begin, indices_end); // TODO!!
    const auto min_index = *min_element(indices_begin, indices_end);
    const auto max_index = *max_element(indices_begin, indices_end);
    if (min_index != 0) {
      auto ss = stringstream();
      ss << "min tex coord index must be zero, was " << min_index;
      throw runtime_error(ss.str());
    }

    const auto vertex_count =
      distance(elements_begin, elements_end) / elements_per_vertex;
    if (max_index >= vertex_count) {
      auto ss = stringstream();
      ss << "max tex coord index must be less than vertex count ("
        << vertex_count << "), was " << max_index;
      throw runtime_error(ss.str());
    }
  }
};

/// Named constructor to help with template type deduction.
template<typename ElementIter, typename IndexIter> inline
TexCoords<ElementIter, IndexIter> make_tex_coords(
  const ElementIter elements_begin,
  const ElementIter elements_end,
  const IndexIter indices_begin,
  const IndexIter indices_end,
  const uint32_t elements_per_vertex,
  const uint32_t indices_per_face)
{
  return TexCoords<ElementIter, IndexIter>(
    elements_begin, elements_end,
    indices_begin, indices_end,
    elements_per_vertex,
    indices_per_face);
}


template<typename ElementIter, typename IndexIter>
class Normals
{
public:
  Normals(
    const ElementIter elements_begin,
    const ElementIter elements_end,
    const IndexIter indices_begin,
    const IndexIter indices_end,
    const uint32_t indices_per_face)
    : elements_begin(elements_begin)
    , elements_end(elements_end)
    , indices_begin(indices_begin)
    , indices_end(indices_end)
    , elements_per_vertex(3)
    , indices_per_face(indices_per_face)
  {
    using namespace std;

    typedef typename iterator_traits<ElementIter>::value_type ElementType;
    typedef typename iterator_traits<IndexIter>::value_type IndexType;

    static_assert(is_floating_point<ElementType>::value,
      "normal elements must be floating point");
    static_assert(is_integral<IndexType>::value,
      "normal index elements must be integral");

    // Elements, can be empty.
    if (elements_begin != elements_end) {
      //ThrowIfElementsEmpty_(elements_begin, elements_end);
      ThrowIfElementsPerVertexIsNotThree_(elements_per_vertex);
      ThrowIfElementCountNotMultipleOfElementsPerVertex_(
        elements_begin, elements_end, elements_per_vertex);
    }

    // Indices, can be empty.
    //ThrowIfIndicesEmpty_(indices_begin, indices_end);
    ThrowIfIndicesPerFaceIsLessThanThree_(indices_per_face);
    ThrowIfIndexCountNotMultipleOfIndicesPerFace_(
      indices_begin, indices_end, indices_per_face);
    ThrowIfInvalidIndexRange_(
      indices_begin, indices_end,
      elements_begin, elements_end, elements_per_vertex);
  }

  const ElementIter elements_begin;
  const ElementIter elements_end;
  const IndexIter indices_begin;
  const IndexIter indices_end;
  const uint32_t elements_per_vertex;
  const uint32_t indices_per_face;

private:
  static void ThrowIfElementsPerVertexIsNotThree_(
    const uint32_t elements_per_vertex)
  {
    using namespace std;
    if (!(elements_per_vertex == 3)) {
      auto ss = stringstream();
      ss << "normal elements per vertex must be 3, was "
        << elements_per_vertex;
      throw runtime_error(ss.str());
    }
  }

  static void ThrowIfElementCountNotMultipleOfElementsPerVertex_(
    const ElementIter elements_begin,
    const ElementIter elements_end,
    const uint32_t elements_per_vertex)
  {
    using namespace std;
    const auto element_count = distance(elements_begin, elements_end);
    if (element_count % elements_per_vertex != 0) {
      auto ss = stringstream();
      ss << "normal element count (" << element_count
        << ") must be a multiple of " << elements_per_vertex;
      throw runtime_error(ss.str());
    }
  }

  static void ThrowIfIndicesPerFaceIsLessThanThree_(
    const uint32_t indices_per_face)
  {
    using namespace std;
    if (!(indices_per_face >= 3)) {
      auto ss = stringstream();
      ss << "normal indices per face must be "
        << "greater than or equal to 3, was "
        << indices_per_face;
      throw runtime_error(ss.str());
    }
  }

  static void ThrowIfIndexCountNotMultipleOfIndicesPerFace_(
    const IndexIter indices_begin,
    const IndexIter indices_end,
    const uint32_t indices_per_face)
  {
    using namespace std;
    const auto index_count = distance(indices_begin, indices_end);
    if (index_count % indices_per_face != 0) {
      auto ss = stringstream();
      ss << "normal index count (" << index_count
        << ") must be a multiple of " << indices_per_face;
      throw runtime_error(ss.str());
    }
  }

  static void ThrowIfInvalidIndexRange_(
    const IndexIter indices_begin,
    const IndexIter indices_end,
    const ElementIter elements_begin,
    const ElementIter elements_end,
    const uint32_t elements_per_vertex)
  {
    using namespace std;
    //ThrowIfIndicesEmpty_(indices_begin, indices_end); // TODO!!
    const auto min_index = *min_element(indices_begin, indices_end);
    const auto max_index = *max_element(indices_begin, indices_end);
    if (min_index != 0) {
      auto ss = stringstream();
      ss << "min normal index must be zero, was " << min_index;
      throw runtime_error(ss.str());
    }

    const auto vertex_count =
      distance(elements_begin, elements_end) / elements_per_vertex;
    if (max_index >= vertex_count) {
      auto ss = stringstream();
      ss << "max normal index must be less than vertex count ("
        << vertex_count << "), was " << max_index;
      throw runtime_error(ss.str());
    }
  }
};

/// Named constructor to help with template type deduction.
template<typename ElementIter, typename IndexIter> inline
Normals<ElementIter, IndexIter> make_normals(
  const ElementIter elements_begin,
  const ElementIter elements_end,
  const IndexIter indices_begin,
  const IndexIter indices_end,
  const uint32_t indices_per_face)
{
  return Normals<ElementIter, IndexIter>(
    elements_begin, elements_end,
    indices_begin, indices_end,
    indices_per_face);
}


template<typename PosIter, typename PosIdxIter>
std::ostream& WriteTriangles(
  std::ostream& os,
  const Positions<PosIter, PosIdxIter>& positions,
  //const Normals normals,
  //const TexCoords tex_coords,
  const std::string& newline = "\n")
{
  using namespace std;

  const auto tex_coords = vector<float>();
  const auto tex_coord_indices = vector<uint32_t>();
  const auto tex_coord_elements_per_vertex = 2;
  const auto normals = vector<float>();
  const auto normal_indices = vector<uint32_t>();

  return detail::Write(
    os,
    positions,
    make_normals(
      begin(normals), end(normals),
      begin(normal_indices), end(normal_indices)),
    make_tex_coords(
      begin(tex_coords), end(tex_coords), 
      begin(tex_coord_indices), end(tex_coord_indices),
      tex_coord_elements_per_vertex,
      positions.indices_per_face),
    newline);
}

} // namespace obj_io
} // namespace thinks

#endif // THINKS_OBJ_IO_H_INCLUDED