/*
* Copyright 2017 Google Inc.
*
* Use of this source code is governed by a BSD-style license that can be
* found in the LICENSE file.
*/

#ifndef HelloWorld_DEFINED
#define HelloWorld_DEFINED

#include "tools/sk_app/Application.h"
#include "tools/sk_app/Window.h"

#include "experimental/ffmpeg/SkVideoDecoder.h"

class SkCanvas;

class HelloFFmpeg : public sk_app::Application, sk_app::Window::Layer {
public:
    HelloFFmpeg(int argc, char** argv, void* platformData);
    ~HelloFFmpeg() override;

    void onIdle() override;

    void onBackendCreated() override;
    void onPaint(SkSurface*) override;
    bool onChar(SkUnichar c, skui::ModifierKey modifiers) override;

private:
    void updateTitle();
    void drawVideo(SkCanvas* canvas);
    void loadVideo();

    sk_app::Window* fWindow;
    sk_app::Window::BackendType fBackendType;

    SkScalar fRotationAngle;

    SkVideoDecoder fDecoder;
};

#endif
