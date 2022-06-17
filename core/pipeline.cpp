
#include "./pipeline.h"
#include "sample.h"
#include<iostream>


static vec3 cal_normal(vec3& normal, vec3* world_coords, const vec2* uvs, const vec2& uv, TGAImage* normal_map)
{
	// calculate the difference in UV coordinate

	float x1 = uvs[1][0] - uvs[0][0];
	float y1 = uvs[1][1] - uvs[0][1];
	float x2 = uvs[2][0] - uvs[0][0];
	float y2 = uvs[2][1] - uvs[0][1];

	vec3 edge1 = world_coords[1] - world_coords[0];
	vec3 edge2 = world_coords[2] - world_coords[0];
	float det = 1 / (x1 * y2 - x2 * y1);
	vec3 t = edge1 * y2 - edge2 * y1;
	vec3 b = -x2 * edge1 + x1 * edge2;

	t *= det;
	b *= det;

	normal = unit_vector(normal);
	t = unit_vector(t - dot(t, normal) * normal);
	b = cross(normal, t);


	vec3 sample = texture_sample(uv, normal_map) * 2 - 1;

	vec3 normal_new = t * sample[0] + b * sample[1] + normal * sample[2];


	return normal_new;

}

static int is_back_facing(vec3 ndc_pos[3])
{
	vec3 AB = ndc_pos[1] - ndc_pos[0];
	vec3 AC = ndc_pos[2] - ndc_pos[0];
	
	return (AB.x() * AC.y() - AC.x() * AB.y()) <= 0;
}

//static int is_back_facing(vec3 ndc_pos[3])
//{
//	vec3 a = ndc_pos[0];
//	vec3 b = ndc_pos[1];
//	vec3 c = ndc_pos[2];
//	float signed_area = a.x() * b.y() - a.y() * b.x() +
//		b.x() * c.y() - b.y() * c.x() +
//		c.x() * a.y() - c.y() * a.x();   //|AB AC| 
//	return signed_area <= 0;
//}


static void set_color(unsigned char* framebuffer, int x, int y, unsigned char color[])
{
	// 坐标轴的以左下为原点，但是framebuffer 存储以左上为起点
	int index = ((WINDOW_HEIGHT - y - 1) * WINDOW_WIDTH + x) * 4;
	for (int i = 0; i < 3; ++i)
		framebuffer[index + i] = color[i];
}

static vec3 compute_barycentric2D(float x, float y, const vec3* v)
{
	float c1 = (x * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * y + v[1].x() * v[2].y() - v[2].x() * v[1].y()) / (v[0].x() * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * v[0].y() + v[1].x() * v[2].y() - v[2].x() * v[1].y());
	float c2 = (x * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * y + v[2].x() * v[0].y() - v[0].x() * v[2].y()) / (v[1].x() * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * v[1].y() + v[2].x() * v[0].y() - v[0].x() * v[2].y());
	return vec3(c1, c2, 1 - c1 - c2);
} 




static bool is_inside_triangle(float alpha, float beta, float gamma)
{
	if (alpha > -EPSILON && beta > -EPSILON && gamma > -EPSILON)
		return true;
	return false;
}

typedef enum
{
	W_PLANE,
	X_RIGHT,
	X_LEFT,
	Y_TOP,
	Y_BOTTOM,
	Z_NEAR,
	Z_FAR
} clip_plane;

// in my implementation, all the w is negative, so here is a little different from openGL
static int is_inside_plane(clip_plane c_plane, vec4 vertex)
{
	switch (c_plane)
	{
	case W_PLANE:
		return vertex.w() <= -EPSILON;
	case X_RIGHT:
		return vertex.x() >= vertex.w();
	case X_LEFT:
		return vertex.x() <= -vertex.w();
	case Y_TOP:
		return vertex.y() >= vertex.w();
	case Y_BOTTOM:
		return vertex.y() <= -vertex.w();
	case Z_NEAR:
		return vertex.z() >= vertex.w();
	case Z_FAR:
		return vertex.z() <= -vertex.w();
	default:
		return 0;
	}
}

