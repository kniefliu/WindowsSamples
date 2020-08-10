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

#ifdef SK_BUILD_FOR_WIN
#include "experimental/svg/model/SkSVGDOM.h"
#endif

using namespace sk_app;

 char svg_data[] = {
 R"(
<svg id="layer_1" data-name="layer 1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 92 30.67">
    <title>logo</title>
    <path d="M37.55,7.65a.3.3,0,0,0-.29-.3H34.35a.31.31,0,0,0-.3.3V23a.3.3,0,0,0,.3.3h2.91a.29.29,0,0,0,.29-.3Z" fill="#23d41e"/>
    <path d="M14.89,23.28H13.52c-.33,0-.51,0-.55-.1a2.53,2.53,0,0,0,1.72-2.1V9.56c0-1.46-2-2.66-4.44-2.66S5.82,8.1,5.82,9.56V21.08c0,1.47,2,2.67,4.43,2.67a8.43,8.43,0,0,0,1-.08,1.23,1.23,0,0,0,.38.85,2.29,2.29,0,0,0,1.61.46h1.62a.29.29,0,0,0,.3-.29V23.58a.27.27,0,0,0-.3-.3m-4.63-1.34c-.55,0-1-.33-1-.74V9.56c0-.4.43-.73,1-.73s1,.33,1,.73V21.2c0,.41-.44.74-1,.74" fill="#23d41e"/>
    <path d="M3.21,7.35H.29a.31.31,0,0,0-.29.3v2.94a.29.29,0,0,0,.29.3H3.21a.29.29,0,0,0,.29-.3V7.65a.3.3,0,0,0-.29-.3" fill="#23d41e"/>
    <path d="M20.23,7.38H17.32a.29.29,0,0,0-.29.29V23a.29.29,0,0,0,.29.3h2.91a.29.29,0,0,0,.29-.3V7.67a.29.29,0,0,0-.29-.29" fill="#23d41e"/>
    <path d="M3.21,12.55H.29a.3.3,0,0,0-.29.29V23a.3.3,0,0,0,.29.3H3.21A.29.29,0,0,0,3.5,23V12.84a.29.29,0,0,0-.29-.29" fill="#23d41e"/>
    <path d="M32.73,7.75a.36.36,0,0,0,0-.1c0-.15-.18-.27-.33-.29H29.49a.3.3,0,0,0-.29.2l-1.91,5-1.92-5a.3.3,0,0,0-.26-.18H22c-.15,0-.21.14-.21.29s0,.1,0,.1l3.65,9.53V23h0a.29.29,0,0,0,.29.3h3a.29.29,0,0,0,.29-.3h0V17.28Z" fill="#23d41e"/>
    <path d="M37.54,5.86a5.65,5.65,0,0,0-.79-2.62A3.92,3.92,0,0,0,35,1.81C32.94.84,26.46,0,18.78,0h0C11.1,0,4.61.84,2.53,1.81A3.92,3.92,0,0,0,.8,3.24,5.65,5.65,0,0,0,0,5.86c0,.09,0,.28.22.28H3.3c.2,0,.22-.15.26-.26a3.23,3.23,0,0,1,.73-1.22A2.82,2.82,0,0,1,6,3.87a100.42,100.42,0,0,1,12.74-.73,100.26,100.26,0,0,1,12.73.73,2.77,2.77,0,0,1,1.74.78A3,3,0,0,1,34,5.87c0,.11.06.26.26.26h3.08c.18,0,.21-.19.21-.28" fill="#23d41e"/>
    <path d="M37.54,24.82a5.58,5.58,0,0,1-.79,2.61A4,4,0,0,1,35,28.87c-2.08,1-8.56,1.8-16.24,1.8h0c-7.68,0-14.17-.84-16.25-1.8A4,4,0,0,1,.8,27.43,5.58,5.58,0,0,1,0,24.82a.23.23,0,0,1,.22-.28H3.3c.2,0,.22.14.26.25A3.34,3.34,0,0,0,4.29,26,2.92,2.92,0,0,0,6,26.8a100.59,100.59,0,0,0,12.74.72,100.42,100.42,0,0,0,12.73-.72A2.87,2.87,0,0,0,33.25,26,3.12,3.12,0,0,0,34,24.8c0-.11.06-.25.26-.25h3.08c.18,0,.21.17.21.28" fill="#23d41e"/>
    <path d="M67.82,10.43a.75.75,0,0,0-.74.59.81.81,0,0,0,.69,1,22.87,22.87,0,0,1,5.29,1.64,1.14,1.14,0,0,0,.42.09.64.64,0,0,0,.66-.41.77.77,0,0,0,0-.45.85.85,0,0,0-.23-.44,2.37,2.37,0,0,0-.3-.23,21,21,0,0,0-5.57-1.73l-.25,0" fill="#23d41e"/>
    <path d="M60.05,13.63c3.64-.22,6-1.36,6.91-3.41h6c.26,0,.67-.1.67-.79s-.41-.74-.67-.74H67.31A4.2,4.2,0,0,0,67.38,8,.64.64,0,0,0,67,7.32a.93.93,0,0,0-1,.22,1,1,0,0,0-.14.32s-.11.55-.14.79a.05.05,0,0,0,0,0H60.62a.69.69,0,0,0-.79.74.71.71,0,0,0,.79.79h4.57s-.12.13-.14.16l-.15.15-.3.3a4,4,0,0,1-.78.51,6.48,6.48,0,0,1-1,.38c-.42.1-.85.18-1.28.24s-1,.11-1.46.15a.85.85,0,0,0-.76.58.71.71,0,0,0,.27.81,1.09,1.09,0,0,0,.5.13" fill="#23d41e"/>
    <path d="M56.73,21.63a13.69,13.69,0,0,1-4.82-1.1A7.53,7.53,0,0,0,54.11,18c.3-.63.36-1.1.16-1.42a1.07,1.07,0,0,0-1-.4H48a6.53,6.53,0,0,0,.3-.79h6.49c.63-.06.8-.4.83-.69l.07,0a1.3,1.3,0,0,0,.31.06.75.75,0,0,0,.7-.54,3.17,3.17,0,0,0,0-2.66,1,1,0,0,0-.84-.38h-2a4.17,4.17,0,0,0,.37-.73,3.14,3.14,0,0,0,.27-.78.67.67,0,0,0,0-.42.49.49,0,0,0-.11-.14A9,9,0,0,0,55.41,9,1.49,1.49,0,0,0,56,8.8a.77.77,0,0,0,.3-.68.72.72,0,0,0-.4-.57,1,1,0,0,0-.39-.08h-.15a50.92,50.92,0,0,1-8.44.65c-1.07,0-2.19,0-3.31-.08-.69,0-.88.35-.92.67s.15.72.83.79a3.92,3.92,0,0,0,.74.06,1.12,1.12,0,0,0,0,.2.9.9,0,0,0,.06.22,5.94,5.94,0,0,0,.58.81c.11.17.19.29.25.4H43.35c-.54,0-1.18.23-1.18,1.32V13.8a.72.72,0,0,0,.79.8.7.7,0,0,0,.3-.06v0a.75.75,0,0,0,.78.8h2.5A9.65,9.65,0,0,1,42.18,20a.82.82,0,0,0-.37,1.18h0v0a.88.88,0,0,0,.72.31,3.25,3.25,0,0,0,1.67-.82,9,9,0,0,0,1.44-1.25c.13-.13.24-.26.35-.39s.35-.4.34-.4a11.19,11.19,0,0,0,2.35,2.1,13.67,13.67,0,0,1-4.73,1,.93.93,0,0,0-.76.43.76.76,0,0,0,.63,1.15,14.54,14.54,0,0,0,1.51-.17,23.21,23.21,0,0,0,2.71-.65,10.21,10.21,0,0,0,1.16-.44,10.94,10.94,0,0,0,1-.51s.1-.07.11-.06a18.24,18.24,0,0,0,6.3,1.65.77.77,0,0,0,.08-1.53M53.19,9.24a.87.87,0,0,0-.11.18h0A4.69,4.69,0,0,1,52,11.2H50.39a16.79,16.79,0,0,0-.69-1.7c1.34-.07,2.51-.16,3.49-.26M49,12.8h6c.11,0,.27,0,.28.17a6.94,6.94,0,0,1-.08,1,1.17,1.17,0,0,0-.5-.1H48.83a3.69,3.69,0,0,0,.18-1m-.8-3.22a.69.69,0,0,0,.1.5c.22.47.39.84.49,1.12H46.59a.62.62,0,0,0-.06-.56,7.2,7.2,0,0,0-.7-1.06Zm-1.08,4.26h-3a1.1,1.1,0,0,0-.43.08V13.8s-.07-.78,0-.92a.38.38,0,0,1,.3-.08h3.4a6.16,6.16,0,0,1-.3,1m3.08,6a7.85,7.85,0,0,1-2.56-2.12h4.67a.28.28,0,0,1,0,.13c0,.21-.32.77-2.14,2" fill="#23d41e"/>
    <path d="M73.92,14H60a.76.76,0,0,0-.85.71.73.73,0,0,0,.13.55.84.84,0,0,0,.59.27h10.8v5.82c0,.13,0,.52-1,.52a9.7,9.7,0,0,1-1.91-.21,1.1,1.1,0,0,0-.25,0,.69.69,0,0,0-.68.53.85.85,0,0,0,0,.66.73.73,0,0,0,.48.3,12.68,12.68,0,0,0,2.44.26c1.55,0,2.36-.57,2.42-1.7V15.5H74a.74.74,0,0,0,.71-.82.73.73,0,0,0-.79-.71" fill="#23d41e"/>
    <path d="M69.18,16.7a1.1,1.1,0,0,0-.7-.3h-7a1,1,0,0,0-.68.3,1,1,0,0,0-.25.61V20a.92.92,0,0,0,.93.9h7a1.1,1.1,0,0,0,.7-.29,1,1,0,0,0,.24-.61V17.31a1,1,0,0,0-.24-.61m-1.26,2.49c0,.25-.11.36-.36.36h-5.1c-.26,0-.36-.11-.36-.36v-1c0-.26.1-.36.36-.36h5.1c.25,0,.36.1.36.36Z" fill="#23d41e"/>
    <path d="M92,9.72c-.07-.62-.44-.71-.66-.71H87.77V8c0-.26-.09-.67-.72-.67s-.78.41-.78.67V9h-4V8c0-.26-.09-.67-.72-.67s-.78.41-.78.67V9H76.94c-.24,0-.66.09-.66.73s.42.73.66.73h3.85V12c0,.23.09.61.72.61s.72-.38.72-.61V10.47h4V12c0,.23.1.61.72.61s.72-.38.72-.61V10.47h3.63c.22,0,.59-.09.66-.72" fill="#23d41e"/>
    <path d="M90,18.77a.79.79,0,0,0-.73.68,1.53,1.53,0,0,0,0,.37A2.33,2.33,0,0,1,89,21.3c-.27.3-.66.22-1.11.23H82.18c-.46,0-.91,0-1.37,0q-.34,0-.69-.06a1,1,0,0,1-.56-.2.37.37,0,0,1-.11-.37.57.57,0,0,1,.27-.3c.51-.29,9.77-5.75,9.93-5.89a1,1,0,0,0,.4-1.15A1.07,1.07,0,0,0,89,13H78.39a.7.7,0,0,0-.8.75.76.76,0,0,0,.87.71h8.83l-.59.33c-.39.21-5.35,3-5.88,3.34l-1.33.79a7,7,0,0,0-1.09.79,2.57,2.57,0,0,0-.73,1,1.37,1.37,0,0,0-.07.34,1.73,1.73,0,0,0,.07.64,1.94,1.94,0,0,0,1.24,1.25,18.79,18.79,0,0,0,4.45.25h4.71c1.19,0,1.58-.21,2-.66a3.75,3.75,0,0,0,.64-2.72,1,1,0,0,0-.5-.92.46.46,0,0,0-.23-.05" fill="#23d41e"/>
</svg>)"
};

