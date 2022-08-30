#ifndef RAYTRACING_MATERIAL_HPP
#define RAYTRACING_MATERIAL_HPP

#include "core/ray.hpp"
#include "../pdf/pdf.hpp"

class HitRecord;

class ScatterRecord {
public:
    Ray specular_ray;
    bool is_specular;
    Color attenuation;
    std::shared_ptr<PDF> pdf_ptr;
};

class Material {
public:
    virtual Color emitted(const Ray &ray_in, const HitRecord &hit_record) {
        return {0, 0, 0};
    }

    virtual bool scatter(
            const Ray &ray_in, const HitRecord &hit_record, ScatterRecord &scatter_record
    ) const {
        return false;
    }

    [[nodiscard]] virtual double scattering_pdf(
            const Ray &r_in, const HitRecord &rec, const Ray &scattered
    ) const {
        return 0;
    }
};


#endif //RAYTRACING_MATERIAL_HPP
