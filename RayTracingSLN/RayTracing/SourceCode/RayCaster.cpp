#include "RayCaster.h"


//添加AABB包围盒
//2*2*2空间内检测
vec3 sceneSize= vec3{2, 2, 2};
//每个单位格子的尺寸
vec3 unitSize = vec3{ 0.25, 0.25, 0.25 };//所以暂时是8*8*8=272个格子



std::tuple<bool, vec3, vec3, Material> scene_intersect(const vec3& orig, const vec3& dir, std::vector<Model>& models) {
    vec3 pt, N;
    Material material;
    vec3 WorldNormal;
    float  hitDistance;
    float minDistance = INFINITY;
    for (int i = 0; i < models.size(); ++i) {
        for (int j = 0; j < models[i].world_coords.size(); ++j) {
            if (IsRayTriangleCollision(orig, dir, models[i].world_coords[j], hitDistance, WorldNormal)) {
                if (hitDistance < minDistance)//和上次不是一个面
                {
                    minDistance = hitDistance;
                    N = WorldNormal;
                    pt = orig + dir * hitDistance+ WorldNormal*0.0001;//碰撞点朝法线方向移动一点
                    material = models[i].material;
                }
            }
        }
    }
    return { minDistance < 1000, pt, N, material };
}

vec3 cast_ray(const vec3& orig, const vec3& dir, std::vector<Model>& models,std::vector<vec3>& lights, const int depth) {
    auto [hit, point, N, material] = scene_intersect(orig, dir, models);
    if (depth > 2 || !hit)
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
        diffuse_light_intensity += std::max((double)0.f, light_dir * N);
        specular_light_intensity += std::pow(std::max((double)0, (-1) * reflect((- 1)*light_dir, N) * dir), material.specular_exponent);
    }
    return material.diffuse_color * diffuse_light_intensity * material.albedo[0] + vec3{ 1., 1., 1. }*specular_light_intensity * material.albedo[1] + reflect_color * material.albedo[2] + refract_color * material.albedo[3];
}