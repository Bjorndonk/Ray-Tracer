#include "rtweekend.h"

#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include "z_cylinder.h"
#include "ellipsoid.h"

#include "camera.h"
#include "material.h"

#include <iostream>

// NOTE: This is a recursive function. Starting with the initial ray cast, it passes to a hit-function that checks the nearest object to be hit and generates a new ray. At this point, the ray is either reflected (in a way determined by the material of the surface being hit) or absored, which is decided by the boolean return value of the scatter function.
color ray_color(const ray& r, const hittable& world, int depth) {
    hit_record rec;

    // NOTE: This (alongside several other minor function changes) is to cap ray reflections at 50 such that an absurd number of reflections generated randomly doesn't blow the stack.
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0,0,0);

    // NOTE: This hit function call has had its second parameter changed from 0 to .001 to account for slight floating point calculation errors by giving this a larger margin of error.
    // NOTE: This fixes "shadow acne".
    if (world.hit(r, 0.001, infinity, rec)) {
        ray scattered;
        color attenuation;
        // NOTE: This if-statement is checking for the case that a ray has been absorbed (this is only prevelant here in the metal class, wherein rays can be set to reflect underneath the surface of the object), which occurs when this function returns false.
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_color(scattered, world, depth-1);
        return color(0,0,0);
    }
    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5*(unit_direction.y() + 1.0);
    return (1.0-t)*color(0.2, 0.2, 1.0) + t*color(0.5, 0.7, 1.0);
}

int main() {

    // Image
    const auto aspect_ratio = 2.0 / 4.0;
    const int image_width = 1000;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 100;
    const int max_depth = 50;

    // World

    hittable_list world;

    auto material_person = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_dumbbell  = make_shared<metal>(color(0.6, 0.6, 0.6), 2.0);
    auto material_ground = make_shared<lambertian>(color(0.2, 1.0, 0.2));
    auto material_pants = make_shared<lambertian>(color(0.5, 0.2, 0.1));
    auto material_shirt = make_shared<lambertian>(color(1, 0.3, 0.3));
    auto material_skin = make_shared<lambertian>(color(0.9, 0.5, 0.4));
    auto material_shoes = make_shared<lambertian>(color(0, 0, 0));
    auto material_sun = make_shared<metal>(color(1, 1, 0.0), 20);

    //world.add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(make_shared<z_cylinder>(point3(0.0, 0.0, 0.0), 0.5, material_dumbbell, 12)); // Dumbbell
    world.add(make_shared<sphere>(point3(0.0, 0.0, -14.0), 4, material_dumbbell)); // Dumbbell
    world.add(make_shared<sphere>(point3(0.0, 0.0, 14.0), 4, material_dumbbell)); // Dumbbell

    world.add(make_shared<sphere>(point3(0.0, 0.0, 7.0), 1.3, material_skin)); // Hands
    world.add(make_shared<sphere>(point3(0.0, 0.0, -7.0), 1.3, material_skin)); // Hands

    world.add(make_shared<sphere>(point3(0.0, -3.0, 7.0), 0.8, material_skin)); // Arms
    world.add(make_shared<sphere>(point3(0.0, -3.0, -7.0), 0.8, material_skin)); // Arms
    world.add(make_shared<sphere>(point3(0.0, -6.0, 6.7), 0.8, material_skin)); // Arms
    world.add(make_shared<sphere>(point3(0.0, -6.0, -6.7), 0.8, material_skin)); // Arms
    world.add(make_shared<sphere>(point3(0.0, -9.0, 6.0), 0.8, material_skin)); // Arms
    world.add(make_shared<sphere>(point3(0.0, -12.0, -5.0), 0.8, material_shirt)); // Arms
    world.add(make_shared<sphere>(point3(0.0, -12.0, 5.0), 0.8, material_shirt)); // Arms
    world.add(make_shared<sphere>(point3(0.0, -9.0, -6.0), 0.8, material_skin)); // Arms

    world.add(make_shared<sphere>(point3(0.0, -7.0, 0.0), 2.5, material_skin)); // Head
    world.add(make_shared<sphere>(point3(0.0, -10.0, 0.0), 0.8, material_skin)); // Neck

    world.add(make_shared<sphere>(point3(0.0, -14.0, 0.0), 4, material_shirt)); // Torso
    world.add(make_shared<sphere>(point3(0.0, -16.0, 0.0), 4, material_shirt)); // Torso
    world.add(make_shared<sphere>(point3(0.0, -18.0, 0.0), 4, material_pants)); // Torso

    world.add(make_shared<sphere>(point3(0.0, -23.0, 2.0), 0.8, material_pants)); // Legs
    world.add(make_shared<sphere>(point3(0.0, -23.0, -2.0), 0.8, material_pants)); // Legs
    world.add(make_shared<sphere>(point3(0.0, -26.0, 2.3), 0.8, material_pants)); // Legs
    world.add(make_shared<sphere>(point3(0.0, -26.0, -2.3), 0.8, material_pants)); // Legs
    world.add(make_shared<sphere>(point3(0.0, -29.0, 2.5), 0.8, material_pants)); // Legs
    world.add(make_shared<sphere>(point3(0.0, -29.0, -2.5), 0.8, material_pants)); // Legs

    world.add(make_shared<sphere>(point3(0.0, -33.0, 2.8), 1.5, material_shoes)); // Feet
    world.add(make_shared<sphere>(point3(0.0, -33.0, -2.8), 1.5, material_shoes)); // Feet

    world.add(make_shared<sphere>(point3(0, -234.5, 0), 200, material_ground)); // Floor

    world.add(make_shared<sphere>(point3(-1000, 400, -80), 80, material_sun)); // Sun

    // Camera

    point3 lookfrom(90,0,0);
    point3 lookat(0,0,0);
    vec3 vup(0,1,0);
    auto dist_to_focus = (lookfrom-lookat).length();
    auto aperture = 0.2;

    camera cam(lookfrom, lookat, vup, 50, aspect_ratio, aperture, dist_to_focus);

    // Render
    
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height-1; j >= 0; --j) {
        // NOTE: This is a progress indicator, printing the number of remaining lines of the image left to be processed.
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            // NOTE: This code has been altered such that is is performed a number of times equal to the set samples_per_pixel variable. Each time, a semi-random ray is cast and the color is returned, but after each loop, that color value is added to a variable that is then averaged in the write_color function after the loop concludes.
            color pixel_color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s) {
                // NOTE: 'u' and 'v' are the horizontal and vertical viewport positions respectively, which are passed into the cam.get_ray function to calculate the ray which is then sent to test for hittable object intersection
                auto u = (i + random_double()) / (image_width-1);
                auto v = (j + random_double()) / (image_height-1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, world, max_depth);
            }
            write_color(std::cout, pixel_color, samples_per_pixel);
        }
    }

    // NOTE: This prints that the image has finished processing before the main function terminates.
    std::cerr << "\nDone.\n";
}