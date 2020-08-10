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

void HelloD3D11Texture::onBackendCreated() {
    this->updateTitle();
    fWindow->show();
    fWindow->inval();
    fD3D11TexureSurface = makeD3D11TextureSurface(100, 100);

    {
        SkImageInfo image_info = SkImageInfo::MakeN32(128, 128, SkAlphaType::kOpaque_SkAlphaType);
        fCpuSurface = SkSurface::MakeRaster(image_info);
    }

    SkImageInfo image_info =
                SkImageInfo::MakeN32(256, 256, SkAlphaType::kOpaque_SkAlphaType);
    if (fWindow->getGrContext()) {
        fGpuSurface =
                SkSurface::MakeRenderTarget(fWindow->getGrContext(), SkBudgeted::kNo, image_info);
    } else {
        fGpuSurface = nullptr;
    }
}

void HelloD3D11Texture::onPaint(SkSurface* surface) {
    auto canvas = surface->getCanvas();

    // Clear background
    canvas->clear(SK_ColorWHITE);

    if (fD3D11TexureSurface) {
        SkPaint paint;
        fD3D11TexureSurface->getCanvas()->clear(SK_ColorBLUE);
        fD3D11TexureSurface->draw(canvas, 0, 0, &paint);
    }
    if (fCpuSurface) {
        SkPaint paint;
        //paint.setAlpha(128);
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
        fCpuSurface->draw(canvas, 100, 0, &paint);
    }
    //if (!fGpuSurface) {
    //    SkImageInfo image_info = SkImageInfo::MakeN32(256, 256, SkAlphaType::kOpaque_SkAlphaType);
    //    if (canvas->getGrContext()) {
    //        fGpuSurface = SkSurface::MakeRenderTarget(canvas->getGrContext(), SkBudgeted::kNo,
    //                                                  image_info);
    //    }
    //}
    if (fGpuSurface) {
        SkPaint paint;
        //paint.setAlpha(128);
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
        fGpuSurface->getCanvas()->drawSimpleText(gpuText, strlen(gpuText), SkTextEncoding::kUTF8,
                                                 10, 50, gpuFont, paint);
        fGpuSurface->draw(canvas, 300, 0, &paint);
    }
    return;

    SkPaint paint;
    paint.setColor(SK_ColorRED);

    // Draw a rectangle with red paint
    SkRect rect = SkRect::MakeXYWH(10, 10, 128, 128);
    canvas->drawRect(rect, paint);

    // Set up a linear gradient and draw a circle
    {
        SkPoint linearPoints[] = { { 0, 0 }, { 300, 300 } };
        SkColor linearColors[] = { SK_ColorGREEN, SK_ColorBLACK };
        paint.setShader(SkGradientShader::MakeLinear(linearPoints, linearColors, nullptr, 2,
                                                     SkTileMode::kMirror));
        paint.setAntiAlias(true);

        canvas->drawCircle(200, 200, 64, paint);

        // Detach shader
        paint.setShader(nullptr);
    }

    // Draw a message with a nice black paint
    SkFont font;
    font.setSubpixel(true);
    font.setSize(20);
    paint.setColor(SK_ColorBLACK);

    canvas->save();
    static const char message[] = "Hello D3D11Texture";

    // Translate and rotate
    canvas->translate(300, 300);
    fRotationAngle += 0.2f;
    if (fRotationAngle > 360) {
        fRotationAngle -= 360;
    }
    canvas->rotate(fRotationAngle);

    // Draw the text
    canvas->drawSimpleText(message, strlen(message), SkTextEncoding::kUTF8, 0, 0, font, paint);

    canvas->restore();
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
    return nullptr;
    if (fWindow && fWindow->getGrGLInterface()) {
        void * d3dTexture = fWindow->makeD3D11Texture2D(w, h);
        if (d3dTexture) {
            return SkSurfaceHelper::MakeFromD3D11Texture2D(
                    fWindow->getGrContext(), d3dTexture, w, h, kTopLeft_GrSurfaceOrigin,
                    SkColorSpace::MakeSRGB(), nullptr, fWindow->getGrGLInterface());
        }
        return nullptr;
    }
    return nullptr;
}
