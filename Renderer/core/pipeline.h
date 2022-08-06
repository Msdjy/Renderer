#pragma once
#include "./macro.h"
#include "./maths.h"
#include "./model.h"
#include"./object.h"
#include"./scene.h"

#include <iostream>
#include <cmath>
#include <random>
#include "../shader/shader.h"



const int WINDOW_HEIGHT = 600;
const int WINDOW_WIDTH = 800;

inline float get_random_float()
{
    static std::random_device dev;
    static std::mt19937 rng(dev());
    static std::uniform_real_distribution<float> dist(0.f, 1.f); // distribution in range [1, 6]

    return dist(rng);
}

inline void UpdateProgress(float progress)
{
    int barWidth = 70;

    std::cout << "[";
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << " %\r";
    std::cout.flush();
};



__declspec(selectany) int envmap_width, envmap_height;
__declspec(selectany) std::vector<vec3> envmap;

static void set_color(unsigned char* framebuffer, int x, int y, unsigned char color[]);

static int get_index(int x, int y);

static bool is_back_pace(vec3 pos[]);

static std::tuple<float, float, float> compute_barycentric2D(float x, float y, vec3 vertexs[]);

static int is_inside_triangle(float alpha, float beta, float gamma);

static void triangle_draw(unsigned char* framebuffer, float* zbuffer, IShader* shader);

void model_draw(unsigned char* framebuffer, float* zbuffer, IShader* shader);



void ray_trace(unsigned char* framebuffer, Scene scene);
void path_trace(unsigned char* framebuffer, Scene scene);
void path_trace_getimage(unsigned char* framebuffer, Scene scene);
void ray_trace_getimage(unsigned char* framebuffer, Scene scene);




