#include "./scene.h"
#include <filesystem>


TGAImage* texture_from_file(const char* file_name)
{
	TGAImage* texture = new TGAImage();
	texture->read_tga_file(file_name);
	texture->flip_vertically();
	return texture;
}


void build_fuhua_scene(Model** model, int& m, IShader** shader_use, IShader** shader_skybox, mat4 perspective, Camera* camera)
{
	m = 4;
	const char* modelname[] =
	{
		"G:/VisualStudio/WorkSpace/Renderer/obj/fuhua/fuhuabody.obj",
		"G:/VisualStudio/WorkSpace/Renderer/obj/fuhua/fuhuahair.obj",
		"G:/VisualStudio/WorkSpace/Renderer/obj/fuhua/fuhuaface.obj",
		"G:/VisualStudio/WorkSpace/Renderer/obj/fuhua/fuhuacloak.obj",
	};



	int vertex = 0, face = 0;
	const char* scene_name = "fuhua";
	PhongShader* shader_phong = new PhongShader();

	for (int i = 0; i < m; i++)
	{
		model[i] = new Model(modelname[i], 0, 1);
		vertex += model[i]->nverts();
		face += model[i]->nfaces();
	}

	shader_phong->payload.camera_perp_matrix = perspective;
	shader_phong->payload.camera = camera;

	*shader_use = shader_phong;
	*shader_skybox = NULL;

	printf("scene name:%s\n", scene_name);
	printf("model number:%d\n", m);
	printf("vertex:%d faces:%d\n", vertex, face);
}


void build_yayi_scene(Model** model, int& m, IShader** shader_use, IShader** shader_skybox, mat4 perspective, Camera* camera)
{
	m = 7;
	const char* modelname[] = {
		"G:/VisualStudio/WorkSpace/Renderer/obj/yayi/yayiface.obj",
		"G:/VisualStudio/WorkSpace/Renderer/obj/yayi/yayibody.obj",
		"G:/VisualStudio/WorkSpace/Renderer/obj/yayi/yayihair.obj",
		"G:/VisualStudio/WorkSpace/Renderer/obj/yayi/yayiarmour1.obj",
		"G:/VisualStudio/WorkSpace/Renderer/obj/yayi/yayiarmour2.obj",
		"G:/VisualStudio/WorkSpace/Renderer/obj/yayi/yayidecoration.obj",
		"G:/VisualStudio/WorkSpace/Renderer/obj/yayi/yayisword.obj"
	};

	int vertex = 0, face = 0;
	const char* scene_name = "yayi";
	PhongShader* shader_phong = new PhongShader();

	for (int i = 0; i < m; i++)
	{
		model[i] = new Model(modelname[i], 0, 1);
		vertex += model[i]->nverts();
		face += model[i]->nfaces();
	}

	shader_phong->payload.camera_perp_matrix = perspective;
	shader_phong->payload.camera = camera;

	*shader_use = shader_phong;
	*shader_skybox = NULL;

	printf("scene name:%s\n", scene_name);
	printf("model number:%d\n", m);
	printf("vertex:%d faces:%d\n", vertex, face);
}

void build_qiyana_scene(Model** model, int& m, IShader** shader_use, IShader** shader_skybox, mat4 perspective, Camera* camera) {
	m = 3;
	const char* modelname[] = {
		"G:/VisualStudio/WorkSpace/Renderer/obj/qiyana/qiyanabody.obj",
		"G:/VisualStudio/WorkSpace/Renderer/obj/qiyana/qiyanaface.obj",
		"G:/VisualStudio/WorkSpace/Renderer/obj/qiyana/qiyanahair.obj"
	};
	int vertex = 0, face = 0;
	const char* scene_name = "qiyana";
	PhongShader* shader_phong = new PhongShader();
	for (int i = 0; i < m; i++)
	{
		model[i] = new Model(modelname[i], 0, 1);
		vertex += model[i]->nverts();
		face += model[i]->nfaces();
	}
	shader_phong->payload.camera_perp_matrix = perspective;
	shader_phong->payload.camera = camera;

	*shader_use = shader_phong;
	*shader_skybox = NULL;
	printf("scene name:%s\n", scene_name);
	printf("model number:%d\n", m);
	printf("vertex:%d faces:%d\n", vertex, face);

}
void build_xier_scene(Model** model, int& m, IShader** shader_use, IShader** shader_skybox, mat4 perspective, Camera* camera)
{
	m = 5;
	const char* modelname[] =
	{
		"G:/VisualStudio/WorkSpace/Renderer/obj/xier/xierbody.obj",
		"G:/VisualStudio/WorkSpace/Renderer/obj/xier/xierhair.obj",
		"G:/VisualStudio/WorkSpace/Renderer/obj/xier/xierface.obj",
		"G:/VisualStudio/WorkSpace/Renderer/obj/xier/xiercloth.obj",
		"G:/VisualStudio/WorkSpace/Renderer/obj/xier/xierarm.obj",
	};

	int vertex = 0, face = 0;
	const char* scene_name = "xier";
	PhongShader* shader_phong = new PhongShader();

	for (int i = 0; i < m; i++)
	{
		model[i] = new Model(modelname[i], 0, 1);
		vertex += model[i]->nverts();
		face += model[i]->nfaces();
	}

	shader_phong->payload.camera_perp_matrix = perspective;
	shader_phong->payload.camera = camera;

	*shader_use = shader_phong;
	*shader_skybox = NULL;

	printf("scene name:%s\n", scene_name);
	printf("model number:%d\n", m);
	printf("vertex:%d faces:%d\n", vertex, face);
}

