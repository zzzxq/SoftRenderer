#pragma once
#include "../shader/shader.h"

vec3 texture_sample(vec2 uv, TGAImage* image);
vec3 cubemap_sampling(vec3 direction, cubemap_t* cubemap);


