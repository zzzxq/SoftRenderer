#include "shader.h"
#include "../core/sample.h"
#include<iostream>






void lambertShader::vertex_shader(int nfaces, int nvertex)
{
	//将顶点转化到四维空间
	vec4 temp_vert = to_vec4(payload.model->vert(nfaces, nvertex), 1.0f);
	vec4 temp_normal = to_vec4(payload.model->normal(nfaces, nvertex), 1.0f);
	//求顶点的属性,uv,法线，世界坐标，裁剪坐标
	payload.uv_attri[nvertex] = payload.model->uv(nfaces, nvertex);
	payload.in_uv[nvertex] = payload.uv_attri[nvertex];
	payload.clipcoord_attri[nvertex] = payload.mvp_matrix * temp_vert;
	payload.in_clipcoord[nvertex] = payload.clipcoord_attri[nvertex];

	for (int i = 0; i < 3; i++)
	{
		payload.worldcoord_attri[nvertex][i] = temp_vert[i];
		payload.in_worldcoord[nvertex][i] = temp_vert[i];
		payload.normal_attri[nvertex][i] = temp_normal[i];
		payload.in_normal[nvertex][i] = temp_normal[i];
	}
}


vec3 lambertShader::fragment_shader(PixelAttri& pixelAttri)
{
	
	vec3 normal = pixelAttri.normal;
	vec3 worldPos = pixelAttri.worldPos;
	vec2 uv = pixelAttri.uv;


	vec3 light_dir(1.0f, 1.0f, 1.0f);

	light_dir = unit_vector(light_dir);


	float costheta = (std::max)(0.0f, (float)dot(light_dir, normal));
	vec3 diffuse = payload.model->diffuse(uv);
	float spec = payload.model->specular(uv);

	vec3 result_color(0, 0, 0);

	result_color = diffuse * costheta;



	return result_color * 255.f;


}


