#ifndef ELLIPSOID_H
#define ELLIPSOID_H

#include "hittable.h"
#include "vec3.h"

class ellipsoid : public hittable {
    public:
        ellipsoid() {}
        ellipsoid(point3 cen, double a, double b, double c, shared_ptr<material> m)
            : center(cen), x_constant(a), y_constant(b), z_constant(c), mat_ptr(m) {};

        virtual bool hit(
            const ray& r, double t_min, double t_max, hit_record& rec) const override;

    public:
        point3 center;
        double x_constant;
        double y_constant;
        double z_constant;
        shared_ptr<material> mat_ptr;
};

bool ellipsoid::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    // NOTE: This is the code that originally existed as a hit-checking function in main. It's process is idential.
    double x_con_squared = x_constant * x_constant;
    double y_con_squared = y_constant * y_constant;
    double z_con_squared = z_constant * z_constant;
    
    auto a = (r.direction().x() * r.direction().x() * y_con_squared * z_con_squared) + (r.direction().x() * r.direction().y() * x_con_squared * z_con_squared) + (r.direction().x() * r.direction().z() * x_con_squared * y_con_squared);
    auto half_b = ((y_con_squared * z_con_squared * r.direction().x()) * (r.origin().x() - center.x())) + ((x_con_squared * z_con_squared * r.direction().y()) * (r.origin().y() - center.y())) + ((x_con_squared * y_con_squared * r.direction().z()) * (r.origin().z() - center.z()));
    auto c = ((y_con_squared * z_con_squared) * ((r.origin().x() * r.origin().x()) - (2 * r.origin().x() * center.x()) + (center.x() * center.x()))) + ((x_con_squared * z_con_squared) * ((r.origin().y() * r.origin().y()) - (2 * r.origin().y() * center.y()) + (center.y() * center.y()))) + ((x_con_squared * y_con_squared) * ((r.origin().z() * r.origin().z()) - (2 * r.origin().z() * center.z()) + (center.z() * center.z()))) - (x_con_squared * y_con_squared * z_con_squared);

    auto discriminant = half_b*half_b - a*c; // NOTE: This is using the *simplified* quadratic formula (wherein b = 2h and the equation is then simplified) to return the smaller of the two possible return values (ie the smaller value of t and hence the position that the ray intersects the sphere the first time).
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
    // The following code is now taking into consideration whether the ray is hitting the shape "internally" or "externally" using the calculated normal.
    vec3 outward_normal = vec3((2 * (rec.p.x() / x_con_squared)), (2 * (rec.p.y() / y_con_squared)), (2 * (rec.p.z() / z_con_squared)));
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mat_ptr;

    return true;
}

#endif