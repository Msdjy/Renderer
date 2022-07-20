#pragma once
#include "./pipeline.h"
#include "./model.h"
#include "../shader/shader.h"

// TODO staic作用
static void set_color(unsigned char* framebuffer, int x, int y, unsigned char color[]) {
	int index = ((WINDOW_HEIGHT - y - 1) * WINDOW_WIDTH + x) * 4;

	for (int i = 0; i < 3; i++) {
		framebuffer[index + i] = color[i];
	}
}

static int get_index(int x, int y) {
    return ((WINDOW_HEIGHT - y - 1) * WINDOW_WIDTH + x);
}

static bool is_back_pace(vec3 pos[]) {
    //vec3 ab = pos[1] - pos[0];
    //vec3 bc = pos[2] - pos[1];
    //return (cross(ab, bc)).norm() > 0.0f;
    //TODO
    vec3 a = pos[0];
    vec3 b = pos[1];
    vec3 c = pos[2];
    float signed_area = a.x() * b.y() - a.y() * b.x() +
        b.x() * c.y() - b.y() * c.x() +
        c.x() * a.y() - c.y() * a.x();   //|AB AC|
    return signed_area <= 0;
}

// TODO 
//static void line(unsigned char* framebuffer, vec2 p0, vec2 p1, unsigned char color[]) {
//    bool steep = false;
//    if (std::abs(p0.x() - p1.x()) < std::abs(p0.y() - p1.y())) {
//        std::swap(p0[0], p0[1]);
//        std::swap(p1[0], p1[1]);
//        steep = true;
//    }
//    if (p0.x() > p1.x()) {
//        std::swap(p0, p1);
//    }
//
//    for (int x = p0.x(); x <= p1.x(); x++) {
//        float t = (x - p0.x()) / (float)(p1.x() - p0.x());
//        int y = p0.y() * (1. - t) + p1.y() * t;
//        if (steep) {
//            set_color(framebuffer, y, x, color);
//        }
//        else {
//            set_color(framebuffer, x, y, color);
//        }
//    }
//}
//
//
//static void triangle(unsigned char* framebuffer, vec3 vertexs[], unsigned char color[]) {
//    line(framebuffer, vertexs[0], vertexs[1], color);
//    line(framebuffer, vertexs[1], vertexs[2], color);
//    line(framebuffer, vertexs[2], vertexs[0], color);
//}


static std::tuple<float, float, float> compute_barycentric2D(float x, float y, vec3 vertexs[])
{
    float c1 = (x * (vertexs[1].y() - vertexs[2].y()) + (vertexs[2].x() - vertexs[1].x()) * y + vertexs[1].x() * vertexs[2].y() - vertexs[2].x() * vertexs[1].y()) / (vertexs[0].x() * (vertexs[1].y() - vertexs[2].y()) + (vertexs[2].x() - vertexs[1].x()) * vertexs[0].y() + vertexs[1].x() * vertexs[2].y() - vertexs[2].x() * vertexs[1].y());
    float c2 = (x * (vertexs[2].y() - vertexs[0].y()) + (vertexs[0].x() - vertexs[2].x()) * y + vertexs[2].x() * vertexs[0].y() - vertexs[0].x() * vertexs[2].y()) / (vertexs[1].x() * (vertexs[2].y() - vertexs[0].y()) + (vertexs[0].x() - vertexs[2].x()) * vertexs[1].y() + vertexs[2].x() * vertexs[0].y() - vertexs[0].x() * vertexs[2].y());
    return { c1, c2, 1 - c1 - c2 };
}





static int is_inside_triangle(float alpha, float beta, float gamma)
{
    int flag = 0;
    // here epsilon is to alleviate precision bug
    if (alpha > -EPSILON && beta > -EPSILON && gamma > -EPSILON)
        flag = 1;

    return flag;
}

