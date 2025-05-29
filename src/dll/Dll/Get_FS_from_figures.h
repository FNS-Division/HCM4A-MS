//!	SUBROUTINE Get_FS_from_figures ( Land_FS_1kW, Sea_FS_1kW )
//!
//!
//!	Input values:							possible values
//!
//!			Figure_frequency	in MHz		100, 600 or 2000				INTEGER*4
//!			Time_percentage*	in %		1, 10 or 50						INTEGER*4
//!			Sea_temperature*				C, W or I (cold, warm or inter)	CHARACTER*1
//!			Figure_Heff			in m		10 to 1200 (see list)			REAL
//!			Figure_distance		in km		1 to 1000 (see list)			INTEGER*4
//!			aLatM*															REAL
//!			* in COMMON
//!
//!	Output values:
//!
//!			Land_figure_FS		in dBµV/m	(for 1 kW)						REAL
//!			Sea_figure_FS		in dBµV/m	(for 1 kW)						REAL
//!
//!	Error values 							   0 = no error					INTEGER*4				
//!											2000 = wrong Figure_frequency	
//!											2001 = wrong Time_percentage
//!											2002 = wrong Sea_temperature
//!											2003 = wrong Figure_Heff
//!											2004 = wrong Figure_distance
//!
//!	Eff. antenna heights:
//!	10, 20, 37.5, 75, 150, 300, 600, 1200 m and	the maximum field strength (for 1kW).
//!
//!	Distances in km:
//!	1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
//!	16, 17, 18, 19, 20, 25, 30, 35, 40, 45, 50, 55, 60,
//!	65, 70, 75, 80, 85, 90, 95, 100, 110, 120, 130, 140,
//!	150, 160, 170, 180, 190, 200, 225, 250, 275, 300,
//!	325, 350, 375, 400, 425, 450, 475, 500, 525, 550,
//!	575, 600, 625, 650, 675, 700, 725, 750, 775, 800,
//!	825, 850, 875, 900, 925, 950, 975, 1000

#ifndef Get_FS_from_figuresH
#define Get_FS_from_figuresH

#include <cmath>
#include <array>

#include "Get_figure_FS_value.h"


struct FS_figures {
    // Inputs
    double Tx_frequency; // MHz
    double Distance;     // km
    double Heff;          // m
    int Time_percentage; // %
    double LatM = 0; // Middle Latitide of path
    char Sea_temperature = 'W'; // Sea_temperature
    
    

    // Outputs
    int HCM_error = 0;
    double Land_FS_1kW;
    double Sea_FS_1kW;

    // Constants
    static constexpr std::array<int, 78> Distances = {
        1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
        25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 100, 110,
        120, 130, 140, 150, 160, 170, 180, 190, 200, 225, 250, 275, 300, 325,
        350, 375, 400, 425, 450, 475, 500, 525, 550, 575, 600, 625, 650, 675,
        700, 725, 750, 775, 800, 825, 850, 875, 900, 925, 950, 975, 1000
    };

    static constexpr std::array<double, 8> Heff_values = {
        10.0f, 20.0f, 37.5f, 75.0f, 150.0f, 300.0f, 600.0f, 1200.0f
    };

    void Get_FS_from_figures();
};

