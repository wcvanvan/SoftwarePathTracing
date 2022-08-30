#ifndef RAYTRACING_RECTANGLE_HPP
#define RAYTRACING_RECTANGLE_HPP

#include <utility>

#include "hit.hpp"
#include "../material/material.hpp"

class xy_rect : public Hittable {
public:
    double x0, x1, y0, y1, z;
    // (x0, y0) will be the closest corner, (x1, y1) will be the farthest corner
    std::shared_ptr<Material> material_ptr;

    xy_rect(double _x0, double _x1, double _y0, double _y1, double _z, std::shared_ptr<Material> mat) :
            material_ptr(std::move(mat)), z(_z) {
        if (_x0 < _x1) {
            x0 = _x0;
            x1 = _x1;
        } else {
            x0 = _x1;
            x1 = _x0;
        }
        if (_y0 < _y1) {
            y0 = _y0;
            y1 = _y1;
        } else {
            y0 = _y1;
            y1 = _y0;
        }
    }

    bool hit(const Ray &r, double t_min, double t_max, HitRecord &record) const override;

    std::shared_ptr<aabb> bounding_box(double time0, double time1, bool &bounded) const override;

};

bool xy_rect::hit(const Ray &r, double t_min, double t_max, HitRecord &record) const {
    double t = (z - r.origin.z()) / r.direction.z();
    if (t < t_min || t > t_max) {
        return false;
    }
    auto hit_point = r.at(t);
    if (hit_point.x() < x0 || hit_point.x() > x1 || hit_point.y() < y0 ||
        hit_point.y() > y1) {
        return false;
    }
    record.t = t;
    record.hit_point = hit_point;
    record.material_ptr = material_ptr;
    record.set_face_normal(r, vec3(0, 0, 1));
    record.exterior_hit = true;
    record.u = (record.hit_point.x() - x0) / (x1 - x0);
    record.v = (record.hit_point.y() - y0) / (y1 - y0);
    return true;
}

std::shared_ptr<aabb> xy_rect::bounding_box(double time0, double time1, bool &bounded) const {
    bounded = true;
    return std::make_shared<aabb>(point3(x0, y0, z - 0.0001), point3(x1, y1, z + 0.0001));
}


class xz_rect : public Hittable {
public:
    double x0, x1, z0, z1, y;
    std::shared_ptr<Material> material_ptr;

    xz_rect(double _x0, double _x1, double _z0, double _z1, double _y, std::shared_ptr<Material> mat) :
            material_ptr(std::move(mat)), y(_y) {
        if (_x0 < _x1) {
            x0 = _x0;
            x1 = _x1;
        } else {
            x0 = _x1;
            x1 = _x0;
        }
        if (_z0 < _z1) {
            z0 = _z0;
            z1 = _z1;
        } else {
            z0 = _z1;
            z1 = _z0;
        }
    }

    bool hit(const Ray &r, double t_min, double t_max, HitRecord &record) const override;

    std::shared_ptr<aabb> bounding_box(double time0, double time1, bool &bounded) const override;

    [[nodiscard]] double pdf_value(const point3 &origin, const vec3 &direction) const override;

    [[nodiscard]] vec3 random_direction(const vec3 &origin) const override;
};

bool xz_rect::hit(const Ray &r, double t_min, double t_max, HitRecord &record) const {
    double t = (y - r.origin.y()) / r.direction.y();
    if (t < t_min || t > t_max) {
        return false;
    }
    auto hit_point = point3(r.at(t));
    if (hit_point.x() < x0 || hit_point.x() > x1 || hit_point.z() < z0 ||
        hit_point.z() > z1) {
        return false;
    }
    record.t = t;
    record.hit_point = hit_point;
    record.material_ptr = material_ptr;
    record.normal = {0, 1, 0};
    record.exterior_hit = r.direction.dot(record.normal) < 0;
    if (!record.exterior_hit) { record.normal = record.normal * (-1); }
    record.u = (record.hit_point.x() - x0) / (x1 - x0);
    record.v = (record.hit_point.z() - z0) / (z1 - z0);
    return true;
}

std::shared_ptr<aabb> xz_rect::bounding_box(double time0, double time1, bool &bounded) const {
    bounded = true;
    return std::make_shared<aabb>(point3(x0, y - 0.0001, z0), point3(x1, y + 0.0001, z1));
}

double xz_rect::pdf_value(const point3 &origin, const vec3 &direction) const {
    HitRecord record;
    if (!this->hit(Ray(origin, direction, 0.0), 0.001, infinity, record)) {
        return 0.0;
    }
    double area = (x1 - x0) * (z1 - z0);
    double distance_squared = record.t * record.t * direction.length() * direction.length();
    double cosine = fabs(record.normal.dot(direction) / direction.length());
    return distance_squared / (area * cosine);
}

vec3 xz_rect::random_direction(const vec3 &origin) const {
    vec3 random_point = {random_number(x0, x1), y, random_number(z0, z1)};
    return (random_point - origin).unit_vector();
}

class yz_rect : public Hittable {
public:
    double y0, y1, z0, z1, x;
    std::shared_ptr<Material> material_ptr;

    yz_rect(double _y0, double _y1, double _z0, double _z1, double _x, std::shared_ptr<Material> mat) :
            material_ptr(std::move(mat)), x(_x) {
        if (_y0 < _y1) {
            y0 = _y0;
            y1 = _y1;
        } else {
            y0 = _y1;
            y1 = _y0;
        }
        if (_z0 < _z1) {
            z0 = _z0;
            z1 = _z1;
        } else {
            z0 = _z1;
            z1 = _z0;
        }
    }

    bool hit(const Ray &r, double t_min, double t_max, HitRecord &record) const override;

    std::shared_ptr<aabb> bounding_box(double time0, double time1, bool &bounded) const override;
};

bool yz_rect::hit(const Ray &r, double t_min, double t_max, HitRecord &record) const {
    auto t = (x - r.origin.x()) / r.direction.x();
    if (t < t_min || t > t_max) {
        return false;
    }
    auto hit_point = point3(r.at(t));
    if (hit_point.y() < y0 || hit_point.y() > y1 || hit_point.z() < z0 ||
        hit_point.z() > z1) {
        return false;
    }
    record.t = t;
    record.hit_point = hit_point;
    record.material_ptr = material_ptr;
    record.set_face_normal(r, vec3(1, 0, 0));
    record.exterior_hit = true;
    record.u = (record.hit_point.y() - y0) / (y1 - y0);
    record.v = (record.hit_point.z() - z0) / (z1 - z0);
    return true;
}

std::shared_ptr<aabb> yz_rect::bounding_box(double time0, double time1, bool &bounded) const {
    bounded = true;
    return std::make_shared<aabb>(point3(x - 0.0001, y0, z0), point3(x + 0.0001, y1, z1));
}

#endif //RAYTRACING_RECTANGLE_HPP
