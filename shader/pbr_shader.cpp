#include "./shader.h"
#include "../core/sample.h"

#include<cmath>

using std::max;

static float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness * roughness;  //传入粗糙度的平方效果更好
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;
	float nom = a2;
	float denom = NdotH2 * (a2 - 1) + 1.0;
	denom = PI * denom * denom;

	return nom / denom;
}

static float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;
	return nom / denom;
}

static float GeometrySchlickGGX_ibl(float NdotV, float roughness)
{
	float k = (roughness * roughness) / 2.0;

	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;
	return nom / denom;
}

static float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx1 = GeometrySchlickGGX(NdotV, roughness);
	float ggx2 = GeometrySchlickGGX(NdotL, roughness);
	return ggx1 * ggx2;
}

static float GeometrySmith_ibl(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx1 = GeometrySchlickGGX_ibl(NdotV, roughness);
	float ggx2 = GeometrySchlickGGX_ibl(NdotL, roughness);
	return ggx1 * ggx2;
}




static vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow((1 - cosTheta), 5.0);
}


void PBRShader::vertex_shader(int nfaces, int nvertex)
{
	//将顶点转化到四维空间
	vec4 temp_vert = to_vec4(payload.model->vert(nfaces, nvertex), 1.0f);
	vec4 temp_normal = to_vec4(payload.model->normal(nfaces, nvertex), 1.0f);
	//求顶点的属性,uv,法线，世界坐标，裁剪坐标
	payload.uv_attri[nvertex] = payload.model->uv(nfaces, nvertex);
	payload.in_uv[nvertex] = payload.uv_attri[nvertex];
	payload.clipcoord_attri[nvertex] = payload.mvp_matrix * temp_vert;
	payload.in_clipcoord[nvertex] = payload.clipcoord_attri[nvertex];

	payload.worldcoord_attri[nvertex] = vec3(payload.model_matrix * temp_vert);
	payload.in_worldcoord[nvertex] = payload.worldcoord_attri[nvertex];
	payload.normal_attri[nvertex] = mat3(payload.model_matrix) * vec3(temp_normal);
	payload.in_normal[nvertex] = payload.normal_attri[nvertex];

}


static vec3 lightPositions[] = {
	   vec3(-10.0f,  10.0f, 10.0f),
	   vec3(10.0f,  10.0f, 10.0f),
	   vec3(-10.0f, -10.0f, 10.0f),
	   vec3(10.0f, -10.0f, 10.0f),
};
static vec3 lightColors[] = {
	vec3(300.0f, 300.0f, 300.0f),
	vec3(300.0f, 300.0f, 300.0f),
	vec3(300.0f, 300.0f, 300.0f),
	vec3(300.0f, 300.0f, 300.0f)
};


static vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
	float r1 = 1.0f - roughness;
	if (r1 < F0[0])
		r1 = F0[0];
	return F0 + (vec3(r1) - F0) * pow(1.0 - cosTheta, 5.0);
}



