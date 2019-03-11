//
// Created by Shiina Miyuki on 2019/3/3.
//

#include "volpath.h"
#include "utils/thread.h"
#include "core/scene.h"
#include "math/sampling.h"

#include "samplers/sobol.h"

namespace Miyuki {
    void VolPath::render(Scene &scene) {
        fmt::print("Integrator: Volumetric Path Tracer\nSamples per pixel:{}\n", spp);
        SamplerIntegrator::render(scene);
    }

    Spectrum VolPath::L(RenderContext &ctx, Scene &scene) {
        RayDifferential ray = ctx.primary;
        Intersection intersection;
        ScatteringEvent event;
        Spectrum Li(0, 0, 0);
        Spectrum beta(1, 1, 1);
        bool specular = false;
        for (int depth = 0; depth < maxDepth; depth++) {
            if (!scene.intersect(ray, &intersection)) {
                break;
            }
            makeScatteringEvent(&event, ctx, &intersection);
            if ((caustics && specular) || depth == 0) {
                Li += event.Le(-1 * ray.d) * beta;
            }
            Li += beta * importanceSampleOneLight(scene, ctx, event);
            auto f = event.bsdf->sample(event);
            specular = event.bsdfLobe.matchFlag(BSDFLobe::specular);
            if (event.pdf < 0) {
                break;
            }
            ray = event.spawnRay(event.wiW);
            beta *= f * Vec3f::absDot(event.wiW, event.Ns()) / event.pdf;
            if (depth >= minDepth) {
                Float p = beta.max();
                if (ctx.sampler->get1D() < p) {
                    beta /= p;
                } else {
                    break;
                }
            }
        }
        return Li;
    }

    VolPath::VolPath(const ParameterSet &set) {
        progressive = false;
        minDepth = set.findInt("volpath.minDepth", 3);
        maxDepth = set.findInt("volpath.maxDepth", 5);
        spp = set.findInt("volpath.spp", 4);
        maxRayIntensity = set.findFloat("volpath.maxRayIntensity", 10000.0f);
        caustics = set.findInt("volpath.caustics", true);
    }
}