#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.h"

class camera {
    public:
        camera(
            // NOTE: The following three variables specify the point where we're looking from (where our "eye" is), the point where we're looking to, and an up-vector 'vup' which is used to specify the rotational axis of our camera - think of the bridge of your nose as an arrow pointing up.
            point3 lookfrom,
            point3 lookat,
            vec3 vup,

            double vfov, // vertical field-of-view in degrees
            double aspect_ratio,
            double aperture,
            double focus_dist
        ) {
            // NOTE: The following 4 lines of code are using a variable 'h' as a ratio of the distance to our plane of focus. This is used to calculate our FOV.
            auto theta = degrees_to_radians(vfov);
            auto h = tan(theta/2);
            auto viewport_height = 2.0 * h;
            auto viewport_width = aspect_ratio * viewport_height;
            
            // NOTE: Here we're defining a new basis of axes such that our camera points in the direction of -w instead of -z, such that we can calculate this basis entirely on the passed FOV, lookfrom point, and lookat point.
            w = unit_vector(lookfrom - lookat);
            u = unit_vector(cross(vup, w));
            v = cross(w, u);

            // NOTE: Now we're defining our viewport in terms of the newly established orthagonal basis.
            origin = lookfrom;
            horizontal = focus_dist * viewport_width * u;
            vertical = focus_dist * viewport_height * v;
            lower_left_corner = origin - horizontal/2 - vertical/2 - focus_dist*w;

            lens_radius = aperture / 2;
        }

        // NOTE: This function has been altered to incorporate defocus blur, approximating and simulating a physical lense and sending the rays from points on that lense.
        ray get_ray(double s, double t) const {
            vec3 rd = lens_radius * random_in_unit_disk();
            vec3 offset = u * rd.x() + v * rd.y();

            return ray(
                origin + offset,
                lower_left_corner + s*horizontal + t*vertical - origin - offset
            );
        }

    private:
        point3 origin;
        point3 lower_left_corner;
        vec3 horizontal;
        vec3 vertical;
        vec3 u, v, w;
        double lens_radius;
};
#endif