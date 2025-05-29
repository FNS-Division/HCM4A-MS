//	This subroutine converts co-ordinates from numbers
//	(longitude, latitude) to a string.
//SUBROUTINE CooConv (Long, Lat, Coo)
	
#ifndef _CooConvH
#define _CooConvH

#include <string>
#include <sstream>
#include <cmath>
#include <iomanip>

// Function to convert coordinates from numbers (longitude, latitude) to a string
std::string CooConv(double longitude, double latitude) {
    std::string coo = "               "; // Initialize the string with 15 spaces
    double lo = longitude;
    double la = latitude;

    if (lo >= 0.0) {
        coo[3] = 'E';
    } else {
        coo[3] = 'W';
        lo = -lo;
    }

    if (la >= 0.0) {
        coo[10] = 'N';
    } else {
        coo[10] = 'S';
        la = -la;
    }

    int i = static_cast<int>(lo);
    lo = (lo - i) * 60.0;
    int j = static_cast<int>(lo);
    lo = (lo - j) * 60.0;
    int k = std::round(lo);
    if (k >= 60) {
        k -= 60;
        j += 1;
        if (j >= 60) {
            j -= 60;
            i += 1;
        }
    }

    // Write the longitude part
    std::ostringstream oss;
    oss << std::setw(3) << std::setfill('0') << i;
    coo.replace(0, 3, oss.str());
    oss.str("");
    oss.clear();
    oss << std::setw(2) << std::setfill('0') << j;
    coo.replace(4, 2, oss.str());
    oss.str("");
    oss.clear();
    oss << std::setw(2) << std::setfill('0') << k;
    coo.replace(6, 2, oss.str());

    i = static_cast<int>(la);
    la = (la - i) * 60.0;
    j = static_cast<int>(la);
    la = (la - j) * 60.0;
    k = std::round(la);
    if (k >= 60) {
        k -= 60;
        j += 1;
        if (j >= 60) {
            j -= 60;
            i += 1;
        }
    }

    // Write the latitude part
    oss.str("");
    oss.clear();
    oss << std::setw(2) << std::setfill('0') << i;
    coo.replace(8, 2, oss.str());
    oss.str("");
    oss.clear();
    oss << std::setw(2) << std::setfill('0') << j;
    coo.replace(11, 2, oss.str());
    oss.str("");
    oss.clear();
    oss << std::setw(2) << std::setfill('0') << k;
    coo.replace(13, 2, oss.str());

    return coo;
}

#endif