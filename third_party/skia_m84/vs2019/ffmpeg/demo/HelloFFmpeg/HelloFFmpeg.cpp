/*
* Copyright 2017 Google Inc.
*
* Use of this source code is governed by a BSD-style license that can be
* found in the LICENSE file.
*/

#include "HelloFFmpeg.h"

#include "include/core/SkCanvas.h"
#include "include/core/SkFont.h"
#include "include/core/SkGraphics.h"
#include "include/core/SkSurface.h"
#include "include/effects/SkGradientShader.h"

#include <filesystem>
#include <locale>
#include <codecvt>
#include <string>

#ifdef SK_BUILD_FOR_WIN
#include <Windows.h>
#endif

using namespace sk_app;


Application* Application::Create(int argc, char** argv, void* platformData) {
     return new HelloFFmpeg(argc, argv, platformData);
 }

HelloFFmpeg::HelloFFmpeg(int argc, char** argv, void* platformData)
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

HelloFFmpeg::~HelloFFmpeg() {
    fWindow->detach();
    delete fWindow;
}

void HelloFFmpeg::updateTitle() {
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

void HelloFFmpeg::drawVideo(SkCanvas* canvas) {
    GrContext* gr = canvas->getGrContext();
    if (!gr) {
        return;
    }

    fDecoder.setGrContext(gr);  // gr can change over time in viewer

    double timeStamp;
    auto img = fDecoder.nextImage(&timeStamp);
    if (!img) {
        (void)fDecoder.rewind();
        img = fDecoder.nextImage(&timeStamp);
    }
    if (img) {
        if (0) {
            SkDebugf("ts %g\n", timeStamp);
        }
        canvas->drawImage(img, 10, 10, nullptr);
    }
}

void HelloFFmpeg::loadVideo() {
    std::string mp4Path;
#ifdef SK_BUILD_FOR_WIN
    const int kModuleFileNameMaxLen = 256;
    wchar_t moduleFullFileName[kModuleFileNameMaxLen] = {0};
    ::GetModuleFileName(NULL, moduleFullFileName, kModuleFileNameMaxLen - 1);
    std::filesystem::path stdFullPath = moduleFullFileName;
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    mp4Path = converter.to_bytes(stdFullPath.parent_path().c_str());
#endif
    mp4Path.append("\\flower.mp4");
    std::unique_ptr<SkStream> mp4Stream = SkStream::MakeFromFile(mp4Path.c_str());
    if (!fDecoder.loadStream(std::move(mp4Stream))) {
        SkDebugf("could not load movie file\n");
    }
    SkDebugf("duration %g\n", fDecoder.duration());
}

void HelloFFmpeg::onBackendCreated() {
    this->updateTitle();
    fWindow->show();
    fWindow->inval();
    
    loadVideo();
}

void HelloFFmpeg::onPaint(SkSurface* surface) {
    auto canvas = surface->getCanvas();

    // Clear background
    canvas->clear(SK_ColorWHITE);

    drawVideo(canvas);
}
double nticks() {
    typedef std::chrono::high_resolution_clock clock;
    typedef std::chrono::duration<float, std::nano> duration;

    static clock::time_point start = clock::now();
    duration elapsed = clock::now() - start;
    return elapsed.count();
}
void HelloFFmpeg::onIdle() {
    // Just re-paint continously
    fWindow->inval();
}

bool HelloFFmpeg::onChar(SkUnichar c, skui::ModifierKey modifiers) {
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
