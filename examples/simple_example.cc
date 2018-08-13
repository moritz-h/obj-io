// Copyright(C) 2018 Tommy Hinks <tommy.hinks@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <simple_example.h>

#include <cassert>
#include <cstdint>
#include <fstream>
#include <vector>

#include <thinks/obj_io/obj_io.h>


namespace {

struct Vec2
{
  float x;
  float y;
};

struct Vec3
{
  float x;
  float y;
  float z;
};

struct Vertex
{
  Vec3 position;
  Vec2 tex_coord;
  Vec3 normal;
};

struct Mesh
{
  std::vector<Vertex> vertices;
  std::vector<std::uint16_t> indices;
};


Mesh ReadMesh(const std::string& filename)
{
  using namespace std;

  auto mesh = Mesh{};

  // We cannot assume the order in which callbacks are invoked, 
  // so we need to keep track of which vertex to add properties to.
  // The first encountered position gets added to the first vertex, etc.
  auto pos_count = uint32_t{ 0 };
  auto tex_count = uint32_t{ 0 };
  auto nml_count = uint32_t{ 0 };

  // Positions.
  auto add_position = 
    thinks::obj_io::MakeAddFunc<thinks::obj_io::Position<float, 3>>(
      [&mesh, &pos_count](const auto& pos) {
        // Check if we need a new vertex.
        if (mesh.vertices.size() <= pos_count) {
          mesh.vertices.push_back(Vertex{});
        }

        // Write the position property of current vertex and 
        // set position index to next vertex. Values are translated
        // from OBJ representation to our vector class.
        mesh.vertices[pos_count++].position = 
          Vec3{ pos.values[0], pos.values[1], pos.values[2] };
      });

  // Faces.
  typedef thinks::obj_io::TriangleFace<thinks::obj_io::Index<uint16_t>> ObjFaceType;
  auto add_face = thinks::obj_io::MakeAddFunc<ObjFaceType>(
    [&mesh](const auto& face) {
      // Add triangle indices into the linear storage of our mesh class.
      mesh.indices.push_back(face.values[0].value);
      mesh.indices.push_back(face.values[1].value);
      mesh.indices.push_back(face.values[2].value);
    });

  // Texture coordinates [optional].
  auto add_tex_coord = 
    thinks::obj_io::MakeAddFunc<thinks::obj_io::TexCoord<float, 2>>(
      [&mesh, &tex_count](const auto& tex) {
        if (mesh.vertices.size() <= tex_count) {
          mesh.vertices.push_back(Vertex{});
        }
        mesh.vertices[tex_count++].tex_coord = Vec2{ tex.values[0], tex.values[1] };
      });

  // Normals [optional].
  auto add_normal = 
    thinks::obj_io::MakeAddFunc<thinks::obj_io::Normal<float>>(
      [&mesh, &nml_count](const auto& nml) {
        if (mesh.vertices.size() <= nml_count) {
          mesh.vertices.push_back(Vertex{});
        }
        mesh.vertices[nml_count++].normal = 
          Vec3{ nml.values[0], nml.values[1], nml.values[2] };
      });

  // Open the OBJ file and populate the mesh while parsing it.
  // Note that we provide the MakeAddFunc with the type to use 
  // while parsing. 
  auto ifs = ifstream(filename);
  assert(ifs);
  const auto result = thinks::obj_io::Read(
    ifs,
    add_position,
    add_face,
    add_tex_coord,
    add_normal);
  ifs.close();

  // Some sanity checks.
  assert(
    result.position_count == result.tex_coord_count &&
    result.position_count == result.normal_count &&
    "incomplete vertices in file");
  assert(result.position_count == mesh.vertices.size() &&
    "bad position count");
  assert(result.tex_coord_count == mesh.vertices.size() &&
    "bad tex_coord count");
  assert(result.normal_count == mesh.vertices.size() &&
    "bad normal count");
  assert(
    pos_count == tex_count && pos_count == nml_count &&
    "all vertices must be completely initialized");

  return mesh;
}

void WriteMesh(const std::string& filename, const Mesh& mesh)
{
  using namespace std;

  const auto vtx_iend = end(mesh.vertices);

  // Mappers have two responsibilities:
  // (1) - Iterating over a certain attribute of the mesh (e.g. positions).
  // (2) - Translating from users types to OBJ types (e.g. Vec3 -> Position<float, 3>)

  // Positions.
  auto pos_vtx_iter = begin(mesh.vertices);
  auto pos_mapper = [&pos_vtx_iter, vtx_iend]() {
    typedef thinks::obj_io::Position<float, 3> ObjPositionType;

    if (pos_vtx_iter == vtx_iend) {
      // End indicates that no further calls should be made to this mapper,
      // in this case because the captured iterator has reached the end
      // of the vector.
      return thinks::obj_io::End<ObjPositionType>();
    }

    // Map indicates that additional positions may be available after this one.
    const auto pos = (*pos_vtx_iter++).position;
    return thinks::obj_io::Map(ObjPositionType(pos.x, pos.y, pos.z));
  };

  // Faces.
  auto idx_iter = begin(mesh.indices);
  const auto idx_iend = end(mesh.indices);
  auto face_mapper = [&idx_iter, idx_iend]() {
    typedef thinks::obj_io::Index<uint16_t> ObjIndexType;
    typedef thinks::obj_io::TriangleFace<ObjIndexType> ObjFaceType;

    // Check that there are 3 more indices (trailing indices handled below).
    if (distance(idx_iter, idx_iend) < 3) {
      return thinks::obj_io::End<ObjFaceType>();
    }

    // Create a face from the mesh indices.
    const auto idx0 = ObjIndexType(*idx_iter++);
    const auto idx1 = ObjIndexType(*idx_iter++);
    const auto idx2 = ObjIndexType(*idx_iter++);
    return thinks::obj_io::Map(ObjFaceType(idx0, idx1, idx2));
  };

  // Texture coordinates [optional]. 
  auto tex_vtx_iter = begin(mesh.vertices);
  auto tex_mapper = [&tex_vtx_iter, vtx_iend]() {
    typedef thinks::obj_io::TexCoord<float, 2> ObjTexCoordType;

    if (tex_vtx_iter == vtx_iend) {
      return thinks::obj_io::End<ObjTexCoordType>();
    }

    const auto tex = (*tex_vtx_iter++).tex_coord;
    return thinks::obj_io::Map(ObjTexCoordType(tex.x, tex.y));
  };

  // Normals [optional]. 
  auto nml_vtx_iter = begin(mesh.vertices);
  auto nml_mapper = [&nml_vtx_iter, vtx_iend]() {
    typedef thinks::obj_io::Normal<float> ObjNormalType;

    if (nml_vtx_iter == vtx_iend) {
      return thinks::obj_io::End<ObjNormalType>();
    }

    const auto nml = (*nml_vtx_iter++).normal;
    return thinks::obj_io::Map(ObjNormalType(nml.x, nml.y, nml.z));
  };

  // Open the OBJ file and pass in the mappers, which will be called 
  // internally to write the contents of the mesh to the file.
  auto ofs = ofstream(filename);
  assert(ofs);
  const auto result = thinks::obj_io::Write(
    ofs,
    pos_mapper,
    face_mapper,
    tex_mapper,
    nml_mapper);
  ofs.close();

  // Some sanity checks.
  assert(result.position_count == mesh.vertices.size() &&
    "bad position count");
  assert(result.tex_coord_count == mesh.vertices.size() &&
    "bad position count");
  assert(result.normal_count == mesh.vertices.size() &&
    "bad normal count");
  assert(result.face_count == mesh.indices.size() / 3 &&
    "bad face count");
  assert(idx_iter == idx_iend && "trailing indices");
}

} // namespace

namespace examples {

void SimpleExample(const std::string& filename)
{
  using namespace std;

  auto mesh = Mesh{
    vector<Vertex>{
      Vertex {
        Vec3{ 1.f, 0.f, 0.f }, // position
        Vec2{ 1.f, 0.f },      // tex_coord
        Vec3{ 0.f, 0.f, 1.f }  // normal
      },
      Vertex {
        Vec3{ 0.f, 1.f, 0.f },
        Vec2{ 0.f, 1.f },
        Vec3{ 0.f, 0.f, 1.f }
      },
      Vertex {
        Vec3{ 1.f, 1.f, 0.f },
        Vec2{ 1.f, 1.f },
        Vec3{ 0.f, 0.f, 1.f }
      }
    },
    vector<uint16_t>{ 0, 1, 2 }
  };

  WriteMesh(filename, mesh);
  const auto mesh2 = ReadMesh(filename);
}

} // namespace examples
