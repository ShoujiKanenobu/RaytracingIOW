#ifndef TEXTURE_H
#define TEXTURE_H

#include "utilitiesconstraints.h"
#include "perlin.h"
#include "rt_stb_image.h"

#include <iostream>

class texture {
    public:
        virtual color value(float u, float v, const point3& p) const = 0;
};

class solid_color : public texture {
    public:
        solid_color() {}
        solid_color(color c) : color_value(c) {}
        solid_color(float r, float g, float b) : solid_color(color(r, g, b)) {}
        virtual color value(float u, float v, const vec3& p) const override {
            return color_value;
        }
    public:
        color color_value;
};

class checkered_texture : public texture {
    public:
        checkered_texture() {}
        checkered_texture(shared_ptr<texture> _even, shared_ptr<texture> _odd) : even(_even), odd(_odd) {}
        checkered_texture(color c1, color c2) : even(make_shared<solid_color>(c1)), odd(make_shared<solid_color>(c2)) {}

        virtual color value(float u, float v, const point3& p) const override {
            auto sines = sin(10*p.x()) * sin(10*p.y()) * sin(10*p.z());
            if(sines < 0)
                return odd->value(u, v, p);
            else
                return even->value(u, v, p);
        }

    public:
    shared_ptr<texture> even, odd;
};

class noise_texture : public texture {
    public:
        noise_texture() {}
        noise_texture(float sc) : scale(sc) {}

        virtual color value(float u, float v, const point3& p) const override {
            return color(1,1,1) * 0.5 * (1 + sin(scale * p.z() + 10*noise.turb(p)));
        }

    public:
        perlin noise;
        float scale;
};

class image_texture : public texture {
    public:
        const static int bytes_per_pixel = 3;
        
        image_texture() : data(nullptr), width(0), height(0), bytes_per_scanline(0) {}

        image_texture(const char* filename) {
            auto components_per_pixel = bytes_per_pixel;

            data = stbi_load(filename, &width, &height, &components_per_pixel, components_per_pixel);

            if(!data)
            {
                std::cerr << "Couldn't load texture image file: " << filename << "\n";
                width = height = 0;
            }

            bytes_per_scanline = bytes_per_pixel * width;
        }

        ~image_texture()
        {
            delete data;
        }

        virtual color value(float u, float v, const point3& p) const override {
            if(data == nullptr)
                return color(0,1,1);
            
            u = clamp(u, 0.0, 1.0);
            v = 1.0 - clamp(v, 0.0, 1.0); //flipping v coord;

            auto i = static_cast<int>(u * width);
            auto j = static_cast<int>(v * height);

            //clamp i and j to width/height;
            if(i >= width) i = width-1;
            if(j >= height) j >= height-1;

            const auto color_scale = 1.0 / 255.0;
            auto pixel = data + j*bytes_per_scanline + i * bytes_per_pixel;

            return color(color_scale * pixel[0], color_scale*pixel[1], color_scale*pixel[2]);

        }
    public:
        unsigned char *data;
        int width, height;
        int bytes_per_scanline;
};

#endif