//static bool insideTriangle(float u, float v, vec2 vertexs[0], vec2 vertexs[1], vec2 vertexs[2]) {
//    vec3 v[3] = { vec3(vertexs[0], 1.0f), vertexs[1], vertexs[2] };
//    vec3 p(u, v, 1.0f)
//
//    f0 = cross();
//    f1 = v[2].cross(v[1]);
//    f2 = v[0].cross(v[2]);
//    Vector3f p(x, y, 1.);
//    if ((p.dot(f0) * f0.dot(v[2]) > 0) && (p.dot(f1) * f1.dot(v[0]) > 0) && (p.dot(f2) * f2.dot(v[1]) > 0))
//        return true;
//    return false;
//}


// 由投影矩阵创建的观察箱(Viewing Box)被称为平截头体(Frustum)，每个出现在平截头体范围内的坐标都会最终出现在用户的屏幕上。
// 将特定范围内的坐标转化到标准化设备坐标系的过程（而且它很容易被映射到2D观察空间坐标）被称之为投影(Projection)，因为使用投影矩阵能将3D坐标投影(Project)到很容易映射到2D的标准化设备坐标系中。

//                   mvp空间应该就是裁剪空间
// 一旦所有顶点被变换到裁剪空间，最终的操作——透视除法(Perspective Division)将会执行，在这个过程中我们将位置向量的x，y，z分量分别除以向量的齐次w分量；
// 透视除法是将4D裁剪空间坐标变换为3D标准化设备坐标的过程。这一步会在每一个顶点着色器运行的最后被自动执行。
// 在这一阶段之后，最终的坐标将会被映射到屏幕空间中（使用glViewport中的设定），并被变换成片段。


// 顶点着色器的输出要求所有的顶点都在裁剪空间内，这正是我们刚才使用变换矩阵所做的。
// OpenGL然后对裁剪坐标执行透视除法从而将它们变换到标准化设备坐标。
// OpenGL会使用glViewPort内部的参数来将标准化设备坐标映射到屏幕坐标，每个坐标都关联了一个屏幕上的点（在我们的例子中是一个800x600的屏幕）。这个过程称为视口变换。

