#include "geometry.h"
vec3 cross(const vec3& v1, const vec3& v2) {
    return { v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x };
}


vec3 reflect(const vec3& I, const vec3& N) {
    return I - N * 2.f * (I * N);
}

vec3 refract(const vec3& I, const vec3& N, const float eta_t, const float eta_i ) { // Snell's law
    float cosi = -std::max((double)-1.f, std::min((double)1.f, I * N));
    if (cosi < 0) return refract(I, ( -1)* N, eta_i, eta_t); // if the ray comes from the inside the object, swap the air and the media
    float eta = eta_i / eta_t;
    float k = 1 - eta * eta * (1 - cosi * cosi);
    return k < 0 ? vec3{ 1,0,0 } : I * eta + N * (eta * cosi - std::sqrt(k)); // k<0 = total reflection, no ray to refract. I refract it anyways, this has no physical meaning
}


#pragma region 射线与面是否相交
bool IsRayTriangleCollision(const vec3& orig, const vec3& dir, const std::vector<vec3>& triange, float& HitDistance, vec3& WorldNormal) {
    //std::cout << "检查" << std::endl;
    float t, u, v;
    vec3  E1 = triange[1] - triange[0];
    vec3  E2 = triange[2] - triange[0];
    vec3  P = cross(dir, E2);
    float det = E1 * P;
    vec3 T = (det > 0) ? (orig - triange[0]) : (triange[0] - orig);
    det = fabs(det);
    
    if (det < 0.00001f) return false;// 原来写0.01f少面了

    u = T * P;

    if (u < 0.0f || u > det) return false;

    vec3 Q = cross(T, E1);
    v = dir * Q;
    t = E2 * Q;
    float fInvDet = 1.0f / det;
    t *= fInvDet;
    u *= fInvDet;
    v *= fInvDet;

    if (t < 0.0f || u < 0.0f || v < 0.0f || u + v > 1) return false;
    vec3  triangleNormal = cross(E1, E2).normalized();
    HitDistance = t;
    WorldNormal = triangleNormal;

    return true;
}
#pragma endregion
