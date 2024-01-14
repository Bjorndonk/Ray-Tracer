#ifndef MATERIAL_H
#define MATERIAL_H

#include "rtweekend.h"

struct hit_record;

// NOTE: The material class is referenced by the program when it wants to know how a ray interacts with the surface its hit; if it scatters, reflects, is absorbed, etc...
class material {
    public:
        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const = 0;
};

// NOTE: This is for matte materials.
class lambertian : public material {
    public:
        lambertian(const color& a) : albedo(a) {}

        // NOTE: This is calculating the new direction of the scattered ray and the amount of light that is reflected by that ray.
        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const override {
            auto scatter_direction = rec.normal + random_unit_vector();
            
            // NOTE: Catch degenerate scatter direction
            if (scatter_direction.near_zero())
                scatter_direction = rec.normal;
            
            scattered = ray(rec.p, scatter_direction);
            attenuation = albedo;
            return true;
        }

    public:
        color albedo;
};

// NOTE: This is for reflective objects such as metal.
class metal : public material {
    public:
        metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

        // NOTE: Again, we're calculating the new ray direction and light absorption, but this time the scatter direction is determined by the angle of incidence.
        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const override {
            vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
            scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere());
            attenuation = albedo;
            // NOTE: The reason we don't just return 'true' here is because there is a chance that the metal fuzz will produce a reflected ray underneath the surface of the sphere. We use this dot product to determine if this is occurring (using similar logic to the calculation method for an internal/external normal). If this returns false, this is occuring, at which point the ray is absorbed and returns no color (this is actually checked for in the main function, somewhere around line 29, in an if condition).
            return (dot(scattered.direction(), rec.normal) > 0);
        }

    public:
        color albedo;
        double fuzz;
};

// NOTE: This is for refractive objects such as glass or diamond.
class dielectric : public material {
    public:
        dielectric(double index_of_refraction) : ir(index_of_refraction) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const override {
            attenuation = color(1.0, 1.0, 1.0);

            // NOTE: Here we're calculating a solutions to Snell's law to calculate the refraction of the ray
            double refraction_ratio = rec.front_face ? (1.0/ir) : ir;

            vec3 unit_direction = unit_vector(r_in.direction());
            double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
            double sin_theta = sqrt(1.0 - cos_theta*cos_theta);

            // NOTE: This is checking and storing whether there exists a solution to Snell's law. If there is not, we reflect the ray instead of refracting.
            bool cannot_refract = refraction_ratio * sin_theta > 1.0;
            vec3 direction;
            // NOTE: This if-statement checks 2 conditions to see whether or not to refract: If there does not exist a solution to Snell's Law, or if we're looking at the reflective surface at a steep enough angle, calculated using a Schlick Approximation (see the reflectance function).
            // NOTE: If either of these conditons are met, we reflect instead of refracting.
            if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
                direction = reflect(unit_direction, rec.normal);
            else
                direction = refract(unit_direction, rec.normal, refraction_ratio);

            scattered = ray(rec.p, direction);
            return true;
        }

    public:
        double ir; // Index of Refraction

    private:
        static double reflectance(double cosine, double ref_idx) {
            // NOTE: Use Schlick's approximation for reflectance.
            auto r0 = (1-ref_idx) / (1+ref_idx);
            r0 = r0*r0;
            return r0 + (1-r0)*pow((1 - cosine),5);
        }
};

#endif