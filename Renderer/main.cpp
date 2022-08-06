#pragma once
//#include "./core/macro.h"
#include "./core/pipeline.h"
#include "./core/scene.h"
#include "./core/camera.h"
//#include "./core/model.h"
#include "./core/tgaimage.h"
//#include "./core/object.h"
#include "./platform/win32.h"
#include "./shader/shader.h"
#include<vector>
#include <fstream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//#include"renderer.hpp"
//#include"camera.hpp"
//#include"light.hpp"
//#include"object.hpp"
//#include"scene.hpp"
//#include"sphere.hpp"
//#include"material.hpp"

enum RenderType { RASTERIZER, RAYTRACING, PATHTRACING};



void clear_zbuffer(int width, int height, float* zbuffer);
void clear_framebuffer(int width, int height, unsigned char* framebuffer);
void update_matrix(Camera& camera, mat4& view_mat, mat4& perspective_mat, IShader* shader);

//constexpr Material      ivory = { 1.0, {0.9,  0.5, 0.1, 0.0}, {0.4, 0.4, 0.3},   50. };
//constexpr Material      glass = { 1.5, {0.0,  0.9, 0.1, 0.8}, {0.6, 0.7, 0.8},  125. };
//constexpr Material red_rubber = { 1.0, {1.4,  0.3, 0.0, 0.0}, {0.3, 0.1, 0.1},   10. };
//constexpr Material     mirror = { 1.0, {0.0, 16.0, 0.8, 0.0}, {1.0, 1.0, 1.0}, 1425. };
//Material      ivory = { 1.0, {0.9,  0.5, 0.1, 0.0}, {0.4, 0.4, 0.3},   50. };
//Material      glass = { 1.5, {0.0,  0.9, 0.1, 0.8}, {0.6, 0.7, 0.8},  125. };
//Material red_rubber = { 1.0, {1.4,  0.3, 0.0, 0.0}, {0.3, 0.1, 0.1},   10. };
//Material     mirror = { 1.0, {0.0, 16.0, 0.8, 0.0}, {1.0, 1.0, 1.0}, 1425. };

