#pragma once
struct Material {
    float refractive_index = 1;
    float albedo[4] = { 2,0,0,0 };
    vec3 diffuse_color = { 0,0,0 };
    float specular_exponent = 0;
};