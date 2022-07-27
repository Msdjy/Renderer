#pragma once
#include"./maths.h"

enum MaterialType { DIFFUSE, MICROFACET };

class Material {
public:
	MaterialType t;
	// 菲尼尔项值由ior得到
	// kd和ks的漫反射和镜面反射由菲尼尔项得到，
	// ？TODO 折射系数怎么考虑，直接当作kd吗
	// TODO 这是哪个值
	// https://zhuanlan.zhihu.com/p/21961722
	// 金属没有漫反射
	// 非金属的值定为常数（0.04, 0.04, 0.04）
	float roughness;
	float metallic;
	float ior;
	vec3 albedo;

	// phone模型所需的参数
	//float albedo[4] = { 2,0,0,0 };
	float specular_exponent = 0;
	Material() {};
	Material(float ior, float roughness, float metallic, vec3 albedo, MaterialType t) :ior(ior), roughness(roughness), metallic(metallic), albedo(albedo), t(t) {
		std::cout << albedo << std::endl;
	};
	vec3 eval(const vec3& in_dir, const vec3& out_dir, const vec3& normal);
	vec3 sample(const vec3& in_dir, const vec3& norma);
	float pdf(const vec3& in_dir, const vec3& out_dir, const vec3& normal);
};
