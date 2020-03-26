/*
 * Copyright 2018 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/**************************************************************************************************
 *** This file was autogenerated from GrRadialGradientLayout.fp; do not modify.
 **************************************************************************************************/
#ifndef GrRadialGradientLayout_DEFINED
#define GrRadialGradientLayout_DEFINED
#include "include/core/SkTypes.h"

#include "src/gpu/gradients/GrGradientShader.h"
#include "src/shaders/gradients/SkRadialGradient.h"

#include "src/gpu/GrCoordTransform.h"
#include "src/gpu/GrFragmentProcessor.h"
class GrRadialGradientLayout : public GrFragmentProcessor {
public:
    static std::unique_ptr<GrFragmentProcessor> Make(const SkRadialGradient& gradient,
                                                     const GrFPArgs& args);
    GrRadialGradientLayout(const GrRadialGradientLayout& src);
    std::unique_ptr<GrFragmentProcessor> clone() const override;
    const char* name() const override { return "RadialGradientLayout"; }
    GrCoordTransform fCoordTransform0;
    SkMatrix44 gradientMatrix;

private:
    GrRadialGradientLayout(SkMatrix44 gradientMatrix)
            : INHERITED(kGrRadialGradientLayout_ClassID,
                        (OptimizationFlags)kPreservesOpaqueInput_OptimizationFlag)
            , fCoordTransform0(gradientMatrix)
            , gradientMatrix(gradientMatrix) {
        this->addCoordTransform(&fCoordTransform0);
    }
    GrGLSLFragmentProcessor* onCreateGLSLInstance() const override;
    void onGetGLSLProcessorKey(const GrShaderCaps&, GrProcessorKeyBuilder*) const override;
    bool onIsEqual(const GrFragmentProcessor&) const override;
    GR_DECLARE_FRAGMENT_PROCESSOR_TEST
    typedef GrFragmentProcessor INHERITED;
};
#endif