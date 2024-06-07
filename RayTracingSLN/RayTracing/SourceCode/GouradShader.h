#pragma once
#include "geometry.h"
#include "tgaimage.h"
#include "our_gl.h"
#include "Model.h"
#include "LoadObj.h"
struct GouradShader : IShader {
    const LoadObj& model;
    vec3 uniform_l;       // light direction in view coordinates
    mat<2, 3> varying_uv;  // triangle uv coordinates, written by the vertex shader, read by the fragment shader
    mat<3, 3> varying_nrm; // normal per vertex to be interpolated by FS
    mat<3, 3> view_tri;    // triangle in view coordinates
    mat<4, 4>Viewport;
    mat<4, 4> ModelView;
    mat<4, 4> Projection;
    vec3 light_dir;
    GouradShader(const LoadObj& m,const vec3 light_dir,mat<4, 4> ModelView, mat<4, 4> Projection) : model(m),ModelView(ModelView),Projection(Projection), light_dir(light_dir){
        uniform_l = proj<3>((ModelView * embed<4>(light_dir, 0.))).normalized(); // transform the light vector to view coordinates
    }


    vec3 varying_intensity = vec3{0,0,0}; // written by vertex shader, read by fragment shader
    virtual vec4 vertex(int iface, int nthvert, vec4& gl_Position)
    {
        varying_uv.set_col(nthvert, model.uv(iface, nthvert));
        varying_nrm.set_col(nthvert, proj<3>((ModelView).invert_transpose() * embed<4>(model.normal(iface, nthvert), 0.)));
        gl_Position = ModelView * embed<4>(model.vert(iface, nthvert));
        view_tri.set_col(nthvert, proj<3>(gl_Position));
        gl_Position = Projection * gl_Position;
        varying_intensity[nthvert] = std::max((double)0.f, model.normal(iface, nthvert) * light_dir); // get diffuse lighting intensity
        vec4 gl_Vertex = embed<4>(model.vert(iface, nthvert)); // read the vertex from .obj file
        return Viewport * Projection * ModelView * gl_Vertex; // transform it to screen coordinates
    }
    //Gouraudfragment
    virtual bool fragment(vec3 bar, TGAColor& gl_FragColor) {
        float intensity = varying_intensity * bar;   // interpolate intensity for the current pixel
        if (intensity > .85) intensity = 1;
        else if (intensity > .60) intensity = .80;
        else if (intensity > .45) intensity = .60;
        else if (intensity > .30) intensity = .45;
        else if (intensity > .15) intensity = .30;
        else intensity = 0;
        ///std::cout << bar <<std::endl ;
        TGAColor c = TGAColor{ 0, 155,255 };
        for (int i : {0, 1, 2})
                gl_FragColor[i] = std::min<int>(c[i] * intensity, 255); // (a bit of ambient light, diff + spec), clamp the result
        return false;                              // no, we do not discard this pixel
    }
};