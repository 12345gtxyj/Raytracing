#pragma once
#include <cmath>
#include <vector>
struct vec3 {
    float x = 0, y = 0, z = 0;
    float& operator[](const int i) { return i == 0 ? x : (1 == i ? y : z); }
    const float& operator[](const int i) const { return i == 0 ? x : (1 == i ? y : z); }
    vec3  operator*(const float v) const { return { x * v, y * v, z * v }; }
    float operator*(const vec3& v) const { return x * v.x + y * v.y + z * v.z; }
    vec3  operator+(const vec3& v) const { return { x + v.x, y + v.y, z + v.z }; }
    vec3  operator-(const vec3& v) const { return { x - v.x, y - v.y, z - v.z }; }
    vec3  operator-()              const { return { -x, -y, -z }; }
    float norm() const { return std::sqrt(x * x + y * y + z * z); }
    vec3 normalized() const { return (*this) * (1.f / norm()); }
};


vec3 cross(const vec3 v1, const vec3 v2);
vec3 reflect(const vec3& I, const vec3& N);
vec3 refract(const vec3& I, const vec3& N, const float eta_t, const float eta_i = 1.f);
bool IsRayTriangleCollision(const vec3& orig, const vec3& dir, vec3 v0, vec3 v1, vec3 v2, float& HitDistance, vec3& WorldNormal);
