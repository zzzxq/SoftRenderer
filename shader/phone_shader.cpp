#include "./shader.h"
#include "../core/sample.h"
#include<iostream>





void PhongShader::vertex_shader(int nfaces, int nvertex)
{
	//将顶点转化到四维空间
	vec4 temp_vert = to_vec4(payload.model->vert(nfaces, nvertex), 1.0f);
	vec4 temp_normal = to_vec4(payload.model->normal(nfaces, nvertex), 1.0f);
	//求顶点的属性,uv,法线，世界坐标，裁剪坐标
	payload.uv_attri[nvertex]        = payload.model->uv(nfaces, nvertex);
	payload.in_uv[nvertex]           = payload.uv_attri[nvertex];
	payload.clipcoord_attri[nvertex] = payload.mvp_matrix * temp_vert;
	payload.in_clipcoord[nvertex]    = payload.clipcoord_attri[nvertex];
	
	for (int i = 0; i < 3; i++)
	{
		payload.worldcoord_attri[nvertex][i]  = temp_vert[i];
		payload.in_worldcoord[nvertex][i]     = temp_vert[i];
		payload.normal_attri[nvertex][i]      = temp_normal[i];
		payload.in_normal[nvertex][i]         = temp_normal[i];
	}
}


float ShadowCalculation(vec3& worldpos, mat4 &lightSpaceMatrix, float *sbuffer, mat4& viewMat, float zNear, float zFar)
{
	vec4 worldposVec4 = to_vec4(worldpos, 1);
	vec4 lightSpacePos = lightSpaceMatrix * worldposVec4; //算出该点在光源坐标下的点
	vec3 projCoords = vec3(lightSpacePos.x(), lightSpacePos.y(), lightSpacePos.z()); 

	//获取深度
	vec4 lightViewSpacePos = viewMat * worldposVec4;
	float z = -lightViewSpacePos.z();
	z = (1 / z - zNear) / (zFar - zNear);

	//从-1到1 转到0到1
	projCoords[0] = 0.5 * (projCoords[0] + 1.0) * (WINDOW_WIDTH - 1);
	projCoords[1] = 0.5 * (projCoords[1] + 1.0) * (WINDOW_HEIGHT - 1);

	//float closestDepth = sbuffer[get_index(projCoords.x(), projCoords.y())]; //获取最近点
	float currentDepth = z;

	float shadow = 0;

	for (int x = -1; x <= 1; ++x) {
		for (int y = -1; y <= 1; ++y) {
			float pcfDepth = sbuffer[get_index(projCoords.x() + x , projCoords.y()  + y)];
			shadow += currentDepth > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;
	return shadow;



	/*float shadow = (currentDepth) > closestDepth ? 1.0 : 0.0;
	return shadow;*/
}


vec3 PhongShader::fragment_shader(PixelAttri& pixelAttri)
{
	//片段着色器
	vec3 normal = pixelAttri.normal;

	//normal = mat3(payload.model_matrix) * normal;

	vec3 worldpos = pixelAttri.worldPos;
	vec2 uv = pixelAttri.uv;
	float* sbuffer = pixelAttri.sbuffer;
	mat4 lightSpaceMatrix = pixelAttri.lightSpaceMatrix;
	mat4 viewSpaceMatrix = pixelAttri.viewMat;
	float zNear = pixelAttri.zNear, zFar = pixelAttri.zFar;

	vec3 ka(0.35, 0.35, 0.35);

	vec3 kd = payload.model->diffuse(uv); //基础颜色，通过贴图方式

	int IsShadow = false;


	float ks;
	if (payload.model->specularmap)
		ks = payload.model->specular(uv);

	vec3 emit(0., 0., 0.);
	if (payload.model->emision_map)
		emit = payload.model->emission(uv);

	float p = 60.0;
	vec3 viewPos = payload.camera->eye;
	//vec3 lightPos(1.f, 2.f, 5.f);

	//vec3 lightDir = lightPos - worldpos;
	vec3 lightDir(1, 1, 1);
	vec3 L_dir = unit_vector(lightDir);
	vec3 V_dir = unit_vector(viewPos - worldpos);
	vec3 Halfv = unit_vector(V_dir + L_dir);
	float costheta = dot(V_dir, normal);
	float r2 = lightDir.norm_squared();

	
	
	vec3 light_ambient_intensity(0.15, 0.15, 0.15);
	vec3 light_diffuse_intensity(0.6, 0.6, 0.6);
	vec3 light_specular_intensity(0.1, 0.1, 0.1);

	vec3 ambient = cwise_product(ka, light_ambient_intensity);
	vec3 diffuse = cwise_product(kd, light_diffuse_intensity) * std::max(0.0f, costheta);

	costheta = dot(Halfv, normal);
	vec3 specular = light_specular_intensity * std::pow(std::max(0.0f, costheta), p) * ks;

	float shadow;

	if (IsShadow)
		shadow = ShadowCalculation(worldpos, lightSpaceMatrix, sbuffer, viewSpaceMatrix, zNear, zFar);
	else
		shadow = 0;


	vec3 result_color(0, 0, 0);
	result_color = ambient + (1 - shadow) * (diffuse + emit);
	return result_color * 255.0f;

}



