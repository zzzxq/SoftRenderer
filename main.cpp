#include <ctime>
#include <iostream>
#include "./platform/win32.h"
#include "./core/pipeline.h"
#include "./core/model.h"
#include "./shader/shader.h"
#include "./core/camera.h"
#include "./core/tgaimage.h"
#include "./core/scene.h"
#include<cstring>


//const vec3 Eye(3.f, 2.6f, 4.f);
//const vec3 Up(0, 1, 0);
//const vec3 Target(0, 0, 0);
const vec3 Eye(3, 2, 4.1);
const vec3 Up(0, 1, 0);
const vec3 Target(0, 0, 0);

const vec3 ligthPos(3.f, 2.6f, 4.f);


const scene_t Scenes[] = {
	{ "fuhua", build_fuhua_scene },
	{ "yayi", build_yayi_scene},
	{ "qiyana", build_qiyana_scene},
	{ "xier", build_xier_scene},
	{ "african_head", build_african_head_scene},
	{ "diablo3_pose", build_diablo_scene},
	{ "room_sksbox", build_room_skybox},
};


void clear_zbuffer(int width, int height, float* zbuffer);
void clear_framebuffer(int width, int height, unsigned char* framebuffer);
void update_matrix(Camera& camera, mat4 view_mat, mat4 perspective_mat, IShader* shader_model, IShader* shader_skybox);

int main() {
	int width = WINDOW_WIDTH, height = WINDOW_HEIGHT;

	unsigned char* framebuffer = (unsigned char*)malloc(sizeof(unsigned char) * width * height * 4);
	float* zbuffer = (float*)malloc(sizeof(float) * width * height);
	float* sbuffer = (float*)malloc(sizeof(float) * width * height);

	memset(framebuffer, 0, sizeof(unsigned char) * width * height * 4);


	mat4 light_view_mat = mat4_lookat(ligthPos, Target, Up); //光源的方向和相机初始方向一致
	mat4 lightProjection = mat4_ortho(-10.f, 10.f, -10.f, 10.f, -0.1f, -100.f);
	mat4 lightSpaceMatrix = lightProjection * light_view_mat;

	int model_num = 0;
	int scene_index = 5;
	Model* model[MAX_MODEL_NUM];
	IShader* shader_model;
	IShader* shader_skybox;

	//从光源--渲染
	clear_zbuffer(width, height, sbuffer);

	build_shadow_scene(model, model_num, &shader_model, light_view_mat, lightProjection);
	shader_model->payload.zNear = 0.1;
	shader_model->payload.zFar = 100;

	for (int m = 0; m < model_num; m++) {
		shader_model->payload.model = model[m];

		IShader* shader = shader_model;
		for (int i = 0; i < model[m]->nfaces(); i++) {
			//draw_triangles(framebuffer, sbuffer, *shader, i);
			Getsbuffer(sbuffer, *shader, i);
		}
	}
	//此时 获取到depthbuffer, 深度信息
	

	Camera camera(Eye, Target, Up, (float)(width) / height);
	float zNear = -0.1, zFar = -10000;
	mat4 model_mat = mat4::identity();
	mat4 view_mat = mat4_lookat(camera.eye, camera.target, camera.up);
	mat4 perspective_mat = mat4_perspective(80, (float)width / height, zNear, zFar);
	 
	srand((unsigned int)time(NULL));
		
	

	Scenes[scene_index].build_scene(model, model_num, &shader_model, &shader_skybox, perspective_mat, &camera);
	shader_model->payload.zNear = -zNear;
	shader_model->payload.zFar = -zFar; //都设为正数

	window_init(width, height, "SRender");
	int num_frames = 0;
	float print_time = platform_get_time();
	while (!window->is_close) {

		float curr_time = platform_get_time();
		clear_framebuffer(width, height, framebuffer);
		clear_zbuffer(width, height, zbuffer);


		handle_events(camera);
		update_matrix(camera, view_mat, perspective_mat, shader_model, shader_skybox);

		//start renderering
		for (int m = 0; m < model_num; m++) {
			shader_model->payload.model = model[m];
			if (shader_skybox != NULL) shader_skybox->payload.model = model[m];
			IShader* shader;
			if (model[m]->is_skybox)
				shader = shader_skybox;
			else
				shader = shader_model;

			for (int i = 0; i < model[m]->nfaces(); i++) {
				//draw_triangles(framebuffer, zbuffer, *shader, i);

				draw_triangles_with_shadows(framebuffer, zbuffer, sbuffer,  *shader, i, lightSpaceMatrix, light_view_mat, 0.1, 100);
			}

		}
		num_frames += 1;
		if (curr_time - print_time >= 1) {
			int sum_millis = (int)((curr_time - print_time) * 1000);
			int avg_millis = sum_millis / num_frames;
			printf("fps: %3d, avg: %3d ms\n", num_frames, avg_millis);
			num_frames = 0;
			print_time = curr_time;
		}



		window->mouse_info.wheel_delta = 0;
		window->mouse_info.orbit_delta = vec2(0, 0);
		window->mouse_info.fv_delta = vec2(0, 0);


		window_draw(framebuffer);
		msg_dispatch();
		
	}

	for (int i = 0; i < model_num; i++)
		if (model[i] != NULL)  delete model[i];
	if (shader_model != NULL)  delete shader_model;
	if (shader_skybox != NULL) delete shader_skybox;
	free(zbuffer);
	free(framebuffer);

	window_destroy();

	system("pause");
	return 0;
}


void clear_zbuffer(int width, int height, float* zbuffer) 
{
	int t = width * height;
	for (int i = 0; i < t; i++) 
		zbuffer[i] = 1;
}
void clear_framebuffer(int width, int height, unsigned char* framebuffer) 
{
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int index = (i * width + j) * 4;

			framebuffer[index + 2] = 80;
			framebuffer[index + 1] = 56;
			framebuffer[index] = 56;
		}
	}
}

void update_matrix(Camera& camera, mat4 view_mat, mat4 perspective_mat, IShader* shader_model, IShader* shader_skybox)
{
	view_mat = mat4_lookat(camera.eye, camera.target, camera.up);
	mat4 mvp = perspective_mat * view_mat;
	shader_model->payload.camera_view_matrix = view_mat;
	shader_model->payload.mvp_matrix = mvp;

	if (shader_skybox != NULL)
	{
		mat4 view_skybox = view_mat;
		view_skybox[0][3] = 0;   //移除平移矩阵
		view_skybox[1][3] = 0;
		view_skybox[2][3] = 0;
		shader_skybox->payload.camera_view_matrix = view_skybox;
		shader_skybox->payload.mvp_matrix = perspective_mat * view_skybox;
	}


}
