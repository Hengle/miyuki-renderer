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

#ifndef MIYUKIRENDERER_RAY_H
#define MIYUKIRENDERER_RAY_H

#include <miyuki.foundation/defs.h>
#include <miyuki.foundation/math.hpp>
#include <miyuki.foundation/vectorize.hpp>

namespace miyuki::core {
    struct Ray {
        Point3f o;
        Vec3f d;
        float tMin, tMax;

        Ray() : tMin(-1), tMax(-1) {}

        Ray(const Point3f &o, const Vec3f &d, Float tMin, Float tMax = MaxFloat)
                : o(o), d(d), tMin(tMin), tMax(tMax) {}
    };

    extern Float RayBias;

    class Shape;

    class BSDF;

    class Material;

    struct MeshTriangle;

    struct Intersection {
        const MeshTriangle *shape = nullptr;
        const Material *material = nullptr;
        float distance = MaxFloat;
        Vec3f wo;
        Point3f p;
        Normal3f Ns, Ng;
        Point2f uv;
        CoordinateSystem localFrame;

        [[nodiscard]] bool hit() const {
            return shape != nullptr;
        }

        void computeLocalFrame() {
            localFrame = CoordinateSystem(Ns);
        }

        [[nodiscard]] Vec3f worldToLocal(const Vec3f &v) const {
            return localFrame.worldToLocal(v);
        }

        [[nodiscard]] Vec3f localToWorld(const Vec3f &v) const {
            return localFrame.localToWorld(v);
        }

        // w should be normalized
        [[nodiscard]] Ray spawnRay(const Vec3f &w) const {
            auto t = RayBias / abs(dot(w, Ng));
            return Ray(p, w, t, MaxFloat);
        }

        [[nodiscard]] Ray spawnTo(const Point3f &p) const {
            return Ray(this->p, (p - this->p), RayBias, 1);
        }
    };

    MYK_VEC_STRUCT_BEGIN(Ray)
        MYK_VEC_MEMBER(tMin)
        MYK_VEC_MEMBER(tMax)
        MYK_VEC_MEMBER(o)
        MYK_VEC_MEMBER(d)
    MYK_VEC_STRUCT_END

    using Ray4 = TRay<4>;
    using Ray8 = TRay<8>;

    MYK_VEC_STRUCT_BEGIN(Intersection)
        MYK_VEC_MEMBER(shape)
        MYK_VEC_MEMBER(material)
        MYK_VEC_MEMBER(distance)
        MYK_VEC_MEMBER(wo)
        MYK_VEC_MEMBER(p)
        MYK_VEC_MEMBER(Ns)
        MYK_VEC_MEMBER(Ng)
        MYK_VEC_MEMBER(uv)
        MYK_VEC_MEMBER(localFrame)
    MYK_VEC_STRUCT_END

    using Intersection4 = TIntersection<4>;
    using Intersection8 = TIntersection<8>;
}
#endif //MIYUKIRENDERER_RAY_H
