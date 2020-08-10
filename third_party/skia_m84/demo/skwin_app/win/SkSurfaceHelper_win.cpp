#include "demo/skwin_app/SkSurfaceHelper.h"

#define GL_GLEXT_PROTOTYPES 1
#define EGL_EGLEXT_PROTOTYPES 1
#define EGL_EGL_PROTOTYPES 1

#if 1

#include <EGL/egl.h>
#include <EGL/eglext.h>

#include "include/core/SkTypes.h"
#include "include/gpu/GrContext.h"
#include "include/gpu/gl/GrGLTypes.h"
#include "include/gpu/gl/GrGLInterface.h"
#include "src/gpu/GrContextPriv.h"
#include "src/gpu/gl/GrGLDefines.h"
// gl error
#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"

// 
#include "src/gpu/gl/GrGLUtil.h"

#include <d3d11.h>

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

GrBackendFormat GetBackendFormat(GrContext* context,
                                 uint32_t bufferFormat,
                                 bool requireKnownFormat) {
    GrBackendApi backend = context->backend();

    if (backend == GrBackendApi::kOpenGL) {
        switch (bufferFormat) {
            // TODO: find out if we can detect, which graphic buffers support GR_GL_TEXTURE_2D
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
    GLCleanupHelper(GrGLuint texID,
                    EGLImageKHR image,
                    EGLDisplay display,
                    ID3D11Texture2D* texture,
                    const GrGLInterface* backendContext)
            : fTexID(texID), fImage(image), fDisplay(display), fTexture(texture), fBackendContext(backendContext) {}
    ~GLCleanupHelper() {
        fBackendContext->fFunctions.fDeleteTextures(1, &fTexID);
        // eglDestroyImageKHR will remove a ref from the AHardwareBuffer
        eglDestroyImageKHR(fDisplay, fImage);

        if (fTexture) {
            // fTexture->Release();
        }
    }

private:
    GrGLuint fTexID;
    EGLImageKHR fImage;
    EGLDisplay fDisplay;
    ID3D11Texture2D* fTexture;
    const GrGLInterface* fBackendContext;
};

void delete_gl_texture(void* context) {
    GLCleanupHelper* cleanupHelper = static_cast<GLCleanupHelper*>(context);
    delete cleanupHelper;
}


static GrBackendTexture make_gl_backend_texture(GrContext* context,
                                                void* d3d11_texture,
                                                int width,
                                                int height,
                                                DeleteImageProc* deleteProc,
                                                DeleteImageCtx* deleteCtx,
                                                const GrBackendFormat& backendFormat,
                                                bool isRenderable,
                                                const GrGLInterface* backendContext) 
{
    auto err_code = backendContext->fFunctions.fGetError();
    while (GL_NO_ERROR != err_code) {
        err_code = backendContext->fFunctions.fGetError();
    }  // clear GL errors

    const UINT bufferSize = 32;

    const EGLint attribs[] = {/*EGL_TEXTURE_INTERNAL_FORMAT_ANGLE, GL_RGB, */ EGL_NONE};
    EGLDisplay display = eglGetCurrentDisplay();
    EGLImage image = eglCreateImageKHR(display, EGL_NO_CONTEXT, EGL_D3D11_TEXTURE_ANGLE,
                                       static_cast<EGLClientBuffer>(d3d11_texture), attribs);

    int x = backendContext->fFunctions.fGetError();

    if (EGL_NO_IMAGE_KHR == image) {
        SkDebugf("Could not create EGL image, err = (%#x)", (int)eglGetError());
        return GrBackendTexture();
    }

    GrGLuint texID;
    backendContext->fFunctions.fGenTextures(1, &texID);
    if (!texID) {
        eglDestroyImageKHR(display, image);
        return GrBackendTexture();
    }

    GrGLuint target = isRenderable ? GR_GL_TEXTURE_2D : GR_GL_TEXTURE_EXTERNAL;

    backendContext->fFunctions.fBindTexture(target, texID);
    GLenum status = GL_NO_ERROR;
    if ((status = backendContext->fFunctions.fGetError()) != GL_NO_ERROR) {
        SkDebugf("glBindTexture failed (%#x)", (int)status);
        backendContext->fFunctions.fDeleteTextures(1, &texID);
        eglDestroyImageKHR(display, image);
        return GrBackendTexture();
    }
    glEGLImageTargetTexture2DOES(target, image);
    if ((status = backendContext->fFunctions.fGetError()) != GL_NO_ERROR) {
        SkDebugf("glEGLImageTargetTexture2DOES failed (%#x)", (int)status);
        backendContext->fFunctions.fDeleteTextures(1, &texID);
        eglDestroyImageKHR(display, image);
        return GrBackendTexture();
    }
    // context->resetContext(kTextureBinding_GrGLBackendState);

    GrGLTextureInfo textureInfo;
    textureInfo.fID = texID;
    SkASSERT(backendFormat.isValid());
    textureInfo.fTarget = target;
    textureInfo.fFormat = GrGLFormatToEnum(backendFormat.asGLFormat());

    *deleteProc = delete_gl_texture;
    *deleteCtx = new GLCleanupHelper(texID, image, display, (ID3D11Texture2D*)d3d11_texture, backendContext);

    return GrBackendTexture(width, height, GrMipMapped::kNo, textureInfo);
}

static bool can_import_protected_content_eglimpl() {
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    const char* exts = eglQueryString(dpy, EGL_EXTENSIONS);
    size_t cropExtLen = strlen(PROT_CONTENT_EXT_STR);
    size_t extsLen = strlen(exts);
    bool equal = !strcmp(PROT_CONTENT_EXT_STR, exts);
    bool atStart = !strncmp(PROT_CONTENT_EXT_STR " ", exts, cropExtLen + 1);
    bool atEnd = (cropExtLen + 1) < extsLen &&
                 !strcmp(" " PROT_CONTENT_EXT_STR, exts + extsLen - (cropExtLen + 1));
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

}

sk_sp<SkSurface> SkSurfaceHelper::MakeFromD3D11Texture2D(GrContext* context,
    void* d3d11Texture2D,
                                                         int textureWidth,
                                                         int textureHeight,
    GrSurfaceOrigin origin,
    sk_sp<SkColorSpace> colorSpace,
    const SkSurfaceProps* surfaceProps,
    const GrGLInterface* backendContext) 
{
    DeleteImageProc deleteImageProc = nullptr;
    DeleteImageCtx deleteImageCtx = nullptr;

    SkColorType colorType = kUnknown_SkColorType;

    GrBackendTexture backendTexture = MakeBackendTexture(
            context, d3d11Texture2D, textureWidth, textureHeight, &deleteImageProc,
                               &deleteImageCtx, colorType, backendContext);
    if (!backendTexture.isValid()) {
        return nullptr;
    }

    sk_sp<SkSurface> surface = SkSurface::MakeFromBackendTexture(
            context, backendTexture, origin, 0, colorType, std::move(colorSpace), surfaceProps,
            deleteImageProc, deleteImageCtx);

    if (!surface) {
        SkASSERT(deleteImageProc);
        deleteImageProc(deleteImageCtx);
    }
    return surface;
    return nullptr;
}

GrBackendTexture SkSurfaceHelper::MakeBackendTexture(GrContext* context,
                                                     void* d3d11Texture2D,
    int textureWidth,
    int textureHeight,
                                    DeleteImageProc* deleteProc,
                                    DeleteImageCtx* deleteCtx,
                                    SkColorType& color_type,
                                                     const GrGLInterface* backendContext) {
    if (context->abandoned()) {
        return GrBackendTexture();
    }

    //     D3D11_TEXTURE2D_DESC desc = { 0 };
    //     d3d11_texture->GetDesc(&desc);

    //    bool isRenderable = (desc.BindFlags & D3D11_BIND_RENDER_TARGET) != 0;

    GrBackendFormat backendFormat = GrD3DTextureUtils::GetBackendFormat(
            context, (uint32_t)DXGI_FORMAT_R8G8B8A8_UNORM /*desc.Format*/, true);

    color_type = GrD3DTextureUtils::GetSkColorTypeFromBufferFormat(
            (uint32_t)DXGI_FORMAT_R8G8B8A8_UNORM /*desc.Format*/);

    if (GrBackendApi::kOpenGL == context->backend()) {
        return GrD3DTextureUtils::make_gl_backend_texture(
                context, d3d11Texture2D, /*desc.Width, desc.Height,*/ textureWidth, textureHeight,
                                       deleteProc, deleteCtx, backendFormat, true /*isRenderable*/,
            backendContext);
    }
    return GrBackendTexture();
}

#endif 