// for the deduction of intersection ratio
// refer to: https://fabiensanglard.net/polygon_codec/clippingdocument/Clipping.pdf
static float get_intersect_ratio(vec4 prev, vec4 curv, clip_plane c_plane)
{
	switch (c_plane)
	{
	case W_PLANE:
		return (prev.w() + EPSILON) / (prev.w() - curv.w());
	case X_RIGHT:
		return (prev.w() - prev.x()) / ((prev.w() - prev.x()) - (curv.w() - curv.x()));
	case X_LEFT:
		return (prev.w() + prev.x()) / ((prev.w() + prev.x()) - (curv.w() + curv.x()));
	case Y_TOP:
		return (prev.w() - prev.y()) / ((prev.w() - prev.y()) - (curv.w() - curv.y()));
	case Y_BOTTOM:
		return (prev.w() + prev.y()) / ((prev.w() + prev.y()) - (curv.w() + curv.y()));
	case Z_NEAR:
		return (prev.w() - prev.z()) / ((prev.w() - prev.z()) - (curv.w() - curv.z()));
	case Z_FAR:
		return (prev.w() + prev.z()) / ((prev.w() + prev.z()) - (curv.w() + curv.z()));
	default:
		return 0;
	}
}

static int clip_with_plane(clip_plane c_plane, int num_vert, payload_t& payload)
{
	int out_vert_num = 0;
	int previous_index, current_index;
	int is_odd = (c_plane + 1) % 2;

	// set the right in and out datas
	vec4* in_clipcoord = is_odd ? payload.in_clipcoord : payload.out_clipcoord;
	vec3* in_worldcoord = is_odd ? payload.in_worldcoord : payload.out_worldcoord;
	vec3* in_normal = is_odd ? payload.in_normal : payload.out_normal;
	vec2* in_uv = is_odd ? payload.in_uv : payload.out_uv;
	vec4* out_clipcoord = is_odd ? payload.out_clipcoord : payload.in_clipcoord;
	vec3* out_worldcoord = is_odd ? payload.out_worldcoord : payload.in_worldcoord;
	vec3* out_normal = is_odd ? payload.out_normal : payload.in_normal;
	vec2* out_uv = is_odd ? payload.out_uv : payload.in_uv;

	// tranverse all the edges from first vertex
	for (int i = 0; i < num_vert; i++)
	{
		current_index = i;
		previous_index = (i - 1 + num_vert) % num_vert;
		vec4 cur_vertex = in_clipcoord[current_index];
		vec4 pre_vertex = in_clipcoord[previous_index];

		int is_cur_inside = is_inside_plane(c_plane, cur_vertex);
		int is_pre_inside = is_inside_plane(c_plane, pre_vertex);
		if (is_cur_inside != is_pre_inside)
		{
			float ratio = get_intersect_ratio(pre_vertex, cur_vertex, c_plane);

			out_clipcoord[out_vert_num] = vec4_lerp(pre_vertex, cur_vertex, ratio);
			out_worldcoord[out_vert_num] = vec3_lerp(in_worldcoord[previous_index], in_worldcoord[current_index], ratio);
			out_normal[out_vert_num] = vec3_lerp(in_normal[previous_index], in_normal[current_index], ratio);
			out_uv[out_vert_num] = vec2_lerp(in_uv[previous_index], in_uv[current_index], ratio);

			out_vert_num++;
		}

		if (is_cur_inside)
		{
			out_clipcoord[out_vert_num] = cur_vertex;
			out_worldcoord[out_vert_num] = in_worldcoord[current_index];
			out_normal[out_vert_num] = in_normal[current_index];
			out_uv[out_vert_num] = in_uv[current_index];

			out_vert_num++;
		}
	}

	return out_vert_num;
}

