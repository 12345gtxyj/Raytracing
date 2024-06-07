#pragma once
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include "geometry.h"
class LoadObj {
    std::vector<vec3> verts{};     // array of vertices
    std::vector<vec3> norms{};     // per-vertex array of normal vectors
    std::vector<int> facet_vrt{};
    std::vector<int> facet_tex{};  // per-triangle indices in the above arrays
    std::vector<int> facet_nrm{};
public:
    LoadObj(const std::string filename);
    int nverts() const;
    int nfaces() const;
    vec3 normal(const int iface, const int nthvert) const; // per triangle corner normal vertex
    vec3 vert(const int i) const;
    vec3 vert(const int iface, const int nthvert) const;
};