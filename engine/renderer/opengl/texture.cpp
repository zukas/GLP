#include "texture.h"
#include "gli/gli.hpp"

#include "gl_headers.h"

uint32_t load_texture(const char *file) {
  gli::texture texture = gli::load(file);
  if (texture.empty())
    return 0;

  gli::gl GL(gli::gl::PROFILE_GL33);
  gli::gl::format const format =
      GL.translate(texture.format(), texture.swizzles());
  GLenum target = GL.translate(texture.target());

  GLuint texture_id = 0;
  glGenTextures(1, &texture_id);
  glBindTexture(target, texture_id);
  glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, 0);
  glTexParameteri(target, GL_TEXTURE_MAX_LEVEL,
                  static_cast<GLint>(texture.levels() - 1));
  glTexParameteri(target, GL_TEXTURE_SWIZZLE_R, format.Swizzles[0]);
  glTexParameteri(target, GL_TEXTURE_SWIZZLE_G, format.Swizzles[1]);
  glTexParameteri(target, GL_TEXTURE_SWIZZLE_B, format.Swizzles[2]);
  glTexParameteri(target, GL_TEXTURE_SWIZZLE_A, format.Swizzles[3]);

  glm::tvec3<GLsizei> const extent(texture.extent());
  GLsizei const face_total =
      static_cast<GLsizei>(texture.layers() * texture.faces());

  switch (texture.target()) {
  case gli::TARGET_1D:
    glTexStorage1D(target, static_cast<GLint>(texture.levels()),
                   format.Internal, extent.x);
    break;
  case gli::TARGET_1D_ARRAY:
  case gli::TARGET_2D:
  case gli::TARGET_CUBE:
    glTexStorage2D(target, static_cast<GLint>(texture.levels()),
                   format.Internal, extent.x,
                   texture.target() == gli::TARGET_2D ? extent.y : face_total);
    break;
  case gli::TARGET_2D_ARRAY:
  case gli::TARGET_3D:
  case gli::TARGET_CUBE_ARRAY:
    glTexStorage3D(target, static_cast<GLint>(texture.levels()),
                   format.Internal, extent.x, extent.y,
                   texture.target() == gli::TARGET_3D ? extent.z : face_total);
    break;
  case gli::TARGET_RECT:
  case gli::TARGET_RECT_ARRAY:
    assert(0);
    break;
  }

  for (std::size_t layer = 0; layer < texture.layers(); ++layer) {
    for (std::size_t face = 0; face < texture.faces(); ++face) {
      for (std::size_t level = 0; level < texture.levels(); ++level) {
        GLsizei const layer_gl = static_cast<GLsizei>(layer);
        glm::tvec3<GLsizei> level_extent(texture.extent(level));
        target =
            gli::is_target_cube(texture.target())
                ? static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face)
                : target;

        switch (texture.target()) {
        case gli::TARGET_1D:
          if (gli::is_compressed(texture.format()))
            glCompressedTexSubImage1D(target, static_cast<GLint>(level), 0,
                                      level_extent.x, format.Internal,
                                      static_cast<GLsizei>(texture.size(level)),
                                      texture.data(layer, face, level));
          else
            glTexSubImage1D(target, static_cast<GLint>(level), 0,
                            level_extent.x, format.External, format.Type,
                            texture.data(layer, face, level));
          break;
        case gli::TARGET_1D_ARRAY:
        case gli::TARGET_2D:
        case gli::TARGET_CUBE:
          if (gli::is_compressed(texture.format()))
            glCompressedTexSubImage2D(
                target, static_cast<GLint>(level), 0, 0, level_extent.x,
                texture.target() == gli::TARGET_1D_ARRAY ? layer_gl
                                                         : level_extent.y,
                format.Internal, static_cast<GLsizei>(texture.size(level)),
                texture.data(layer, face, level));
          else
            glTexSubImage2D(
                target, static_cast<GLint>(level), 0, 0, level_extent.x,
                texture.target() == gli::TARGET_1D_ARRAY ? layer_gl
                                                         : level_extent.y,
                format.External, format.Type, texture.data(layer, face, level));
          break;
        case gli::TARGET_2D_ARRAY:
        case gli::TARGET_3D:
        case gli::TARGET_CUBE_ARRAY:
          if (gli::is_compressed(texture.format()))
            glCompressedTexSubImage3D(
                target, static_cast<GLint>(level), 0, 0, 0, level_extent.x,
                level_extent.y,
                texture.target() == gli::TARGET_3D ? level_extent.z : layer_gl,
                format.Internal, static_cast<GLsizei>(texture.size(level)),
                texture.data(layer, face, level));
          else
            glTexSubImage3D(
                target, static_cast<GLint>(level), 0, 0, 0, level_extent.x,
                level_extent.y,
                texture.target() == gli::TARGET_3D ? level_extent.z : layer_gl,
                format.External, format.Type, texture.data(layer, face, level));
          break;
        case gli::TARGET_RECT:
        case gli::TARGET_RECT_ARRAY:
          assert(0);
          break;
        }
      }
    }
  }
  return texture_id;
}

void destroy_texture(uint32_t texture_id) { glDeleteTextures(1, &texture_id); }
