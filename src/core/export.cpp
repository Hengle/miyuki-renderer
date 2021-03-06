// MIT License
// 
// Copyright (c) 2019 椎名深雪
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.



#include "export.h"
#include "accelerators/embree-backend.h"
#include "accelerators/sahbvh.h"
#include "bsdfs/diffusebsdf.h"
#include "bsdfs/microfacet.h"
#include "bsdfs/mixbsdf.h"
#include "cameras/perspective-camera.h"
#include "integrators/rtao.h"
#include "integrators/pt.h"
#include "integrators/guided-pt.h"
#include "samplers/random-sampler.h"
#include "shaders/common-shader.h"
#include "shaders/expr-shader.h"
#include "mesh-importers/wavefront-importer.h"
#include "samplers/sobol-sampler.h"
#include <miyuki.renderer/graph.h>
#include <miyuki.renderer/material.h>
#include <miyuki.renderer/mesh.h>
#include "lightdistributions/uniformlightdistribution.h"
#include "denoisers/oidndenoiser.h"

namespace miyuki::core {
    std::shared_ptr<SerializeContext>  Initialize() {
        auto ctx = std::make_shared<SerializeContext>();
        ctx->registerType<Material>();
        ctx->registerType<SceneGraph>();
        ctx->registerType<BVHAccelerator>();
        ctx->registerType<Mesh>();
        ctx->registerType<MeshInstance>();
        ctx->registerType<FloatShader>();
        ctx->registerType<RGBShader>();
        ctx->registerType<ImageTextureShader>();
        ctx->registerType<PerspectiveCamera>();
        ctx->registerType<DiffuseBSDF>();
        ctx->registerType<MicrofacetBSDF>();
        ctx->registerType<MixBSDF>();
        ctx->registerType<RTAO>();
        ctx->registerType<PathTracer>();
        ctx->registerType<GuidedPathTracer>();
        ctx->registerType<RandomSampler>();
        ctx->registerType<SobolSampler>();
        ctx->registerType<EmbreeAccelerator>();
        ctx->registerType<WavefrontImporter>();
        ctx->registerType<UniformLightDistribution>();
        ctx->registerType<ColorRamp>();
        ctx->registerType<ExprShader>();
        ctx->registerType<MathShader>();
        ctx->registerType<MathShaderAdd>();
        ctx->registerType<MathShaderMul>();
        ctx->registerType<NoiseShader>();
        ctx->registerType<SeparateX>();
        ctx->registerType<SeparateY>();
        ctx->registerType<SeparateZ>();
        ctx->registerType<OIDNDenoiser>();
        return ctx;
    }

} // namespace miyuki::core