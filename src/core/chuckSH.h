#include <cmath>
#include <vector>
#include "SHCT.h"

#define MAX_ORDER 12

static const float degree2rad = E_PI / 180.0f;
static const float rad2deg = 180.0f / E_PI;

constexpr float rangeReduce(float val, float lo, float hi)
{
    float range = hi - lo;
    float result = std::fmodf(val - lo, range);
    if (result < 0)
    {
        result += range;
    }
    return result + lo;
}

NLOUP<MAX_ORDER> norms; // create LOUP

std::vector<float> SH(unsigned order_, const float azimuth_, const float zenith_, bool n3d) // SH calc
{
    float azimuth_shift = rangeReduce((azimuth_)*degree2rad, 0.f, 2.f * E_PI); // reduce to range of 0 < azi < 2pi & shift "perspective" so that azi = 0 and zeni = 0 is a unity vector facing outwards from the listener (vector pointing from roughly the nose forward)
    float zenith_shift = rangeReduce((90.f - zenith_) * degree2rad, 0.f, E_PI * 0.5f);
    float coszeni = cosf(zenith_shift);
    int size = (order_ + 1) * (order_ + 1);                    // pre-compute size of vector to be returned
    std::vector<float> result = std::vector<float>(size, 0.f); // instantiate vector that is the size of the results that shall be returned
    for (int order = 0; order <= (int)order_; order++)
    {
        if (order == 0)
            result[0] = norms.SN3D(order, 0);
        for (int degree = -order; degree <= order; degree++)
        {
            float n = n3d ? norms.N3D(order, degree) : norms.SN3D(order, degree);                          // normalization term if n3d bool = TRUE, return N3D else SN3D
            float p = (std::assoc_legendre((unsigned int)order, (unsigned int)abs(degree), coszeni));      // legendre NOTE: degree of legendre is current ambisonic order & order of legendre is current ambisonic degree (very frustrating)
            float r = (degree < 0) ? sinf(abs(degree) * (azimuth_shift)) : cosf(degree * (azimuth_shift)); // degree positive? Re(exp(i*azimuth*degree)) degree negative? Im(exp(i*azimuth*degree))
            result[(order * order) + order + degree] = n * p * r;
        }
    }
    return result;
}

void SH(unsigned order_, const float azimuth_, const float zenith_, std::vector<float> result, bool n3d) // SH calc
{
    float azimuth_shift = rangeReduce((azimuth_)*degree2rad, 0.f, 2.f * E_PI);         // reduce to range of 0 < azi < 2pi & shift "perspective" so that azi = 0 and zeni = 0 is a unity vector facing outwards from the listener (vector pointing from roughly the nose forward)
    float zenith_shift = rangeReduce((90.f - zenith_) * degree2rad, 0.f, E_PI * 0.5f); // reduce to range of 0 < zen < pi
    float coszeni = cosf(zenith_shift);                                                // pre calculate cos(zenith)
    int size = (order_ + 1) * (order_ + 1) + 1;                                        // pre-compute size of vector to be returned
    if (result.capacity() != size)
        result.reserve(size);                          // reserve space for vector
    for (int order = 0; order <= (int)order_; order++) // all orders from 0 - desired order
    {
        if (order == 0)
            result[0] = norms.SN3D(order, 0);                // Y^0_0 is omnidirectional
        for (int degree = -order; degree <= order; degree++) // all degrees of current order
        {
            float n = n3d ? norms.N3D(order, degree) : norms.SN3D(order, degree); // normalization term if n3d bool = TRUE, return N3D else SN3D
            float p = (std::assoc_legendref((unsigned int)abs(degree), (unsigned int)order, coszeni));
            float r = (degree <= 0) ? sinf(abs(degree) * (azimuth_shift)) : cosf(degree * (azimuth_shift)); // degree positive? Re(exp(i*azimuth*degree)) degree negative? Im(exp(i*azimuth*degree))
            result[(order * order) + order + degree] = n * p * r;                                           // place inside vector so it is ordered as Y^0_0, Y^1_-1, Y^1_0, Y^1_1
        }
    }
}
