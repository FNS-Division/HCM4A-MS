#ifndef _PtoP_H
#define _PtoP_H

#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <algorithm>

#include "HCM4AMobileServiceStruct.h"
#include "Position_of_mobile.h"
#include "CooConv.h"
#include "Antenna_correction.h"
#include "TCA_correction_calculation.h"
#include "Dh_calculation.h"
#include "Dh_correction.h"
#include "Get_FS_from_figures.h"
#include "HcmTerrain.h"

const std::string WHITESPACE = " \n\r\t\f\v";
 

struct PointInfoResult {
    int16_t Height = -9999;
    int16_t M_Type = 0;
    int32_t Error = 0;
};


std::string ltrim(const std::string &s)
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}
 
std::string rtrim(const std::string &s)
{
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}
 
std::string trim(const std::string &s) {
    return rtrim(ltrim(s));
}

void P_to_P_calculation(double longTx, double latTx, double longRx, double latRx, int TimePerc,
						double freqTx,double freqRx, bool p2p, 
						double &hTx, double &hRx, HCM4AMobileService_Parameters& param, HcmTerrain& ter) {
	double newLongTx = longTx;
	double newLatTx = latTx;
	double newLongRx = longRx;
	double newLatRx = latRx;
	bool with_morpho = true;
	double insidePowerToRx;

	int i1, i2, hsum;
	double xTCA;
	std::vector<double> hdz(10002), hdc(10002), hdf(10002);
	std::vector<double> a = {0.0, 0.07, 0.13, 0.2, 0.29, 0.37, 0.46, 0.56, 0.67, 0.8, 1.0};

	double hAntTx, hAntRx, Tx_serv_area, Rx_serv_area,
			txElevation, txAzimuth = 0.0;

	// Clear param.HCM_error and all relevant Info for P_to_P_calculation subroutine
	param.HCM_error = 0;
	for (int i=0; i < param.Info.size(); i++) param.Info[i] = false;
	
	
	try {
		Tx_serv_area = std::stod(param.Rad_of_Tx_serv_area);
	} catch (const std::invalid_argument&) {
		param.HCM_error = 1012;
		return;
	}
	if (p2p){
		try {
			Rx_serv_area = std::stod(param.Rad_of_Rx_serv_area);
		} catch (const std::invalid_argument&) {
			param.HCM_error = 1029;
			return;
		}
	} else { Rx_serv_area = 0;}

	//	Calculate new positions, if Tx or Rx or both are mobiles and not lines or '99':
	//	for lines new positions are calculated in CBR_Coordinates !
	if ((p2p) and (Tx_serv_area > 0 || Rx_serv_area > 0)){
		PositionOfMobile(longTx, latTx, longRx, latRx, newLongTx, newLatTx, newLongRx, newLatRx,
						Tx_serv_area, Rx_serv_area, true, param);
	}  	
	
					
	param.Coo_Tx_new = CooConv(newLongTx, newLatTx);
	param.Coo_Rx_new = CooConv(newLongRx, newLatRx);

	// Convert coordinates
	//008E111753N0716
	newLongTx = std::stod(param.Coo_Tx_new.substr(0, 3)) + 
				std::stod(param.Coo_Tx_new.substr(4, 2)) / 60.0 + 
				std::stod(param.Coo_Tx_new.substr(6, 2)) / 3600.0;
	if (param.Coo_Tx_new[3] == 'W') newLongTx = -newLongTx;
	newLatTx = std::stod(param.Coo_Tx_new.substr(8, 2)) + 
				std::stod(param.Coo_Tx_new.substr(11, 2)) / 60.0 + 
				std::stod(param.Coo_Tx_new.substr(13, 2)) / 3600.0;
	if (param.Coo_Tx_new[10] == 'S') newLatTx = -newLatTx;
	newLongRx = std::stod(param.Coo_Rx_new.substr(0, 3)) + 
				std::stod(param.Coo_Rx_new.substr(4, 2)) / 60.0 + 
				std::stod(param.Coo_Rx_new.substr(6, 2)) / 3600.0;
	if (param.Coo_Rx_new[3] == 'W') newLongRx = -newLongRx;
	newLatRx = std::stod(param.Coo_Rx_new.substr(8, 2)) + 
				std::stod(param.Coo_Rx_new.substr(11, 2)) / 60.0 + 
				std::stod(param.Coo_Rx_new.substr(13, 2)) / 3600.0;
	if (param.Coo_Rx_new[10] == 'S') newLatRx = -newLatRx;
	
	param.Coo_Tx_new = std::to_string(newLongTx).substr(0, 6) + ";" + std::to_string(newLatTx).substr(0, 6);
	param.Coo_Rx_new = std::to_string(newLongRx).substr(0, 6) + ";" + std::to_string(newLatRx).substr(0, 6);

	// Calculate distance and directions again with new coordinates
	double tmpD = 0;
	CalcDistance(newLongTx, newLatTx, newLongRx, newLatRx, tmpD);
	param.Distance = tmpD;
	
	if (param.Distance < param.PD) {
		param.Info[7] = true;
		param.HCM_error = 1000;
		return;
	}
	if (param.Distance > 1000.0) {
		param.Calculated_FS = -999.9;
		param.HCM_error = 1028;
		return;
	}

	// Calculate the direction from Rx to Tx
	CalcDirection(newLongRx, newLatRx, newLongTx, newLatTx, param.Dir_Rx_Tx);

	// Calculate the direction from Tx to Rx
	CalcDirection(newLongTx, newLatTx, newLongRx, newLatRx, param.Dir_Tx_Rx);

	// Checking Rx site height
	PointInfoResult newRxH;// = Point_info(newLongRx, newLatRx, param.Topo_path, param.Morpho_path);
	newRxH.Error = 0;
	newRxH.Height = std::trunc(ter.GetHeigth(newLongRx, newLatRx));
	
	if (param.HCM_error != 0) return;
	param.H_Datab_Rx = newRxH.Height;

	//  p2p indicating point-to-point calculation
	if (p2p) {
		if (trim(param.H_Rx_input).length()==0 || Rx_serv_area > 0.0) {
			hRx = newRxH.Height;
			param.Info[8] = true;
		} else {
			try {
				hRx = std::stoi(trim(param.H_Rx_input));
			} catch (...) {
				param.HCM_error = 1030;
				return;
			}
			if (hRx != newRxH.Height) {
				if (std::abs(hRx - newRxH.Height) <= hRx / 10) {
					param.Info[9] = true;
				} else {
					param.Info[10] = true;
				}
			}
		}
	} else {
		hRx = newRxH.Height;
		param.Info[8] = true;
	}



	// Checking Tx site height
	//PointInfoResult newTxH = Point_info(newLongTx, newLatTx, param.Topo_path, param.Morpho_path);
	PointInfoResult newTxH;
	newTxH.Error = 0;
	newTxH.Height = std::trunc(ter.GetHeigth(newLongTx, newLatTx));
	
	if (param.HCM_error != 0) return;
	param.H_Datab_Tx = newTxH.Height;

	if (trim(param.H_Tx_input).length()==0 || Tx_serv_area > 0.0) {
		hTx = newTxH.Height;
		param.Info[1] = true;
	} else {
		try {
			hTx = std::stoi( param.H_Tx_input);
		} catch (const std::invalid_argument&) {
			param.HCM_error = 1013;
			return;
		}
		if (hTx != newTxH.Height) {
			if (std::abs(hTx - newTxH.Height) <= hTx / 10) {
				param.Info[2] = true;
			} else {
				param.Info[3] = true;
			}
		}
	}
	
	try {
		hAntTx = std::stod(param.H_Tx_ant);
	} catch (const std::invalid_argument&) {
		param.HCM_error = 1009;
		return;
	}

	try {
		hAntRx = std::stod(param.H_Rx_ant);
	} catch (const std::invalid_argument&) {
		param.HCM_error = 1022;
		return;
	}
	
	param.V_angle_Tx_Rx = std::atan2(hRx + hAntRx - (hTx + hAntTx), (1000* param.Distance));

	// Calculation of power in direction of the receiver 'Power_to_Rx'
	
	
	if (param.Ant_typ_H_Tx == "000ND00" && param.Ant_typ_V_Tx == "000ND00") {
		param.Tx_ant_corr = 0.0;
		param.H_diff_angle_Tx_Rx = 0.0;
		param.V_diff_angle_Tx_Rx = 0.0;
	} else {
		try {
			txElevation = std::stod(param.Ele_Tx_input);
		} catch (const std::invalid_argument&) {
			param.HCM_error = 1031;
			return;
		}
		try {
			txAzimuth = std::stod(param.Azi_Tx_input);
		} catch (const std::invalid_argument&) {
			param.HCM_error = 1032;
			return;
		}
		
		Antenna_correction(param.Dir_Tx_Rx, txAzimuth, param.V_angle_Tx_Rx, txElevation,
							param.Ant_typ_H_Tx, param.Ant_typ_V_Tx,
							param.H_diff_angle_Tx_Rx, param.V_diff_angle_Tx_Rx,
							param.Tx_ant_corr, param.HCM_error);
		if (param.HCM_error != 0) return;
	}
							
	insidePowerToRx = param.Power_to_Rx - param.Tx_ant_corr;

	
	// Calculation of free space field strength 'Free_space_FS'
	param.Free_space_FS = 77.0 - 20.0 * std::log10(param.Distance) + insidePowerToRx;

	// If distance is less than 1 km, use free space field strength
	if (param.Distance < 1.0) {
		param.Calculated_FS = param.Free_space_FS;
		param.Info[11] = true;
		return;
	}

	
	//GetProfile(p2p, hTx, hRx, newLongTx, newLatTx, newLongRx, newLatRx, param);
	ter.GetProfile( newLongTx, newLatTx, newLongRx, newLatRx, p2p, hTx, hRx);
	param.PN = ter.countSteps;
	param.PD = ter.stepProfile;
	for (int i=0; i<ter.countSteps; ++i){
		param.T_Prof[i] = std::round(ter.T_Prof[i]);
		param.M_Prof[i] = ter.M_Prof[i];
	}
	
	if (param.HCM_error != 0) return;

	// Calculation of the first Fresnel zone
	if (p2p && (Tx_serv_area + Rx_serv_area == 0.0)) {
		int j = param.PN - 2;
		for (int i = 1; i <= j; ++i) {
			hdz[i] = param.T_Prof[i + 1] + param.PD * i * (param.Distance - param.PD * i) / 17.0;
		}

		if (param.Distance > 2.0) {
			int i1 = static_cast<int>(std::round(1.0 / param.PD));
			int i2 = static_cast<int>(std::round((param.Distance - 1.0) / param.PD));
			for (int i = i1; i <= i2; ++i) {
				hdz[i] += 10.0;
			}
		}

		for (int i = 1; i <= j; ++i) {
			hdc[i] = hAntRx * param.PD * i / param.Distance - hdz[i];
			double x = param.PD * i * (param.Distance - param.PD * i);
			hdf[i] = (x <= 0.0) ? 0.0 : 547.7 * std::sqrt(x / (param.Distance * freqTx));
		}

		bool free = true;
		for (int i = 1; i <= j; ++i) {
			if ((hdc[i] - hdf[i]) < 0.0) {
				free = false;
				break;
			}
		}

		if (free) {
			param.Calculated_FS = param.Free_space_FS;
			param.Info[12] = true;
			return;
		}
	}

	// Calculation of the field strength according to the ITU-R method
	if (p2p) {
		i1 = hAntTx;
		i2 = hAntRx;
	} else {
		i1 = hTx + hAntTx;
	}

	// Calculate Terrain Clearance Angles and TCA correction factors
	param.Tx_TCA_corr = 0;
	if (Tx_serv_area == 0.0) {
		param.Tx_TCA = -90.0;
		int jtx = (param.Distance >= 16.0) ? std::round(16.0 / param.PD) : param.PN - 2;
		for (int i = 1; i <= jtx; ++i) {
			xTCA = (param.T_Prof[1 + i] - (i1)) / (param.PD * i * 1000.0);
			xTCA = std::atan(xTCA) * (180.0 / M_PI); // Convert to degrees
			if (xTCA > param.Tx_TCA) param.Tx_TCA = xTCA;
		}
		
		param.Tx_TCA_corr = TCACorrectionCalculation(param.Tx_TCA, freqTx, param.Distance);
	}
	
	param.Rx_TCA_corr = 0;
	if (p2p && (Rx_serv_area == 0.0)) {
		param.Rx_TCA = -90.0;
		int jrx = (param.Distance >= 16.0) ? std::round(16.0 / param.PD) : param.PN - 2;
		for (int i = 1; i <= jrx; ++i) {
			xTCA = (param.T_Prof[param.PN - i] - (i2)) / (param.PD * i * 1000.0);
			xTCA = std::atan(xTCA) * (180.0 / M_PI); // Convert to degrees
			if (xTCA > param.Rx_TCA) param.Rx_TCA = xTCA;
		}
		param.Rx_TCA_corr = TCACorrectionCalculation(param.Rx_TCA, freqRx, param.Distance);
	}

	// Effective antenna heights
	int d1,d2;
	if (param.Distance < 15.0) {
		d1 = std::round(param.PN / 15.0);
		d2 = param.PN - 1;
	}else{
		d1 = std::round(1.0 / param.PD);
		d2 = std::round(15 / param.PD);		
	}
	
   // Transmitter:
    if (Tx_serv_area > 0.0) {
        // Height of a mobile 'hmTx'
        param.Heff_Tx = static_cast<double>(hAntTx);
    } else {
        hsum = 0;
        for (int I = d1; I <= d2; ++I) {
            hsum += param.T_Prof[I + 1];
        }
        param.Heff_Tx = i1 - static_cast<double>(hsum) / static_cast<double>(d2 - d1 + 1);
        
    }
    if (param.Heff_Tx < 3.0) param.Heff_Tx = 3.0;

	if ((!p2p) || (Rx_serv_area > 0)) {
		param.Heff_Rx = hAntRx;
	} else {
		hsum = 0;
		for (int i = d1; i <= d2; ++i) {
			hsum += param.T_Prof[param.PN - i];
		}
		param.Heff_Rx = i2 - static_cast<double>(hsum) / (d2 - d1 + 1);
	}
	if (param.Heff_Rx < 3.0) param.Heff_Rx = 3.0;

	param.Heff = (param.Heff_Tx * param.Heff_Rx) / 10.0;

	// Calculate distance over sea
	param.D_sea_calculated = 0.0;
	if (with_morpho) {
		bool null = false;
		double ds1 = 0.0;
		int j1 = 0;
		if (param.Distance > 1.0) {
			for (int i = 1; i <= param.PN; ++i) {
				if (param.M_Prof[i] == 1) { // Assuming 1 represents 'sea'
					if (null) {
						++j1;
					} else {
						j1 = 0;
						null = true;
					}
				} else {
					if (null) {
						ds1 = static_cast<double>(j1) * param.PD;
						param.D_sea_calculated += ds1;
						ds1 = 0.0;
						j1 = 0;
						null = false;
					}
				}
			}
			if (null) {
				ds1 = static_cast<double>(j1) * param.PD;
				param.D_sea_calculated += ds1;
				null = false;
			}
		}
	}

	if (param.D_sea_calculated > param.Distance) {
		param.Info[13] = true;
		param.D_sea_calculated = param.Distance;
	}

	// Calculate Delta h and Dh correction factor
	if ((param.Distance <= 10.0) || (param.D_sea_calculated >= param.Distance) || ((Tx_serv_area > 0.0) && (!p2p))) {
		param.Dh = 50.0;
		param.Dh_corr = 0.0;
	} else {
		
		DhCalculation(param);
		DhCorrection(param.Dh, param.Distance, freqTx, param.Dh_corr);
	}

	// Calculate the field strength from the figures
	FS_figures fsf;
	fsf.Tx_frequency = freqTx;
	fsf.Distance = param.Distance;     // km
	fsf.Heff = param.Heff;          // m
	fsf.Time_percentage = TimePerc; // %
	fsf.LatM = 0;//(latTx + latRx)/2; // Middle Latitide of path
	fsf.Sea_temperature = param.Sea_temperature[0]; // Sea_temperature
	fsf.Get_FS_from_figures();
	
	param.HCM_error = fsf.HCM_error;
	if (param.HCM_error != 0) return;

	// Corrections according to the power
	param.Land_FS = fsf.Land_FS_1kW - 30.0 + insidePowerToRx;
	param.Sea_FS = fsf.Sea_FS_1kW - 30.0 + insidePowerToRx;

	// Clearance angle corrections and delta h correction
	param.Land_FS += param.Tx_TCA_corr + param.Rx_TCA_corr - param.Dh_corr;

	if (param.Info[11] || param.Info[12]) {
		// Do nothing, already set
	} else {
		if (param.D_sea_calculated == 0.0) {
			param.Calculated_FS = param.Land_FS;
		} else {
			// Mixed path calculation
			if (TimePerc < 10) {
				double factorOfPathOverSea = param.D_sea_calculated / param.Distance;
				double x = factorOfPathOverSea * 10.0;
				double a1 = a[static_cast<int>(x) + 1];
				double a2, ax = 0;
				if (x < 10.0) {
					a2 = a[static_cast<int>(x) + 2];
				} else {
					a2 = a1;
				}
				x -= static_cast<int>(x);
				ax = a1 + x * (a2 - a1);
				param.Calculated_FS = param.Land_FS + ax * (param.Sea_FS - param.Land_FS);
			} else {
				param.Calculated_FS = param.Land_FS * (param.Distance - param.D_sea_calculated) / 
										param.Distance + param.Sea_FS * param.D_sea_calculated / param.Distance;
			}
		}
	}
}
#endif
