#include"./object.h"


Intersection Sphere::intersect(const vec3& eye, const vec3& dir) {
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
	if (t0 > .001) {
		inter.pos = eye + t0 * dir;
		inter.normal = normalize(inter.pos - center);
		inter.distance = t0;
		inter.is_intersect = true;
		inter.material = material;
		return inter;
	}
	if (t1 > .001) {
		inter.pos = eye + t1 * dir;
		inter.normal = normalize(inter.pos - center);
		inter.distance = t0;
		inter.is_intersect = true;
		inter.material = material;
		return inter;
	}

	return inter;
}