static int homo_clipping(payload_t& payload)
{
	int num_vertex = 3;
	num_vertex = clip_with_plane(W_PLANE, num_vertex, payload);
	num_vertex = clip_with_plane(X_RIGHT, num_vertex, payload);
	num_vertex = clip_with_plane(X_LEFT, num_vertex, payload);
	num_vertex = clip_with_plane(Y_TOP, num_vertex, payload);
	num_vertex = clip_with_plane(Y_BOTTOM, num_vertex, payload);
	num_vertex = clip_with_plane(Z_NEAR, num_vertex, payload);
	num_vertex = clip_with_plane(Z_FAR, num_vertex, payload);
	return num_vertex;
}

static void transform_attri(payload_t& payload, int index0, int index1, int index2)
{
	payload.clipcoord_attri[0] = payload.out_clipcoord[index0];
	payload.clipcoord_attri[1] = payload.out_clipcoord[index1];
	payload.clipcoord_attri[2] = payload.out_clipcoord[index2];
	payload.worldcoord_attri[0] = payload.out_worldcoord[index0];
	payload.worldcoord_attri[1] = payload.out_worldcoord[index1];
	payload.worldcoord_attri[2] = payload.out_worldcoord[index2];
	payload.normal_attri[0] = payload.out_normal[index0];
	payload.normal_attri[1] = payload.out_normal[index1];
	payload.normal_attri[2] = payload.out_normal[index2];
	payload.uv_attri[0] = payload.out_uv[index0];
	payload.uv_attri[1] = payload.out_uv[index1];
	payload.uv_attri[2] = payload.out_uv[index2];
}