vec3 PBRShader::fragment_shader(PixelAttri& pixelAttri)
{

	vec3 normal = pixelAttri.normal;
	vec3 worldPos = pixelAttri.worldPos;
	vec2 uv = pixelAttri.uv;
	//float metallic = payload.model->metallic_s;
	//float roughness = payload.model->roughness_s;
	//vec3 albedo(0.5f, 0.0f, 0.0f); //漫反射系数

	float metallic = payload.model->metalness(uv);
	float roughness = payload.model->roughness(uv);
	vec3 albedo = pow_vec3(payload.model->diffuse(uv), 2.2); //从srgb转到线性空间

	float ao = 1.0f; //环境光遮蔽

	vec3 N = unit_vector(normal);
	
	vec3 V = unit_vector(payload.camera->eye - worldPos);
	float NdotV = float_max(dot(N, V), 0.0);

	vec3 F0 = vec3(0.04, 0.04, 0.04);
	F0 = vec3_lerp(F0, albedo, metallic);

	vec3 Lo(0.0, 0.0, 0.0);

	for (int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); i++)
	{
		vec3 L_dir = lightPositions[i] - worldPos;
		vec3 L = unit_vector(L_dir);
		vec3 H = unit_vector(V + L);
		float distance = L_dir.norm_squared(); //距离的平方
		float attenuation = 1.0 / distance;   //衰减系数
		vec3 radiance = lightColors[i] * attenuation;

		//下面这段有问题，然后加上光源位置调整。

		float NDF = DistributionGGX(N, H, roughness);
		float G = GeometrySmith(N, V, L, roughness);
		float costheta = float_clamp(dot(H, V), 0.0f, 1.0f);
		vec3 F = fresnelSchlick(costheta, F0); //反射率

		vec3 numerator = NDF * G * F;
		float denominator = 4.0f * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;

		vec3 specular = numerator / denominator;

		vec3 KS = F;        //反射系数
		vec3 KD = 1.0 - KS; //折射系数

		KD *= (1.0f - metallic); //对于金属没有漫反射

		float NdotL = max(dot(N, L), 0.0);

		Lo += (KD * albedo / PI + specular) * radiance * NdotL;

	}
	//vec3 ambient = vec3(0.03, 0.03, 0.03) * ao * albedo;

	vec3 irradiance = cubemap_sampling(N, payload.iblmap->irradiance_map);

	vec3 kS = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
	vec3 kD = 1.0 - kS;
	kD *= 1.0 - metallic;

	//diffuse部分
	vec3 diffuse = kD * irradiance * albedo * ao;

	//specular color
	vec3 R = unit_vector(2.0 * dot(V, N) * N - V); //根据观察方向和法线，计算反射光线R
	vec2 lut_uv = vec2(NdotV, roughness);
	vec3 lut_sample = texture_sample(lut_uv, payload.iblmap->brdf_lut);
	float specular_scale = lut_sample.x();
	float specular_bias  = lut_sample.y();
	vec3 specular = F0 * specular_scale + specular_bias;
	float max_mip_level = (float)(payload.iblmap->mip_levels - 1);
	int specular_miplevel = (int)(roughness * max_mip_level + 0.5f);

	vec3 prefilter_color = cubemap_sampling(R, payload.iblmap->prefilter_maps[specular_miplevel]);

	prefilter_color = pow_vec3(prefilter_color, 2.2);
	specular = cwise_product(prefilter_color, specular); 

	vec3 color = diffuse + specular + Lo;


	color = color / (color + 1.0);  //色调映射

	color = pow_vec3(color, 1.0 / 2.2);

	return color * 255.f;
}




vec3 PBRShader::direct_fragment_shader(PixelAttri& pixelAttri)  //直接光部分
{
	vec3 normal = pixelAttri.normal;
	vec3 worldPos = pixelAttri.worldPos;
	vec2 uv = pixelAttri.uv;

	float metallic = payload.model->metallic_s;
	float roughness = payload.model->roughness_s;
	vec3 albedo(0.5f, 0.0f, 0.0f); //漫反射系数

	//float metallic = payload.model->metalness(uv);
	//float roughness = payload.model->roughness(uv);
	//vec3 albedo = pow_vec3(payload.model->diffuse(uv), 2.2); //从srgb转到线性空间

	float ao = 1.0f; //环境光遮蔽

	vec3 N = unit_vector(normal);

	vec3 V = unit_vector(payload.camera->eye - worldPos);

	vec3 F0 = vec3(0.04, 0.04, 0.04);
	F0 = vec3_lerp(F0, albedo, metallic);

	vec3 Lo(0.0, 0.0, 0.0);

	for (int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); i++)
	{
		vec3 L_dir = lightPositions[i] - worldPos;
		vec3 L = unit_vector(L_dir);
		vec3 H = unit_vector(V + L);
		float distance = L_dir.norm_squared(); //距离的平方
		float attenuation = 1.0 / distance;   //衰减系数
		vec3 radiance = lightColors[i] * attenuation;

		//下面这段有问题，然后加上光源位置调整。

		float NDF = DistributionGGX(N, H, roughness);
		float G = GeometrySmith(N, V, L, roughness);
		float costheta = float_clamp(dot(H, V), 0.0f, 1.0f);
		vec3 F = fresnelSchlick(costheta, F0); //反射率

		vec3 numerator = NDF * G * F;
		float denominator = 4.0f * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;

		vec3 specular = numerator / denominator;

		vec3 KS = F;        //反射系数
		vec3 KD = 1.0 - KS; //折射系数

		KD *= (1.0f - metallic); //对于金属没有漫反射

		float NdotL = max(dot(N, L), 0.0);

		Lo += (KD * albedo / PI + specular) * radiance * NdotL;

	}

	vec3 ambient = vec3(0.03, 0.03, 0.03) * ao * albedo;

	vec3 color = ambient + Lo;

	color = color / (color + 1.0);  //色调映射

	color = pow_vec3(color, 1.0 / 2.2);

	return color * 255.f;
}