void FS_figures::Get_FS_from_figures() {
    // Error checks (as per original comments)
    if (Distance > 1000.0) {
        HCM_error = 1028;
        return;
    }
    if (Distance <= 0.0) {
        HCM_error = 1000;
        return;
    }

    const double E_free_1kW = 107.0f - 20.0f * std::log10(Distance);

    // Handle distances <1 km
    if (Distance < 1.0) {
        Land_FS_1kW = E_free_1kW;
        Sea_FS_1kW = E_free_1kW;
        return;
    }

    // Find distance interval
    int d_inf = Distances[0];
    int d_sup = Distances[1];
    for (int i = 1; i < Distances.size(); i++) {
        if (Distance <= Distances[i]) {
            d_inf = Distances[i - 1];
            d_sup = Distances[i];
            break;
        }
    }
        
    // Determine frequency interval
    int f_inf, f_sup;
    if (Tx_frequency <= 600.0)
        { f_inf = 100; f_sup = 600; }
    else
        { f_inf = 600; f_sup = 2000; }

    // Clamp Heff to 0-3000
    if (Heff < 0.0f) Heff = 0.0f;
    if (Heff > 3000.0f) Heff = 3000.0f;

    // Find Heff interval
    double h_inf, h_sup;
    if (Heff < 10.0f) {
        h_inf = 10.0f;
        h_sup = 10.0f;
    } else {
        h_inf = Heff_values[0];
        h_sup = Heff_values[1];
        for (int i = 1; i < Heff_values.size(); i++) {
            if (Heff <= Heff_values[i]) {
                h_inf = Heff_values[i - 1];
                h_sup = Heff_values[i];
                break;
            }
        }
    }

    if (Heff >= 10.0f) {
        // Trilinear interpolation case
        double L_Eiii, L_Eiis, L_Eisi, L_Eiss, L_Esii, L_Esis, L_Essi, L_Esss;
        double S_Eiii, S_Eiis, S_Eisi, S_Eiss, S_Esii, S_Esis, S_Essi, S_Esss;

        Get_figure_FS_value(f_inf, h_inf, d_inf, Sea_temperature, Time_percentage, LatM, L_Eiii, S_Eiii);
        Get_figure_FS_value(f_inf, h_inf, d_sup, Sea_temperature, Time_percentage, LatM, L_Eiis, S_Eiis);
        Get_figure_FS_value(f_inf, h_sup, d_inf, Sea_temperature, Time_percentage, LatM, L_Eisi, S_Eisi);
        Get_figure_FS_value(f_inf, h_sup, d_sup, Sea_temperature, Time_percentage, LatM, L_Eiss, S_Eiss);
        Get_figure_FS_value(f_sup, h_inf, d_inf, Sea_temperature, Time_percentage, LatM, L_Esii, S_Esii);
        Get_figure_FS_value(f_sup, h_inf, d_sup, Sea_temperature, Time_percentage, LatM, L_Esis, S_Esis);
        Get_figure_FS_value(f_sup, h_sup, d_inf, Sea_temperature, Time_percentage, LatM, L_Essi, S_Essi);
        Get_figure_FS_value(f_sup, h_sup, d_sup, Sea_temperature, Time_percentage, LatM, L_Esss, S_Esss);
        if (HCM_error != 0) return;

        // Height interpolation
        double h_ratio = std::log10(Heff / h_inf) / std::log10(h_sup / h_inf);
        double L_E_ii = L_Eiii + (L_Eisi - L_Eiii) * h_ratio;
        double S_E_ii = S_Eiii + (S_Eisi - S_Eiii) * h_ratio;
        double L_E_is = L_Eiis + (L_Eiss - L_Eiis) * h_ratio;
        double S_E_is = S_Eiis + (S_Eiss - S_Eiis) * h_ratio;
        double L_E_si = L_Esii + (L_Essi - L_Esii) * h_ratio;
        double S_E_si = S_Esii + (S_Essi - S_Esii) * h_ratio;
        double L_E_ss = L_Esis + (L_Esss - L_Esis) * h_ratio;
        double S_E_ss = S_Esis + (S_Esss - S_Esis) * h_ratio;

        // Distance interpolation
        double L_E__i, L_E__s, S_E__i, S_E__s;
        if (d_sup == d_inf) {
            L_E__i = L_E_ii;
            S_E__i = S_E_ii;
            L_E__s = L_E_si;
            S_E__s = S_E_si;
        } else {
            double d_ratio = std::log10(Distance / d_inf) / std::log10(static_cast<double>(d_sup) / d_inf);
            L_E__i = L_E_ii + (L_E_is - L_E_ii) * d_ratio;
            S_E__i = S_E_ii + (S_E_is - S_E_ii) * d_ratio;
            L_E__s = L_E_si + (L_E_ss - L_E_si) * d_ratio;
            S_E__s = S_E_si + (S_E_ss - S_E_si) * d_ratio;
        }

        // Frequency interpolation
        if (f_sup == f_inf) {
            Land_FS_1kW = L_E__i;
            Sea_FS_1kW = S_E__i;
        } else {
            double f_ratio = std::log10(Tx_frequency / f_inf) / std::log10(static_cast<double>(f_sup) / f_inf);
            Land_FS_1kW = L_E__i + (L_E__s - L_E__i) * f_ratio;
            Sea_FS_1kW = S_E__i + (S_E__s - S_E__i) * f_ratio;
        }
    } else {
        // Heff <10m case
        const double dh1 = 4.1f * std::sqrt(Heff);
        const double dh10 = 12.96534f; // 4.1*sqrt(10)

        if (Distance < dh1) {
            // Distance < dh1 case
            double L_E_ii, L_E_is, L_E_si, L_E_ss;
            double S_E_ii, S_E_is, S_E_si, S_E_ss;

            // Calculate E10d
            Get_figure_FS_value(f_inf, 10.0f, d_inf, Sea_temperature, Time_percentage, LatM, L_E_ii, S_E_ii);
            Get_figure_FS_value(f_inf, 10.0f, d_sup, Sea_temperature, Time_percentage, LatM, L_E_is, S_E_is);
            Get_figure_FS_value(f_sup, 10.0f, d_inf, Sea_temperature, Time_percentage, LatM, L_E_si, S_E_si);
            Get_figure_FS_value(f_sup, 10.0f, d_sup, Sea_temperature, Time_percentage, LatM, L_E_ss, S_E_ss);
            if (HCM_error != 0) return;

            double L_E10d_i, L_E10d_s, S_E10d_i, S_E10d_s;
            if (d_sup == d_inf) {
                L_E10d_i = L_E_ii;
                S_E10d_i = S_E_ii;
                L_E10d_s = L_E_si;
                S_E10d_s = S_E_si;
            } else {
                double d_ratio = std::log10(Distance / d_inf) / std::log10(static_cast<double>(d_sup) / d_inf);
                L_E10d_i = L_E_ii + (L_E_is - L_E_ii) * d_ratio;
                S_E10d_i = S_E_ii + (S_E_is - S_E_ii) * d_ratio;
                L_E10d_s = L_E_si + (L_E_ss - L_E_si) * d_ratio;
                S_E10d_s = S_E_si + (S_E_ss - S_E_si) * d_ratio;
            }

            double L_E10d, S_E10d;
            if (f_sup == f_inf) {
                L_E10d = L_E10d_i;
                S_E10d = S_E10d_i;
            } else {
                double f_ratio = std::log10(Tx_frequency / f_inf) / std::log10(static_cast<double>(f_sup) / f_inf);
                L_E10d = L_E10d_i + (L_E10d_s - L_E10d_i) * f_ratio;
                S_E10d = S_E10d_i + (S_E10d_s - S_E10d_i) * f_ratio;
            }

            // Calculate E10dh10
            int dh10_d_inf = 12, dh10_d_sup = 13;
            Get_figure_FS_value(f_inf, 10.0f, dh10_d_inf, Sea_temperature, Time_percentage, LatM, L_E_ii, S_E_ii);
            Get_figure_FS_value(f_inf, 10.0f, dh10_d_sup, Sea_temperature, Time_percentage, LatM, L_E_is, S_E_is);
            Get_figure_FS_value(f_sup, 10.0f, dh10_d_inf, Sea_temperature, Time_percentage, LatM, L_E_si, S_E_si);
            Get_figure_FS_value(f_sup, 10.0f, dh10_d_sup, Sea_temperature, Time_percentage, LatM, L_E_ss, S_E_ss);
            if (HCM_error != 0) return;

            double L_E10dh10_i, L_E10dh10_s, S_E10dh10_i, S_E10dh10_s;
            if (dh10_d_sup == dh10_d_inf) {
                L_E10dh10_i = L_E_ii;
                S_E10dh10_i = S_E_ii;
                L_E10dh10_s = L_E_si;
                S_E10dh10_s = S_E_si;
            } else {
                double d_ratio = std::log10(dh10 / dh10_d_inf) / std::log10(static_cast<double>(dh10_d_sup) / dh10_d_inf);
                L_E10dh10_i = L_E_ii + (L_E_is - L_E_ii) * d_ratio;
                S_E10dh10_i = S_E_ii + (S_E_is - S_E_ii) * d_ratio;
                L_E10dh10_s = L_E_si + (L_E_ss - L_E_si) * d_ratio;
                S_E10dh10_s = S_E_si + (S_E_ss - S_E_si) * d_ratio;
            }

            double L_E10dh10, S_E10dh10;
            if (f_sup == f_inf) {
                L_E10dh10 = L_E10dh10_i;
                S_E10dh10 = S_E10dh10_i;
            } else {
                double f_ratio = std::log10(Tx_frequency / f_inf) / std::log10(static_cast<double>(f_sup) / f_inf);
                L_E10dh10 = L_E10dh10_i + (L_E10dh10_s - L_E10dh10_i) * f_ratio;
                S_E10dh10 = S_E10dh10_i + (S_E10dh10_s - S_E10dh10_i) * f_ratio;
            }

            // Calculate E10dh1
            double L_E10dh1, S_E10dh1;
            if (dh1 < 1.0f) {
                L_E10dh1 = E_free_1kW;
                S_E10dh1 = E_free_1kW;
            } else {
                // Find dh1 interval
                int dh1_d_inf = Distances[0], dh1_d_sup = Distances[1];
                for (int i = 0; i < 77; ++i) {
                    if (dh1 >= Distances[i]) {
                        dh1_d_inf = Distances[i];
                        dh1_d_sup = Distances[i + 1];
                    }
                }

                Get_figure_FS_value(f_inf, 10.0f, dh1_d_inf, Sea_temperature, Time_percentage, LatM, L_E_ii, S_E_ii);
                Get_figure_FS_value(f_inf, 10.0f, dh1_d_sup, Sea_temperature, Time_percentage, LatM, L_E_is, S_E_is);
                Get_figure_FS_value(f_sup, 10.0f, dh1_d_inf, Sea_temperature, Time_percentage, LatM, L_E_si, S_E_si);
                Get_figure_FS_value(f_sup, 10.0f, dh1_d_sup, Sea_temperature, Time_percentage, LatM, L_E_ss, S_E_ss);
                if (HCM_error != 0) return;

                double L_E10dh1_i, L_E10dh1_s, S_E10dh1_i, S_E10dh1_s;
                if (dh1_d_sup == dh1_d_inf) {
                    L_E10dh1_i = L_E_ii;
                    S_E10dh1_i = S_E_ii;
                    L_E10dh1_s = L_E_si;
                    S_E10dh1_s = S_E_si;
                } else {
                    double d_ratio = std::log10(dh1 / dh1_d_inf) / std::log10(static_cast<double>(dh1_d_sup) / dh1_d_inf);
                    L_E10dh1_i = L_E_ii + (L_E_is - L_E_ii) * d_ratio;
                    S_E10dh1_i = S_E_ii + (S_E_is - S_E_ii) * d_ratio;
                    L_E10dh1_s = L_E_si + (L_E_ss - L_E_si) * d_ratio;
                    S_E10dh1_s = S_E_si + (S_E_ss - S_E_si) * d_ratio;
                }

                if (f_sup == f_inf) {
                    L_E10dh1 = L_E10dh1_i;
                    S_E10dh1 = S_E10dh1_i;
                } else {
                    double f_ratio = std::log10(Tx_frequency / f_inf) / std::log10(static_cast<double>(f_sup) / f_inf);
                    L_E10dh1 = L_E10dh1_i + (L_E10dh1_s - L_E10dh1_i) * f_ratio;
                    S_E10dh1 = S_E10dh1_i + (S_E10dh1_s - S_E10dh1_i) * f_ratio;
                }
            }

            Land_FS_1kW = L_E10dh10 + L_E10d - L_E10dh1;
            Sea_FS_1kW = S_E10dh10 + S_E10d - S_E10dh1;
        } else {
            // Distance >= dh1 case
            const double dhx = dh10 + Distance - dh1;
            if (dhx < 1.0f) {
                Land_FS_1kW = E_free_1kW;
                Sea_FS_1kW = E_free_1kW;
            } else {
                // Find dhx interval
                int dhx_d_inf = Distances[0], dhx_d_sup = Distances[1];
                for (int i = 0; i < 77; ++i) {
                    if (dhx >= Distances[i]) {
                        dhx_d_inf = Distances[i];
                        dhx_d_sup = Distances[i + 1];
                    }
                }

                double L_E_ii, L_E_is, L_E_si, L_E_ss;
                double S_E_ii, S_E_is, S_E_si, S_E_ss;
                Get_figure_FS_value(f_inf, 10.0f, dhx_d_inf, Sea_temperature, Time_percentage, LatM, L_E_ii, S_E_ii);
                Get_figure_FS_value(f_inf, 10.0f, dhx_d_sup, Sea_temperature, Time_percentage, LatM, L_E_is, S_E_is);
                Get_figure_FS_value(f_sup, 10.0f, dhx_d_inf, Sea_temperature, Time_percentage, LatM, L_E_si, S_E_si);
                Get_figure_FS_value(f_sup, 10.0f, dhx_d_sup, Sea_temperature, Time_percentage, LatM, L_E_ss, S_E_ss);
                if (HCM_error != 0) return;

                double L_E10dhx_i, L_E10dhx_s, S_E10dhx_i, S_E10dhx_s;
                if (dhx_d_sup == dhx_d_inf) {
                    L_E10dhx_i = L_E_ii;
                    S_E10dhx_i = S_E_ii;
                    L_E10dhx_s = L_E_si;
                    S_E10dhx_s = S_E_si;
                } else {
                    double d_ratio = std::log10(dhx / dhx_d_inf) / std::log10(static_cast<double>(dhx_d_sup) / dhx_d_inf);
                    L_E10dhx_i = L_E_ii + (L_E_is - L_E_ii) * d_ratio;
                    S_E10dhx_i = S_E_ii + (S_E_is - S_E_ii) * d_ratio;
                    L_E10dhx_s = L_E_si + (L_E_ss - L_E_si) * d_ratio;
                    S_E10dhx_s = S_E_si + (S_E_ss - S_E_si) * d_ratio;
                }

                if (f_sup == f_inf) {
                    Land_FS_1kW = L_E10dhx_i;
                    Sea_FS_1kW = S_E10dhx_i;
                } else {
                    double f_ratio = std::log10(Tx_frequency / f_inf) / std::log10(static_cast<double>(f_sup) / f_inf);
                    Land_FS_1kW = L_E10dhx_i + (L_E10dhx_s - L_E10dhx_i) * f_ratio;
                    Sea_FS_1kW = S_E10dhx_i + (S_E10dhx_s - S_E10dhx_i) * f_ratio;
                }
            }
        }
    }
}

#endif