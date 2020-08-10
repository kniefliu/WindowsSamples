/*
* Copyright 2017 Google Inc.
*
* Use of this source code is governed by a BSD-style license that can be
* found in the LICENSE file.
*/

#include "demo/HelloD3D11Texture.h"

#include "include/core/SkCanvas.h"
#include "include/core/SkFont.h"
#include "include/core/SkGraphics.h"
#include "include/core/SkSurface.h"
#include "include/effects/SkGradientShader.h"

#include "demo/skwin_app/SkSurfaceHelper.h"


using namespace skwin_app;


Application* Application::Create(int argc, char** argv, void* platformData) {
    return new HelloD3D11Texture(argc, argv, platformData);
}

HelloD3D11Texture::HelloD3D11Texture(int argc, char** argv, void* platformData)
        : fBackendType(Window::kNativeGL_BackendType)
        , fRotationAngle(0) {
#ifdef SK_BUILD_FOR_WIN
    fBackendType = Window::kANGLE_BackendType;
#endif
    SkGraphics::Init();

    fWindow = Window::CreateNativeWindow(platformData);
    fWindow->setRequestedDisplayParams(DisplayParams());

    // register callbacks
    fWindow->pushLayer(this);

    fWindow->attach(fBackendType);
}

HelloD3D11Texture::~HelloD3D11Texture() {
    fWindow->detach();
    delete fWindow;
}

void HelloD3D11Texture::updateTitle() {
    if (!fWindow/* || fWindow->sampleCount() <= 1*/) {
        return;
    }

    SkString title("Hello World ");
#ifdef SK_BUILD_FOR_WIN
    title.append(Window::kRaster_BackendType == fBackendType ? "Raster" : "ANGLE");
#else
    title.append(Window::kRaster_BackendType == fBackendType ? "Raster" : "OpenGL");
#endif
    fWindow->setTitle(title.c_str());
}

void HelloD3D11Texture::onResize(int width, int height) {
    fGpuSurface = nullptr;
    fD3D11TexureSurface = nullptr;
    fD3D11Texture = nullptr;
    

    if (width == 0 || height == 0) {

    } else {
        SkImageInfo image_info = SkImageInfo::MakeN32(256, 256, SkAlphaType::kOpaque_SkAlphaType);
        if (fWindow->getGrContext()) {
            fGpuSurface = SkSurface::MakeRenderTarget(fWindow->getGrContext(), SkBudgeted::kNo,
                                                      image_info);
        } else {
            fGpuSurface = nullptr;
        }

        fD3D11TextureNeedToCreate = true;
        // paint error if texture created here.
        //fD3D11TexureSurface = makeD3D11TextureSurface(fD3D11TextureWidth, fD3D11TextureHeight);
    }
}

void HelloD3D11Texture::onBackendCreated() {
    this->updateTitle();
    fWindow->show();
    fWindow->inval();

    // cpu surface
    {
        SkImageInfo image_info = SkImageInfo::MakeN32(128, 128, SkAlphaType::kOpaque_SkAlphaType);
        fCpuSurface = SkSurface::MakeRaster(image_info);
    }
}

