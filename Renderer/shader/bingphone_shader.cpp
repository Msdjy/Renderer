#pragma once
#include "./shader.h"

vec3 texture_sample(vec2 uv, TGAImage* image)
{
	uv[0] = fmod(uv[0], 1);
	uv[1] = fmod(uv[1], 1);
	//printf("%f %f\n", uv[0], uv[1]);
	int uv0 = uv[0] * image->width();
	int uv1 = uv[1] * image->width();
	TGAColor c = image->get(uv0, uv1);
	vec3 res;
	for (int i = 0; i < 3; i++)
		res[2 - i] = (float)c[i] / 255.f;
	return res;
}


// world space -> NDC space -> screen space -> fagment shader
//            mvp              
//                 裁剪        重心插值（修正）=> 片段的法向量，uv，z值
//                                              世界空间的坐标，光源坐标，相机坐标
void BingPhoneShader::vertex_shader() {
	// 顶点属性，法向量，坐标，uv
	//payload_shader.vertexs;
	//payload_shader.normals;
	//payload_shader.texcoords;
	vec4 vertexs[3];
	for (int j = 0; j < 3; j++) {
		vertexs[j]  = to_vec4(payload_shader.vertexs[j], 1.0f);
		payload_shader.mvp_vertexs[j]  = payload_shader.vp_mat * payload_shader.model_mat * vertexs[j];
		// view空间坐标应该没用到
		//payload_shader.view_vertexs[j] = payload_shader.view_mat * vertexs[j];
	}
	// 法线矩阵
	// only model matrix can change normal vector in world space ( Normal Matrix: tranverse(inverse(model)) )
	payload_shader.normal_mat = payload_shader.model_mat.inverse_transpose();
}

vec3 BingPhoneShader::fragment_shader(float alpha, float beta, float gamma) {
	//插值
		//顶点法向量
		//顶点vu值
		//纹理
	//光源
	//相机位姿
	

	float normalizer = 1.0 / (alpha / payload_shader.mvp_vertexs[0].w()
							+ beta  / payload_shader.mvp_vertexs[1].w()
							+ gamma / payload_shader.mvp_vertexs[2].w());

	vec4 normal4[3];
	for (int i = 0; i < 3; i++) {
		normal4[i] = payload_shader.normal_mat * to_vec4(payload_shader.normals[i], 1.0f);
	}
	// 插值法向量，世界空间坐标，uv
	vec3 normal = (alpha * payload_shader.normals[0] / payload_shader.mvp_vertexs[0].w()
				+ beta  * payload_shader.normals[1] / payload_shader.mvp_vertexs[1].w()
				+ gamma * payload_shader.normals[2] / payload_shader.mvp_vertexs[2].w()) * normalizer;
	vec3 position = (alpha * payload_shader.vertexs[0] / payload_shader.mvp_vertexs[0].w()
		          + beta  * payload_shader.vertexs[1] / payload_shader.mvp_vertexs[1].w()
		          + gamma * payload_shader.vertexs[2] / payload_shader.mvp_vertexs[2].w()) * normalizer;
	vec2 texcoords = (alpha * payload_shader.texcoords[0] / payload_shader.mvp_vertexs[0].w()
				   + beta  * payload_shader.texcoords[1] / payload_shader.mvp_vertexs[1].w()
				   + gamma * payload_shader.texcoords[2] / payload_shader.mvp_vertexs[2].w()) * normalizer;

	vec3 view_pos = payload_shader.camera->eye;
	vec3 view_dir = normalize(view_pos - position);
	
 
	vec3 diffuse = texture_sample(texcoords, &payload_shader.diffuse_map);
	//std::cout << texcoords << std::endl;

	vec3 color(0,0,0);
	vec3 amblient(0.1,0.1,0.1);
	color += amblient;

	//light
	for (auto light : payload_shader.lights) {
		vec3 light_dir = normalize(light->position - position);
		vec3 half_dir = normalize(light_dir + view_dir);

		float distance = (light->position - position).norm();
		float attenuation = 1.0 / (light->constant + light->linear * distance + light->quadratic * (distance * distance));

		float diff = std::max(dot(light_dir, normal), 0.0);
		color += diff * diffuse * light->power * attenuation;

		float spec = pow(std::max(dot(half_dir, normal), 0.0), 64.0f);
		color += spec * diffuse * light->power * attenuation;
	}

	//std::cout << color << std::endl;


	return color;
}