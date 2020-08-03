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

#include "gles2/gl2platform.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "egl/eglext_angle.h"
#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"

#include <iostream>

using namespace sk_app;

Application* Application::Create(int argc, char** argv, void* platformData) {
    return new HelloWorld(argc, argv, platformData);
}

HelloWorld::HelloWorld(int argc, char** argv, void* platformData)
        : fBackendType(Window::kANGLE_BackendType)
        , fRotationAngle(0) {
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
    if (!fWindow || fWindow->sampleCount() <= 1) {
        return;
    }

    SkString title("Hello World ");
    title.append(Window::kRaster_BackendType == fBackendType ? "Raster" : "OpenGL");
    fWindow->setTitle(title.c_str());
}

void HelloWorld::onBackendCreated() {
    this->updateTitle();
    fWindow->show();
    fWindow->inval();
}

bool g_first = true;
sk_sp<SkSurface> gpu_surface;
sk_sp<SkSurface> gpu_surface2;
sk_sp<SkSurface> gpu_d3d_surface;

extern void* GetD3D11Texture();

void HelloWorld::onPaint(SkSurface* surface) {
    static int count = 0;
    if (count < 10) {
        std::cout << __FUNCTION__ << " begin..." << std::endl;
    }
    auto canvas = surface->getCanvas();

    // Clear background
    canvas->clear(SK_ColorWHITE);

    if (g_first && canvas->getGrContext()) {
        //{
        //    SkImageInfo image_info =
        //            SkImageInfo::MakeN32(256, 256, SkAlphaType::kOpaque_SkAlphaType);
        //    gpu_surface = SkSurface::MakeRenderTarget(canvas->getGrContext(), SkBudgeted::kNo,
        //                                              image_info);
        //}
        //{
        //    SkImageInfo image_info =
        //            SkImageInfo::MakeN32(128, 128, SkAlphaType::kOpaque_SkAlphaType);
        //    gpu_surface2 = SkSurface::MakeRenderTarget(canvas->getGrContext(), SkBudgeted::kNo,
        //                                               image_info);
        //}
        { 
            gpu_d3d_surface = SkSurface::MakeFromD3D11Texture2D(
                    canvas->getGrContext(), GetD3D11Texture(), kTopLeft_GrSurfaceOrigin,
                    SkColorSpace::MakeSRGB());
        }
        g_first = false;
    }

    if (canvas->getGrContext()) {
        //{
        //    // gpu_surface2->getCanvas()->clear(SK_ColorBLUE);
        //    SkPaint paint;
        //    paint.setColor(SK_ColorBLUE);
        //    gpu_surface2->getCanvas()->drawRect(SkRect::MakeXYWH(0, 0, 50, 50), paint);
        //}
        //{
        //    SkPaint paint;
        //    paint.setAlpha(128);
        //    gpu_surface2->draw(gpu_surface->getCanvas(), 0, 0, &paint);
        //}

        { 
            if (gpu_d3d_surface) {
                SkPaint paint;
                gpu_d3d_surface->getCanvas()->clear(SK_ColorRED);
                gpu_d3d_surface->draw(canvas, 200, 200, &paint);
            }
        }

        //{
        //    SkPaint paint;
        //    gpu_surface->draw(canvas, 0, 0, &paint);
        //}


    }
    if (count < 10) {
        std::cout << __FUNCTION__ << " end" << std::endl;
    }
    count++;
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
        fBackendType = Window::kRaster_BackendType == fBackendType ? Window::kNativeGL_BackendType
                                                                   : Window::kRaster_BackendType;
        fWindow->detach();
        fWindow->attach(fBackendType);
    }
    return true;
}
