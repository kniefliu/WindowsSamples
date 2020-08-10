#pragma once

#include "include/core/SkSurface.h"

struct GrGLInterface;


typedef void* DeleteImageCtx;
typedef void (*DeleteImageProc)(DeleteImageCtx);

class SkSurfaceHelper {
public:
    static sk_sp<SkSurface> MakeFromD3D11Texture2D(GrContext* context,
                                                   void* d3d11Texture2D,
                                                   int textureWidth,
                                                   int textureHeight,
                                                   GrSurfaceOrigin origin,
                                                   sk_sp<SkColorSpace> colorSpace,
                                                   const SkSurfaceProps* surfaceProps,
                                                   const GrGLInterface* backendContext);

private:
    static GrBackendTexture MakeBackendTexture(GrContext* context,
                                        void* d3d11Texture2D,
                                               int textureWidth,
                                               int textureHeight,
                                        DeleteImageProc* deleteProc,
                                        DeleteImageCtx* deleteCtx,
                                        SkColorType& color_type,
                                        const GrGLInterface* fackendContext);
};
