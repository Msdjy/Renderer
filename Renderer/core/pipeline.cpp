#pragma once
#include "./pipeline.h"

#include <thread>
#include <mutex>
#include <cmath>
std::mutex mutex_ins;


// framebuffer以左下角为原点
// 屏幕空间坐标以左上角为原点
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




vec3 reflect(vec3 in_dir, vec3 normal) {
    // 2
    return in_dir - 2 * dot(in_dir, normal) * normal;
}

vec3 refract(vec3 in_dir, vec3 normal, float refractive_index) {
    float cosi = float_clamp( dot(in_dir, normal), - 1, 1);
    float etai = 1, etat = refractive_index;
    vec3 n = normal;
    if (cosi < 0) { cosi = -cosi; }
    else { std::swap(etai, etat); n = -normal; }
    float eta = etai / etat;
    float k = 1 - eta * eta * (1 - cosi * cosi);
    return k < 0 ? vec3(1,0,0) : eta * in_dir + (eta * cosi - sqrtf(k)) * n;
}


Intersection scene_intersect(const vec3& orig, const vec3& dir, IShader* shader) {

    Intersection inter;
    float maxdistance = std::numeric_limits<float>::max();
    maxdistance = 10000;

    //// 棋盘平台的相交代码，并没有用object或model，是用坐标特殊实现的
    //float nearest_dist = 1e10;
    //if (std::abs(dir.y()) > .001) { // intersect the ray with the checkerboard, avoid division by zero
    //    float d = -(orig.y() + 4) / dir.y(); // the checkerboard plane has equation y = -4
    //    vec3 p = orig + dir * d;
    //    if (d > .001 && d < nearest_dist && std::abs(p.x()) < 10 && p.z()<-10 && p.z()>-30) {
    //        nearest_dist = d;
    //        inter.is_intersect = true;
    //        inter.distance = nearest_dist;
    //        inter.pos = p;
    //        inter.normal = { 0,1,0 };
    //        inter.material.diffuse_color = (int(.5 * inter.pos.x() + 1000) + int(.5 * inter.pos.z())) & 1 ? vec3{ .3, .3, .3 } : vec3{ .3, .2, .1 };
    //    }
    //}

    //for (auto object : shader->payload_shader.objects) {
    //    Intersection tmp_inter = object->intersect(orig, dir);
    //    
    //    if (tmp_inter.is_intersect && tmp_inter.distance < maxdistance) {
    //        //std::cout << tmp_inter.material.albedo<<" " << std::endl;
    //        //std::cout << tmp_inter.distance << " " << std::endl;
    //        inter = tmp_inter;
    //        maxdistance = tmp_inter.distance;
    //    }
    //}
    for (int i = 0; i < shader->payload_shader.objects.size(); i++) {
        auto object = shader->payload_shader.objects[i];
        Intersection tmp_inter = object->intersect(orig, dir);

        if (tmp_inter.is_intersect && tmp_inter.distance < maxdistance) {
            //std::cout << tmp_inter.material.albedo<<" " << std::endl;
            //std::cout << tmp_inter.distance << " " << std::endl;
            inter = tmp_inter;
            maxdistance = tmp_inter.distance;
        }
    }
    return inter;
}

// 按光线方向 取环境贴图的颜色
vec3 get_mapColor(vec3 dir) {
    if (dir.y() == 1.0f || dir.y() == -1.0f)return vec3(1, 1, 1);
    float cosb = dir.y();
    float cosa = dot(normalize(vec3(dir.x(), 0, dir.z())), vec3(1, 0, 0));
    float angle_b = acos(cosb);
    float angle_a = acos(cosa);
    angle_a = dir.z() < 0 ? angle_a : angle_a + 2.0f * PI - 2.0f * angle_a;
    int j = angle_b / PI * envmap_height;
    int i = angle_a / 2.0f / PI * envmap_width;
    
    return envmap[j * envmap_width + i];
}

