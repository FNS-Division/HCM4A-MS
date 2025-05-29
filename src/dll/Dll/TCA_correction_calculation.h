//!	Subroutine to calculate the Terrain Clearance Angle (TCA) correction.
//!
//!
//!	Input values:
//!				TCA			Terran Clearance Angle in degrees
//!				Frequency	Transmitter frequency in MHz
//!
//!
//!	Output values:
//!				TCA_corr	Correction according TCA in dB
//!
//!
//!
//	SUBROUTINE TCA_correction_calculation (TCAI, Frequency, TCAI_Corr)


#ifndef _TCACorrH
#define _TCACorrH

#include <cmath>

double TCACorrectionCalculation(double TCAI, double Frequency, double Distance) {
    int f_inf, f_sup;
    double TCA_c_100, TCA_c_600, TCA_c_2000;
    double v100, v600, v2000;
    double TCAI_corr;

    // Calculate v values
    v100 = 0.649 * TCAI;
    v600 = 1.592 * TCAI;
    v2000 = 2.915 * TCAI;

    // Compute TCA corrections
    TCA_c_100 = 9.1 - (6.9 + 20.0 * std::log10(std::sqrt(std::pow(v100 - 0.1, 2) + 1.0) + (v100 - 0.1)));
    TCA_c_600 = 13.1 - (6.9 + 20.0 * std::log10(std::sqrt(std::pow(v600 - 0.1, 2) + 1.0) + (v600 - 0.1)));
    TCA_c_2000 = 17.3 - (6.9 + 20.0 * std::log10(std::sqrt(std::pow(v2000 - 0.1, 2) + 1.0) + (v2000 - 0.1)));

    // Apply limits to corrections
    if (TCA_c_100 >= 0.0) TCA_c_100 = 0.0;
    if (TCA_c_100 < -32.0) TCA_c_100 = -32.0;

    if (TCA_c_600 >= 0.0) TCA_c_600 = 0.0;
    if (TCA_c_600 < -35.0) TCA_c_600 = -35.0;

    if (TCA_c_2000 >= 0.0) TCA_c_2000 = 0.0;
    if (TCA_c_2000 < -36.0) TCA_c_2000 = -36.0;

    // Determine frequency range
    if (Frequency <= 600.0) {
        f_inf = 100;
        f_sup = 600;
    } else {
        f_inf = 600;
        f_sup = 2000;
    }

    // Interpolate correction based on frequency
    if (f_inf == 100 && f_sup == 600) {
        TCAI_corr = TCA_c_100 + (TCA_c_600 - TCA_c_100) * std::log10(Frequency / 100.0) * std::log10(600.0 / 100.0);
    } else if (f_inf == 600 && f_sup == 2000) {
        TCAI_corr = TCA_c_600 + (TCA_c_2000 - TCA_c_600) * std::log10(Frequency / 600.0) * std::log10(2000.0 / 600.0);
    } else {
        TCAI_corr = 0.0; // Default case, adjust if needed
    }

    // Apply distance correction if necessary
    if (Distance < 16.0) {
        TCAI_corr *= Distance / 16.0;
    }

    return TCAI_corr;
}
#endif