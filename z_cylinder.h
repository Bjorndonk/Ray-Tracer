#ifndef Z_CYLINDER_H
#define Z_CYLINDER_H

#include "hittable.h"
#include "vec3.h"

class z_cylinder : public hittable {
    public:
        z_cylinder() {}
        z_cylinder(point3 cen, double r, shared_ptr<material> m, double z)
            : center(cen), radius(r), mat_ptr(m), z_val(z) {};

        virtual bool hit(
            const ray& r, double t_min, double t_max, hit_record& rec) const override;

    public:
        point3 center;
        double radius;
        shared_ptr<material> mat_ptr;
        double z_val;
};

bool z_cylinder::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {

    // Calculating Quadratic function variables
    auto a = (r.direction().x() * r.direction().x()) + (r.direction().y() * r.direction().y());
    auto half_b = (r.origin().x() * r.direction().x()) - (r.direction().x() * center.x()) + (r.origin().y() * r.direction().y()) - (r.direction().y() * r.origin().y());
    auto c = (r.origin().x() * r.origin().x()) - (2 * r.origin().x() * center.x()) + (center.x() * center.x()) + (r.origin().y() * r.origin().y()) - (2 * r.origin().y() * center.y()) + (center.y() * center.y()) - (radius * radius);

    // Calculating Descriminant
    auto discriminant = half_b*half_b - a*c;

    // This is temporary code for an infinite cylinder
    if (discriminant < 0) return false;
    auto sqrtd = sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    auto root = (-half_b - sqrtd) / a;
    if (root < t_min || t_max < root) {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root)
            return false;
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    
    // NOTE: This code is determining the endpoints of the cylinder.
    if (rec.p.z() <= (center.z() - z_val) || rec.p.z() >= (center.z() + z_val)) {
        return false;
    }

    // NOTE: The following code is now taking into consideration whether the ray is hitting the shape "internally" or "externally" using the calculated normal.
    vec3 outward_normal = rec.p - center;
    double magnitude = sqrt((outward_normal.x() * outward_normal.x()) + (outward_normal.y() * outward_normal.y()));
    vec3 unit_normal = vec3((outward_normal.x() / magnitude), (outward_normal.y() / magnitude), 0);

    rec.set_face_normal(r, unit_normal);
    rec.mat_ptr = mat_ptr;

    return true;
}

#endif