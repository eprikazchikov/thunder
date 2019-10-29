#include "handletools.h"

#include <components/camera.h>
#include <resources/mesh.h>

#include <float.h>

#define SIDES 180

Matrix4 HandleTools::s_View;
Matrix4 HandleTools::s_Projection;

template<typename T>
float distanceToSegment(const T &a, const T &b, const T &p) {
    T v = b - a;
    T w = p - a;

    float c1    = w.dot(v);
    if(c1 <= 0.0f) {
        return w.sqrLength();
    }

    float c2    = v.dot(v);
    if( c2 <= c1 ) {
        return (p - b).sqrLength();
    }

    T l = a + v * (c1 / c2);
    return (p - l).sqrLength();
}

HandleTools::HandleTools() {

}

Vector3Vector HandleTools::pointsArc(const Quaternion &rotation, float size, float start, float angle, bool center) {
    Vector3Vector result;
    int sides       = abs(SIDES / 360.0f * angle);
    float theta     = angle / float(sides - 1) * DEG2RAD;
    float tfactor   = tanf(theta);
    float rfactor   = cosf(theta);

    float x = size * cosf(start * DEG2RAD);
    float y = size * sinf(start * DEG2RAD);

    if(center) {
        result.push_back(Vector3());
    }

    for(int i = 0; i < sides; i++) {
        result.push_back(rotation * Vector3(x, 0, y));

        float tx = -y;
        float ty =  x;

        x += tx * tfactor;
        y += ty * tfactor;

        x *= rfactor;
        y *= rfactor;
    }
    return result;
}

float HandleTools::distanceToPoint(const Matrix4 &matrix, const Vector3 &position) {
    Matrix4 mv  = s_View * matrix;
    Vector3 ssp;
    Camera::project(position, mv, s_Projection, ssp);
    ssp.y    = 1.0f - ssp.y;

    Vector2 ss(ssp.x, ssp.y);
    return (Handles::m_sMouse - ss).length();
}

float HandleTools::distanceToPath(const Matrix4 &matrix, const Vector3Vector &points) {
    Matrix4 mv  = s_View * matrix;
    float result    = FLT_MAX;
    bool first      = true;
    Vector2 back;
    for(auto it : points) {
        Vector3 ssp;
        Camera::project(it, mv, s_Projection, ssp);
        ssp.y    = 1.0f - ssp.y;
        Vector2 ss(ssp.x, ssp.y);
        if(!first) {
            result  = std::min(distanceToSegment(back, ss, Handles::m_sMouse), result);
        } else {
            first   = false;
        }
        back    = ss;
    }
    return sqrtf(result);
}

float HandleTools::distanceToMesh(const Matrix4 &matrix, const Mesh *mesh) {
    Mesh::IndexVector indices   = mesh->indices(0);
    Vector3Vector vertices      = mesh->vertices(0);
    if(indices.empty()) {
        return distanceToPath(matrix, vertices);
    }
    Matrix4 mv  = s_View * matrix;
    float result= FLT_MAX;
    if((vertices.size() % 2) == 0) {
        for(uint32_t i = 0; i < indices.size() - 1; i += 2) {
            Vector3 a;
            Camera::project(vertices[indices[i]], mv, s_Projection, a);
            a.y     = 1.0f - a.y;
            Vector3 b;
            Camera::project(vertices[indices[i+1]], mv, s_Projection, b);
            b.y     = 1.0f - b.y;
            Vector2 ssa(a.x, a.y);
            Vector2 ssb(b.x, b.y);
            result  = std::min(distanceToSegment(ssa, ssb, Handles::m_sMouse), result);
        }
    }
    return sqrtf(result);
}

void HandleTools::setViewProjection(const Matrix4 &view, const Matrix4 &projection) {
    s_View          = view;
    s_Projection    = projection;
}
