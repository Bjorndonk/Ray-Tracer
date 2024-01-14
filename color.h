#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"

#include <iostream>

// NOTE: This function has been rewritten from its original conception to incorporate antialiasing; once each pixel has accumulated sample, a single division operator is performed to average the samples, and then is "clamped" using the clamp function to normalize the 256 color range into a 0 < x < 1 range, then multiplying it by the 256-unit range of color values.
void write_color(std::ostream &out, color pixel_color, int samples_per_pixel) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // NOTE: We're gamma correcting here, as image processors assume the 0-1 values have a transform being stored as a byte.
    // Divide the color by the number of samples and gamma-correct for gamma=2.0.
    auto scale = 1.0 / samples_per_pixel;
    r = sqrt(scale * r);
    g = sqrt(scale * g);
    b = sqrt(scale * b);

    // Write the translated [0,255] value of each color component.
    out << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
        << static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
        << static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
}

#endif