void rasterize_singlethread(vec4* clipcoord_attri, unsigned char* framebuffer, float* zbuffer, IShader& shader)  //透视投影
{
	//渲染三角形
	vec3 ndc_pos[3];   
	vec3 screen_pos[3];
	unsigned char c[3];
	int width = WINDOW_WIDTH, height = WINDOW_HEIGHT;

	//齐次除法
	for (int i = 0; i < 3; ++i) {
		ndc_pos[i][0] = clipcoord_attri[i][0] / clipcoord_attri[i].w();
		ndc_pos[i][1] = clipcoord_attri[i][1] / clipcoord_attri[i].w();
		ndc_pos[i][2] = clipcoord_attri[i][2] / clipcoord_attri[i].w();
	}
	for (int i = 0; i < 3; i++) {
		screen_pos[i][0] = 0.5 * (ndc_pos[i][0] + 1.0) * (width - 1);
		screen_pos[i][1] = 0.5 * (ndc_pos[i][1] + 1.0) * (height - 1);
		screen_pos[i][2] = -clipcoord_attri[i].w();   //记录深度值，裁剪空间的-w分量，即为视图空间下的深度值

	}

	//光栅化过程
	float xmin = float_min(screen_pos[0].x(), float_min(screen_pos[1].x(), screen_pos[2].x()));
	float xmax = float_max(screen_pos[0].x(), float_max(screen_pos[1].x(), screen_pos[2].x()));
	float ymin = float_min(screen_pos[0].y(), float_min(screen_pos[1].y(), screen_pos[2].y()));
	float ymax = float_max(screen_pos[0].y(), float_max(screen_pos[1].y(), screen_pos[2].y()));

	xmin = float_max(0, xmin); ymin = float_max(0, ymin);
	xmax = float_min(width - 1, xmax); ymax = float_min(height - 1, ymax);


	if (is_back_facing(ndc_pos)) {
		return;
	}



	

	for (int x = xmin; x <= xmax; x++) {
		for (int y = ymin; y <= ymax; y++) {
			vec3 barycentric = compute_barycentric2D((float)(x + 0.5), (float)(y + 0.5), screen_pos);
			float alpha = barycentric.x(), beta = barycentric.y(), gamma = barycentric.z();

			if (is_inside_triangle(alpha, beta, gamma)) {
				int index = get_index(x, y);

				float Zt = 1.0 / (alpha / clipcoord_attri[0].w() + beta / clipcoord_attri[1].w() + gamma / clipcoord_attri[2].w());
				//float z = (alpha * screen_pos[0].z() / clipcoord_attri[0].w() + beta * screen_pos[1].z() / clipcoord_attri[1].w() +
					//gamma * screen_pos[2].z() / clipcoord_attri[2].w()) * Zt; //获取原视图坐标下的深度信息(0, 1)之间
				float z = -Zt;
				//此时计算出view space下的深度 转到01之间
				float zNear = shader.payload.zNear, zFar = shader.payload.zFar;

				z = (1 / z - 1 / zNear) / (1 / zFar - 1 / zNear);


				//插值矫正，得到视图空间下的深度值， z > 0
				//记录深度最小的，即离的最近的
				if (zbuffer[index] > z) {
					zbuffer[index] = z;
					vec4* clip_coords = shader.payload.clipcoord_attri;
					vec3* world_coords = shader.payload.worldcoord_attri;
					vec3* normals = shader.payload.normal_attri;
					vec2* uvs = shader.payload.uv_attri;
					//透视矫正
					vec3 normal = (alpha * normals[0] / clip_coords[0].w() + beta * normals[1] / clip_coords[1].w() +
						gamma * normals[2] / clip_coords[2].w()) * Zt;
					vec2 uv = (alpha * uvs[0] / clip_coords[0].w() + beta * uvs[1] / clip_coords[1].w() +
						gamma * uvs[2] / clip_coords[2].w()) * Zt;
					vec3 worldpos = (alpha * world_coords[0] / clip_coords[0].w() + beta * world_coords[1] / clip_coords[1].w() +
						gamma * world_coords[2] / clip_coords[2].w()) * Zt;

					if (shader.payload.model->normalmap_tangent)
						normal = cal_normal(normal, world_coords, uvs, uv, shader.payload.model->normalmap_tangent);
					else if (shader.payload.model->normalmap)
						normal = texture_sample(uv, shader.payload.model->normalmap) * 2 - 1;
					PixelAttri pixelAttri{normal, worldpos, uv};
					vec3 color = shader.fragment_shader(pixelAttri);
					for (int i = 0; i < 3; i++) {
						c[i] = (int)float_clamp(color[i], 0, 255);
					}
					set_color(framebuffer, x, y, c);
				}
			}
		}
	}
}

