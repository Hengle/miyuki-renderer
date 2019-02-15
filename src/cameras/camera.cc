//
// Created by Shiina Miyuki on 2019/2/13.
//

#include "camera.h"
#include "../core/ray.h"
#include "../samplers/sampler.h"

using namespace Miyuki;

Float Camera::generatePrimaryRay(Sampler &sampler, const Point2i &raster, Ray *ray) {
    Float x = -(2 * (Float) raster.x() / filmDimension.x() - 1) * static_cast<Float>(filmDimension.x()) /
              filmDimension.y();
    Float y = 2 * (1 - (Float) raster.y() / filmDimension.y()) - 1;
    Float dx = 2.0f / filmDimension.y(), dy = 2.0f / filmDimension.y();
    Vec3f ro = viewpoint;
    auto z = (Float) (2.0 / tan(fov / 2));
    Vec3f jitter = Vec3f(dx * sampler.randFloat(), dy * sampler.randFloat(), 0);
    Vec3f rd = Vec3f(x, y, 0) + jitter - Vec3f(0, 0, -z);
    rd.normalize();
    rd.w() = 1;
    rd = matrix.mult(rd);
    rd.normalize();
    *ray = Ray{ro, rd};
//    Float p1, p2;
//    pdfWe(*ray,&p1,&p2);
//    CHECK(p1 > 0 && p2 > 0);
    return 1;
}

void Camera::lookAt(const Vec3f &pos) {
    assert(false);
}

void Camera::initTransformMatrix() {
    matrix = Matrix4x4::rotation(Vec3f(1, 0, 0), -direction.y());
    matrix = matrix.mult(Matrix4x4::rotation(Vec3f(0, 1, 0), direction.x()));
    matrix = matrix.mult(Matrix4x4::rotation(Vec3f(0, 0, 1), direction.z()));
    // matrix = matrix.mult(Matrix4x4::translation(viewpoint));
    Matrix4x4::inverse(matrix, invMatrix);
    /* Now the perspective matrix
     z0 = 2 / tan(fov/2)
     The image plane is from (-w/h, -1, z0) to (w/h, 1, z0)
     For a point p = (x, y, z)
     The projected point p' = (z0 * x/z, z0 *y/z, z0):
     \frac{x'}{x} = \frac{z0}{z}
     \frac{y'}{y} = \frac{z0}{z}
     The homogeneous coordinate is (z0 *x, z0*y, z0 *z ,z)
     */
    auto z0 = (Float) (2.0 / tan(fov / 2));
    Float m[4][4] = {
            {z0, 0,  0,  0},
            {0,  z0, 0,  0},
            {0,  0,  z0, 0},
            {0,  0,  1,  0}
    };
    perspectiveMatrix = Matrix4x4(m);
}

void Camera::pdfWe(const Ray &ray, Float *pdfPos, Float *pdfDir) const {
    auto rd = ray.d;
    rd.w() = 1;
    rd = invMatrix.mult(rd);
    auto cosT = Vec3f::dot(rd, Vec3f(0, 0, 1));
    if (cosT < 0) {
        *pdfPos = *pdfDir = 0;
        return;
    }
    auto z0 = (Float) (2.0 / tan(fov / 2));
    Point2f raster(rd.x() / rd.z() * z0, rd.y() / rd.z() * z0);
    Float lensArea = 1;
    Float A = 2 * (2.0f * filmDimension.x() / filmDimension.y());
    *pdfPos = 1 / lensArea;
    auto cosT2 = cosT * cosT;
    *pdfDir = 1 / (A *lensArea * cosT2 * cosT2);

}