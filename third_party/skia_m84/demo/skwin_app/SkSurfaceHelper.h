#pragma once

#include "include/core/SkSurface.h"

class SkSurfaceHelper {
public:
    static sk_sp<SkSurface> MakeFromD3D11Texture2D(GrContext* context,
                                                   void* d3d11Texture2D,
                                                   GrSurfaceOrigin origin,
                                                   sk_sp<SkColorSpace> colorSpace,
                                                   const SkSurfaceProps* surfaceProps = nullptr);
};