void build_african_head_scene(Model** model, int& m, IShader** shader_use, IShader** shader_skybox, mat4 perspective, Camera* camera)
{
	if (m == 0) {
		m = 3;
		const char* modelname[] =
		{
			"G:/VisualStudio/WorkSpace/Renderer/obj/african_head/african_head.obj",
			"G:/VisualStudio/WorkSpace/Renderer/obj/african_head/african_head_eye_inner.obj",
			//"G:/VisualStudio/WorkSpace/Renderer/obj/african_head/african_head_eye_outer.obj",
			"G:/VisualStudio/WorkSpace/Renderer/obj/floor/floor.obj",

		};

		int vertex = 0, face = 0;

		for (int i = 0; i < m; i++)
		{
			model[i] = new Model(modelname[i], 0, 0);
			vertex += model[i]->nverts();
			face += model[i]->nfaces();
		}
	}
	const char* scene_name = "african_head";

	PhongShader* shader_phong = new PhongShader();

	*shader_use = shader_phong;
	shader_phong->payload.camera = camera;

	shader_phong->payload.camera_perp_matrix = perspective;
	
	printf("scene name:%s\n", scene_name);


}
void build_shadow_scene(Model** model, int& m, IShader** shader_use, mat4 view_mat, mat4 perspective)
{
	m = 2;
	const char* modelname[] =
	{
		"G:/VisualStudio/WorkSpace/Renderer/obj/diablo3_pose/diablo3_pose.obj",
		//"G:/VisualStudio/WorkSpace/Renderer/obj/african_head/african_head_eye_outer.obj",
		"G:/VisualStudio/WorkSpace/Renderer/obj/floor/floor.obj",

	};

	int vertex = 0, face = 0;
	const char* scene_name = "shadow_map";
	DepthShader* shadowshader = new DepthShader();

	for (int i = 0; i < m; i++)
	{
		model[i] = new Model(modelname[i], 0, 0);
		vertex += model[i]->nverts();
		face += model[i]->nfaces();
	}

	*shader_use = shadowshader;

	shadowshader->payload.camera_perp_matrix = perspective;
	shadowshader->payload.camera_view_matrix = view_mat;
	shadowshader->payload.mvp_matrix = (perspective * view_mat);

	printf("scene name:%s\n", scene_name);
	printf("model number:%d\n", m);
	printf("vertex:%d faces:%d\n", vertex, face);
}





void build_diablo_scene(Model** model, int& m, IShader** shader_use, IShader** shader_skybox, mat4 perspective, Camera* camera)
{
	if (m == 0)
	{
		m = 2;
		const char* modelname[] =
		{
			"G:/VisualStudio/WorkSpace/Renderer/obj/diablo3_pose/diablo3_pose.obj",
			"G:/VisualStudio/WorkSpace/Renderer/obj/floor/floor.obj",

		};

		int vertex = 0, face = 0;
		

		for (int i = 0; i < m; i++)
		{
			model[i] = new Model(modelname[i], 0, 0);
			vertex += model[i]->nverts();
			face += model[i]->nfaces();
		}
	}
	const char* scene_name = "diablo3_pose";
	PhongShader* shader_phong = new PhongShader();

	shader_phong->payload.camera_perp_matrix = perspective;
	shader_phong->payload.camera = camera;

	*shader_use = shader_phong;
	*shader_skybox = NULL;

	printf("scene name:%s\n", scene_name);
	/*printf("model number:%d\n", m);
	printf("vertex:%d faces:%d\n", vertex, face);*/
}

void build_room_skybox(Model** model, int& m, IShader** shader_use, IShader** shader_skybox, mat4 perspective, Camera* camera)
{
	m = 2;
	const char* modelname[] =
	{
		"G:/VisualStudio/WorkSpace/Renderer/obj/skybox2/box.obj",
		"G:/VisualStudio/WorkSpace/Renderer/obj/diablo3_pose/diablo3_pose.obj",

	};
	int vertex = 0, face = 0;
	const char* scene_name = "roomBox";

	SkyboxShader* shader_sky = new SkyboxShader();
	PhongShader* shader_phong = new PhongShader();

	model[0] = new Model(modelname[0], 1, 0); vertex += model[0]->nverts(); face += model[0]->nfaces();
	model[1] = new Model(modelname[1], 0, 0); vertex += model[1]->nverts(); face += model[1]->nfaces();

	shader_sky->payload.camera = camera;
	shader_sky->payload.camera_perp_matrix = perspective;

	shader_phong->payload.camera = camera;
	shader_phong->payload.camera_perp_matrix = perspective;

	*shader_skybox = shader_sky;
	*shader_use = shader_phong;

	printf("scene name:%s\n", scene_name);
	printf("model number:%d\n", m);
	printf("vertex:%d faces:%d\n", vertex, face);
}



void build_helmet_scene(Model** model, int& m, IShader** shader_use, IShader** shader_skybox, mat4 perspective, Camera* camera);
void build_gun_scene(Model** model, int& m, IShader** shader_use, IShader** shader_skybox, mat4 perspective, Camera* camera);



