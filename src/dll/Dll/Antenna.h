//!	Subroutine to calculate the gain (loss) of an antenna.
//!
//!
//!	Input values 
//!
//!	A_typ	Type of antenna (TR25-08)
//!	Angle	Angle, where loss is calculated [0..360 or -180..+180 degrees]
//!
//!	Output values
//!
//!	RHO		Loss [linear] (1.0 = no loss;  max. = 0.01)
//!	Error	Error-value (0 = no error; 1038 = error) 

#ifndef _AntennaH
#define _AntennaH


#include <cmath>
#include <iostream>
#include <string>


void Antenna(const std::string& A_typ, double Angle, double& RHO, int& Error) {
    std::string TPE = A_typ.substr(3, 2);
    RHO = 1.0;
    Error = 0;

    if (TPE == "ND") return;

    double Alpha = (Angle < 180.0) ? Angle : Angle - 360.0;

    double LEAD, TRAIL;
    try {
        LEAD = std::stod(A_typ.substr(0, 3));
        TRAIL = std::stod(A_typ.substr(5, 2)) / 100.0;
    } catch (...) {
        Error = 1038;
        return;
    }

    double COLEA = cos(LEAD * M_PI / 180.0);
    double TERM1 = 1.0 - pow(COLEA, 2.0);
    double COAL = cos(Alpha * M_PI / 180.0);

    if (TPE == "EA") {
        if (LEAD > 65.0 || LEAD == 0.0) {
            Error = 1038;
            return;
        } else {
            double TERM2 = 1.0 - pow(sqrt(2.0) * COLEA - 1.0, 2.0);
            double B = 0.5 * TERM1 / TERM2;
            RHO = 4.0 * B * COAL / ((4.0 * B - 1.0) * pow(COAL, 2.0) + 1.0);
        }
    } else if (TPE == "EB") {
        if (LEAD > 79.0 || LEAD == 0.0) {
            Error = 1038;
            return;
        } else {
            double TERM2 = 1.44 - pow(sqrt(2.0) * COLEA - 0.8, 2.0);
            double B = 0.72 * TERM1 / TERM2;
            double X = B * (B - 0.2) * pow(COAL, 2.0) + 0.2 * B;
            if (X < 0.0) X = 0.0;
            RHO = (1.6 * B * COAL + 2.4 * sqrt(X)) / ((4.0 * B - 1.44) * pow(COAL, 2.0) + 1.44);
        }
    } else if (TPE == "EC") {
        if (LEAD > 96.0 || LEAD == 0.0) {
            Error = 1038;
            return;
        } else {
            double TERM2 = 1.96 - pow(sqrt(2.0) * COLEA - 0.6, 2.0);
            double B = 0.98 * TERM1 / TERM2;
            double X = B * (B - 0.4) * pow(COAL, 2.0) + 0.4 * B;
            if (X < 0.0) X = 0.0;
            RHO = (1.2 * B * COAL + 2.8 * sqrt(X)) / ((4.0 * B - 1.96) * pow(COAL, 2.0) + 1.96);
        }
    } else if (TPE == "DE") {
        if (LEAD > 65.0 || LEAD == 0.0) {
            Error = 1038;
            return;
        } else {
            double TERM2 = 2.0 - pow(2.0 * COLEA - sqrt(2.0), 2.0);
            double B = TERM1 / TERM2;
            RHO = abs(4.0 * B * COAL / ((4.0 * B - 1.0) * pow(COAL, 2.0) + 1.0));
        }
    } else if (TPE == "LA") {
        if (LEAD > 120.0 || LEAD == 0.0) {
            Error = 1038;
            return;
        } else {
            double B = Alpha * M_PI / 180.0;
            if (abs(B) <= 3.0 * (LEAD * M_PI / 180.0) / 2.0) {
                double TERM2 = cos(M_PI / 3.0 * B / (LEAD * M_PI / 180.0));
                RHO = cos((1.0 - TERM2) * M_PI / 2.0);
            } else {
                RHO = TRAIL;
            }
        }
    } else if (TPE == "KA") {
        if (LEAD > 100.0) {
            Error = 1038;
            return;
        } else {
            double B = LEAD / 100.0;
            double X = pow(1.0 - B, 2.0) * pow(COAL, 2.0) + 4.0 * B;
            if (X < 0.0) X = 0.0;
            double TERM2 = sqrt(X);
            RHO = ((1.0 - B) * COAL + TERM2) / 2.0;
        }
    } else if (TPE == "CA") {
        if (LEAD > 100.0) {
            Error = 1038;
            return;
        } else {
            double B = LEAD / 100.0;
            double X = pow(1.0 - pow(B, 2.0), 2.0) * pow(cos(Alpha * M_PI / 90.0), 2.0) + 4.0 * pow(B, 2.0);
            if (X < 0.0) X = 0.0;
            double TERM2 = sqrt(X);
            X = ((1.0 - pow(B, 2.0)) * cos(Alpha * M_PI / 90.0) + TERM2) / 2.0;
            if (X < 0.0) X = 0.0;
            RHO = sqrt(X);
        }
    } else if (TPE == "CB") {
        if (LEAD > 100.0) {
            Error = 1038;
            return;
        } else {
            double B = LEAD / 100.0;
            double X = pow(1.0 - pow(B, 2.0), 2.0) * pow(cos(Alpha * M_PI / 60.0), 2.0) + 4.0 * pow(B, 2.0);
            if (X < 0.0) X = 0.0;
            double TERM2 = sqrt(X);
            X = ((1.0 - pow(B, 2.0)) * cos(Alpha * M_PI / 60.0) + TERM2) / 2.0;
            if (X < 0.0) X = 0.0;
            RHO = sqrt(X);
        }
    } else if (TPE == "CC") {
        if (LEAD > 100.0) {
            Error = 1038;
            return;
        } else {
            double B = LEAD / 100.0;
            double X = pow(1.0 - pow(B, 2.0), 2.0) * pow(cos(Alpha * M_PI / 45.0), 2.0) + 4.0 * pow(B, 2.0);
            if (X < 0.0) X = 0.0;
            double TERM2 = sqrt(X);
            X = ((1.0 - pow(B, 2.0)) * cos(Alpha * M_PI / 45.0) + TERM2) / 2.0;
            if (X < 0.0) X = 0.0;
            RHO = sqrt(X);
        }
    } else if (TPE == "TA" || TPE[0] == 'P') {
        if (LEAD < 1.0 || LEAD > 890.0) {
            Error = 1038;
            return;
        } else {
            if (COAL > 0.0) {
                double X = -0.1505 / log10(cos(LEAD * M_PI / 180.0 / 10.0));
                RHO = pow(COAL, X);
            } else {
                RHO = 0.01;
            }
        }
    } else if (A_typ[3] == 'V' || A_typ[3] == 'W') {
        int M, N;
        
        try{
			M = std::stoi(A_typ.substr(0, 1));
		} catch (...) {
            M = 0;
		}
        
        try {
            N = std::stoi(A_typ.substr(1, 2));
        } catch (...) {
            Error = 1038;
            return;
        }

        int A = M * 5 + 15;
        double E = 1.0;
        char E_char = A_typ[4];
        switch (E_char) {
            case 'A': E = 1E-5; break;
            case 'B': E = 0.05; break;
            case 'C': E = 0.10; break;
            case 'D': E = 0.15; break;
            case 'E': E = 0.20; break;
            case 'F': E = 0.25; break;
            case 'G': E = 0.30; break;
            case 'H': E = 0.35; break;
            case 'I': E = 0.40; break;
            default: Error = 1038; return;
        }

        double K5 = pow((1.0 + E) / 2.0, 2.0);
        double B = K5 / 2.0 * (1.0 - pow(cos(A * M_PI / 180.0), 2.0)) / (K5 - pow(cos(A * M_PI / 180.0) / sqrt(2.0) - (1.0 - E) / 2.0, 2.0));
        double K4 = B - K5;
        double K3 = B * E * K5;
        double K2 = pow(B, 2.0) * K5 - K3;
        double K1 = B * (1.0 - E) / 2.0;
        double R1 = (K1 * COAL + sqrt(K2 * pow(COAL, 2.0) + K3)) / (K4 * pow(COAL, 2.0) + K5);
        double COAX = cos((Alpha - 2.0 * N) * M_PI / 180.0);
        double R2 = (K1 * COAX + sqrt(K2 * pow(COAX, 2.0) + K3)) / (K4 * pow(COAX, 2.0) + K5);
        RHO = R1;
        if (RHO < R2) RHO = R2;

        if (A_typ[3] == 'W') {
            int R, P;
            try {
                R = std::stoi(A_typ.substr(5, 1));
                P = std::stoi(A_typ.substr(6, 1));
            } catch (...) {
                Error = 1038;
                return;
            }

            double R0 = R / 20.0;
            double P0 = P / 20.0 + 0.35;
            if (Alpha >= 0.0 && Alpha <= 2.0 * N) {
                if (RHO < P0) RHO = P0;
            } else {
                if (RHO < R0) RHO = R0;
            }
            TRAIL = 0.01;
        }
    } else {
        Error = 1038;
        return;
    }

    if (RHO < TRAIL) RHO = TRAIL;
    if (RHO < 0.01) RHO = 0.01;
    if (RHO > 1.0) RHO = 1.0;
}

#endif