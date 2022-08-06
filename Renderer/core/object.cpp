#include"./object.h"
#include "./pipeline.h"



//
bool Object::has_emmission()const {
	return emission.norm_squared() > EPSILON;
}

vec3 Object::get_emmission()const {
	return emission;
}


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
	if (t0 < 0)t0 = t1;
	// offset the original point by .001 to avoid occlusion by the object itself
	if (t0 > .1) {
		inter.pos = eye + t0 * dir;
		inter.normal = normalize(inter.pos - center);
		inter.distance = t0;
		inter.is_intersect = true;
		inter.material = material;
		inter.emission = emission;
		//inter.pos = inter.pos + 0.1 * inter.normal;
		if (dot(dir, inter.normal) > 0)inter.normal = -inter.normal;
		return inter;
	}

	return inter;
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


Intersection Triangle::intersect(const vec3& eye, const vec3& dir)const {
	Intersection inter;
	vec3 S = eye - v0;
	vec3 S1 = cross(dir, E2);
	vec3 S2 = cross(S, E1);
	float coeff = 1.0 / dot(S1, E1); // 共同系数
	float t = coeff * dot(S2, E2);
	float b1 = coeff * dot(S1, S);
	float b2 = coeff * dot(S2, dir);
	if (t >= 0.1 && b1 >= 0 && b2 >= 0 && (1 - b1 - b2) >= 0)
	{
		//u = b1;
		//v = b2;
		inter.distance = t;
		inter.is_intersect = true;
		inter.pos = eye + t * dir;
		inter.normal = normal;

		inter.material = material;
		inter.emission = emission;
		//if (dot(dir, inter.normal) > 0)inter.normal = -inter.normal;
	}
	return inter;
}

float Triangle::getArea()const {
	return area;
}

void Triangle::sample(Intersection& inter, float& pdf)const {
	//float theta = get_random_float(), phi = get_random_float();
	//vec3 dir(std::cos(phi), std::sin(phi) * std::cos(theta), std::sin(phi) * std::sin(theta));
	//inter.pos = v0 + (theta * (v1 - v0) + phi * (v2 - v0)) / 2.0;

	//inter.normal = normal;
	//inter.emission = get_emmission();
	////std::cout << inter.emission << std::endl;
	//pdf = 1.0f / getArea();
	// TODO 采样，上面的采样为啥不对
	float x = std::sqrt(get_random_float()), y = get_random_float();
	inter.normal = normal;
	inter.emission = get_emmission();
	inter.pos = v0 * (1.0f - x) + v1 * (x * (1.0f - y)) + v2 * (x * y);
	pdf = 1.0f / area;
}

