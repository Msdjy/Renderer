#pragma once
#include "./macro.h"
#include "./maths.h"
#include "./model.h"
#include"./object.h"


#include "../shader/shader.h"


const int WINDOW_HEIGHT = 600;
const int WINDOW_WIDTH = 800;

__declspec(selectany) int envmap_width, envmap_height;
__declspec(selectany) std::vector<vec3> envmap;

static void set_color(unsigned char* framebuffer, int x, int y, unsigned char color[]);

static int get_index(int x, int y);

static bool is_back_pace(vec3 pos[]);

static std::tuple<float, float, float> compute_barycentric2D(float x, float y, vec3 vertexs[]);

static int is_inside_triangle(float alpha, float beta, float gamma);

static void triangle_draw(unsigned char* framebuffer, float* zbuffer, IShader* shader);

static void model_draw(unsigned char* framebuffer, float* zbuffer, IShader* shader);

static vec3 castRay(const vec3& eye, const vec3& ray_dir, IShader* shader, const int depth);

void ray_trace(unsigned char* framebuffer, IShader* shader);




