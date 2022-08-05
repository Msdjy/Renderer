#pragma once

#include "./macro.h"
#include "./maths.h"
#include "./model.h"
#include "./object.h"
#include "../shader/shader.h"

#include <iostream>
#include <cmath>
#include <random>


const float RussianRoulette = 0.8;



class Scene {
public:
	//Scene();
	void add(Object* object) { objects.push_back(object); };
	void add(Light* light) { lights.push_back(light); };


	Intersection scene_intersect(const vec3& orig, const vec3& dir);
	void sample_InterLight(Intersection& inter, float& pdf);
	vec3 castRay_pathTracing(const vec3& ori, const vec3& dir, const int depth = 4);



	vec3 castRay_whited(const vec3& eye, const vec3& ray_dir, const int depth = 4);
	vec3 reflect(vec3 in_dir, vec3 normal);
	vec3 refract(vec3 in_dir, vec3 normal, float refractive_index);

	std::vector<Object* > objects;

	std::vector<Light* > lights; // 只有Whitted用这个作为点光源
	Camera* camera;
};


