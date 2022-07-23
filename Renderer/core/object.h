#pragma once
#include"./camera.h"
#include"./maths.h"



struct Material {
	float refractive_index = 1;
	float albedo[4] = { 2,0,0,0 };
	vec3 diffuse_color = { 0,0,0 };
	float specular_exponent = 0;
};

struct Intersection {
	Material material;
	vec3 pos;
	vec3 normal;
	float distance;
	bool is_intersect = false;
	bool is_plane = false;
};



class Object {
public:
	Object() {};
	// C++ ������ʹ�ó��������� �Ķ���:            ������ ���� û��ǿ�������
	// ��������������� TODO
	virtual Intersection intersect(const vec3& eye, const vec3& dir) { Intersection t; return t; };
};


class Sphere : public Object{
public:
	Sphere(vec3 c, float r, Material material) :center(c), radius(r), material(material) {};
	Material material;
	vec3 center;
	float radius;

	virtual Intersection intersect(const vec3& eye, const vec3& dir);
};


