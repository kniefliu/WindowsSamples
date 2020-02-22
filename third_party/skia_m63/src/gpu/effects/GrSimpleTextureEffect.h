/*
 * Copyright 2017 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/*
 * This file was autogenerated from GrSimpleTextureEffect.fp; do not modify.
 */
#ifndef GrSimpleTextureEffect_DEFINED
#define GrSimpleTextureEffect_DEFINED
#include "SkTypes.h"
#if SK_SUPPORT_GPU
#include "GrFragmentProcessor.h"
#include "GrCoordTransform.h"
#include "GrColorSpaceXform.h"
class GrSimpleTextureEffect : public GrFragmentProcessor {
public:
    sk_sp<GrColorSpaceXform> colorXform() const { return fColorXform; }
    SkMatrix44 matrix() const { return fMatrix; }

    static std::unique_ptr<GrFragmentProcessor> Make(sk_sp<GrTextureProxy> proxy,
                                                     sk_sp<GrColorSpaceXform>
                                                             colorSpaceXform,
                                                     const SkMatrix& matrix) {
        return std::unique_ptr<GrFragmentProcessor>(
                new GrSimpleTextureEffect(std::move(proxy), std::move(colorSpaceXform), matrix,
                                          GrSamplerState(GrSamplerState::WrapMode::kClamp,
                                                         GrSamplerState::Filter::kNearest)));
    }

    /* clamp mode */
    static std::unique_ptr<GrFragmentProcessor> Make(sk_sp<GrTextureProxy> proxy,
                                                     sk_sp<GrColorSpaceXform>
                                                             colorSpaceXform,
                                                     const SkMatrix& matrix,
                                                     GrSamplerState::Filter filter) {
        return std::unique_ptr<GrFragmentProcessor>(new GrSimpleTextureEffect(
                std::move(proxy), std::move(colorSpaceXform), matrix,
                GrSamplerState(GrSamplerState::WrapMode::kClamp, filter)));
    }

    static std::unique_ptr<GrFragmentProcessor> Make(sk_sp<GrTextureProxy> proxy,
                                                     sk_sp<GrColorSpaceXform>
                                                             colorSpaceXform,
                                                     const SkMatrix& matrix,
                                                     const GrSamplerState& p) {
        return std::unique_ptr<GrFragmentProcessor>(
                new GrSimpleTextureEffect(std::move(proxy), std::move(colorSpaceXform), matrix, p));
    }
    GrSimpleTextureEffect(const GrSimpleTextureEffect& src);
    std::unique_ptr<GrFragmentProcessor> clone() const override;
    const char* name() const override { return "SimpleTextureEffect"; }

private:
    GrSimpleTextureEffect(sk_sp<GrTextureProxy> image, sk_sp<GrColorSpaceXform> colorXform,
                          SkMatrix44 matrix, GrSamplerState samplerParams)
            : INHERITED(kGrSimpleTextureEffect_ClassID,
                        (OptimizationFlags)kCompatibleWithCoverageAsAlpha_OptimizationFlag |
                                (GrPixelConfigIsOpaque(image->config())
                                         ? kPreservesOpaqueInput_OptimizationFlag
                                         : kNone_OptimizationFlags))
            , fImage(std::move(image), samplerParams)
            , fColorXform(colorXform)
            , fMatrix(matrix)
            , fImageCoordTransform(matrix, fImage.proxy()) {
        this->addTextureSampler(&fImage);
        this->addCoordTransform(&fImageCoordTransform);
    }
    GrGLSLFragmentProcessor* onCreateGLSLInstance() const override;
    void onGetGLSLProcessorKey(const GrShaderCaps&, GrProcessorKeyBuilder*) const override;
    bool onIsEqual(const GrFragmentProcessor&) const override;
    GR_DECLARE_FRAGMENT_PROCESSOR_TEST
    TextureSampler fImage;
    sk_sp<GrColorSpaceXform> fColorXform;
    SkMatrix44 fMatrix;
    GrCoordTransform fImageCoordTransform;
    typedef GrFragmentProcessor INHERITED;
};
#endif
#endif
