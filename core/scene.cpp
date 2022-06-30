#include "./scene.h"
#include <filesystem>


TGAImage* texture_from_file(const char* file_name)
{
	TGAImage* texture = new TGAImage();
	texture->read_tga_file(file_name);
	texture->flip_vertically();
	return texture;
}

cubemap_t* cubemap_from_files(const char* positive_x, const char* negative_x,
	const char* positive_y, const char* negative_y,
	const char* positive_z, const char* negative_z)
{
	cubemap_t* cubemap = new cubemap_t();
	cubemap->faces[0] = texture_from_file(positive_x);
	cubemap->faces[1] = texture_from_file(negative_x);
	cubemap->faces[2] = texture_from_file(positive_y);
	cubemap->faces[3] = texture_from_file(negative_y);
	cubemap->faces[4] = texture_from_file(positive_z);
	cubemap->faces[5] = texture_from_file(negative_z);
	return cubemap;
}



void load_ibl_map(payload_t& p, const char* env_path)
{
	int i, j;
	iblmap_t* iblmap = new iblmap_t();
	const char* faces[6] = { "px", "nx", "py", "ny", "pz", "nz" };
	char paths[6][256];

	iblmap->mip_levels = 10;

	/* diffuse environment map */
	for (j = 0; j < 6; j++) {
		sprintf_s(paths[j], "%s/i_%s.tga", env_path, faces[j]);
	}
	iblmap->irradiance_map = cubemap_from_files(paths[0], paths[1], paths[2],
		paths[3], paths[4], paths[5]);

	/* specular environment maps */
	for (i = 0; i < iblmap->mip_levels; i++) {
		for (j = 0; j < 6; j++) {
			sprintf_s(paths[j], "%s/m%d_%s.tga", env_path, i, faces[j]);
		}
		iblmap->prefilter_maps[i] = cubemap_from_files(paths[0], paths[1],
			paths[2], paths[3], paths[4], paths[5]);
	}

	/* brdf lookup texture */
	iblmap->brdf_lut = texture_from_file("G:/VisualStudio/WorkSpace/Renderer/obj/common/BRDF_LUT.tga");

	p.iblmap = iblmap;

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



Model* getUVsphere(const char *filename)
{
	Model *m = new Model();
	std::vector<vec3> verts;
	std::vector<std::vector<int>> faces; // attention, this Vec3i means vertex/uv/normal
	std::vector<vec3> norms;
	std::vector<vec2> uvs;
	std::vector<int> indices;

	const unsigned int X_SEGMENTS = 64;
	const unsigned int Y_SEGMENTS = 64;
	for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
	{
		for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
		{
			float xSegment = (float)x / (float)X_SEGMENTS;
			float ySegment = (float)y / (float)Y_SEGMENTS;
			float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
			float yPos = std::cos(ySegment * PI);
			float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

			verts.push_back(vec3(xPos, yPos, zPos));
			uvs.push_back(vec2(xSegment, ySegment));
			norms.push_back(vec3(xPos, yPos, zPos));
		}
	}

	bool oddRow = false;
	for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
	{
		if (!oddRow) // even rows: y == 0, y == 2; and so on
		{
			for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
			{
				indices.push_back(y * (X_SEGMENTS + 1) + x);
				indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
			}
		}
		else
		{
			for (int x = X_SEGMENTS; x >= 0; --x)
			{
				indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
				indices.push_back(y * (X_SEGMENTS + 1) + x);
			}
		}
		oddRow = !oddRow;
	}
	unsigned int indexCount = static_cast<unsigned int>(indices.size());

	//此时indices里存储了顶点下标
	for (int i = 0; i < indexCount - 2; i++) {  //indices[0, 1, 2] 为第一个三角形  indices[2, 1, 3] 为第二个, 2,3,4,第三个， 4,3,4第四个
		faces.push_back(std::vector<int>());
		for (int j = 0; j < 3; j++) { //3个顶点
			if (i % 2 == 0) {
				faces[i] = { indices[i], indices[i], indices[i], 
							indices[i + 1], indices[i + 1], indices[i + 1], 
							indices[i + 2], indices[i + 2], indices[i + 2] };  //位置，uv，法线的下标一样
			}
			else {
				faces[i] = { indices[i + 1], indices[i + 1], indices[i + 1],
							indices[i], indices[i], indices[i],
							indices[i + 2], indices[i + 2], indices[i + 2] };  //位置，uv，法线的下标一样
			}
			
		}
	}
	m->setVerts(std::move(verts));
	m->setNorm(std::move(norms));
	m->setUV(std::move(uvs));
	m->setFaces(std::move(faces));
	const char* modelname[] =
	{
		"G:/VisualStudio/WorkSpace/Renderer/obj/sphere_gold2/sphere.obj",
	};
	
	m->create_map(filename);
	return m;
}

void build_sphere_scene(Model** model, int& m, IShader** shader_use, IShader** shader_skybox, mat4 perspective, Camera* camera)
{
	m = 1;
	const char* modelname[] =
	{
		"G:/VisualStudio/WorkSpace/Renderer/obj/sphere/sphere.obj",
	};

	int vertex = 0, face = 0;
	const char* scene_name = "sphere";
	PBRShader* shader_PBR = new PBRShader();
	//model[0] = new Model();
	model[0] = getUVsphere(modelname[0]);

	vertex += model[0]->nverts();
	face += model[0]->nfaces();
	/*for (int i = 0; i < m; i++)
	{
		model[i] = new Model(modelname[i], 0, 0);
		vertex += model[i]->nverts();
		face += model[i]->nfaces();
	}*/

	shader_PBR->payload.camera_perp_matrix = perspective;
	shader_PBR->payload.camera = camera;

	*shader_use = shader_PBR;
	*shader_skybox = NULL;

	printf("scene name:%s\n", scene_name);
	printf("model number:%d\n", m);
	printf("vertex:%d faces:%d\n", vertex, face);
}


void build_IBL_sphere_scene(Model** model, int& m, IShader** shader_use, IShader** shader_skybox, mat4 perspective, Camera* camera)
{
	m = 6;
	const char* modelname[] =
	{
		"G:/VisualStudio/WorkSpace/Renderer/obj/sphere_red/sphere.obj",

		"G:/VisualStudio/WorkSpace/Renderer/obj/sphere/sphere.obj",

		"G:/VisualStudio/WorkSpace/Renderer/obj/sphere_gold/sphere.obj",
		"G:/VisualStudio/WorkSpace/Renderer/obj/sphere_white/sphere.obj",

		"G:/VisualStudio/WorkSpace/Renderer/obj/sphere_floor/sphere.obj",

		"G:/VisualStudio/WorkSpace/Renderer/obj/skybox4/box.obj",
	};

	int vertex = 0, face = 0;
	const char* scene_name = "sphere";
	PBRShader* shader_PBR = new PBRShader();
	SkyboxShader* shader_sky = new SkyboxShader();

	for (int i = 0; i < 5; i++) {
		model[i] = getUVsphere(modelname[i]); vertex += model[i]->nverts(); face += model[i]->nfaces();
		std::cout << "model " << i << " done" << std::endl;
	}

	model[5] = new Model(modelname[5], 1, 0); vertex += model[5]->nverts(); face += model[5]->nfaces();  //读取天空盒


	shader_sky->payload.camera = camera;
	shader_sky->payload.camera_perp_matrix = perspective;

	shader_PBR->payload.camera = camera;
	shader_PBR->payload.camera_perp_matrix = perspective;

	load_ibl_map(shader_PBR->payload, "G:/VisualStudio/WorkSpace/Renderer/obj/common2");


	*shader_skybox = shader_sky;
	*shader_use = shader_PBR;


	printf("scene name:%s\n", scene_name);
	printf("model number:%d\n", m);
	printf("vertex:%d faces:%d\n", vertex, face);
}




void build_helmet_scene(Model** model, int& m, IShader** shader_use, IShader** shader_skybox, mat4 perspective, Camera* camera)
{
	m = 2;
	const char* modelname[] =
	{
		"G:/VisualStudio/WorkSpace/Renderer/obj/helmet/helmet.obj",
		"G:/VisualStudio/WorkSpace/Renderer/obj/skybox4/box.obj",
	};

	PBRShader* shader_pbr = new PBRShader();
	SkyboxShader* shader_sky = new SkyboxShader();

	int vertex = 0, face = 0;
	const char* scene_name = "helmet";
	model[0] = new Model(modelname[0], 0, 0); vertex += model[0]->nverts(); face += model[0]->nfaces();
	model[1] = new Model(modelname[1], 1, 0); vertex += model[1]->nverts(); face += model[1]->nfaces();

	shader_pbr->payload.camera_perp_matrix = perspective;
	shader_pbr->payload.camera = camera;
	shader_sky->payload.camera_perp_matrix = perspective;
	shader_sky->payload.camera = camera;

	load_ibl_map(shader_pbr->payload, "G:/VisualStudio/WorkSpace/Renderer/obj/common2");

	*shader_use = shader_pbr;
	*shader_skybox = shader_sky;

	printf("scene name:%s\n", scene_name);
	printf("model number:%d\n", m);
	printf("vertex:%d faces:%d\n", vertex, face);
}
void build_gun_scene(Model** model, int& m, IShader** shader_use, IShader** shader_skybox, mat4 perspective, Camera* camera)
{
	m = 2;
	const char* modelname[] =
	{
		"G:/VisualStudio/WorkSpace/Renderer/obj/gun/Cerberus.obj",
		"G:/VisualStudio/WorkSpace/Renderer/obj/skybox4/box.obj",
	};

	PBRShader* shader_pbr = new PBRShader();
	SkyboxShader* shader_sky = new SkyboxShader();

	int vertex = 0, face = 0;
	const char* scene_name = "gun";
	model[0] = new Model(modelname[0], 0, 0); vertex += model[0]->nverts(); face += model[0]->nfaces();
	model[1] = new Model(modelname[1], 1, 0); vertex += model[1]->nverts(); face += model[1]->nfaces();

	shader_pbr->payload.camera_perp_matrix = perspective;
	shader_pbr->payload.camera = camera;
	shader_sky->payload.camera_perp_matrix = perspective;
	shader_sky->payload.camera = camera;

	load_ibl_map(shader_pbr->payload, "G:/VisualStudio/WorkSpace/Renderer/obj/common2");

	*shader_use = shader_pbr;
	*shader_skybox = shader_sky;

	printf("scene name:%s\n", scene_name);
	printf("model number:%d\n", m);
	printf("vertex:%d faces:%d\n", vertex, face);
}