#include "example/SkottieData.inc"



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

void HelloWorld::onBackendCreated() {
    this->updateTitle();
    fWindow->show();
    fWindow->inval();

#ifdef SK_BUILD_FOR_WIN
    std::unique_ptr<SkMemoryStream> svg_stream = SkMemoryStream::MakeCopy(svg_data, strlen(svg_data));
    fDom = SkSVGDOM::MakeFromStream(*svg_stream);
#endif
    
    //fAnimation = skottie::Animation::Builder()
    //                         .makeFromFile(R"(skia_path\resources\skottie\skottie_sample_search.json)");
    fAnimation = skottie::Animation::Builder().make(skottie_sample_search,
                                                    strlen(skottie_sample_search));
}

void HelloWorld::onPaint(SkSurface* surface) {
    auto canvas = surface->getCanvas();

    // Clear background
    canvas->clear(SK_ColorWHITE);

#ifdef SK_BUILD_FOR_WIN
    if (fDom) {
        fDom->setContainerSize(SkSize::Make(200, 200));
        fDom->render(canvas);
    }
#endif

    if (fAnimation) {
        auto dest = SkRect::MakeXYWH(400, 100, kSkottieSize, kSkottieSize);
        fAnimation->render(canvas, &dest);
    }

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
double nticks() {
    typedef std::chrono::high_resolution_clock clock;
    typedef std::chrono::duration<float, std::nano> duration;

    static clock::time_point start = clock::now();
    duration elapsed = clock::now() - start;
    return elapsed.count();
}
void HelloWorld::onIdle() {
    // Just re-paint continously
    fWindow->inval();

    if (fAnimation) {
        const auto duration = fAnimation->duration();
        fAnimation->seek(std::fmod(1e-9 * nticks(), duration) / duration);
    }
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
