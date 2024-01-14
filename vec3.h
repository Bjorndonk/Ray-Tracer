#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>

using std::sqrt;

// BEGIN CUSTOM FUNCTIONS
// NOTE: This is a solution I had to impliment on my own. These functions are identically declared in rtweekend.h, but when compiling the calls in this header couldn't find the function's declaration in rtweekend.h. So, I duplicated the functions here under a new name and called the local instance in the relevant functions in this file.
inline double random_double_local() {
    // Returns a random real in [0,1).
    return rand() / (RAND_MAX + 1.0);
}

inline double random_double_local(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max-min)*random_double_local();
}
// END CUSTOM FUNCTIONS

class vec3 {
    public:
        vec3() : e{0,0,0} {}
        vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

        double x() const { return e[0]; }
        double y() const { return e[1]; }
        double z() const { return e[2]; }

        vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
        double operator[] (int i) const { return e[i]; }
        double& operator[] (int i) { return e[i]; }

        // NOTE: Function for vector addition
        vec3& operator+=(const vec3 &v) {
            e[0] += v.e[0];
            e[1] += v.e[1];
            e[2] += v.e[2];
            return *this;
        }

        // NOTE: Function for vector multiplication
        vec3& operator*=(const double t) {
            e[0] *= t;
            e[1] *= t;
            e[2] *= t;
            return *this;
        }

        // NOTE: Function for vector division
        vec3& operator/=(const double t) {
            return *this *= 1/t;
        }

        // NOTE: Function for returning vector length (ie magnitude)
        double length() const {
            return sqrt(length_squared());
        }

        // NOTE: Function that returns the internal component of the square-root function being used to calculate a vector's length
        double length_squared() const {
            return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
        }

    public:
        double e[3];

        // NOTE: These functions are for returning a reflected light vector by generating a point in the unit cube that exists tangent to the normal line of the point the light vector hit on the sphere. Another function generates a vector in this way, checks if its within the unit sphere, and if it is not, repeats the generation process until it recieves a valid vector.
        inline static vec3 random() {
            return vec3(random_double_local(), random_double_local(), random_double_local());
        }

        inline static vec3 random(double min, double max) {
            return vec3(random_double_local(min,max), random_double_local(min,max), random_double_local(min,max));
        }

        // NOTE: This is used for debugging an edge case wherein a returned vector is near-zero in every dimension. If my memory serves, this can lead the program to process infinite ray reflections.
        bool near_zero() const {
            // Return true if the vector is close to zero in all dimensions.
            const auto s = 1e-8;
            return (fabs(e[0]) < s) && (fabs(e[1]) < s) && (fabs(e[2]) < s);
        }

};

// Type aliases for vec3
using point3 = vec3;   // 3D point
using color = vec3;    // RGB color

// vec3 Utility Functions

// NOTE: Function for printing a vector
inline std::ostream& operator<<(std::ostream &out, const vec3 &v) {
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

// NOTE: Functions for vector arithmetic
inline vec3 operator+(const vec3 &u, const vec3 &v) {
    return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator-(const vec3 &u, const vec3 &v) {
    return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3 &u, const vec3 &v) {
    return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator*(double t, const vec3 &v) {
    return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

inline vec3 operator*(const vec3 &v, double t) {
    return t * v;
}

inline vec3 operator/(vec3 v, double t) {
    return (1/t) * v;
}

inline double dot(const vec3 &u, const vec3 &v) {
    return u.e[0] * v.e[0]
         + u.e[1] * v.e[1]
         + u.e[2] * v.e[2];
}

inline vec3 cross(const vec3 &u, const vec3 &v) {
    return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
                u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline vec3 unit_vector(vec3 v) {
    return v / v.length();
}

// NOTE: This is the function that generates reflection vectors in the unit cube, checks if they're within the unit circle, and re-generates until a valid reflection vector is returned.
vec3 random_in_unit_sphere() {
    while (true) {
        auto p = vec3::random(-1,1);
        if (p.length_squared() >= 1) continue;
        return p;
    }
}

// NOTE: By normalizing the vectors here, we convert the vector generated inside the unit sphere to be a vector that ends on the surface of the unit sphere.
// NOTE: This is to properly impliment a true Lambertian Reflection
vec3 random_unit_vector() {
    return unit_vector(random_in_unit_sphere());
}

// NOTE: The following function is an alternative method provided in the text that instead of incorporating the normal vector of an intersection point simply generates a reflection vector based on all available angles from the relection point. It can be swapped between freely.
vec3 random_in_hemisphere(const vec3& normal) {
    vec3 in_unit_sphere = random_in_unit_sphere();
    if (dot(in_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
        return in_unit_sphere;
    else
        return -in_unit_sphere;
}

// NOTE: This is the reflection function for materials such as metal, wherein the vector is reflected in a deterministic way (before adding fuzz).
vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2*dot(v,n)*n;
}

// NOTE: This function is for transparent objects wherein the light is refracted within the object.
vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat) {
    auto cos_theta = fmin(dot(-uv, n), 1.0);
    vec3 r_out_perp =  etai_over_etat * (uv + cos_theta*n);
    vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}

// NOTE: This is used for defocusing: we generate a disk centered at the lookfrom point from which our virtual picture is captured.
vec3 random_in_unit_disk() {
    while (true) {
        auto p = vec3(random_double_local(-1,1), random_double_local(-1,1), 0);
        if (p.length_squared() >= 1) continue;
        return p;
    }
}

#endif