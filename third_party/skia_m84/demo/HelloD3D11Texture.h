/*
* Copyright 2017 Google Inc.
*
* Use of this source code is governed by a BSD-style license that can be
* found in the LICENSE file.
*/

#ifndef HelloWorld_DEFINED
#define HelloWorld_DEFINED

#include "demo/skwin_app/Application.h"
#include "demo/skwin_app/Window.h"

class SkCanvas;

class HelloD3D11Texture : public skwin_app::Application, skwin_app::Window::Layer {
public:
    HelloD3D11Texture(int argc, char** argv, void* platformData);
    ~HelloD3D11Texture() override;

    void onIdle() override;

    void onBackendCreated() override;
    void onPaint(SkSurface*) override;
    bool onChar(SkUnichar c, skui::ModifierKey modifiers) override;

private:
    void updateTitle();

    skwin_app::Window* fWindow;
    skwin_app::Window::BackendType fBackendType;

    SkScalar fRotationAngle;
};

#endif
