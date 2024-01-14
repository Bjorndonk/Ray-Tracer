#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"
#include "rtweekend.h"

class material;

struct hit_record {
    point3 p;
    vec3 normal;
    // NOTE: This is here because we need to point the hittable and material classes to each other to be referenced
    shared_ptr<material> mat_ptr;
    double t;

    // NOTE: The following code is calculating which side of the object the ray is hitting - from the inside or the outside of the object - and storing it as a boolean value. This is one of the two described options in the book, the other of which would use a dot product of the ray and the normal (which will be positive if external and negative if internal) once the image is being colored. This will be helpful for glass objects in the future.
    bool front_face;
    inline void set_face_normal(const ray& r, const vec3& outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
        // NOTE: Reminder that 'outward_normal' is assumed to have unit length
        normal = front_face ? outward_normal :-outward_normal;
    }
};

class hittable {
    public:
        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
};

#endif