static void triangle_draw(unsigned char* framebuffer, float* zbuffer, IShader* shader) {
    
        // NDC transformation
        // homogeneous division
        for (int i = 0; i < 3; i++)
        {
            shader->payload_shader.NDC_vertexs[i][0] = shader->payload_shader.mvp_vertexs[i][0] / shader->payload_shader.mvp_vertexs[i].w();
            shader->payload_shader.NDC_vertexs[i][1] = shader->payload_shader.mvp_vertexs[i][1] / shader->payload_shader.mvp_vertexs[i].w();
            shader->payload_shader.NDC_vertexs[i][2] = shader->payload_shader.mvp_vertexs[i][2] / shader->payload_shader.mvp_vertexs[i].w();
            
        }

        // 
        if (is_back_pace(shader->payload_shader.NDC_vertexs)) {
            return;
        }

        // viewport transformation
        int width = WINDOW_WIDTH;
        int height = WINDOW_HEIGHT;

        for (int i = 0; i < 3; i++)
        {
            shader->payload_shader.screen_vertexs[i][0] = 0.5 * (width - 1) * (shader->payload_shader.NDC_vertexs[i][0] + 1.0);
            shader->payload_shader.screen_vertexs[i][1] = 0.5 * (height - 1) * (shader->payload_shader.NDC_vertexs[i][1] + 1.0);
            //shader->payload_shader.screen_vertexs[i][2] = -shader->payload_shader.view_vertexs[i].z();	//view space z-value
            shader->payload_shader.screen_vertexs[i][2] = -shader->payload_shader.mvp_vertexs[i].w();	//view space z-value
            // 这两个值是一样的，mvp空间的w值齐次坐标
            //std::cout << shader->payload_shader.view_vertexs[i].z() << "   " << shader->payload_shader.mvp_vertexs[i].w() << std::endl;
        }

        // getBOX
        auto minx = std::min(shader->payload_shader.screen_vertexs[0].x(), std::min(shader->payload_shader.screen_vertexs[1].x(), shader->payload_shader.screen_vertexs[2].x()));
        auto maxx = std::max(shader->payload_shader.screen_vertexs[0].x(), std::max(shader->payload_shader.screen_vertexs[1].x(), shader->payload_shader.screen_vertexs[2].x()));

        auto miny = std::min(shader->payload_shader.screen_vertexs[0].y(), std::min(shader->payload_shader.screen_vertexs[1].y(), shader->payload_shader.screen_vertexs[2].y()));
        auto maxy = std::max(shader->payload_shader.screen_vertexs[0].y(), std::max(shader->payload_shader.screen_vertexs[1].y(), shader->payload_shader.screen_vertexs[2].y()));

        minx = (int)std::floor(minx); // 对x进行向下取整
        maxx = (int)std::ceil(maxx); // 对y进行向上取整
        miny = (int)std::floor(miny); // 对x进行向下取整
        maxy = (int)std::ceil(maxy); // 对y进行向上取整

        //std::cout << vertexs[1] << std::endl;
        //std::cout << minx <<" "<< maxx <<" "<<  miny << " " << maxy << std::endl;

        for (int i = minx; i <= maxx; i++) {
            for (int j = miny; j <= maxy; j++) {
                if (i < 0 || i >= width || j < 0 || j >= height)continue;
                // 求屏幕空间中的重心坐标
                auto [alpha, beta, gamma] = compute_barycentric2D((float)(i + 0.5), (float)(j + 0.5), shader->payload_shader.screen_vertexs);
                if (alpha > 0 && beta > 0 && gamma > 0) {

                    // interpolation correct term
                    // 这边的修正也是用了真实空间的深度值，mvp空间的w值齐次坐标
                    float normalizer = 1.0 / (alpha / shader->payload_shader.mvp_vertexs[0].w()
                        + beta / shader->payload_shader.mvp_vertexs[1].w()
                        + gamma / shader->payload_shader.mvp_vertexs[2].w());
                    //for larger z means away from camera, needs to interpolate z-value as a property			
                    float z = (alpha * shader->payload_shader.screen_vertexs[0].z() / shader->payload_shader.mvp_vertexs[0].w()
                        + beta * shader->payload_shader.screen_vertexs[1].z() / shader->payload_shader.mvp_vertexs[1].w()
                        + gamma * shader->payload_shader.screen_vertexs[2].z() / shader->payload_shader.mvp_vertexs[2].w()) * normalizer;



                    if (zbuffer[get_index(i, j)] > z) {
                        zbuffer[get_index(i, j)] = z;
                        //std::cout << z << std::endl;
                        vec3 color = shader->fragment_shader(alpha, beta, gamma);
                        // color
                        unsigned char c[3];
                        for (int t = 0; t < 3; t++)
                        {
                            c[t] = (int)float_clamp(color[t] * 255, 0, 255);

                        }

                        set_color(framebuffer, i, j, c);
                    }
                }

            }
        }
    
}



static void model_draw(unsigned char* framebuffer, float* zbuffer, IShader* shader) {
    Model* model = shader->payload_shader.model;
    for (int i = 0; i < model->nfaces(); i++) {
        for (int j = 0; j < 3; j++) {
            shader->payload_shader.vertexs[j] = model->vert(i, j);
            shader->payload_shader.normals[j] = model->normal(i, j);
            shader->payload_shader.texcoords[j] = model->uv(i, j);
            // TODO Q:报错，表达式必须是可修改的左值？
            //payload_shader.vertexs = vertexs;
            //payload_shader.normals = normals;
            //payload_shader.texcoords = texcoords;
        }
        shader->vertex_shader();

        triangle_draw(framebuffer, zbuffer, shader);
    }
}