int main() 
{
    // initialization
    // --------------
    // malloc memory for zbuffer and framebuffer
    int width = WINDOW_WIDTH, height = WINDOW_HEIGHT;
    float* zbuffer = (float*)malloc(sizeof(float) * width * height);
    unsigned char* framebuffer = (unsigned char*)malloc(sizeof(unsigned char) * width * height * 4);
    memset(framebuffer, 0, sizeof(unsigned char) * width * height * 4);

	// TODO
	// 很耗内存
	//int n = -1;
	//unsigned char* pixmap = stbi_load("./image/envmap.jpg", &envmap_width, &envmap_height, &n, 0);
	//if (!pixmap || 3 != n) {
	//	std::cerr << "Error: can not load the environment map" << std::endl;
	//	return -1;
	//}
	//envmap = std::vector<vec3>(envmap_width * envmap_height);
	//for (int j = envmap_height - 1; j >= 0; j--) {
	//	for (int i = 0; i < envmap_width; i++) {
	//		envmap[i + j * envmap_width] = vec3(pixmap[(i + j * envmap_width) * 3 + 0], pixmap[(i + j * envmap_width) * 3 + 1], pixmap[(i + j * envmap_width) * 3 + 2]) * (1 / 255.);
	//	}
	//}
	//stbi_image_free(pixmap);

    // initialize window
    window_init(width, height, "Render");

	// create camera
	vec3 eye(50, 40, 168);
	vec3 target(0, 0, -1);
	target = eye + target;
	vec3 up(0, 1, 0);
	float fov = 60;
	float aspect = (float)(width) / height;
	Camera camera(eye, target, up, aspect, fov);

	// model
	//Model* model = new Model("./objects/african_head/african_head.obj");

	// sphere
	// Material(float ior, float roughness, float metallic, vec3 albedo, MaterialType t = DIFFUSE)
	Material      Left     = { 30, 1, 0, vec3(0.63f, 0.065f, 0.05f),	DIFFUSE };
	Material      Right    = { 30, 1, 0, vec3(0.14f, 0.45f, 0.091f),	DIFFUSE };
	Material	  Back	   = { 30, 1, 0, vec3(0.4f,0.2f,0.2f),	DIFFUSE };
	Material	  Front	   = { 30, 1, 0, vec3(),				DIFFUSE };
	Material	  Front2   = { 1.0, 1, 0, vec3(.5f,.5f,.5f),				DIFFUSE };
	Material      Botm_Top = { 30, 1, 0, vec3(0.4f,0.2f,0.2f),	DIFFUSE };
	Material      Mirr	   = { 20, 0.05, 1, vec3(1,0.86,0.57) * .999, MICROFACET };
	Material      Glas	   = { 1.6, 0.05, 0.0, vec3(0.0,0.02,0.02) * .999, MICROFACET };
	
	Sphere spheres[] = {
		// Sphere(float r, vec3 c, vec3 emission, Material material)
		// TODO 1e5会有奇怪的方框和圆形
		Sphere(1e3, vec3(1e3 + 1,40.8,81.6),   vec3(),			Left),//Left 
		Sphere(1e3, vec3(-1e3 + 99,40.8,81.6), vec3(),			Right),//Rght 
		Sphere(1e3, vec3(50,40.8, 1e3),        vec3(),			Back),//Back 
		Sphere(1e3, vec3(50,40.8,-1e3 + 170),  vec3(),			Front),//Frnt 
		Sphere(1e3, vec3(50, 1e3, 81.6),       vec3(),			Botm_Top),//Botm 
		//Sphere(1e3, vec3(50, -1e3 + 81.6,81.6), vec3(),			Botm_Top),//Top 
		Sphere(16.5,vec3(27,16.5,47),          vec3(),			Mirr),//Mirr 
		Sphere(16.5,vec3(73,16.5,70),          vec3(),			Glas),//Glas 
		//Sphere(5, vec3(50,77,81.6), vec3(300,300,300),  Front2) //Lite 
	};
	float slen = 10;
	Triangle triangles[] = {
		// Triangle(vec3 v0, vec3 v1, vec3 v2, vec3 e, Material m) :Object(e, m), v0(v0), v1(v1), v2(v2) {
		Triangle(vec3(50 + slen, 81.59, 80 - slen), vec3(50 + slen, 81.59, 80 + slen), vec3(50 - slen, 81.59, 80 - slen), vec3(100,100,100), Front2),
		Triangle(vec3(50 - slen, 81.59, 80 + slen), vec3(50 - slen, 81.59, 80 - slen), vec3(50 + slen, 81.59, 80 + slen), vec3(100,100,100), Front2),
		//Triangle(vec3(50 + slen, 81.59, 80 - slen), vec3(50 + slen, 81.59, 80 + slen), vec3(50 - slen, 81.59, 80 - slen), vec3(100,100,100), Front2),
		//Triangle(vec3(50 - slen, 81.59, 80 + slen), vec3(50 - slen, 81.59, 80 - slen), vec3(50 + slen, 81.59, 80 + slen), vec3(100,100,100), Front2),
		Triangle(vec3(99, 81.6, 0), vec3(99, 81.6, 170), vec3(1, 81.6, 0), vec3(0), Botm_Top),
		Triangle(vec3(1, 81.6, 170), vec3(1, 81.6, 0), vec3(99, 81.6, 170), vec3(0), Botm_Top),
	};

	// 目前只用在RAYTRACING和光栅化作为点光源
	Light* light1 = new Light;
	light1->position = vec3(-20, 20, 20);
	light1->power = vec3(100, 100, 100);

	Light* light2 = new Light;
	light2->position = vec3(10, 10, 150);
	light2->power = vec3(10, 10, 10);

	Light* light3 = new Light;
	light3->position = vec3(50, 65, 81.6);
	light3->power = vec3(20, 20, 20);

	mat4 model_mat;
	mat4 view_mat;
	mat4 perspective_mat;
	

	Scene scene;

	IShader* shader = new BingPhoneShader();

	RenderType rt = PATHTRACING;
	switch(rt) {
		case RAYTRACING: {
			// scene
			for (auto& sphere : spheres) {
				scene.add(&sphere);
			}
			scene.camera = &camera;
			// light
			scene.add(light1);
			scene.add(light2);
			scene.add(light3);
			break;
		}
		case PATHTRACING: {
			// scene
			for (auto& sphere : spheres) {
				scene.add(&sphere);
			}
			for (auto& triangle : triangles) {
				scene.add(&triangle);
			}
			scene.camera = &camera;
			break;
		}
		case RASTERIZER: {
			// mvp
			float zNear = -0.1;
			float zFar = -50;

			model_mat = mat4_translate(0, 0, 0);
			//model_mat		 = mat4::identity();
			view_mat = mat4_lookat(camera.eye, camera.target, camera.up);
			perspective_mat = mat4_perspective(fov, aspect, zNear, zFar);

			// znear大于zfar，top大于bottom
			float top = tan(fov / 2.0f * PI / 180.0f) * abs(zNear);
			float bottom = -top;
			float right = top * aspect;
			float left = -right;
			mat4 ortho_mat = mat4_ortho(left, right, bottom, top, zNear, zFar);
			perspective_mat = ortho_mat * perspective_mat;

			// shader payload
			// auto sphere : spheres TODO
			for (auto& sphere : spheres) {
				shader->payload_shader.objects.push_back(&sphere);
			}
			shader->payload_shader.lights.push_back(light1);
			shader->payload_shader.lights.push_back(light2);
			shader->payload_shader.lights.push_back(light3);
			shader->payload_shader.camera = &camera;

			shader->payload_shader.model_mat = model_mat;
			shader->payload_shader.view_mat = view_mat;
			shader->payload_shader.perspective_mat = perspective_mat;
			shader->payload_shader.vp_mat = perspective_mat * view_mat;
			Model* model = new Model("./objects/african_head/african_head.obj");
			shader->payload_shader.model = model;
			break;
		}
	}

	//// 多spp输出图片
	//{
	//	path_trace_getimage(framebuffer, scene);

	//	FILE* fp = fopen("binary.ppm", "wb");
	//	(void)fprintf(fp, "P6\n%d %d\n255\n", WINDOW_WIDTH, WINDOW_HEIGHT);
	//	for (int y = 0; y < WINDOW_HEIGHT; y++) {
	//		for (int x = 0; x < WINDOW_WIDTH; x++) {
	//			int index = (y * WINDOW_WIDTH + x) * 4;
	//			static unsigned char color[3];
	//			for (int i = 0; i < 3; i++) {
	//				color[i] = framebuffer[index + i];
	//			}
	//			fwrite(color, 1, 3, fp);
	//		}
	//	}

	//	fclose(fp);

	//	return 0;
	//}
	

	// render loop
	// -----------
	int num_frames = 0;
	float print_time = platform_get_time();

	switch (rt) {
		case RAYTRACING: {
			while (!window->is_close)
			{
				float curr_time = platform_get_time();

				// clear buffer
				clear_framebuffer(width, height, framebuffer);
				clear_zbuffer(width, height, zbuffer);

				ray_trace(framebuffer, scene);

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
			break;
		}
		case PATHTRACING: {
			while (!window->is_close)
			{
				float curr_time = platform_get_time();

				// clear buffer
				clear_framebuffer(width, height, framebuffer);
				clear_zbuffer(width, height, zbuffer);


				path_trace(framebuffer, scene);


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
			break;
		}
		case RASTERIZER: {
			while (!window->is_close)
			{
				float curr_time = platform_get_time();

				// clear buffer
				clear_framebuffer(width, height, framebuffer);
				clear_zbuffer(width, height, zbuffer);


				// handle events and update view, perspective matrix
				handle_events(camera);
				update_matrix(camera, view_mat, perspective_mat, shader);

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
			break;

		}
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