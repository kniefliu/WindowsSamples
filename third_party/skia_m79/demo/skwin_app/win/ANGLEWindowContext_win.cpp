
/*
 * Copyright 2015 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#define EGL_EGL_PROTOTYPES 1

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include "include/gpu/gl/GrGLAssembleInterface.h"
#include "src/gpu/gl/GrGLDefines.h"
#include "demo/skwin_app/GLWindowContext.h"
#include "demo/skwin_app/win/WindowContextFactory_win.h"

#include <d3d11.h>

using skwin_app::GLWindowContext;
using skwin_app::DisplayParams;

namespace {

EGLDisplay get_angle_egl_display(HDC hdc) {
    PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT;
    eglGetPlatformDisplayEXT =
            (PFNEGLGETPLATFORMDISPLAYEXTPROC)eglGetProcAddress("eglGetPlatformDisplayEXT");

    // We expect ANGLE to support this extension
    if (!eglGetPlatformDisplayEXT) {
        return EGL_NO_DISPLAY;
    }

    // We currently only support D3D11 ANGLE.
    static constexpr EGLint kType = EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE;
    static constexpr EGLint attribs[] = {EGL_PLATFORM_ANGLE_TYPE_ANGLE, kType, EGL_NONE};
    return eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, hdc, attribs);
}

class ANGLEGLWindowContext_win : public GLWindowContext {
public:
    ANGLEGLWindowContext_win(HWND, const DisplayParams&);
    ~ANGLEGLWindowContext_win() override;

    void* makeD3D11Texture2D(int w, int h) override;

protected:
    void onSwapBuffers() override;

    sk_sp<const GrGLInterface> onInitializeContext() override;
    void onDestroyContext() override;

private:
    HWND fHWND;
    EGLDisplay fDisplay = EGL_NO_DISPLAY;
    EGLContext fEGLContext = EGL_NO_CONTEXT;
    EGLSurface fEGLSurface = EGL_NO_SURFACE;

    typedef GLWindowContext INHERITED;
};

void* QueryDeviceObjectFromANGLE(EGLDisplay egl_display) {
    intptr_t egl_device = 0;
    intptr_t device = 0;

    PFNEGLQUERYDISPLAYATTRIBEXTPROC QueryDisplayAttribEXT = nullptr;

    {
        QueryDisplayAttribEXT = reinterpret_cast<PFNEGLQUERYDISPLAYATTRIBEXTPROC>(
                eglGetProcAddress("eglQueryDisplayAttribEXT"));

        if (!QueryDisplayAttribEXT) return nullptr;
    }

    PFNEGLQUERYDEVICEATTRIBEXTPROC QueryDeviceAttribEXT = nullptr;

    {
        QueryDeviceAttribEXT = reinterpret_cast<PFNEGLQUERYDEVICEATTRIBEXTPROC>(
                eglGetProcAddress("eglQueryDeviceAttribEXT"));
        if (!QueryDeviceAttribEXT) return nullptr;
    }

    {
        if (!QueryDisplayAttribEXT(egl_display, EGL_DEVICE_EXT, &egl_device)) return nullptr;
    }
    if (!egl_device) return nullptr;

    {
        if (!QueryDeviceAttribEXT(reinterpret_cast<EGLDeviceEXT>(egl_device),
                                  EGL_D3D11_DEVICE_ANGLE, &device)) {
            return nullptr;
        }
    }

    return reinterpret_cast<void*>(device);
}

ANGLEGLWindowContext_win::ANGLEGLWindowContext_win(HWND wnd, const DisplayParams& params)
        : INHERITED(params), fHWND(wnd) {
    this->initializeContext();
}

ANGLEGLWindowContext_win::~ANGLEGLWindowContext_win() { this->destroyContext(); }

sk_sp<const GrGLInterface> ANGLEGLWindowContext_win::onInitializeContext() {
    HDC dc = GetDC(fHWND);
    fDisplay = get_angle_egl_display(dc);
    if (EGL_NO_DISPLAY == fDisplay) {
        return nullptr;
    }

    EGLint majorVersion;
    EGLint minorVersion;
    if (!eglInitialize(fDisplay, &majorVersion, &minorVersion)) {
        SkDebugf("Could not initialize display!\n");
        return nullptr;
    }
    EGLint numConfigs;
    fSampleCount = this->getDisplayParams().fMSAASampleCount;
    const int sampleBuffers = fSampleCount > 1 ? 1 : 0;
    const int eglSampleCnt = fSampleCount > 1 ? fSampleCount : 0;
    const EGLint configAttribs[] = {EGL_RENDERABLE_TYPE,
                                    // We currently only support ES3.
                                    EGL_OPENGL_ES3_BIT,
                                    EGL_RED_SIZE,
                                    8,
                                    EGL_GREEN_SIZE,
                                    8,
                                    EGL_BLUE_SIZE,
                                    8,
                                    EGL_ALPHA_SIZE,
                                    8,
                                    EGL_SAMPLE_BUFFERS,
                                    sampleBuffers,
                                    EGL_SAMPLES,
                                    eglSampleCnt,
                                    EGL_NONE};

    EGLConfig surfaceConfig;
    if (!eglChooseConfig(fDisplay, configAttribs, &surfaceConfig, 1, &numConfigs)) {
        SkDebugf("Could not create choose config!\n");
        return nullptr;
    }
    // We currently only support ES3.
    const EGLint contextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
    fEGLContext = eglCreateContext(fDisplay, surfaceConfig, nullptr, contextAttribs);
    if (EGL_NO_CONTEXT == fEGLContext) {
        SkDebugf("Could not create context!\n");
        return nullptr;
    }
    fEGLSurface = eglCreateWindowSurface(fDisplay, surfaceConfig, fHWND, nullptr);
    if (EGL_NO_SURFACE == fEGLSurface) {
        SkDebugf("Could not create surface!\n");
        return nullptr;
    }
    if (!eglMakeCurrent(fDisplay, fEGLSurface, fEGLSurface, fEGLContext)) {
        SkDebugf("Could not make contxt current!\n");
        return nullptr;
    }

    sk_sp<const GrGLInterface> angle_interface(GrGLMakeAssembledInterface(
            nullptr,
            [](void* ctx, const char name[]) -> GrGLFuncPtr { return eglGetProcAddress(name); }));
    if (angle_interface) {
        angle_interface->fFunctions.fClearStencil(0);
        angle_interface->fFunctions.fClearColor(0, 0, 0, 0);
        angle_interface->fFunctions.fStencilMask(0xffffffff);
        angle_interface->fFunctions.fClear(GR_GL_STENCIL_BUFFER_BIT | GR_GL_COLOR_BUFFER_BIT);

        // use DescribePixelFormat to get the stencil depth.
        int pixelFormat = GetPixelFormat(dc);
        PIXELFORMATDESCRIPTOR pfd;
        DescribePixelFormat(dc, pixelFormat, sizeof(pfd), &pfd);
        fStencilBits = pfd.cStencilBits;

        RECT rect;
        GetClientRect(fHWND, &rect);
        fWidth = rect.right - rect.left;
        fHeight = rect.bottom - rect.top;
        angle_interface->fFunctions.fViewport(0, 0, fWidth, fHeight);
    }
    return angle_interface;
}

void ANGLEGLWindowContext_win::onDestroyContext() {
    eglMakeCurrent(fDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (EGL_NO_CONTEXT != fEGLContext) {
        eglDestroyContext(fDisplay, fEGLContext);
    }
    if (EGL_NO_SURFACE != fEGLSurface) {
        eglDestroySurface(fDisplay, fEGLSurface);
    }
    if (EGL_NO_DISPLAY != fDisplay) {
        eglTerminate(fDisplay);
    }
}

void ANGLEGLWindowContext_win::onSwapBuffers() {
    if (!eglSwapBuffers(fDisplay, fEGLSurface)) {
        SkDebugf("Could not complete eglSwapBuffers.\n");
    }
}

void* ANGLEGLWindowContext_win::makeD3D11Texture2D(int w, int h) {
    ID3D11Device* d3dDevice = (ID3D11Device*)QueryDeviceObjectFromANGLE(fDisplay);
    if (!d3dDevice) {
        return nullptr;
    }
    CD3D11_TEXTURE2D_DESC desc(DXGI_FORMAT_R8G8B8A8_UNORM, w, h, 1, 1,
                               D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    ID3D11Texture2D* newTexture;
    d3dDevice->CreateTexture2D(&desc, nullptr, &newTexture);
    return newTexture;
}

}  // anonymous namespace

namespace skwin_app {
namespace window_context_factory {

std::unique_ptr<WindowContext> MakeANGLEForWin(HWND wnd, const DisplayParams& params) {
    std::unique_ptr<WindowContext> ctx(new ANGLEGLWindowContext_win(wnd, params));
    if (!ctx->isValid()) {
        return nullptr;
    }
    return ctx;
}

}  // namespace window_context_factory
}  // namespace skwin_app
