//
// Created by Shiina Miyuki on 2019/3/3.
//

#include "volpath.h"
#include "utils/thread.h"
#include "core/scene.h"
#include "math/sampling.h"
#include "core/progress.h"
#include "thirdparty/hilbert/hilbert_curve.hpp"
#include "samplers/sobol.h"

namespace Miyuki {
    void VolPath::render(Scene &scene) {
        fmt::print("Integrator: Volumetric Path Tracer\nSamples per pixel:{}\n", spp);
        auto &film = *scene.film;
        Point2i nTiles = film.imageDimension() / TileSize + Point2i{1, 1};

        // 2^M >= nTiles.x() * nTiles.y()
        // M >= log2(nTiles.x() * nTiles.y());
        int M = std::ceil(std::log2(nTiles.x() * nTiles.y()));
        nTiles = Point2i(std::pow(2, M / 2), std::pow(2, M / 2));
        std::mutex mutex;
        ProgressReporter reporter(nTiles.x() * nTiles.y(), [&](int cur, int total) {
            if (spp > 16) {
                if (cur % 16 == 0) {
                    std::lock_guard<std::mutex> lockGuard(mutex);
                    if (reporter.count() % 16 == 0) {
                        fmt::print("Rendered tiles: {}/{} Elapsed:{} Remaining:{}\n",
                                   cur,
                                   total, reporter.elapsedSeconds(), reporter.estimatedTimeToFinish());
                        scene.update();
                    }
                }
            }
        });
        int dim = 4 + 10 * maxDepth;
        InitSobolSamples(dim);
        std::vector<Seed> seeds(Thread::pool->numThreads());
        Thread::parallelFor2D(nTiles, [&](Point2i tile, uint32_t threadId) {
            scene.arenas[threadId].reset();
            int tileIndex = tile.x() + nTiles.x() * tile.y();
            int tx, ty;
            ::d2xy(M, tileIndex, tx, ty);
            if (tx >= film.width() || ty >= film.height())
                return;
            for (int i = 0; i < TileSize; i++) {
                for (int j = 0; j < TileSize; j++) {
                    if (!scene.processContinuable()) {
                        return;
                    }
                    int x = tx * TileSize + i;
                    int y = ty * TileSize + j;
                    if (x >= film.width() || y >= film.height())
                        continue;
                    auto raster = Point2i{x, y};
                    SobolSampler sampler(&seeds[threadId], dim);
                    for (int s = 0; s < spp; s++) {
                        auto ctx = scene.getRenderContext(raster, &scene.arenas[threadId], &sampler);
                        auto Li = removeNaNs(L(ctx, scene));
                        Li = clampRadiance(Li, maxRayIntensity);
                        film.addSample({x, y}, Li, ctx.weight);
                    }
                }
            }
            reporter.update();
        });
        scene.update();
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
        maxRayIntensity = set.findFloat("volpath.maxRayIntensity", 100.0f);
        caustics = set.findInt("volpath.caustics", true);
    }
}