void rasterize_singlethread_with_shadows(vec4* clipcoord_attri, unsigned char* framebuffer, float* zbuffer, float* sbuffer, IShader& shader, mat4& lightSpaceMatrix, mat4& viewMat, float lightZnear, float lightZfar)
{
	//渲染三角形
	vec3 ndc_pos[3];
	vec3 screen_pos[3];
	unsigned char c[3];
	int width = WINDOW_WIDTH, height = WINDOW_HEIGHT;

	//齐次除法
	for (int i = 0; i < 3; ++i) {
		ndc_pos[i][0] = clipcoord_attri[i][0] / clipcoord_attri[i].w();
		ndc_pos[i][1] = clipcoord_attri[i][1] / clipcoord_attri[i].w();
		ndc_pos[i][2] = clipcoord_attri[i][2] / clipcoord_attri[i].w();
	}
	for (int i = 0; i < 3; i++) {
		screen_pos[i][0] = 0.5 * (ndc_pos[i][0] + 1.0) * (width - 1);
		screen_pos[i][1] = 0.5 * (ndc_pos[i][1] + 1.0) * (height - 1);
		screen_pos[i][2] = -clipcoord_attri[i].w();   //记录深度值，裁剪空间的-w分量，即为视图空间下的深度值


	}

	//光栅化过程
	float xmin = float_min(screen_pos[0].x(), float_min(screen_pos[1].x(), screen_pos[2].x()));
	float xmax = float_max(screen_pos[0].x(), float_max(screen_pos[1].x(), screen_pos[2].x()));
	float ymin = float_min(screen_pos[0].y(), float_min(screen_pos[1].y(), screen_pos[2].y()));
	float ymax = float_max(screen_pos[0].y(), float_max(screen_pos[1].y(), screen_pos[2].y()));

	xmin = float_max(0, xmin); ymin = float_max(0, ymin);
	xmax = float_min(width - 1, xmax); ymax = float_min(height - 1, ymax);


	if (is_back_facing(ndc_pos)) {
		return;
	}



	for (int x = xmin; x <= xmax; x++) {
		for (int y = ymin; y <= ymax; y++) {
			vec3 barycentric = compute_barycentric2D((float)(x + 0.5), (float)(y + 0.5), screen_pos);
			float alpha = barycentric.x(), beta = barycentric.y(), gamma = barycentric.z();

			if (is_inside_triangle(alpha, beta, gamma)) {
				int index = get_index(x, y);

				float Zt = 1.0 / (alpha / clipcoord_attri[0].w() + beta / clipcoord_attri[1].w() + gamma / clipcoord_attri[2].w());
				float z = -Zt;
				//此时计算出view space下的深度 转到01之间
				float zNear = shader.payload.zNear, zFar = shader.payload.zFar;
				z = (1 / z - 1 / zNear) / (1 / zFar - 1 / zNear);
				//插值矫正，得到视图空间下的深度值， z > 0
				//记录深度最小的，即离的最近的
				if (zbuffer[index] > z) {
					zbuffer[index] = z;

					vec4* clip_coords = shader.payload.clipcoord_attri;
					vec3* world_coords = shader.payload.worldcoord_attri;
					vec3* normals = shader.payload.normal_attri;
					vec2* uvs = shader.payload.uv_attri;
					//透视矫正
					vec3 normal = (alpha * normals[0] / clip_coords[0].w() + beta * normals[1] / clip_coords[1].w() +
						gamma * normals[2] / clip_coords[2].w()) * Zt;
					vec2 uv = (alpha * uvs[0] / clip_coords[0].w() + beta * uvs[1] / clip_coords[1].w() +
						gamma * uvs[2] / clip_coords[2].w()) * Zt;
					vec3 worldpos = (alpha * world_coords[0] / clip_coords[0].w() + beta * world_coords[1] / clip_coords[1].w() +
						gamma * world_coords[2] / clip_coords[2].w()) * Zt;

					if (shader.payload.model->normalmap_tangent)
						normal = cal_normal(normal, world_coords, uvs, uv, shader.payload.model->normalmap_tangent);
					else if (shader.payload.model->normalmap)
						normal = texture_sample(uv, shader.payload.model->normalmap) * 2 - 1;
					PixelAttri pixelAttri{ normal, worldpos, uv, sbuffer, lightSpaceMatrix, viewMat, lightZnear, lightZfar};
					vec3 color = shader.fragment_shader(pixelAttri);

					
					for (int i = 0; i < 3; i++) {
						c[i] = (int)float_clamp(color[i], 0, 255);
					}
					set_color(framebuffer, x, y, c);
				}
			}
		}
	}
}


void draw_triangles(unsigned char* framebuffer, float* zbuffer, IShader& shader, int nface)
{
	for (int i = 0; i < 3; ++i) {
		shader.vertex_shader(nface, i);  //顶点着色器
	}

	//int num_vertex = homo_clipping(shader.payload);


	//// triangle assembly and reaterize
	//for (int i = 0; i < num_vertex - 2; i++) {
	//	int index0 = 0;
	//	int index1 = i + 1;
	//	int index2 = i + 2;
	//	// transform data to real vertex attri
	//	transform_attri(shader.payload, index0, index1, index2);

		rasterize_singlethread(shader.payload.clipcoord_attri, framebuffer, zbuffer, shader);
	//}

}

