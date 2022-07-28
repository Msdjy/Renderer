#include"./object.h"
#include "./pipeline.h"

Intersection Sphere::intersect(const vec3& eye, const vec3& dir)const {
	//std::cout <<center<<" " << radius << " " << material.albedo << " " << material.t << std::endl;
	//get();
	Intersection inter;
	// 球与线相交
	// (O + td - C)^2 = r^2;
	// 解方程求t
	vec3 co = eye - center;
	float a = dir.norm_squared();
	float b = 2 * dot(co, dir);
	float c = co.norm_squared() - radius * radius;

	if (b * b - 4 * a * c < 0)return inter;
	float b24ac = sqrt(b * b - 4 * a * c);
	float t1 = (-b + b24ac) / 2 / a;
	float t0 = (-b - b24ac) / 2 / a;
	
	// offset the original point by .001 to avoid occlusion by the object itself
	if (t0 > .1) {
		inter.pos = eye + t0 * dir;
		inter.normal = normalize(inter.pos - center);
		inter.distance = t0;
		inter.is_intersect = true;
		inter.material = material;
		inter.emission = emission;
		if (dot(dir, inter.normal) > 0)inter.normal = -inter.normal;
		inter.pos = inter.pos + 0.1 * inter.normal;
		return inter;
	}
	if (t1 > .1) {
		inter.pos = eye + t1 * dir;
		inter.normal = normalize(inter.pos - center);
		inter.distance = t1;
		inter.is_intersect = true;
		inter.material = material;
		inter.emission = emission;
		if (dot(dir, inter.normal) > 0)inter.normal = -inter.normal;
		inter.pos = inter.pos + 0.1 * inter.normal;
		return inter;
	}

	return inter;
}

//
bool Sphere::has_emmission()const {
	//std::cout << emission << std::endl;
	return emission.norm_squared() > EPSILON;
}

vec3 Sphere::get_emmission()const {
	//std::cout << emission << std::endl;
	return emission;
}




float Sphere::getArea()const {
	
	return radius * radius * 4.0f * PI;
}

void Sphere::sample(Intersection& inter, float& pdf)const {
	float theta = 2.0 * PI * get_random_float(), phi = PI * get_random_float();
	vec3 dir(std::cos(phi), std::sin(phi) * std::cos(theta), std::sin(phi) * std::sin(theta));
	inter.pos = center + radius * dir;
	inter.normal = dir;
	inter.emission = get_emmission();
	//std::cout << inter.emission << std::endl;
	pdf = 1.0f / getArea();
}


