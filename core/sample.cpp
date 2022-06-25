
#include "sample.h"
#include <cstdlib>


static int cal_cubemap_uv(vec3 direction, vec2& uv)
{
	int face_index = -1;
	float ma = 0, sc = 0, tc = 0;
	float abs_x = abs(direction[0]), abs_y = abs(direction[1]), abs_z = abs(direction[2]);

	if (abs_x > abs_y && abs_x > abs_z) {
		//ƫ��x��
		ma = abs_x;
		if (direction[0] > 0) {
			face_index = 0;
			sc = direction.z();
			tc = direction.y();
		}
		else {
			face_index = 1;
			sc = -direction.z();
			tc = direction.y();
		}
	}
	else if (abs_y > abs_z) {
		ma = abs_y;
		if (direction.y() > 0)					/* positive y */
		{
			face_index = 2;
			sc = +direction.x();
			tc = +direction.z();
		}
		else									/* negative y */
		{
			face_index = 3;
			sc = +direction.x();
			tc = -direction.z();
		}
	}
	else {
		ma = abs_z;
		if (direction.z() > 0)					/* positive z */
		{
			face_index = 4;
			sc = -direction.x();
			tc = +direction.y();
		}
		else									/* negative z */
		{
			face_index = 5;
			sc = +direction.x();
			tc = +direction.y();
		}
	}
	uv[0] = (sc / ma + 1.0f) / 2.0f;  //������ת����01֮��
	uv[1] = (tc / ma + 1.0f) / 2.0f;

	return face_index;
}



vec3 texture_sample(vec2 uv, TGAImage* image)
{
	uv[0] = fmod(uv[0], 1);
	uv[1] = fmod(uv[1], 1);
	//printf("%f %f\n", uv[0], uv[1]);
	int uv0 = uv[0] * image->get_width();

	int uv1 = uv[1] * image->get_height();
	TGAColor c = image->get(uv0, uv1);
	vec3 res;
	for (int i = 0; i < 3; i++)
		res[2 - i] = (float)c[i] / 255.f;
	return res;
}

vec3 cubemap_sampling(vec3 direction, cubemap_t* cubemap)
{
	vec2 uv;
	vec3 color;
	int face_index = cal_cubemap_uv(direction, uv); //ͨ��Ƭ�εķ��򣬼���uv��face_index
	color = texture_sample(uv, cubemap->faces[face_index]);

	return color;
}