void Getsbuffer(float* sbuffer, IShader& shader, int nface)
{
	for (int i = 0; i < 3; ++i) {
		shader.vertex_shader(nface, i);  //顶点着色器
	}

	vec4* clipcoord_attri = shader.payload.clipcoord_attri;
	vec4 worldCoord[3], viewSpaceCoords[3];
	mat4 viewMat = shader.payload.camera_view_matrix;

	for (int i = 0; i < 3; i++) {
		worldCoord[i] = to_vec4(shader.payload.worldcoord_attri[i], 1.0f);
		viewSpaceCoords[i] = viewMat * worldCoord[i];
	}

	vec3 ndc_pos[3];
	vec3 screen_pos[3];
	unsigned char c[3];
	int width = WINDOW_WIDTH, height = WINDOW_HEIGHT;

	//齐次除法--正交矩阵不需要
	for (int i = 0; i < 3; ++i) {
		ndc_pos[i][0] = clipcoord_attri[i][0];
		ndc_pos[i][1] = clipcoord_attri[i][1];
		ndc_pos[i][2] = clipcoord_attri[i][2];
	}
	for (int i = 0; i < 3; i++) {
		screen_pos[i][0] = 0.5 * (ndc_pos[i][0] + 1.0) * (width - 1);
		screen_pos[i][1] = 0.5 * (ndc_pos[i][1] + 1.0) * (height - 1); 
		//正交投影，深度值利用view mat
		screen_pos[i][2] = viewSpaceCoords[i].z();   //视图空间下的深度值

	}

	//光栅化过程
	float xmin = float_min(screen_pos[0].x(), float_min(screen_pos[1].x(), screen_pos[2].x()));
	float xmax = float_max(screen_pos[0].x(), float_max(screen_pos[1].x(), screen_pos[2].x()));
	float ymin = float_min(screen_pos[0].y(), float_min(screen_pos[1].y(), screen_pos[2].y()));
	float ymax = float_max(screen_pos[0].y(), float_max(screen_pos[1].y(), screen_pos[2].y()));

	xmin = float_max(0, xmin); ymin = float_max(0, ymin);
	xmax = float_min(width - 1, xmax); ymax = float_min(height - 1, ymax);


	if (!is_back_facing(ndc_pos)) {
		return;
	}


	for (int x = xmin; x <= xmax; x++) {
		for (int y = ymin; y <= ymax; y++) {
			vec3 barycentric = compute_barycentric2D((float)(x + 0.5), (float)(y + 0.5), screen_pos);
			float alpha = barycentric.x(), beta = barycentric.y(), gamma = barycentric.z();

			if (is_inside_triangle(alpha, beta, gamma)) {
				int index = get_index(x, y);

				float Zt = 1.0 / (alpha / screen_pos[0][2] + beta / screen_pos[1][2] + gamma / screen_pos[2][2]);
				float z = -Zt;
				//此时计算出view space下的深度 转到01之间
				float zNear = shader.payload.zNear, zFar = shader.payload.zFar;

				z = (1 / z - 1 / zNear) / (1 / zFar - 1 / zNear);

				//插值矫正，得到视图空间下的深度值， z > 0
				//记录深度最小的，即离的最近的
				if (sbuffer[index] > z) {
					sbuffer[index] = z;
				}
			}
		}
	}








}


void draw_triangles_with_shadows(unsigned char* framebuffer, float* zbuffer, float* sbuffer, IShader& shader, int nface, mat4& lightSpaceMatrix, mat4& viewMat, float lightZnear, float lightZfar)
{
	for (int i = 0; i < 3; ++i) {
		shader.vertex_shader(nface, i);  //顶点着色器
	}
	rasterize_singlethread_with_shadows(shader.payload.clipcoord_attri, framebuffer, zbuffer, sbuffer, shader, lightSpaceMatrix, viewMat, lightZnear, lightZfar);

}

