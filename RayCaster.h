#pragma once
#include <tuple>
#include "geometry.h"
#include "Material.h"
#include "Model.h"
#include <vector>
#include<cmath>
vec3 cast_ray(const vec3& orig, const vec3& dir, std::vector<Model>& models, std::vector<vec3>& lights, const int depth = 0);
std::tuple<bool, vec3, vec3, Material> scene_intersect(const vec3& orig, const vec3& dir, std::vector<Model>& models);