// TODO Cornell Box场景有问题，球边缘的黑部分
static vec3 castRay_whited(const vec3& eye, const vec3& ray_dir, IShader* shader, const int depth = 4) {
    vec3 color(0, 0, 0);
    if (depth < 0){
        //vec3 mapcolor = get_mapColor(ray_dir);
        return color;
    }

    Intersection inter = scene_intersect(eye, ray_dir, shader);
    vec3 reflect_color;
    {
        vec3 reflect_dir = normalize(reflect(ray_dir, inter.normal));
        reflect_color = castRay_whited(inter.pos, reflect_dir, shader, depth - 1);
    }
    vec3 refract_color;
    {

        vec3 refract_dir = normalize(refract(ray_dir, inter.normal, inter.material.ior));
        refract_color = castRay_whited(inter.pos, refract_dir, shader, depth - 1);
    }
   
    vec3 diffuse = inter.material.albedo;
    //vec3 amblient = diffuse * get_mapColor(inter.normal) * 0.1;
    //color += amblient;

    vec3 diffuse_f;
    vec3 sepcular_f;
    //light
    for (auto light : shader->payload_shader.lights) {
        //std::cout << sepcular_f << "asd" << diffuse_f << std::endl;

        if (dot(ray_dir, inter.normal) > 0)inter.normal = -inter.normal;
        vec3 light_dir = normalize(light->position - inter.pos);
        if (scene_intersect(inter.pos, light_dir, shader).distance < (light->position - inter.pos).norm())continue;

        vec3 half_dir = normalize(light_dir + -ray_dir);

        float distance = (light->position - inter.pos).norm();
        float attenuation = 1.0 / (light->constant + light->linear * distance + light->quadratic * (distance * distance));

        float diff = std::max(dot(light_dir, inter.normal), 0.0);
        diffuse_f += diff * attenuation * light->power;
        float specf;
        if (inter.material.t == SPEC)specf = 1425;
        if (inter.material.t == REFR)specf = 125;
        if (inter.material.t == DIFFUSE)specf = 10;
        float spec = pow(std::max(dot(half_dir, inter.normal), 0.0), specf);
        sepcular_f += spec * attenuation* light->power;
    }
    //Material      ivory = { 1.0, {0.9,  0.5, 0.1, 0.0}, {0.4, 0.4, 0.3},   50. };
    //Material      glass = { 1.5, {0.0,  0.9, 0.1, 0.8}, {0.6, 0.7, 0.8},  125. };
    //Material red_rubber = { 1.0, {1.4,  0.3, 0.0, 0.0}, {0.3, 0.1, 0.1},   10. };
    //Material     mirror = { 1.0, {0.0, 16.0, 0.8, 0.0}, {1.0, 1.0, 1.0}, 1425. };
    // 原本这里是用albedo的四个值来分配直接光diff和spec和反射光和折射光。
    //
    if (inter.material.t == SPEC) {
        color += (diffuse_f * 0.0 + sepcular_f * 16.0) * diffuse + 0.8 * reflect_color +  0 * refract_color;
    }
    if (inter.material.t == REFR) {
        color += (diffuse_f * 0.0 + sepcular_f * 0.9) * diffuse + 0.1 * reflect_color + 0.8 * refract_color;
    }
    if(inter.material.t == DIFFUSE)color += (diffuse_f * 1.4  + sepcular_f * 0.3) * diffuse;
    return color;
}

void sample_InterLight(Intersection& inter, float& pdf, IShader* shader)
{
    float emit_area_sum = 0;
    for (auto object : shader->payload_shader.objects) {
        //std::cout<< object->emission <<std::endl;
        if (object->has_emmission()) {
            emit_area_sum += object->getArea();
        }
    }
    float p = get_random_float() * emit_area_sum;
    emit_area_sum = 0;
    
    for (auto object : shader->payload_shader.objects) {
        if (object->has_emmission()) {
            emit_area_sum += object->getArea();
            if (p <= emit_area_sum) {
                object->sample(inter, pdf);
                break;
            }
        }
    }
}


// 蒙特卡洛方法，可以自定义的部分
// 1. 采样方法
// 2. brdf, 有基于物理的brdf，或经验模型

