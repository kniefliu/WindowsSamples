// GENERATED FILE - DO NOT EDIT.
// Generated by generate_entry_points.py using data from gl.xml and wgl.xml.
//
// Copyright 2020 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// validationGL42_autogen.h:
//   Validation functions for the OpenGL 4.2 entry points.

#ifndef LIBANGLE_VALIDATION_GL42_AUTOGEN_H_
#define LIBANGLE_VALIDATION_GL42_AUTOGEN_H_

#include "common/PackedEnums.h"

namespace gl
{
class Context;

bool ValidateDrawArraysInstancedBaseInstance(const Context *context,
                                             PrimitiveMode modePacked,
                                             GLint first,
                                             GLsizei count,
                                             GLsizei instancecount,
                                             GLuint baseinstance);
bool ValidateDrawElementsInstancedBaseInstance(const Context *context,
                                               GLenum mode,
                                               GLsizei count,
                                               GLenum type,
                                               const void *indices,
                                               GLsizei instancecount,
                                               GLuint baseinstance);
bool ValidateDrawElementsInstancedBaseVertexBaseInstance(const Context *context,
                                                         PrimitiveMode modePacked,
                                                         GLsizei count,
                                                         DrawElementsType typePacked,
                                                         const void *indices,
                                                         GLsizei instancecount,
                                                         GLint basevertex,
                                                         GLuint baseinstance);
bool ValidateDrawTransformFeedbackInstanced(const Context *context,
                                            GLenum mode,
                                            TransformFeedbackID idPacked,
                                            GLsizei instancecount);
bool ValidateDrawTransformFeedbackStreamInstanced(const Context *context,
                                                  GLenum mode,
                                                  TransformFeedbackID idPacked,
                                                  GLuint stream,
                                                  GLsizei instancecount);
bool ValidateGetActiveAtomicCounterBufferiv(const Context *context,
                                            ShaderProgramID programPacked,
                                            GLuint bufferIndex,
                                            GLenum pname,
                                            const GLint *params);
bool ValidateTexStorage1D(const Context *context,
                          GLenum target,
                          GLsizei levels,
                          GLenum internalformat,
                          GLsizei width);
}  // namespace gl

#endif  // LIBANGLE_VALIDATION_GL42_AUTOGEN_H_