void HelloD3D11Texture::onPaint(SkSurface* surface) {
    auto canvas = surface->getCanvas();

    // Clear background
    canvas->clear(SK_ColorWHITE);

    if (fCpuSurface) {
        fCpuSurface->getCanvas()->clear(SK_ColorWHITE);
        SkPaint paint;
        // paint.setAlpha(128);
        paint.setAntiAlias(true);
        SkIRect cpuRect = fCpuSurface->imageInfo().bounds();
        {
            cpuRect.inset(2, 2);
            SkPaint cpuRectPaint;
            cpuRectPaint.setStrokeWidth(2);
            cpuRectPaint.setStyle(SkPaint::kStroke_Style);
            cpuRectPaint.setColor(SK_ColorGRAY);
            fCpuSurface->getCanvas()->drawIRect(cpuRect, cpuRectPaint);
        }
        char cpuText[] = "CPU";
        SkFont cpuFont;
        cpuFont.setSize(40);
        fCpuSurface->getCanvas()->drawSimpleText(cpuText, strlen(cpuText), SkTextEncoding::kUTF8,
                                                 10, 50, cpuFont, paint);
        fCpuSurface->draw(canvas, 300, 300, &paint);
    }

    if (canvas->getGrContext()) {
        if (fD3D11TextureNeedToCreate) {
            fD3D11TexureSurface = makeD3D11TextureSurface(fD3D11TextureWidth, fD3D11TextureHeight);
            fD3D11TextureNeedToCreate = false;
        }
        if (fD3D11TexureSurface) {
            fD3D11TexureSurface->getCanvas()->clear(SK_ColorWHITE);
            SkPaint paint;
            paint.setAntiAlias(true);
            // paint.setAlpha(128);
            SkIRect texRect = fD3D11TexureSurface->imageInfo().bounds();
            {
                texRect.inset(2, 2);
                SkPaint texRectPaint;
                texRectPaint.setStrokeWidth(2);
                texRectPaint.setStyle(SkPaint::kStroke_Style);
                texRectPaint.setColor(SK_ColorGRAY);
                fD3D11TexureSurface->getCanvas()->drawIRect(texRect, texRectPaint);
            }
            char texText[] = "D3D11Texture";
            SkFont texFont;
            texFont.setSize(40);
            fD3D11TexureSurface->getCanvas()->drawSimpleText(
                    texText, strlen(texText),
                                                     SkTextEncoding::kUTF8, 2, 50, texFont, paint);
            fD3D11TexureSurface->flush();
            fD3D11TexureSurface->draw(canvas, 10, 10, &paint);
        }
        if (fGpuSurface) {
            fGpuSurface->getCanvas()->clear(SK_ColorWHITE);
            SkPaint paint;
            // paint.setAlpha(128);
            paint.setAntiAlias(true);
            SkIRect gpuRect = fGpuSurface->imageInfo().bounds();
            {
                gpuRect.inset(2, 2);
                SkPaint gpuRectPaint;
                gpuRectPaint.setStrokeWidth(2);
                gpuRectPaint.setStyle(SkPaint::kStroke_Style);
                gpuRectPaint.setColor(SK_ColorGRAY);
                fGpuSurface->getCanvas()->drawIRect(gpuRect, gpuRectPaint);
            }
            char gpuText[] = "GPU";
            SkFont gpuFont;
            gpuFont.setSize(40);
            fGpuSurface->getCanvas()->drawSimpleText(gpuText, strlen(gpuText),
                                                     SkTextEncoding::kUTF8, 10, 50, gpuFont, paint);
            fGpuSurface->flush();
            fGpuSurface->draw(canvas, 300, 10, &paint);
        }
    }
    return;
}

void HelloD3D11Texture::onIdle() {
    // Just re-paint continously
    fWindow->inval();
}

bool HelloD3D11Texture::onChar(SkUnichar c, skui::ModifierKey modifiers) {
    if (' ' == c) {
#ifdef SK_BUILD_FOR_WIN
        fBackendType = Window::kRaster_BackendType == fBackendType ? Window::kANGLE_BackendType
                                                                   : Window::kRaster_BackendType;
#else
        fBackendType = Window::kRaster_BackendType == fBackendType ? Window::kNativeGL_BackendType
                                                                   : Window::kRaster_BackendType;
#endif
        fWindow->detach();
        fWindow->attach(fBackendType);
    }
    return true;
}

sk_sp<SkSurface> HelloD3D11Texture::makeD3D11TextureSurface(int w, int h) {
    //return nullptr;
    if (fWindow && fWindow->getGrGLInterface()) {
        fD3D11Texture = fWindow->makeD3D11Texture2D(w, h);
        if (fD3D11Texture) {
            return SkSurfaceHelper::MakeFromD3D11Texture2D(
                    fWindow->getGrContext(), fD3D11Texture, w, h, kBottomLeft_GrSurfaceOrigin,
                    SkColorSpace::MakeSRGB(), nullptr, fWindow->getGrGLInterface());
        }
        return nullptr;
    }
    return nullptr;
}