// pbr的brdf，反射光部分可以分为漫反射和镜面反射
static vec3 castRay_pathTracing(const vec3& ori, const vec3& dir, IShader* shader, const int depth = 0) {

    vec3 color_dir;
    vec3 color_indir;
    Intersection inter = scene_intersect(ori, dir, shader);
    if (!inter.is_intersect) {
        return vec3(0);
    }
    
    // 光线直接看到的是光源
    if (inter.emission.norm_squared() > 1) {
        return inter.emission;
    }


    // 观察点到光源是否有遮挡
    // 无遮挡，光线cast直接得到光源颜色，这是直接光照
    float pdf_light;
    Intersection inter_light;
    sample_InterLight(inter_light, pdf_light, shader);
    vec3 l_dir = inter_light.pos - inter.pos;
    vec3 light_dir = normalize(l_dir);

    //if ( scene_intersect(inter.pos, light_dir, shader).distance - l_dir.norm() > -1e-1) {
    if (scene_intersect(inter.pos, light_dir, shader).emission.norm_squared() > 1) {
        color_dir = inter_light.emission
                    * inter.material.eval(dir, light_dir, inter.normal)
                    * dot(inter.normal, light_dir) // TODO 算能量算的是出射光看到的能量吗
                    // 球型光不能这样算 
                    * std::max(dot(inter_light.normal, -light_dir), 0.0)
                    * 1
                    / pdf_light / (l_dir).norm_squared();
        //format(color_dir);
        //for (int t = 0; t < 3; t++) {
        //    if (color_dir[t] > 1)std::cout << "color_dir"<< color_dir << std::endl;
        //}
    }
    
    // 蒙特卡洛，计算光线是否继续cast
    if (get_random_float() > RussianRoulette)
        return  color_dir;
    // 这是间接光照
    vec3 sample_dir;
    float pdf;
    if (inter.material.roughness > 0.99 || inter.material.t == DIFFUSE) {
        sample_dir = inter.material.sample(dir, inter.normal);
        pdf = inter.material.pdf(dir, sample_dir, inter.normal);
    }// 重要性采样
    // 镜面球还会有黑点
    else inter.material.ImporttanceSampleGGX(inter.normal, dir, sample_dir, pdf);

    // 如果采样方向是光源，就舍弃这一次间接光，因为作为直接光照以及计算过了，亮点太多
    // 这个加不加都有白噪点
    if(scene_intersect(inter.pos, sample_dir, shader).emission.norm_squared() > 1)return color_dir;
    color_indir = castRay_pathTracing(inter.pos, sample_dir, shader, depth + 1)
                * inter.material.eval(dir, sample_dir, inter.normal)
                * dot(inter.normal, sample_dir) // TODO 算能量算的是出射光看到的能量吗
                / pdf
                / RussianRoulette;
    //for (int t = 0; t < 3; t++) {
    //    if (color_indir[t] > 1)std::cout << "color_indir" << std::endl;
    //}
    //format(color_indir);
    //if (color_indir[0] < 0 || color_indir[1] < 0 || color_indir[2] < 0)std::cout << "asd" << std::endl;
    vec3 color = color_dir + color_indir;
    return color;
}


// TODO static 有问题
void ray_trace(unsigned char* framebuffer, IShader* shader) {
    Camera* camera = shader->payload_shader.camera;
    
    //// 屏幕遍历左下角开始
    //for (int i = 0; i < WINDOW_HEIGHT; i++) {
    //    for (int j = 0; j < WINDOW_WIDTH; j++) {
    //        float u = ((float)j + 0.5f) / WINDOW_WIDTH;
    //        float v = ((float)i + 0.5f) / WINDOW_HEIGHT;
    //        //std::cout << camera->left_top_dir << std::endl;
    //        vec3 ray_dir = normalize(camera->left_top_dir - camera->vertical * camera->vertical_len + u * camera->horizontal * camera->horizontal_len
    //                                                                                                + v * camera->vertical * camera->vertical_len);
    //        //std::cout << ray_dir<<" "<< camera->left_top_dir <<" asd "<<tan(45.0f / 180.0f * PI) << std::endl;
    //        vec3 color = castRay(camera->eye, ray_dir, shader);
    //        // color
    //        unsigned char c[3];
    //        for (int t = 0; t < 3; t++)
    //        {
    //            c[t] = (int)float_clamp(color[t]  * 255, 0, 255);
    //        }
    //        set_color(framebuffer, j, i, c);
    //    }
    //}

    int spp = 1;
    // 多线程 // 为啥我这只有两倍差距
    int process = 0;
    auto castRayMultiThreading = [&](uint32_t rowStart, uint32_t rowEnd, uint32_t colStart, uint32_t colEnd)
    {
        for (uint32_t i = rowStart; i < rowEnd; ++i) {
            for (uint32_t j = colStart; j < colEnd; ++j) {
                float u = ((float)j + 0.5f) / WINDOW_WIDTH;
                float v = ((float)i + 0.5f) / WINDOW_HEIGHT;
                //std::cout << camera->left_top_dir << std::endl;
                vec3 ray_dir = normalize(camera->left_top_dir - camera->vertical * camera->vertical_len + u * camera->horizontal * camera->horizontal_len
                    + v * camera->vertical * camera->vertical_len);
                //std::cout << ray_dir<<" "<< camera->left_top_dir <<" asd "<<tan(45.0f / 180.0f * PI) << std::endl;
               
                vec3 color;
                for (int k = 0; k < spp; k++) {
                    color += castRay_pathTracing(camera->eye, ray_dir, shader) / spp;
                }
                color = color / (float)spp;

                const float gamma = 2.2;
                vec3 hdrColor = color;

                //// Reinhard色调映射
                vec3 mapped;
                for (int w = 0; w < 3; w++) {
                    mapped[w] = hdrColor[w] / (hdrColor[w] + 1.0f);
                    mapped[w] = std::pow(mapped[w], 1.0 / gamma);
                }
                color = mapped;

                // color
                unsigned char c[3];
                for (int t = 0; t < 3; t++)
                {
                    c[t] = (int)float_clamp(color[t] * 255, 0, 255);
                }
                set_color(framebuffer, j, i, c);
                process++;
            }
            // 互斥锁，用于打印处理进程
            std::lock_guard<std::mutex> g1(mutex_ins);
            //UpdateProgress(1.0 * process / WINDOW_WIDTH / WINDOW_HEIGHT);
        }
    };
    int id = 0;
    constexpr int bx = 5;
    constexpr int by = 5;
    std::thread th[bx * by];
    int strideX = (WINDOW_HEIGHT + 1) / bx;
    int strideY = (WINDOW_WIDTH + 1) / by;

    // 分块计算光线追踪
    for (int i = 0; i < WINDOW_HEIGHT; i += strideX)
    {
        for (int j = 0; j < WINDOW_WIDTH; j += strideY)
        {
            th[id] = std::thread(castRayMultiThreading, i, std::min(i + strideX, WINDOW_HEIGHT), j, std::min(j + strideY, WINDOW_WIDTH));
            id++;
        }
    }

    for (int i = 0; i < bx * by; i++) th[i].join();
    //UpdateProgress(1.f);
}




