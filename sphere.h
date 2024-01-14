#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"

class sphere : public hittable {
    public:
        sphere() {}
        // NOTE: This has been updated such that once an intersection is found, it can reference the sphere's material in order to determine how the ray will interact with the surface of the sphere.
        sphere(point3 cen, double r, shared_ptr<material> m)
            : center(cen), radius(r), mat_ptr(m) {};

        virtual bool hit(
            const ray& r, double t_min, double t_max, hit_record& rec) const override;

    public:
        point3 center;
        double radius;
        // NOTE: To reference material properties
        shared_ptr<material> mat_ptr;
};

bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    // NOTE: This is a hit-checking function that checks the intersection point (if it exists) between a ray and a sphere by parameterizing the two functions, given the origin and radius of the sphere
    vec3 oc = r.origin() - center;
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius*radius;

    // NOTE: This is using the *simplified* quadratic formula (wherein b = 2h and the equation is then simplified) to return the smaller of the two possible return values (ie the smaller value of t and hence the position that the ray intersects the sphere the first time).
    auto discriminant = half_b*half_b - a*c; 
    if (discriminant < 0) return false;
    auto sqrtd = sqrt(discriminant);

    // NOTE: Finding the nearest root that lies in the acceptable range.
    auto root = (-half_b - sqrtd) / a;
    if (root < t_min || t_max < root) {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root)
            return false;
    }

    // NOTE: This is storing the hit data in the hit-record. t: the t-value of the ray, p: the point of intersection, normal: the normal of the surface of intersection
    rec.t = root;
    rec.p = r.at(rec.t);
    // NOTE: The following code is now taking into consideration whether the ray is hitting the shape "internally" or "externally" using the calculated normal.
    vec3 outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);
    // NOTE: This sets the material of the ray's current intersection to be referenced for how that ray then interacts with that surface material.
    rec.mat_ptr = mat_ptr;

    return true;
}

#endif