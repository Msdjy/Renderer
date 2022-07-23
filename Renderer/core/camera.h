#pragma once

#include"./maths.h"

class Camera {
public:
	Camera(vec3 eye, vec3 target, vec3 up, float aspect, float fov);
	~Camera();

	vec3 eye;
	vec3 target;
	vec3 up;
	float aspect;

	vec3 horizontal;
	vec3 vertical;
	float fov;
	float screen_distance = 1;
	float horizontal_len;
	float vertical_len;
	vec3 left_top_dir;

	vec3 x;
	vec3 y;
	vec3 z;
};


//handle event
void updata_camera_pos(Camera& camera);
void handle_events(Camera& camera);


