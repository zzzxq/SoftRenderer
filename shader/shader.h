#pragma once
#include "../core/global.h"
#include "../core/maths.h"
#include "../core/model.h"
#include "../core/camera.h"


const int WINDOW_HEIGHT = 780;
const int WINDOW_WIDTH = 1280;

static int get_index(int x, int y)
{
	return ((WINDOW_HEIGHT - y - 1) * WINDOW_WIDTH + x);
}


struct light
{
	vec3 pos;
	vec3 intensity;
};

typedef struct cubemap
{
	TGAImage* faces[6];
}cubemap_t;


typedef struct iblmap
{
	int mip_levels;
	cubemap_t* irradiance_map;
	cubemap_t* prefilter_maps[15];
	TGAImage* brdf_lut;
} iblmap_t;

typedef struct
{
	//light_matrix for shadow mapping, (to do)
	mat4 model_matrix;
	mat4 camera_view_matrix;
	mat4 light_view_matrix;
	mat4 camera_perp_matrix;
	mat4 light_perp_matrix;
	mat4 mvp_matrix;

	Camera* camera;
	Model* model;

	//vertex attribute
	vec3 normal_attri[3];
	vec2 uv_attri[3];
	vec3 worldcoord_attri[3];
	vec4 clipcoord_attri[3];

	//for homogeneous clipping
	vec3 in_normal[MAX_VERTEX];
	vec2 in_uv[MAX_VERTEX];
	vec3 in_worldcoord[MAX_VERTEX];
	vec4 in_clipcoord[MAX_VERTEX];
	vec3 out_normal[MAX_VERTEX];
	vec2 out_uv[MAX_VERTEX];
	vec3 out_worldcoord[MAX_VERTEX];
	vec4 out_clipcoord[MAX_VERTEX];

	float zNear, zFar;

	//for image-based lighting
	iblmap_t* iblmap;
}payload_t;


struct PixelAttri {
	vec3 normal;
	vec3 worldPos;
	vec2 uv;
	float zNear, zFar;
	float* sbuffer;
	mat4 lightSpaceMatrix;
	mat4 viewMat;
};






class IShader
{
public:
	payload_t payload;
	virtual void vertex_shader(int nfaces, int nvertex) {}
	virtual vec3 fragment_shader(PixelAttri &pixelAttri) { return vec3(0, 0, 0); }
};

class lambertShader : public IShader
{
	void vertex_shader(int nfaces, int nvertex);
	vec3 fragment_shader(PixelAttri& pixelAttri);
};

class DepthShader : public IShader
{
	void vertex_shader(int nfaces, int nvertex);
	vec3 fragment_shader(PixelAttri& pixelAttri);
};


class PhongShader :public IShader
{
public:
	void vertex_shader(int nfaces, int nvertex);
	vec3 fragment_shader(PixelAttri& pixelAttri);

};

class PBRShader :public IShader
{
public:
	void vertex_shader(int nfaces, int nvertex);
	vec3 fragment_shader(PixelAttri& pixelAttri);
	vec3 direct_fragment_shader(PixelAttri& pixelAttri);
};

class SkyboxShader :public IShader
{
public:
	void vertex_shader(int nfaces, int nvertex);
	vec3 fragment_shader(PixelAttri& pixelAttri);
};



