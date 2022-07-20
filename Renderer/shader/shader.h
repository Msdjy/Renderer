#pragma once
#include "../core/maths.h"
#include "../core/camera.h"
#include "../core/tgaimage.h"
#include "../core/model.h"
#include <vector>

struct Light {
	vec3 position;
	vec3 power;

	float constant = 1.0f;
	float linear = 0.09f;
	float quadratic = 0.032f;
};

struct payload {
	// mat
	mat4 model_mat;
	mat4 view_mat;
	mat4 perspective_mat;
	mat4 vp_mat;
	mat4 normal_mat;

	// world space point attributes
	vec3 vertexs[3];
	vec3 normals[3];
	vec2 texcoords[3];
	// space point position
	//vec4 view_vertexs[3]; // // view空间坐标应该没用到
	vec4 mvp_vertexs[3];
	vec3 NDC_vertexs[3];
	vec3 screen_vertexs[3];

	// light
	std::vector<Light*> lights;
	
	// camera
	Camera* camera;

	//model
	Model* model;
	TGAImage diffuse_map;
};




class IShader {
public:
	payload payload_shader;

	virtual void vertex_shader() {};
	virtual vec3 fragment_shader(float alpha, float beta, float gamma) { return vec3(0, 0, 0); };
};

class BingPhoneShader : public IShader{
	void vertex_shader();
	vec3 fragment_shader(float alpha, float beta, float gamma);
};


//class PBRShader : public IShader {
//	void vertex_shader();
//	vec3 fragment_shader(float alpha, float beta, float gamma);
//};