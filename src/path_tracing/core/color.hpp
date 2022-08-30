#ifndef RAYTRACING_COLOR_HPP
#define RAYTRACING_COLOR_HPP

#include "ray.hpp"
#include "geometries/hit.hpp"
#include "utils.hpp"
#include "pdf/cosine_pdf.hpp"
#include "pdf/hittable_pdf.hpp"
#include "pdf/mixture_pdf.hpp"


Color
ray_color(Ray &ray, const HittableList &world, const std::shared_ptr<Hittable> &lights, Color &background, int depth) {
    if (depth <= 0) {
        return {0, 0, 0};
    }
    HitRecord hit_record;
    if (!world.hit(ray, 0.001, infinity, hit_record)) {
        return background;
    }
    ScatterRecord scatter_record;
    Color emitted = hit_record.material_ptr->emitted(ray, hit_record);
    if (!hit_record.material_ptr->scatter(ray, hit_record, scatter_record)) {
        return emitted;
    }
    if (scatter_record.is_specular) {
        return scatter_record.attenuation *
               ray_color(scatter_record.specular_ray, world, lights, background, depth - 1);
    }
    Ray ray_scattered;
    double pdf_value{};
    MixturePDF mix_pdf(std::make_shared<HittablePDF>(lights, hit_record.hit_point), scatter_record.pdf_ptr);
    ray_scattered = {hit_record.hit_point, mix_pdf.generate(), ray.time};
    pdf_value = mix_pdf.value(ray_scattered.direction);
    return emitted +
           scatter_record.attenuation * hit_record.material_ptr->scattering_pdf(ray, hit_record, ray_scattered) *
           ray_color(ray_scattered, world, lights, background, depth - 1) / pdf_value;
}

void write_color(std::ostream &out, Color &pixel_color, int samples_per_pixel) {
    double r = pixel_color.x() / samples_per_pixel;
    double g = pixel_color.y() / samples_per_pixel;
    double b = pixel_color.z() / samples_per_pixel;

    // Replace NaN components with zero
    if (r != r) r = 0.0;
    if (g != g) g = 0.0;
    if (b != b) b = 0.0;

    r = sqrt(r);
    g = sqrt(g);
    b = sqrt(b);

    // Write the translated [0,255] value of each color component.
    out << int(256 * clamp(r, 0.0, 0.999)) << ' '
        << int(256 * clamp(g, 0.0, 0.999)) << ' '
        << int(256 * clamp(b, 0.0, 0.999)) << '\n';
}

#endif //RAYTRACING_COLOR_HPP
