/*
 * Copyright 2019 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */



#define GL_GLEXT_PROTOTYPES 1
#define EGL_EGLEXT_PROTOTYPES 1
#define EGL_EGL_PROTOTYPES 1

#include "gles2/gl2platform.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "egl/eglext_angle.h"
#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"


#include "src/gpu/GrD3DTextureUtils.h"

#include "include/core/SkTypes.h"
#include "include/gpu/GrContext.h"
#include "include/gpu/gl/GrGLTypes.h"
#include "src/gpu/GrContextPriv.h"
#include "src/gpu/gl/GrGLDefines.h"


#include "d3d11.h"
#include "gl/gl.h"
#include "gl/GrGLUtil.h"
#include <d3d9types.h>

#include <Windows.h>

#define PROT_CONTENT_EXT_STR "EGL_EXT_protected_content"


namespace GrD3DTextureUtils {

SkColorType GetSkColorTypeFromBufferFormat(uint32_t bufferFormat) {
    switch (bufferFormat) {
        case DXGI_FORMAT_R8G8B8A8_UNORM:
            return kRGBA_8888_SkColorType;
        case DXGI_FORMAT_R16G16B16A16_UNORM:
            return kRGBA_F16_SkColorType;
        case DXGI_FORMAT_R10G10B10A2_UNORM:
        case DXGI_FORMAT_R10G10B10A2_UINT:
            return kRGBA_1010102_SkColorType;
        case DXGI_FORMAT_R16G16B16A16_FLOAT:
            return kRGBA_F16_SkColorType;
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
            return kRGBA_F32_SkColorType;
        default:
            // Given that we only use this texture as a source, colorType will not impact how Skia
            // uses the texture.  The only potential affect this is anticipated to have is that for
            // some format types if we are not bound as an OES texture we may get invalid results
            // for SKP capture if we read back the texture.
            return kRGBA_8888_SkColorType;
    }
}

GrBackendFormat GetBackendFormat(GrContext* context, uint32_t bufferFormat, bool requireKnownFormat) {
    GrBackendApi backend = context->backend();

    if (backend == GrBackendApi::kOpenGL) {
        switch (bufferFormat) {
            //TODO: find out if we can detect, which graphic buffers support GR_GL_TEXTURE_2D
            case DXGI_FORMAT_R8G8B8A8_UNORM:
            case DXGI_FORMAT_R8G8B8A8_SNORM:
                return GrBackendFormat::MakeGL(GR_GL_RGBA8, GR_GL_TEXTURE_EXTERNAL);
            case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
            case DXGI_FORMAT_R8G8B8A8_UINT:
                return GrBackendFormat::MakeGL(GR_GL_RGBA8UI, GR_GL_TEXTURE_EXTERNAL);
            case DXGI_FORMAT_R8G8B8A8_SINT:
                return GrBackendFormat::MakeGL(GR_GL_RGBA8I, GR_GL_TEXTURE_EXTERNAL);

            case DXGI_FORMAT_R16G16B16A16_UNORM:
            case DXGI_FORMAT_R16G16B16A16_SNORM:
                return GrBackendFormat::MakeGL(GR_GL_RGBA16I, GR_GL_TEXTURE_EXTERNAL);
            case DXGI_FORMAT_R16G16B16A16_UINT:
                return GrBackendFormat::MakeGL(GR_GL_RGBA16UI, GR_GL_TEXTURE_EXTERNAL);
            case DXGI_FORMAT_R16G16B16A16_SINT:
                return GrBackendFormat::MakeGL(GR_GL_RGBA16I, GR_GL_TEXTURE_EXTERNAL);
            case DXGI_FORMAT_R16G16B16A16_FLOAT:
                return GrBackendFormat::MakeGL(GR_GL_RGBA16F, GR_GL_TEXTURE_EXTERNAL);


            case DXGI_FORMAT_R32G32B32A32_SINT:
                return GrBackendFormat::MakeGL(GR_GL_RGBA32I, GR_GL_TEXTURE_EXTERNAL);
            case DXGI_FORMAT_R32G32B32A32_UINT:
                return GrBackendFormat::MakeGL(GR_GL_RGBA32UI, GR_GL_TEXTURE_EXTERNAL);
            case DXGI_FORMAT_R32G32B32A32_FLOAT:
                return GrBackendFormat::MakeGL(GR_GL_RGBA32F, GR_GL_TEXTURE_EXTERNAL);

            case DXGI_FORMAT_R10G10B10A2_UNORM:
            case DXGI_FORMAT_R10G10B10A2_UINT:
                return GrBackendFormat::MakeGL(GR_GL_RGB10_A2, GR_GL_TEXTURE_EXTERNAL);

            default:
                if (requireKnownFormat) {
                    return GrBackendFormat();
                } else {
                    return GrBackendFormat::MakeGL(GR_GL_RGBA8, GR_GL_TEXTURE_EXTERNAL);
                }
        }
    }

    return GrBackendFormat();
}

class GLCleanupHelper {
public:
    GLCleanupHelper(GrGLuint texID, EGLImageKHR image, EGLDisplay display, ID3D11Texture2D* texture)
        : fTexID(texID)
        , fImage(image)
        , fDisplay(display)
        , fTexture(texture) { }
    ~GLCleanupHelper() {
        glDeleteTextures(1, &fTexID);
        // eglDestroyImageKHR will remove a ref from the AHardwareBuffer
        eglDestroyImageKHR(fDisplay, fImage);

        if (fTexture) {
            //fTexture->Release();
        }
    }
private:
    GrGLuint    fTexID;
    EGLImageKHR fImage;
    EGLDisplay  fDisplay;
    ID3D11Texture2D* fTexture;
};

void delete_gl_texture(void* context) {
    GLCleanupHelper* cleanupHelper = static_cast<GLCleanupHelper*>(context);
    delete cleanupHelper;
}

typedef GLenum (*GL_GetErrorFuncType)(void);
GL_GetErrorFuncType g_glGetError = nullptr;

static GrBackendTexture make_gl_backend_texture(GrContext* context, void* d3d11_texture,
        int width, int height,
        DeleteImageProc* deleteProc,
        DeleteImageCtx* deleteCtx,
        const GrBackendFormat& backendFormat,
        bool isRenderable) {

    if (!g_glGetError) {
        HMODULE hlib = ::LoadLibraryA("libGLESv2.dll");
        g_glGetError = (GL_GetErrorFuncType)::GetProcAddress(hlib, "glGetError");
    }


    auto err_code = g_glGetError();
    while (GL_NO_ERROR != err_code) {
        err_code = g_glGetError();
    } //clear GL errors

    const UINT bufferSize = 32;


//     ID3D11Texture2D *d3d11_texture = nullptr;
//     CD3D11_TEXTURE2D_DESC desc(DXGI_FORMAT_R8G8B8A8_UNORM, bufferSize, bufferSize, 1, 1,
//         D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);
//     EXPECT_TRUE(SUCCEEDED(mD3D11Device->CreateTexture2D(&desc, nullptr, &d3d11_texture)));

    const EGLint attribs[] = { /*EGL_TEXTURE_INTERNAL_FORMAT_ANGLE, GL_RGB, */EGL_NONE };
    EGLDisplay display = eglGetCurrentDisplay();
    EGLImage image = eglCreateImageKHR(display, EGL_NO_CONTEXT, EGL_D3D11_TEXTURE_ANGLE,
        static_cast<EGLClientBuffer>(d3d11_texture), attribs);

    int x = g_glGetError();


    if (EGL_NO_IMAGE_KHR == image) {
        SkDebugf("Could not create EGL image, err = (%#x)", (int)eglGetError());
        return GrBackendTexture();
    }

    GrGLuint texID;
    glGenTextures(1, &texID);
    if (!texID) {
        eglDestroyImageKHR(display, image);
        return GrBackendTexture();
    }

    GrGLuint target = isRenderable ? GR_GL_TEXTURE_2D : GR_GL_TEXTURE_EXTERNAL;

    glBindTexture(target, texID);
    GLenum status = GL_NO_ERROR;
    if ((status = g_glGetError()) != GL_NO_ERROR) {
        SkDebugf("glBindTexture failed (%#x)", (int)status);
        glDeleteTextures(1, &texID);
        eglDestroyImageKHR(display, image);
        return GrBackendTexture();
    }
    glEGLImageTargetTexture2DOES(target, image);
    if ((status = g_glGetError()) != GL_NO_ERROR) {
        SkDebugf("glEGLImageTargetTexture2DOES failed (%#x)", (int)status);
        glDeleteTextures(1, &texID);
        eglDestroyImageKHR(display, image);
        return GrBackendTexture();
    }
    //context->resetContext(kTextureBinding_GrGLBackendState);

    GrGLTextureInfo textureInfo;
    textureInfo.fID = texID;
    SkASSERT(backendFormat.isValid());
    textureInfo.fTarget = target;
    textureInfo.fFormat = GrGLFormatToEnum(backendFormat.asGLFormat());

    *deleteProc = delete_gl_texture;
    *deleteCtx = new GLCleanupHelper(texID, image, display, (ID3D11Texture2D*)d3d11_texture);

    return GrBackendTexture(width, height, GrMipMapped::kNo, textureInfo);
}

static bool can_import_protected_content_eglimpl() {
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    const char* exts = eglQueryString(dpy, EGL_EXTENSIONS);
    size_t cropExtLen = strlen(PROT_CONTENT_EXT_STR);
    size_t extsLen = strlen(exts);
    bool equal = !strcmp(PROT_CONTENT_EXT_STR, exts);
    bool atStart = !strncmp(PROT_CONTENT_EXT_STR " ", exts, cropExtLen+1);
    bool atEnd = (cropExtLen+1) < extsLen
                  && !strcmp(" " PROT_CONTENT_EXT_STR,
                  exts + extsLen - (cropExtLen+1));
    bool inMiddle = strstr(exts, " " PROT_CONTENT_EXT_STR " ");
    return equal || atStart || atEnd || inMiddle;
}

static bool can_import_protected_content(GrContext* context) {
    if (GrBackendApi::kOpenGL == context->backend()) {
        // Only compute whether the extension is present once the first time this
        // function is called.
        static bool hasIt = can_import_protected_content_eglimpl();
        return hasIt;
    }
    return false;
}

GrBackendTexture MakeBackendTexture(GrContext* context, void* d3d11_texture,
                                    DeleteImageProc* deleteProc,
                                    DeleteImageCtx* deleteCtx,
                                    SkColorType& color_type) {
    if (context->abandoned()) {
        return GrBackendTexture();
    }

//     D3D11_TEXTURE2D_DESC desc = { 0 };
//     d3d11_texture->GetDesc(&desc);

//    bool isRenderable = (desc.BindFlags & D3D11_BIND_RENDER_TARGET) != 0;

    GrBackendFormat backendFormat =
            GetBackendFormat(context, (uint32_t)DXGI_FORMAT_R8G8B8A8_UNORM /*desc.Format*/, true);

    color_type =
            GetSkColorTypeFromBufferFormat((uint32_t)DXGI_FORMAT_R8G8B8A8_UNORM /*desc.Format*/);

    if (GrBackendApi::kOpenGL == context->backend()) {
        return make_gl_backend_texture(context, d3d11_texture, /*desc.Width, desc.Height,*/50, 50, deleteProc,
                                       deleteCtx, backendFormat,
                                       true/*isRenderable*/);
    } 
    return GrBackendTexture();
}

} // GrD3DTextureUtils


