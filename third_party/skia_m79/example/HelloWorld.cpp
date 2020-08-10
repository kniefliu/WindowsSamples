/*
* Copyright 2017 Google Inc.
*
* Use of this source code is governed by a BSD-style license that can be
* found in the LICENSE file.
*/

#include "example/HelloWorld.h"

#include "include/core/SkCanvas.h"
#include "include/core/SkFont.h"
#include "include/core/SkGraphics.h"
#include "include/core/SkSurface.h"
#include "include/effects/SkGradientShader.h"

using namespace sk_app;

Application* Application::Create(int argc, char** argv, void* platformData) {
    return new HelloWorld(argc, argv, platformData);
}

HelloWorld::HelloWorld(int argc, char** argv, void* platformData)
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

HelloWorld::~HelloWorld() {
    fWindow->detach();
    delete fWindow;
}

void HelloWorld::updateTitle() {
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

void HelloWorld::onResize(int width, int height) {
    if (width == 0 || height == 0) {
        fGpuSurface = nullptr;
    } else {
        SkImageInfo image_info = SkImageInfo::MakeN32(256, 256, SkAlphaType::kOpaque_SkAlphaType);
        if (fWindow->getGrContext()) {
            fGpuSurface = SkSurface::MakeRenderTarget(fWindow->getGrContext(), SkBudgeted::kYes,
                                                      image_info);
        } else {
            fGpuSurface = nullptr;
        }
    }
}

void HelloWorld::onBackendCreated() {
    this->updateTitle();
    fWindow->show();
    fWindow->inval();

    {
        SkImageInfo image_info = SkImageInfo::MakeN32(128, 128, SkAlphaType::kOpaque_SkAlphaType);
        fCpuSurface = SkSurface::MakeRaster(image_info);
    }
}

void HelloWorld::onPaint(SkSurface* surface) {
    auto canvas = surface->getCanvas();

    // Clear background
    canvas->clear(SK_ColorWHITE);

    if (fCpuSurface) {
        SkPaint paint;
        // paint.setAlpha(128);
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
        fCpuSurface->flush();
        fCpuSurface->draw(canvas, 100, 0, &paint);
    }

    if (canvas->getGrContext()) {
        // create gpu surface every paint.
        static bool created = false;
        if (false/*!created*/)
        {
            SkImageInfo image_info =
                    SkImageInfo::MakeN32(256, 256, SkAlphaType::kOpaque_SkAlphaType);
            if (fWindow->getGrContext()) {
                fGpuSurface = SkSurface::MakeRenderTarget(fWindow->getGrContext(), SkBudgeted::kNo,
                                                          image_info);
            } else {
                fGpuSurface = nullptr;
            }
            created = true;
        }
        if (fGpuSurface) {
            SkPaint paint;
            // paint.setAlpha(128);
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
            fGpuSurface->getCanvas()->flush();
            fGpuSurface->draw(canvas, 300, 0, &paint);
            canvas->flush();
        }
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
    static const char message[] = "Hello World";

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

void HelloWorld::onIdle() {
    // Just re-paint continously
    fWindow->inval();
}

bool HelloWorld::onChar(SkUnichar c, skui::ModifierKey modifiers) {
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