// TODO static 有问题
void ray_trace_getimage(unsigned char* framebuffer, IShader* shader) {
    Camera* camera = shader->payload_shader.camera;

    //// 屏幕遍历左下角开始
    //for (int i = 0; i < WINDOW_HEIGHT; i++) {
    //    for (int j = 0; j < WINDOW_WIDTH; j++) {
    //        float u = ((float)j + 0.5f) / WINDOW_WIDTH;
    //        float v = ((float)i + 0.5f) / WINDOW_HEIGHT;
    //        //std::cout << camera->left_top_dir << std::endl;
    //        vec3 ray_dir = normalize(camera->left_top_dir - camera->vertical * camera->vertical_len + u * camera->horizontal * camera->horizontal_len
    //                                                                                                + v * camera->vertical * camera->vertical_len);
    //        //std::cout << ray_dir<<" "<< camera->left_top_dir <<" asd "<<tan(45.0f / 180.0f * PI) << std::endl;
    //        vec3 color = castRay(camera->eye, ray_dir, shader);
    //        // color
    //        unsigned char c[3];
    //        for (int t = 0; t < 3; t++)
    //        {
    //            c[t] = (int)float_clamp(color[t]  * 255, 0, 255);
    //        }
    //        set_color(framebuffer, j, i, c);
    //    }
    //}

    int spp = 16;
    // 多线程 // 为啥我这只有两倍差距
    int process = 0;
    auto castRayMultiThreading = [&](uint32_t rowStart, uint32_t rowEnd, uint32_t colStart, uint32_t colEnd)
    {
        for (uint32_t i = rowStart; i < rowEnd; ++i) {
            for (uint32_t j = colStart; j < colEnd; ++j) {
                float u = ((float)j + 0.5f) / WINDOW_WIDTH;
                float v = ((float)i + 0.5f) / WINDOW_HEIGHT;
                //std::cout << camera->left_top_dir << std::endl;
                vec3 ray_dir = normalize(camera->left_top_dir - camera->vertical * camera->vertical_len + u * camera->horizontal * camera->horizontal_len
                    + v * camera->vertical * camera->vertical_len);
                vec3 color;
                for (int k = 0; k < spp; k++) {
                    vec3 castColor = castRay_pathTracing(camera->eye, ray_dir, shader);
                    color += castColor;
                }
                color = color / (float)spp;


                const float gamma = 2.2;
                vec3 hdrColor = color;

                // Reinhard色调映射
                vec3 mapped;
                for (int w = 0; w < 3; w++) {
                    mapped[w] = hdrColor[w] / (hdrColor[w] + 1.0f);
                    mapped[w] = std::pow(mapped[w], 1.0 / gamma);
                }
                color = mapped;



                // color
                unsigned char c[3];
                for (int t = 0; t < 3; t++)
                {
                    c[t] = (int)float_clamp(color[t] * 255, 0, 255);
                }
                set_color(framebuffer, j, i, c);
                process++;
            }
            // 互斥锁，用于打印处理进程
            std::lock_guard<std::mutex> g1(mutex_ins);
            UpdateProgress(1.0 * process / WINDOW_WIDTH / WINDOW_HEIGHT);
        }
    };
    int id = 0;
    constexpr int bx = 5;
    constexpr int by = 5;
    std::thread th[bx * by];
    int strideX = (WINDOW_HEIGHT + 1) / bx;
    int strideY = (WINDOW_WIDTH + 1) / by;

    // 分块计算光线追踪
    for (int i = 0; i < WINDOW_HEIGHT; i += strideX)
    {
        for (int j = 0; j < WINDOW_WIDTH; j += strideY)
        {
            th[id] = std::thread(castRayMultiThreading, i, std::min(i + strideX, WINDOW_HEIGHT), j, std::min(j + strideY, WINDOW_WIDTH));
            id++;
        }
    }

    for (int i = 0; i < bx * by; i++) th[i].join();
    UpdateProgress(1.f);
}