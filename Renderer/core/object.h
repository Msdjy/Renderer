#pragma once
#include"./camera.h"
#include"./maths.h"
#include"./material.h"

#include<iostream>


struct Intersection {
	Material material;
	vec3 pos;
	vec3 normal;
	float distance;
	bool is_intersect = false;
	vec3 emission;
};



class Object {
public:
	Object() {};

	virtual float getArea()const = 0;
	virtual void sample(Intersection& inter, float& pdf)const = 0;
	virtual Intersection intersect(const vec3& eye, const vec3& dir)const = 0;
	virtual void get() { std::cout << "asd" << std::endl; };
	vec3 emission;
	virtual vec3 get_emmission()const = 0;
	virtual bool has_emmission()const = 0;
};

class Sphere : public Object{
public:
	Sphere() {};
	Sphere(float r, vec3 c, vec3 e, Material m) :radius(r), center(c), emission(e), material(m) {
		//std::cout << m.albedo << std::endl;
		std::cout << material.albedo << std::endl;
	};
	void get() {
		std::cout << material.albedo<<" "<<material.roughness << std::endl;
	}
	virtual float getArea()const;
	virtual void sample(Intersection& inter, float& pdf)const;
	Intersection intersect(const vec3& eye, const vec3& dir)const;
	// TODO 子类访问父类函数，或属性
	bool has_emmission()const;
	vec3 get_emmission()const;
	vec3 emission;
	vec3 center;
	float radius;
	Material material;

};


