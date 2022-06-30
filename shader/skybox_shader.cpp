#include "./shader.h"
#include "../core/sample.h"

void SkyboxShader::vertex_shader(int nfaces, int nvertex)
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

vec3 SkyboxShader::fragment_shader(PixelAttri& pixelAttri)
{
	vec3 worldpos = pixelAttri.worldPos;
	vec3 result_color = cubemap_sampling(worldpos, payload.model->environment_map);
	return result_color * 255.0f;
}
