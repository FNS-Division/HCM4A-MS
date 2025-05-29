//	This subroutine calculates the distance  between two points.
// SUBROUTINE Calc_distance (LonA, LatA, LonB, LatB, D)
	
#ifndef _CalcDistanceH
#define _CalcDistanceH

#include <cmath>


void CalcDistance(double LonA, double LatA, double LonB, double LatB, double& D) {
	const double M_PI = 3.14159265358979323846264;
    const double R = 6371.29; // Earth radius in kilometers
    double rg = M_PI / 180.0;


    double delta_lat = LatB - LatA;
    double delta_lon = LonB - LonA;

    // Haversine formula components
	double a = pow(sin(delta_lat / 2.0 * rg), 2) + 
               cos(LatA * rg) * cos(LatB * rg) * pow(sin(delta_lon / 2.0 * rg), 2);

    // Calculate central angle and distance
    D = 2.0 * R * atan2(sqrt(a), sqrt(1.0 - a));

}

#endif