// SH normalization
#include <cmath>
#include <vector>
#define R_PI 3.1415926535897932

// lookup tables of normalization terms as -n <= m <= n including m = 0
double order_zero = 0.2820947918;
double order_first[3] = { 0.5641895835, 0.4886025119, 0.2820947918 };
double order_second[5] = { 1.954410048, 0.9772050238, 0.4886025119, 0.162867504, 0.081433752 };
double order_third[7] = { 10.7047447, 4.370193722, 1.381976598, 0.4886025119, 0.1151647165, 0.036418281, 0.014867701 };
double order_fourth[9] = { 80.10697414, 28.32209232, 7.569397566, 1.784124116, 0.4886025119, 0.0892062050, 0.0210261044, 0.0056194628, 0.0019867801 };

int factorial(int n)
{
    if (n > 1) return n * factorial(n - 1);
    else return 1;
}

float norm(const unsigned order, const int degree)
{
    float l_frac = (((2 * order) + 1) / (4.f * R_PI));
    int r_frac = (factorial(order - degree) / factorial(order + degree));
    return  sqrtf(l_frac / r_frac);;
}

float SH(const int degree, const unsigned order, float azimuth_, float zenith_)
{
    float azimuth = azimuth_ * R_PI / 180;
    float zenith = zenith_ * R_PI / 180;
    switch (order)
    {
    case 0 :
        return order_zero;
    case 1 :
        if (degree >= 0) return order_first[degree + order] * norm(order, degree)* std::assoc_legendref(abs(degree), order, cosf(zenith))* cosf(degree * azimuth);
        else if (degree < 0) return order_first[degree + order] * norm(order, degree)* std::assoc_legendref(abs(degree), order, cosf(zenith))* sinf(abs(degree) * azimuth);
        break;
    case 2 :
        if (degree >= 0) return order_second[degree + order] * norm(order, degree)* std::assoc_legendref(abs(degree), order, cosf(zenith))* cosf(degree * azimuth);
        else if (degree < 0) return order_second[degree + order] * norm(order, degree)* std::assoc_legendref(abs(degree), order, cosf(zenith))* sinf(abs(degree) * azimuth);
        break;
    case 3:
        if (degree >= 0) return order_third[degree + order] * norm(order, degree) * std::assoc_legendref(abs(degree), order, cosf(zenith)) * cosf(degree * azimuth);
        else if (degree < 0) return order_third[degree + order] * norm(order, degree) * std::assoc_legendref(abs(degree), order, cosf(zenith)) * sinf(abs(degree) * azimuth);
        break;
    case 4:
        if (degree >= 0) return order_fourth[degree + order] * norm(order, degree) * std::assoc_legendref(abs(degree), order, cosf(zenith)) * cosf(degree * azimuth);
        else if (degree < 0) return order_fourth[degree + order] * norm(order, degree) * std::assoc_legendref(abs(degree), order, cosf(zenith)) * sinf(abs(degree) * azimuth);
        break;
    default:
        break;
    }
    return 0;
}

std::vector<float> SH(const unsigned order, float azimuth, float zenith) // - n <= m <= n including m = 0
{
    std::vector<float> result;
    result.reserve((2 * order) + 1);
    switch (order)
    {
    case 0:
        result[0] = order_zero;
        return result;
    case 1:

        for (int degree = -order; degree <= (int)order; degree++)
        {
            if (degree >= 0) result[degree + order] = order_first[degree + order] * std::assoc_legendref(abs(degree), order, cosf(zenith)) * cosf(degree * azimuth);
            else if (degree < 0) result[degree + order] = order_first[degree + order] * std::assoc_legendref(abs(degree), order, sinf(zenith)) * sinf(abs(degree) * azimuth);
        }
        break;

    case 2:
        for (int degree = -order; degree <= (int)order; degree++)
        {
            if (degree >= 0) result[degree + order] = order_second[degree + order] * std::assoc_legendref(abs(degree), order, cosf(zenith)) * cosf(degree * azimuth);
            else if (degree < 0) result[degree + order] = order_second[degree + order] * std::assoc_legendref(abs(degree), order, cosf(zenith)) * sinf(abs(degree) * azimuth);
        }
        break;
    case 3:
        for (int degree = -order; degree <= (int)order; degree++)
        {
            if (degree >= 0) result[degree + order] = order_third[degree + order] * std::assoc_legendref(abs(degree), order, cosf(zenith)) * cosf(degree * azimuth);
            else if (degree < 0) result[degree + order] = order_third[degree + order] * std::assoc_legendref(abs(degree), order, cosf(zenith)) * sinf(abs(degree) * azimuth);
        }
        break;
    case 4:
        for (int degree = -order; degree <= (int)order; degree++)
        {
            if (degree >= 0) result[degree + order] = order_fourth[degree + order] * std::assoc_legendref(abs(degree), order, cosf(zenith)) * cosf(degree * azimuth);
            else if (degree < 0) result[degree + order] = order_fourth[degree + order] * std::assoc_legendref(abs(degree), order, cosf(zenith)) * sinf(abs(degree) * azimuth);
        }
        break;
    default:
        break;
    }
    return result;
}
