// GENERATED FILE - DO NOT EDIT.
// Generated by generate_entry_points.py using data from gl.xml.
//
// Copyright 2019 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// entry_points_gl_4_6_autogen.cpp:
//   Defines the GL 4.6 entry points.

#include "libGL/entry_points_gl_4_6_autogen.h"

#include "libANGLE/Context.h"
#include "libANGLE/Context.inl.h"
#include "libANGLE/entry_points_utils.h"
#include "libANGLE/gl_enum_utils_autogen.h"
#include "libANGLE/validationEGL.h"
#include "libANGLE/validationES.h"
#include "libANGLE/validationES1.h"
#include "libANGLE/validationES2.h"
#include "libANGLE/validationES3.h"
#include "libANGLE/validationES31.h"
#include "libANGLE/validationES32.h"
#include "libANGLE/validationESEXT.h"
#include "libANGLE/validationGL46_autogen.h"
#include "libGLESv2/global_state.h"

namespace gl
{
void GL_APIENTRY MultiDrawArraysIndirectCount(GLenum mode,
                                              const void *indirect,
                                              GLintptr drawcount,
                                              GLsizei maxdrawcount,
                                              GLsizei stride)
{
    Context *context = GetValidGlobalContext();
    EVENT("glMultiDrawArraysIndirectCount",
          "context = %d, GLenum mode = %s, const void *indirect = 0x%016" PRIxPTR
          ", GLintptr drawcount = %llu, GLsizei maxdrawcount = %d, GLsizei stride = %d",
          CID(context), GLenumToString(GLenumGroup::PrimitiveType, mode), (uintptr_t)indirect,
          static_cast<unsigned long long>(drawcount), maxdrawcount, stride);

    if (context)
    {
        std::unique_lock<std::mutex> shareContextLock = GetShareGroupLock(context);
        bool isCallValid                              = (context->skipValidation() ||
                            ValidateMultiDrawArraysIndirectCount(context, mode, indirect, drawcount,
                                                                 maxdrawcount, stride));
        if (isCallValid)
        {
            context->multiDrawArraysIndirectCount(mode, indirect, drawcount, maxdrawcount, stride);
        }
        ANGLE_CAPTURE(MultiDrawArraysIndirectCount, isCallValid, context, mode, indirect, drawcount,
                      maxdrawcount, stride);
    }
}

void GL_APIENTRY MultiDrawElementsIndirectCount(GLenum mode,
                                                GLenum type,
                                                const void *indirect,
                                                GLintptr drawcount,
                                                GLsizei maxdrawcount,
                                                GLsizei stride)
{
    Context *context = GetValidGlobalContext();
    EVENT("glMultiDrawElementsIndirectCount",
          "context = %d, GLenum mode = %s, GLenum type = %s, const void *indirect = 0x%016" PRIxPTR
          ", GLintptr drawcount = %llu, GLsizei maxdrawcount = %d, GLsizei stride = %d",
          CID(context), GLenumToString(GLenumGroup::PrimitiveType, mode),
          GLenumToString(GLenumGroup::DefaultGroup, type), (uintptr_t)indirect,
          static_cast<unsigned long long>(drawcount), maxdrawcount, stride);

    if (context)
    {
        std::unique_lock<std::mutex> shareContextLock = GetShareGroupLock(context);
        bool isCallValid                              = (context->skipValidation() ||
                            ValidateMultiDrawElementsIndirectCount(
                                context, mode, type, indirect, drawcount, maxdrawcount, stride));
        if (isCallValid)
        {
            context->multiDrawElementsIndirectCount(mode, type, indirect, drawcount, maxdrawcount,
                                                    stride);
        }
        ANGLE_CAPTURE(MultiDrawElementsIndirectCount, isCallValid, context, mode, type, indirect,
                      drawcount, maxdrawcount, stride);
    }
}

void GL_APIENTRY PolygonOffsetClamp(GLfloat factor, GLfloat units, GLfloat clamp)
{
    Context *context = GetValidGlobalContext();
    EVENT("glPolygonOffsetClamp",
          "context = %d, GLfloat factor = %f, GLfloat units = %f, GLfloat clamp = %f", CID(context),
          factor, units, clamp);

    if (context)
    {
        std::unique_lock<std::mutex> shareContextLock = GetShareGroupLock(context);
        bool isCallValid                              = (context->skipValidation() ||
                            ValidatePolygonOffsetClamp(context, factor, units, clamp));
        if (isCallValid)
        {
            context->polygonOffsetClamp(factor, units, clamp);
        }
        ANGLE_CAPTURE(PolygonOffsetClamp, isCallValid, context, factor, units, clamp);
    }
}

void GL_APIENTRY SpecializeShader(GLuint shader,
                                  const GLchar *pEntryPoint,
                                  GLuint numSpecializationConstants,
                                  const GLuint *pConstantIndex,
                                  const GLuint *pConstantValue)
{
    Context *context = GetValidGlobalContext();
    EVENT("glSpecializeShader",
          "context = %d, GLuint shader = %u, const GLchar *pEntryPoint = 0x%016" PRIxPTR
          ", GLuint numSpecializationConstants = %u, const GLuint *pConstantIndex = 0x%016" PRIxPTR
          ", const GLuint *pConstantValue = 0x%016" PRIxPTR "",
          CID(context), shader, (uintptr_t)pEntryPoint, numSpecializationConstants,
          (uintptr_t)pConstantIndex, (uintptr_t)pConstantValue);

    if (context)
    {
        std::unique_lock<std::mutex> shareContextLock = GetShareGroupLock(context);
        bool isCallValid =
            (context->skipValidation() ||
             ValidateSpecializeShader(context, shader, pEntryPoint, numSpecializationConstants,
                                      pConstantIndex, pConstantValue));
        if (isCallValid)
        {
            context->specializeShader(shader, pEntryPoint, numSpecializationConstants,
                                      pConstantIndex, pConstantValue);
        }
        ANGLE_CAPTURE(SpecializeShader, isCallValid, context, shader, pEntryPoint,
                      numSpecializationConstants, pConstantIndex, pConstantValue);
    }
}
}  // namespace gl
