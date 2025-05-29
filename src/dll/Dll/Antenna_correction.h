//!
//!	Antenna_correction.f90			
//!	This file is part of HCM.
//!
//	SUBROUTINE Ctransf (azi,aziM,ele,eleM,hda,vda)
//!
//!	Subroutine to calculate H_diff_angle, V_diff_angle
//!	as spherical coordinate transformation 
//!	all distances and angles are in radians and normalized to the sphere
//!
//!	azi  ... azi from Tx to Rx (  0..360 in degrees)
//!	aziM ... azi of Tx antenna (  0..360 in degrees)
//!	ele  ... ele from Tx to Rx (-90.. 90 in degrees)
//!	eleM ... ele of Tx antenna (-90.. 90 in degrees)
//!	hda  ... resulting horiz angle Tx antenna to Rx (-180..180 in degrees)
//!	vda	 ... resulting vert angle TX antenna to Rx (-90..+90 in degrees)


#ifndef _Antenna_correctionH
#define _Antenna_correctionH


#include <cmath>
#include <string>
#include <algorithm> // For std::max
#include "Antenna.h"

// Helper functions for degree-based trigonometric operations
double sind(double deg) {
    return sin(deg * M_PI / 180.0);
}

double cosd(double deg) {
    return cos(deg * M_PI / 180.0);
}

double datan2d(double y, double x) {
    return atan2(y, x) * 180.0 / M_PI;
}

double asind(double x) {
    return asin(x) * 180.0 / M_PI;
}



void Ctransf(double azi, double aziM, double ele, double eleM, double& hda, double& vda) {
    double a = pow(sind((ele - eleM) / 2.0), 2.0) + cosd(eleM) * cosd(ele) * pow(sind((azi - aziM) / 2.0), 2.0);
    double d;

    if (a >= 1.0) {
        d = 180.0;
    } else {
        d = 2.0 * datan2d(sqrt(a), sqrt(1.0 - a));
    }

    double b = datan2d(sind(azi - aziM) * cosd(ele),
                       cosd(eleM) * sind(ele) - sind(eleM) * cosd(ele) * cosd(azi - aziM));

    vda = static_cast<double>(asind(sind(d) * cosd(b)));
    hda = static_cast<double>(datan2d(sind(d) * sind(b), cosd(d)));
}

void Antenna_correction(double azi, double aziM, double ele, double eleM,
                        const std::string& hCod, const std::string& vCod,
                        double& hda, double& vda, double& a, int& Error) {
    Error = 0;
    a = 1.0;

    if (vCod[3] == 'T') { // Mechanical tilt
        hda = static_cast<double>(azi - aziM);
        vda = static_cast<double>(ele - eleM);
        double hb, vb;

        Antenna(hCod, hda, hb, Error);
        Antenna(vCod, vda, vb, Error);
        if (Error != 0) return;

        double w = (1.0 + cosd(2.0 * static_cast<double>(ele))) / 2.0;
        a = (hb * w + (1.0 - w)) * vb;

    } else if (vCod[3] == 'P') { // Electrical tilt
        Ctransf(azi, aziM, ele, eleM, hda, vda);
        double eleE = 65.0 - static_cast<double>(vCod[4]);
        double dEle = vda - eleE;
        double hb, vb, w2;

        Antenna(hCod, hda, hb, Error);
        Antenna(vCod, dEle, vb, Error);
        if (Error != 0) return;

        double k = (90.0 + eleE) / 90.0;
        double w1 = (dEle > 0.0) ? dEle * k : dEle / k;
        double w = pow((1.0 + cosd(2.0 * w1)) / 2.0, 6.0);

        Antenna(vCod, 180.0, w2, Error);
        a = std::max(hb * vb, (hb * w + (1.0 - w)) * w2);

    } else { // Normal case
        Ctransf(azi, aziM, ele, eleM, hda, vda);
        double vfe = 1.0, vbe = 1.0, vb = 1.0;
        double h = 1.0, hb = 1.0;

        if (!(vda == 0.0 || vCod.substr(3, 2) == "ND")) {
            Antenna(vCod, -vda, vfe, Error);
            Antenna(vCod, 180.0 + vda, vbe, Error);
            Antenna(vCod, 180.0, vb, Error);
            if (Error != 0) return;
        }

        if (!(hda == 0.0 || hCod.substr(3, 2) == "ND")) {
            Antenna(hCod, hda, h, Error);
            Antenna(hCod, 180.0, hb, Error);
            if (Error != 0) return;
        }

        // Backlobe matching
        if (vb > hb) {
            double k = hb / vb;
            vbe *= sqrt(pow(sind(vda), 2.0) + pow(k * cosd(vda), 2.0));
            vbe = std::max(vbe, 0.01);
        } else if (vb < hb) {
            double delv = vfe - vbe;
            if (delv > 0.0) {
                double k = (hb - vb) / (1.0 - vb);
                vbe += k * delv;
                vbe = std::min(vbe, 1.0);
            }
        }

        // Weighting factors
        double w;
        if (hb == 1.0) {
            w = std::abs(hda) / 180.0;
        } else if (hb < 0.9f) {
            w = (1.0 - h) / (1.0 - hb);
            w = std::min(w, 1.0);
        } else {
            double w1 = std::abs(hda) / 180.0;
            double w2 = (1.0 - h) / (1.0 - hb);
            w2 = std::min(w2, 1.0);
            double k = std::min((1.0 - hb) * 10.0, 1.0);
            w = (1.0 - k) * w1 + k * w2;
        }

        // Interpolation
        double vae = w * vbe + (1.0 - w) * vfe;
        vae = std::clamp(vae, 0.01, 1.0);
        double va0 = w * hb + (1.0 - w);
        va0 = std::clamp(va0, 0.01, 1.0);

        double ra = h / va0;
        a = vae * sqrt(pow(sind(vda), 2.0) + pow(ra * cosd(vda), 2.0));
    }

    // Final clamping and conversion to dB
    a = std::clamp(a, 0.01, 1.0);
    a = -20.0 * log10(a);
}



#endif