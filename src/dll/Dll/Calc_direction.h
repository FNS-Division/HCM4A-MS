//	Subroutine to calculate (horizontal) direction.

#ifndef _CalcDirectionH
#define _CalcDirectionH

#include <cmath>

void CalcDirection(double LonA, double LatA, double LonB, double LatB, double& Azi) {
	const double M_PI = 3.14159265358979323846264;
    double delta_lon = LonB - LonA;
    double rg = M_PI / 180.0;
    
    // Compute numerator (y) and denominator (x) for atan2
    double y = sin(delta_lon * rg) * cos(LatB * rg);
    double x = cos(LatA * rg) * sin(LatB * rg) - sin(LatA * rg) * cos(LatB * rg) * cos(delta_lon * rg);
    
    // Calculate azimuth in degrees and adjust to [0, 360)
    Azi = std::atan2(y, x) * 180.0 / M_PI;
    if (Azi < 0.0) Azi += 360.0;
}

#endif