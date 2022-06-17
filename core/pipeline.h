#pragma once
#include "./global.h"
#include "./maths.h"
//#include "./spainlock.hpp"
#include "../shader/shader.h"
#include "../platform/win32.h"





void Getsbuffer(float* zbuffer, IShader& shader, int nface);

static vec3 cal_normal(vec3& normal, vec3* world_coords, const vec2* uvs, const vec2& uv, TGAImage* normal_map);
void rasterize_singlethread(vec4* clipcoord_attri, unsigned char* framebuffer, float* zbuffer, IShader& shader);
void rasterize_multithread(vec4* clipcoord_attri, unsigned char* framebuffer, float* zbuffer, IShader& shader);
void draw_triangles(unsigned char* framebuffer, float* zbuffer, IShader& shader, int nface);
void rasterize_singlethread_with_shadows(vec4* clipcoord_attri, unsigned char* framebuffer, float* zbuffer, float *sbuffer, IShader& shader, mat4& lightSpaceMatrix, float, float);

void draw_triangles_with_shadows(unsigned char* framebuffer, 
	float* zbuffer, float* sbuffer, IShader& shader, int nface, mat4& lightSpaceMatrix, mat4& viewMat, float, float);

