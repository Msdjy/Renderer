
#include "./scene.h"
#include "./pipeline.h"

vec3 Scene::reflect(vec3 in_dir, vec3 normal) {
    // 2
    return in_dir - 2 * dot(in_dir, normal) * normal;
}

vec3 Scene::refract(vec3 in_dir, vec3 normal, float refractive_index) {
    float cosi = float_clamp(dot(in_dir, normal), -1, 1);
    float etai = 1, etat = refractive_index;
    vec3 n = normal;
    if (cosi < 0) { cosi = -cosi; }
    else { std::swap(etai, etat); n = -normal; }
    float eta = etai / etat;
    float k = 1 - eta * eta * (1 - cosi * cosi);
    return k < 0 ? vec3(1, 0, 0) : eta * in_dir + (eta * cosi - sqrtf(k)) * n;
}

Intersection Scene::scene_intersect(const vec3& orig, const vec3& dir) {

    Intersection inter;
    float maxdistance = std::numeric_limits<float>::max();
    //maxdistance = 10000;

    // 棋盘平台的相交代码，并没有用object或model，是用坐标特殊实现的
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

    for (int i = 0; i < objects.size(); i++) {
        auto object = objects[i];
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


void Scene::sample_InterLight(Intersection& inter, float& pdf)
{
    float emit_area_sum = 0;
    for (auto object : objects) {
        //std::cout<< object->emission <<std::endl;
        if (object->has_emmission()) {
            emit_area_sum += object->getArea();
        }
    }
    float p = get_random_float() * emit_area_sum;
    emit_area_sum = 0;

    for (auto object : objects) {
        if (object->has_emmission()) {
            emit_area_sum += object->getArea();
            if (p <= emit_area_sum) {
                object->sample(inter, pdf);
                break;
            }
        }
    }
}



// TODO Cornell Box场景有问题，球边缘的黑部分
vec3 Scene::castRay_whited(const vec3& eye, const vec3& ray_dir, const int depth) {
    vec3 color(0, 0, 0);
    if (depth < 0) {
        //vec3 mapcolor = get_mapColor(ray_dir);
        return color;
    }

    Intersection inter = scene_intersect(eye, ray_dir);
    vec3 reflect_color;
    {
        vec3 reflect_dir = normalize(reflect(ray_dir, inter.normal));
        reflect_color = castRay_whited(inter.pos, reflect_dir,depth - 1);
    }
    vec3 refract_color;
    {
        vec3 refract_dir = normalize(refract(ray_dir, inter.normal, inter.material.ior));
        refract_color = castRay_whited(inter.pos, refract_dir, depth - 1);
    }

    vec3 diffuse = inter.material.albedo;
    //vec3 amblient = diffuse * get_mapColor(inter.normal) * 0.1;
    //color += amblient;

    vec3 diffuse_f;
    vec3 sepcular_f;
    //light
    for (auto light : lights) {

        //std::cout << sepcular_f << "asd" << diffuse_f << std::endl;

        if (dot(ray_dir, inter.normal) > 0)inter.normal = -inter.normal;
        vec3 light_dir = normalize(light->position - inter.pos);
        if (scene_intersect(inter.pos, light_dir).distance < (light->position - inter.pos).norm())continue;

        vec3 half_dir = normalize(light_dir + -ray_dir);

        float distance = (light->position - inter.pos).norm();
        float attenuation = 1.0 / (light->constant + light->linear * distance + light->quadratic * (distance * distance));

        float diff = std::max(dot(light_dir, inter.normal), 0.0);
        diffuse_f += diff * attenuation * light->power;
        float specf = 10;
        if (inter.material.t == SPEC)specf = 1425;
        if (inter.material.t == REFR)specf = 125;
        if (inter.material.t == MICROFACET)specf = 100;
        if (inter.material.t == DIFFUSE)specf = 10;
        float spec = pow(std::max(dot(half_dir, inter.normal), 0.0), specf);
        sepcular_f += spec * attenuation * light->power;
    }
    //Material      ivory = { 1.0, {0.9,  0.5, 0.1, 0.0}, {0.4, 0.4, 0.3},   50. };
    //Material      glass = { 1.5, {0.0,  0.9, 0.1, 0.8}, {0.6, 0.7, 0.8},  125. };
    //Material red_rubber = { 1.0, {1.4,  0.3, 0.0, 0.0}, {0.3, 0.1, 0.1},   10. };
    //Material     mirror = { 1.0, {0.0, 16.0, 0.8, 0.0}, {1.0, 1.0, 1.0}, 1425. };
    // 原本这里是用albedo的四个值来分配直接光diff和spec和反射光和折射光。
    //
    if (inter.material.t == SPEC) {
        color += (diffuse_f * 0.0 + sepcular_f * 16.0) * diffuse + 0.8 * reflect_color + 0 * refract_color;
    }
    if (inter.material.t == REFR) {
        color += (diffuse_f * 0.0 + sepcular_f * 0.9) * diffuse + 0.1 * reflect_color + 0.8 * refract_color;
    }
    if (inter.material.t == DIFFUSE)color += (diffuse_f * 1.4 + sepcular_f * 0.3) * diffuse;
    return color;
}


// 蒙特卡洛方法，可以自定义的部分
// 1. 采样方法
// 2. brdf, 有基于物理的brdf，或经验模型

// pbr的brdf，反射光部分可以分为漫反射和镜面反射
vec3 Scene::castRay_pathTracing(const vec3& ori, const vec3& dir, const int depth) {

    vec3 color_dir;
    vec3 color_indir;
    Intersection inter = scene_intersect(ori, dir);
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
    sample_InterLight(inter_light, pdf_light);
    vec3 l_dir = inter_light.pos - inter.pos;
    vec3 light_dir = normalize(l_dir);

    //if ( scene_intersect(inter.pos, light_dir, shader).distance - l_dir.norm() > -1e-1) {
    if (scene_intersect(inter.pos, light_dir).emission.norm_squared() > 1 && (l_dir).norm_squared() > 0.01 && pdf_light > EPSILON) {
        color_dir = inter_light.emission
            * inter.material.eval(dir, light_dir, inter.normal)
            * dot(inter.normal, light_dir) // TODO 算能量算的是出射光看到的能量吗
            // 球型光不能这样算 
            * std::max(dot(inter_light.normal, -light_dir), 0.0)
            //* std::max(dot(inter_light.normal, -light_dir), 0.0)
            * 1
            / pdf_light / (l_dir).norm_squared();
    }
    //if (color_dir.norm() > vec3(1).norm())color_dir = vec3(1);

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
    if (scene_intersect(inter.pos, sample_dir).emission.norm_squared() > 1 || pdf < EPSILON)return color_dir;
    color_indir = castRay_pathTracing(inter.pos, sample_dir, depth - 1)
        * inter.material.eval(dir, sample_dir, inter.normal)
        * dot(inter.normal, sample_dir) // TODO 算能量算的是出射光看到的能量吗
        / pdf
        / RussianRoulette;

    vec3 color = color_dir + color_indir;
    //if (color.norm() > vec3(1).norm())color = vec3(1);
    return color;
}
