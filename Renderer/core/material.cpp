#include"./material.h"
#include"./pipeline.h"

#include <cmath>
#include <iostream>


static float FresnelSchlick(const vec3& H, const vec3& L, const float& F0) {
    // TODO
    //float inter = 0.04f * (1 - metalness) + 0.96f * metalness;
    float inter = F0;
    float cosTheta = dot(H, L);

    return inter + (1.f - inter) * pow(1.f - cosTheta, 5.f);
}

static float GeometrySchlickGGX(float NdotV, float k) {
    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float k = (roughness + 1) * (roughness + 1) / 8.0;
    float NdotV = dot(N, V);
    float NdotL = dot(N, L);

    return GeometrySchlickGGX(NdotL, k) * GeometrySchlickGGX(NdotV, k);
}

float DistributionGGX(const vec3& N, const vec3& H, float roughness) {
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float NdotH = dot(N, H);
    float NdotH2 = NdotH * NdotH;

    return alpha / PI / pow(NdotH2 * (alpha2 - 1) + 1, 2);
}

// TODO 
void fresnel(const vec3& I, const vec3& N, const float& ior, float& kr)
{
    float cosi = dot(I, N);
    float etai = 1, etat = ior;
    if (cosi > 0) { std::swap(etai, etat); }
    // Compute sini using Snell's law
    float sint = etai / etat * sqrtf(std::max(0.f, 1 - cosi * cosi));
    // Total internal reflection
    if (sint >= 1) {
        kr = 1;
    }
    else {
        float cost = sqrtf(std::max(0.f, 1 - sint * sint));
        cosi = fabsf(cosi);
        float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
        float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
        kr = (Rs * Rs + Rp * Rp) / 2;
    }
    // As a consequence of the conservation of energy, transmittance is given by:
    // kt = 1 - kr;
}

vec3 Material::eval(const vec3& in_dir, const vec3& out_dir, const vec3& normal) {
    switch (t) {
        case DIFFUSE:
        {
            // calculate the contribution of diffuse   model
            float cosalpha = dot(normal, out_dir);
            if (cosalpha > 0.0f) {
                vec3 diffuse = albedo * (1 - metallic);
                diffuse = diffuse / PI;
                return diffuse;
            }
            else
                return vec3(0.0f);
            break;
        }
        case MICROFACET:
        {
            float cosalpha = dot(normal, out_dir);
            if (cosalpha > 0.0f) {

                vec3 V = -in_dir;
                vec3 L = out_dir;
                vec3 N = normal;
                // Vector3f diffuse = Kd / M_PI;
                // 需要归一化
                vec3 H = normalize(V + L);

                // 入射与出射的方向都要朝外
                // float F = FresnelSchlick(wh, -wi, metallic);
                float G = GeometrySmith(N, V, L, roughness);
                float D = DistributionGGX(N, H, roughness);

                // 计算 fresnel 系数: F
                
                float F0;
                //fresnel(in_dir, N, ior, F0);
                F0 = metallic;
                float F = FresnelSchlick(H, L, F0);

                // 能量守恒
                float ks_ = F;
                float kd_ = 1.0f - ks_;
                vec3 diffuse = albedo * (1 - metallic);
                vec3 specular;
                for (int i = 0; i < 3; i++) {
                    specular[i] = std::lerp(0.04f, albedo[i], metallic);
                }
                float diff = 1.0 / PI;

                float Ndotwi = std::max(dot(N, V), 0.0);
                float Ndotwo = std::max(dot(N, L), 0.0);

                float nominator = D * G * F;
                float denominator = 4 * Ndotwi * Ndotwo;
                // std::max(denominator, 0.001f)
                // Vector3f mirror_reflection =  nominator /denominator;
                //std::cout << nominator << std::endl;
                vec3 mirror_reflection = nominator / std::max(denominator, 0.001f);
                // std::cout<<mirror_reflection.x<<" "<<mirror_reflection.y<<" "<<mirror_reflection.z<<std::endl;
                // return diffuse;
                // mirror_reflection.x = clamp(0, 1, mirror_reflection.x);
                // mirror_reflection.y = clamp(0, 1, mirror_reflection.y);
                // mirror_reflection.z = clamp(0, 1, mirror_reflection.z);
                // std::cout<<mirror_reflection.x<<" "<<mirror_reflection.y<<" "<<mirror_reflection.z<<std::endl;
                // return Ks * mirror_reflection;
                //return kd_ * Kd * diffuse + Ks * mirror_reflection;
                return kd_ * diff * diffuse + ks_ * mirror_reflection * specular;
                //return kd_ * diff * diffuse + ks_ * mirror_reflection * specular;
            }
            else
                return vec3(0.0f);
            break;


        }

    }
}

float Material::pdf(const vec3& in_dir, const vec3& out_dir, const vec3& normal) {
    switch (t) {
        case DIFFUSE:
        {
            // uniform sample probability 1 / (2 * PI)
            if (dot(out_dir, normal) > 0.0f)
                return 0.5f / PI;
            else
                return 0.0f;
            break;
        }
        case MICROFACET:
        {           //为什么这个pdf用上面的
            // TODO
            if (dot(out_dir, normal) > 0.0f)
                return 0.5f / PI;
            else
                return 0.0f;
            break;
        }
    }
}

// TODO
vec3 toWorld(const vec3& a, const vec3& N) {
    vec3 B, C;
    if (std::abs(N.x()) > std::abs(N.y())) {
        float invLen = 1.0f / std::sqrt(N.x() * N.x() + N.z() * N.z());
        C = vec3(N.z() * invLen, 0.0f, -N.x() * invLen);
    }
    else {
        float invLen = 1.0f / std::sqrt(N.y() * N.y() + N.z() * N.z());
        C = vec3(0.0f, N.z() * invLen, -N.y() * invLen);
    }
    B = cross(C, N);
    return a.x() * B + a.y() * C + a.z() * N;
}

// 按照该材质的性质，给定入射方向与法向量，用某种分布采样一个出射方向
// TODO
vec3 Material::sample(const vec3& in_dir, const vec3& normal) {
    switch (t) {
        case DIFFUSE:
        {
            // uniform sample on the hemisphere
            float x_1 = get_random_float(), x_2 = get_random_float();
            float z = std::fabs(1.0f - 2.0f * x_1);
            float r = std::sqrt(1.0f - z * z), phi = 2 * PI * x_2;
            vec3 localRay(r * std::cos(phi), r * std::sin(phi), z);
            return toWorld(localRay, normal);

            break;
        }
        case MICROFACET:
        {
            // uniform sample on the hemisphere
            float x_1 = get_random_float(), x_2 = get_random_float();
            // 而且这样采样方法对不上pdf //
            // 为什么乘2，乘2后，z可以采样到负值，负值又会舍去，消耗时间 TODO
            float z = std::fabs(1.0f - 2.0f * x_1);
            //float z = std::fabs(1.0f - 1.0f * x_1);
            float r = std::sqrt(1.0f - z * z), phi = 2 * PI * x_2;
            vec3 localRay(r * std::cos(phi), r * std::sin(phi), z);
            return toWorld(localRay, normal);

            break;
        }
    }
}