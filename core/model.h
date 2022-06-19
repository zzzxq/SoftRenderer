#pragma once

#include<string>
#include<vector>
#include"maths.h"
#include"tgaimage.h"


typedef struct cubemap cubemap_t;

class Model 
{
private:
	//����verts��uvs��norms ����¼�����ж������Ϣ

	//faces[i][j] = k ��ʾ�ڼ����� �� �ڼ����������Ϣ�� faces[0]�� abc, def, ghi. 
	// face[0][0-3]�ֱ��ʾ��һ�������pos��uv��norm���±꣬����
	std::vector<vec3> verts;  
	std::vector<std::vector<int> > faces; // attention, this Vec3i means vertex/uv/normal
	std::vector<vec3> norms;
	std::vector<vec2> uvs;

	void load_cubemap(const char* filename);
	void create_map(const char* filename);
	void load_texture(std::string filename, const char* suffix, TGAImage* img);
	void load_texture(std::string filename, const char* suffix, TGAImage& img);


public:
	Model(const char* filename, int is_skybox = 0, int is_from_mmd = 0);
	~Model();
	//skybox
	cubemap_t* environment_map;
	int is_skybox;

	int is_from_mmd;
	TGAImage* diffusemap;
	TGAImage* normalmap; 
	TGAImage* normalmap_tangent;
	TGAImage* specularmap;
	TGAImage* roughnessmap;
	TGAImage* metalnessmap;
	TGAImage* occlusion_map;
	TGAImage* emision_map;

	int nverts();
	int nfaces();
	vec3 normal(int iface, int nthvert);
	vec3 normal_mapping(vec2 uv);
	vec3 vert(int i);
	vec3 vert(int iface, int nthvert);

	vec2 uv(int iface, int nthvert);
	vec3 diffuse(vec2 uv);
	float roughness(vec2 uv);
	float metalness(vec2 uv);
	vec3 emission(vec2 uv);
	float occlusion(vec2 uv);
	vec3 specular(vec2 uv);

	std::vector<int> face(int idx);
};

