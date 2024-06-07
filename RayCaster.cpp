#include "RayCaster.h"

std::tuple<bool, vec3, vec3, Material> scene_intersect(const vec3& orig, const vec3& dir, std::vector<Model>& models) {
    vec3 pt, N;
    Material material;
    vec3 WorldNormal;
    float  hitDistance;
    float minDistance = INFINITY;
    for (int i = 0; i < models.size(); ++i) {
        for (int j = 0; j < models[i].world_coords.size(); ++j) {
            if (IsRayTriangleCollision(orig, dir, models[i].world_coords[j][0], models[i].world_coords[j][1], models[i].world_coords[j][2], hitDistance, WorldNormal)) {
                if (hitDistance > 0.1 && hitDistance < minDistance)//和上次不是一个面
                {
                    minDistance = hitDistance;
                    N = WorldNormal;
                    pt = orig + dir.normalized() * hitDistance;
                    material = models[i].material;
                }
            }
        }
    }
    return { minDistance < 1000, pt, N, material };
}

vec3 cast_ray(const vec3& orig, const vec3& dir, std::vector<Model>& models,std::vector<vec3>& lights, const int depth) {
    auto [hit, point, N, material] = scene_intersect(orig, dir, models);
    if (depth > 4 || !hit)
        return { 1.0, 1.0, 1.0 }; // background color
   // std::cout << "开始检测" << std::endl;
    vec3 reflect_dir = reflect(dir, N).normalized();
    vec3 refract_dir = refract(dir, N, material.refractive_index).normalized();
    vec3 reflect_color = cast_ray(point, reflect_dir,  models, lights,depth + 1);
    vec3 refract_color = cast_ray(point, refract_dir,  models, lights, depth + 1);

    float diffuse_light_intensity = 0, specular_light_intensity = 0;
    for (const vec3& light : lights) { // checking if the point lies in the shadow of the light
        vec3 light_dir = (light - point).normalized();
        auto [hit, shadow_pt, trashnrm, trashmat] = scene_intersect(point, light_dir, models);
        if (hit && (shadow_pt - point).norm() < (light - point).norm()) continue;
        diffuse_light_intensity += std::max(0.f, light_dir * N);
        specular_light_intensity += std::pow(std::max(0.f, -reflect(-light_dir, N) * dir), material.specular_exponent);
    }
    return material.diffuse_color * diffuse_light_intensity * material.albedo[0] + vec3{ 1., 1., 1. }*specular_light_intensity * material.albedo[1] + reflect_color * material.albedo[2] + refract_color * material.albedo[3];
}