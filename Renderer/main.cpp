#pragma once
#include "./core/macro.h"
#include "./core/pipeline.cpp"
#include "./core/pipeline.h"
#include "./core/camera.h"
#include "./core/model.h"
#include "./core/tgaimage.h"
#include "./platform/win32.h"
#include "./shader/shader.h"
#include<vector>


//#include"renderer.hpp"
//#include"camera.hpp"
//#include"light.hpp"
//#include"object.hpp"
//#include"scene.hpp"
//#include"sphere.hpp"
//#include"material.hpp"



void clear_zbuffer(int width, int height, float* zbuffer);
void clear_framebuffer(int width, int height, unsigned char* framebuffer);
void update_matrix(Camera& camera, mat4& view_mat, mat4& perspective_mat, IShader* shader);



int main() 
{
    // initialization
    // --------------
    // malloc memory for zbuffer and framebuffer
    int width = WINDOW_WIDTH, height = WINDOW_HEIGHT;
    float* zbuffer = (float*)malloc(sizeof(float) * width * height);
    unsigned char* framebuffer = (unsigned char*)malloc(sizeof(unsigned char) * width * height * 4);
    memset(framebuffer, 0, sizeof(unsigned char) * width * height * 4);


    // initialize window
    window_init(width, height, "Render");

	// create camera
	vec3 eye(0, 1, 5);
	vec3 target(0, 1, 0);
	vec3 up(0, 1, 0);
	float aspect = (float)(width) / height;
	Camera camera(eye, target, up, aspect);

	// mvp
	float zNear = -0.1;
	float zFar = -50;
	
	mat4 model_mat		 = mat4_translate(0, 0, 0) ;
	//mat4 model_mat		 = mat4::identity();
	mat4 view_mat		 = mat4_lookat(camera.eye, camera.target, camera.up);
	mat4 perspective_mat = mat4_perspective(60, aspect, zNear, zFar);
	
	// znear大于zfar，top大于bottom
	float halve = 60 / 2 * PI / 180;
	float top = tan(halve) * abs(zNear);
	float bottom = -top;
	float right = top * aspect;
	float left = -right;
	mat4 ortho_mat = mat4_ortho(left, right, bottom, top, zNear, zFar);

	perspective_mat = ortho_mat * perspective_mat;

	// light
	Light* light1 = new Light;
	light1->position = vec3(5,5,5);
	light1->power = vec3(10,10,10);	

	Light* light2 = new Light;
	light2->position = vec3(-5,-5,5);
	light2->power = vec3(10,10,10);
	
	// model
	Model* model = new Model("./objects/african_head.obj");
	
	// shader
	IShader* shader  = new BingPhoneShader();
	shader->payload_shader.lights.push_back(light1);
	//shader->payload_shader.lights.push_back(light2);
	shader->payload_shader.camera = &camera;

	shader->payload_shader.model_mat = model_mat;
	shader->payload_shader.view_mat = view_mat;
	shader->payload_shader.perspective_mat = perspective_mat;
	shader->payload_shader.vp_mat = perspective_mat * view_mat;

	shader->payload_shader.model = model;
	shader->payload_shader.diffuse_map = model->diffuse();

	// render loop
	// -----------
	int num_frames = 0;
	float print_time = platform_get_time();
	while (!window->is_close)
	{
		float curr_time = platform_get_time();

		// clear buffer
		clear_framebuffer(width, height, framebuffer);
		clear_zbuffer(width, height, zbuffer);
		
		
		// 
		// handle events and update view, perspective matrix
		handle_events(camera);
		update_matrix(camera, view_mat, perspective_mat, shader);
		//std::cout << camera.eye << std::endl;


		model_draw(framebuffer, zbuffer, shader);

		// calculate and display FPS
		num_frames += 1;
		if (curr_time - print_time >= 1) {
			int sum_millis = (int)((curr_time - print_time) * 1000);
			int avg_millis = sum_millis / num_frames;
			printf("fps: %3d, avg: %3d ms\n", num_frames, avg_millis);
			num_frames = 0;
			print_time = curr_time;
		}

		// reset mouse information
		window->mouse_info.wheel_delta = 0;
		window->mouse_info.orbit_delta = vec2(0, 0);
		window->mouse_info.fv_delta = vec2(0, 0);

		// send framebuffer to window 
		window_draw(framebuffer);
		msg_dispatch();
	}



	free(zbuffer);
	free(framebuffer);
	window_destroy();

	system("pause");
	return 0;

}

void clear_zbuffer(int width, int height, float* zbuffer)
{
	for (int i = 0; i < width * height; i++)
		zbuffer[i] = 100000;
}

void clear_framebuffer(int width, int height, unsigned char* framebuffer)
{
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			int index = (i * width + j) * 4;

			framebuffer[index + 2] = 80;
			framebuffer[index + 1] = 56;
			framebuffer[index] = 56;
		}
	}
}


void update_matrix(Camera& camera, mat4& view_mat, mat4& perspective_mat, IShader* shader) {
	//TODO
	view_mat = mat4_lookat(camera.eye, camera.target, camera.up);
	mat4 vp = perspective_mat * view_mat;

	shader->payload_shader.view_mat = view_mat;
	shader->payload_shader.vp_mat = vp;
}