//
// Created by Shiina Miyuki on 2019/3/3.
//

#ifndef MIYUKI_VOLPATH_H
#define MIYUKI_VOLPATH_H

#include <core/scene.h>

#include "integrators/integrator.h"
#include "core/parameter.h"

namespace Miyuki {
    class VolPath : public SamplerIntegrator {
    protected:
        bool progressive;
        int minDepth;
        int maxDepth;
        bool caustics;
        bool adaptive;
        int nLuminanceSample = 100000;
        Float maxSampleFactor = 32;
        Float requiredPValue = 0.05;
        double maxError = 0.05;
        Float heuristic;
    public:
        VolPath(const ParameterSet &set);

        void render(Scene &scene) override;

    protected:
        Spectrum L(RenderContext &ctx, Scene &scene) override {
            return LRandomWalk(ctx, scene);
        }

        Spectrum LPathTraced(RenderContext &ctx, Scene &scene);

        Spectrum LRandomWalk(RenderContext &ctx, Scene &scene);
    };

}
#endif //MIYUKI_VOLPATH_H
