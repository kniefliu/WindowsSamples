/*
 * Copyright 2019 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
#ifndef GrD3DTextureUtils_DEFINED
#define GrD3DTextureUtils_DEFINED

#include "include/core/SkTypes.h"



#include "include/gpu/GrBackendSurface.h"
#include "include/gpu/GrTypes.h"

class GrContext;
struct ID3D11Texture2D;

namespace GrD3DTextureUtils {

SkColorType GetSkColorTypeFromBufferFormat(uint32_t bufferFormat);

GrBackendFormat GetBackendFormat(GrContext* context, ID3D11Texture2D* d3d11_texture,
                                 uint32_t bufferFormat, bool requireKnownFormat);

typedef void* DeleteImageCtx;
typedef void (*DeleteImageProc)(DeleteImageCtx);

GrBackendTexture MakeBackendTexture(GrContext* context, void* d3d11_texture,
                                    DeleteImageProc* deleteProc,
                                    DeleteImageCtx* deleteCtx,
                                    SkColorType& color_type);

} // GrD3DTextureUtils

#endif
