#ifndef MATERIAL_H
#define MATERIAL_H

#include "utilitiesconstraints.h"
#include "hittable.h"
#include "texture.h"

class material {
    public:
        virtual color emitted(float u, float v, const point3& p) const {
            return color(0,0,0);
        }
        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const = 0;
};

//diffuse
class lambertian : public material {
    public:
        lambertian(const color& a) : albedo(make_shared<solid_color>(a)) {}
        lambertian(shared_ptr<texture> a) : albedo(a) {}
        virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
            auto scatter_direction = rec.normal + random_unit_vector();

            //Stop true zeros
            if(scatter_direction.near_zero())
                scatter_direction = rec.normal;

            scattered = ray(rec.p, scatter_direction, r_in.time());
            attenuation = albedo->value(rec.u, rec.v, rec.p);
            return true;
        }

        public:
            shared_ptr<texture> albedo;
};

class metal : public material {
    public:
        metal(const color& a, float f) : albedo(a), fuzz(f < 1 ? f : 1){}
        virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
            vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
            scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere(), r_in.time());
            attenuation = albedo;
            return (dot(scattered.direction(), rec.normal) > 0);
        }

        public:
            color albedo;
            float fuzz;
};

class dielectric : public material {
    public: 
        dielectric(float index_of_refraction) : ir(index_of_refraction) {}
        virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
            attenuation = color (1.0, 1.0, 1.0);
            float refract_ratio = rec.front_face ? (1.0/ir) : ir;
            
            vec3 unit_dir = unit_vector(r_in.direction());
            vec3 refracted = refract(unit_dir, rec.normal, refract_ratio);
            
            float cos_theta = fmin(dot(-unit_dir, rec.normal), 1.0);
            float sin_theta = sqrt(1.0 - cos_theta*cos_theta);

            vec3 direction;
            bool cant_refract = refract_ratio * sin_theta > 1.0;
            if(cant_refract || reflectance(cos_theta, refract_ratio) > random_float())
            {
                direction = reflect(unit_dir, rec.normal);
            } else {
                direction = refract(unit_dir, rec.normal, refract_ratio);
            }

            scattered = ray(rec.p, refracted, r_in.time());
            return true;
        }
    public:
        float ir;
    private:
        static float reflectance(float cosine, float ref_idx)
        {
            auto r0 = (1-ref_idx) / (1+ref_idx);
            r0 = r0*r0;
            return r0 + (1-r0)*pow((1-cosine), 5);
        }
};

class diffuse_light : public material
{
    public:
        diffuse_light(shared_ptr<texture> a) : emit(a) {}
        diffuse_light(color c) : emit (make_shared<solid_color>(c)) {}

        virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
            return false;
        }

        virtual color emitted(float u, float v, const point3& p) const override {
            return emit->value(u, v, p);
        }

    public:
        shared_ptr<texture> emit;
};

class isotropic : public material 
{
    public: 
        isotropic(color c) : albedo(make_shared<solid_color>(c)) {}
        isotropic(shared_ptr<texture> a) : albedo(a) {}

        virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
            scattered = ray(rec.p, random_in_unit_sphere(), r_in.time());
            attenuation = albedo->value(rec.u, rec.v, rec.p);
            return true;
        }
    public:
        shared_ptr<texture> albedo;
};

#endif