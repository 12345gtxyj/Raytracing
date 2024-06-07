#pragma once
#include <vector>
#include "Material.h"
#include "geometry.h"
struct Model {
    std::vector<std::vector<vec3>> world_coords;
    Material material;
    Model(std::vector<std::vector<vec3>> world_coords, Material material)
    {
        this->world_coords = world_coords;
        this->material = material;
    }
};