//!
//!	Permissible_FS_calculation
//!	This file is part of HCM.
//!
//!
//!	Subroutine to calculate the permissible field strength.
//!
//!
//!	Subroutine to calculate the permissible field strength.
//!
//!	Input values
//!
//!	Tx_frequency		Tx frequency [MHz]
//!	Rx_frequency		Rx frequency [MHz]
//!	Desig_of_Rx_emmis	Designation of Rx emission
//!	Desig_of_Tx_emmis	Designation of Tx emission
//!	Ant_typ_H_Rx		Type of antenna horizontal
//!	Ant_typ_V_Rx		Type of antenna vertical
//!	Distance			Distance between Tx and Rx points
//!	Dir_Rx_Tx			Direction Rx to Tx
//!	Azi_Rx_input		Azimuth of maximum radiation
//!	Ele_Rx_input		Elevation angle of main radiation
//!	Depol_loss			Depolarization loss [format 99.9 (positiv)]
//!	Type_of_Rx_ant		Type of Rx antenna [E or I]
//!	Cor_fact_frequ_diff	Input value of correction factor according
//!						frequency difference [dB] [format 99.9]
//!	Rx_ant_gain			Gain of Rx-antenna
//!	C_Mode				Mode of calculation
//!    
//!
//!	Output values
//!
//!	HCM_Error			Error value
//!	Perm_FS				Permissible field strength
//!	V_angle_Rx_Tx		Vertical angle Rx to Tx
//!	V_diff_angle_Rx_Tx	Difference angle vertical
//!	H_diff_angle_Rx_Tx	Difference angle horizontal
//!	Rx_Azimuth			Rx azimuth
//!	Rx_Elevation		Rx elevation
//!	Rx_ant_corr			Correction according antenna
//!	Rx_ant_type_corr	Correction according antennatype [E or I]
//!	Delta_frequency		Delta frequency
//!	Corr_delta_f		Correction factor according delta frequency
//!	Channel_sp_Rx		Channel spacing of Rx (0 for wideband)
//!	Channel_sp_Tx		Channel spacing of Tx (0 for wideband)
//!
//!
//!
//!	HCM_Error values 
//!
//!	1039	Error in input data of correction factor according frequency difference.
//!	1040	Channel spacing outside definition range (Rx)! 
//!	1041	Channel spacing outside definition range (Tx)! 
//!	1042	Error in Rx elevation
//!	1043	Error in Rx azimuth
//!	1044	Error in Rx type of antenna ("E" or "I")
//!	1045	Error in gain of Rx antenna
//!	1046	Error in input data of depolarization loss
//!
//!
//!	param.Info values
//!
//!	param.Info14	Input value of correction factor according frequency difference is used
//!	param.Info15	Frequency difference outside definition range!
//!	param.Info17	Channel spacing outside curve range, broadband formula is used!
//!	param.Info18	Correction factors for the band 380 - 400 MHz are used.



#ifndef _Permissible_FS_h
#define _Permissible_FS_h

#include <iostream>
#include <cmath>
#include <string>
#include <algorithm>
#include "HCM4AMobileServiceStruct.h"

// Include definitions (assuming these are defined elsewhere)
// #include "HCM_MS_V7_definitions.h"

void Permissible_FS_calculation(double freqTx, double freqRx, double Tx_serv_area, double Rx_serv_area,
					bool p2p, double hTx, double hRx, HCM4AMobileService_Parameters& param) {
//    int IOS, I, CSXR, CSXT;
//    bool TX_TETRA, RX_TETRA, TX_DIG, RX_DIG;
//    double acorrB1, FACTOR, OMEGA, acorrsinus;
//    double GANT, DPN, B1, B2, X1, CDFN, CDFW;
//    std::string DRX, DTX;
//
//    double V_angle_Rx_Tx = 0.0;
//    double Rx_ant_corr = 0.0;
//    double H_diff_angle_Rx_Tx = 0.0;
//    double V_diff_angle_Rx_Tx = 0.0;
//    int Channel_sp_Tx = 0;
//    int Channel_sp_Rx = 0;
//    double Corr_delta_f = 0.0;
//    double Rx_ant_type_corr = 0.0;
	bool TX_TETRA, RX_TETRA, TX_DIG, RX_DIG;
	double CDFN, CDFW, GANT, DPN, acorrB1, acorrsinus, OMEGA, B1, B2, X1, FACTOR, hAntTx, hAntRx, Rx_Azimuth, Rx_Elevation;
	int I, CSXR, CSXT;
	std::string DRX, DTX;


	// Delta frequency in Hz:
	param.Delta_frequency = std::round(std::abs(freqTx - freqRx) * 1e6);
	
	// Input value for correction factor according delta frequency ?
	if (param.Cor_fact_frequ_diff != "    ") {
		param.Info[14] = true;
		// Input value of correction factor according frequency difference is used
		try {
			param.Corr_delta_f = std::stod(param.Cor_fact_frequ_diff);
		} catch (const std::invalid_argument&) {
			param.HCM_error = 1039;
			// Error in input data of correction factor according frequency difference.
			return;
		}
	} else {
		TX_DIG = (param.Desig_of_Tx_emis.substr(5, 1).find_first_of("1279") != std::string::npos);
		RX_DIG = (param.Desig_of_Rx_emis.substr(5, 1).find_first_of("1279") != std::string::npos);
		TX_TETRA = (param.Desig_of_Tx_emis.substr(0, 7) == "25K0G7W");
		RX_TETRA = (param.Desig_of_Rx_emis.substr(0, 7) == "25K0G7W");

		// Bandwidth of TX:
		if (TX_TETRA && !RX_TETRA) {
			CSXT = 16000;
		} else {
			DTX = param.Desig_of_Tx_emis.substr(0, 4);
			if (DTX[3] == 'H') {
				FACTOR = 1.0;
				I = 4;
			} else {
				I = DTX.find('K');
				if (I == std::string::npos) {
					I = DTX.find('M');
					if (I == std::string::npos) {
						param.HCM_error = 1041;
						// Channel spacing outside definition range (Tx)!
						return;
					} else {
						FACTOR = 1000000.0;
					}
				} else {
					FACTOR = 1000.0;
				}
			}
			// Replace 'K' or 'M' with '.':
			DTX[I] = '.';
			try {
				X1 = std::stod(DTX);
			} catch (const std::invalid_argument&) {
				param.HCM_error = 1041;
				// Channel spacing outside definition range (Tx)!
				return;
			}
			CSXT = std::round(X1 * FACTOR);
		}

		// bail out for line calculations
		if (!p2p) {
			param.Delta_frequency = 0;
			// WB/NB correction
			if (TX_DIG && (CSXT > 16000) && (freqTx <= 470.0) && (param.Perm_FS_input == "     ")) {
				param.Perm_FS += 6 * std::log10(CSXT / 25000.0);
			}
			return;
		}

		// shortcut for TETRA <> TETRA
		if (TX_TETRA && RX_TETRA) {
			// Annex3.4.3
			if (param.Delta_frequency <= 24999) {
				param.Corr_delta_f = 0.0;
			} else if (param.Delta_frequency <= 50000) {
				param.Corr_delta_f = 45.0;
			} else {
				param.Corr_delta_f = 70.0;
			}
		} else {
			// Bandwidth of Rx:
			if (RX_TETRA && !TX_TETRA) {
				CSXR = 16000;
			} else {
				DRX = param.Desig_of_Rx_emis.substr(0, 4);
				if (DRX[3] == 'H') {
					FACTOR = 1.0;
					I = 4;
				} else {
					I = DRX.find('K');
					if (I == std::string::npos) {
						I = DRX.find('M');
						if (I == std::string::npos) {
							param.HCM_error = 1040;
							// Channel spacing outside definition range (Rx)!
							return;
						} else {
							FACTOR = 1000000.0;
						}
					} else {
						FACTOR = 1000.0;
					}
				}
				// Replace 'K' or 'M' with '.':
				DRX[I] = '.';
				try {
					X1 = std::stod(DRX);
				} catch (const std::invalid_argument&) {
					param.HCM_error = 1040;
					// Channel spacing outside definition range (Rx)!
					return;
				}
				CSXR = std::round(X1 * FACTOR);
			}

			// Annex3.2
			if (CSXT > CSXR) {
				B1 = CSXT;
				B2 = CSXR;
			} else {
				B2 = CSXT;
				B1 = CSXR;
			}

			OMEGA = param.Delta_frequency / B1;

			// Annex3.4.1 TETRA Tx vs. narrowband Rx
			if (TX_TETRA && (CSXR <= 25000)) {
				// acorrB1:
				if (OMEGA < 0.5) {
					acorrB1 = 0.0;
				} else if (OMEGA <= 1.0) {
					acorrB1 = 32.0 * OMEGA - 16.0;
				} else if (OMEGA <= 1.4) {
					acorrB1 = 112.0 * OMEGA - 96.0;
				} else {
					acorrB1 = 41.0 * OMEGA;
				}

				// acorrsinus:
				if (OMEGA < 0.4) {
					acorrsinus = 0.0;
				} else if (OMEGA <= 0.7) {
					acorrsinus = 50.0 * OMEGA - 21.0;
				} else if (OMEGA <= 1.0) {
					acorrsinus = 225.0 * OMEGA - 145.0;
				} else {
					acorrsinus = -20.0 * OMEGA + 100.0;
				}
				param.Corr_delta_f = std::min(acorrsinus - (acorrsinus - acorrB1) * (B2 / B1), 70.0);
			} else if (RX_TETRA && (CSXT <= 25000)) {
				// Annex3.4.2 narrowband Tx vs. TETRA Rx
				// acorrB1:
				if (OMEGA < 0.45) {
					acorrB1 = 0.0;
				} else if (OMEGA <= 0.63) {
					acorrB1 = 55.0 * OMEGA - 23.0;
				} else if (OMEGA <= 0.93) {
					acorrB1 = 180.0 * OMEGA - 100.0;
				} else {
					acorrB1 = 12.5 * OMEGA + 57.0;
				}

				// acorrsinus:
				if (OMEGA < 0.45) {
					acorrsinus = 0.0;
				} else if (OMEGA <= 0.7) {
					acorrsinus = 225.0 * OMEGA - 101.0;
				} else {
					acorrsinus = 13.0 * OMEGA + 58.0;
				}
				param.Corr_delta_f = std::min(acorrsinus - (acorrsinus - acorrB1) * (B2 / B1), 70.0);
			} else {
				// Annex3.3 narrowband w/o TETRA
				// acorrB1:
				if (OMEGA < 0.5) {
					acorrB1 = 0.0;
				} else if (OMEGA <= 1.0) {
					acorrB1 = 47.0 * OMEGA - 24.0;
				} else if (OMEGA <= 1.3) {
					acorrB1 = 80.0 * OMEGA - 55.0;
				} else {
					acorrB1 = 38.0 * OMEGA;
				}

				// acorrsinus:
				if (OMEGA < 0.5) {
					acorrsinus = 0.0;
				} else if (OMEGA <= 1.3) {
					acorrsinus = 88.0 * OMEGA - 44.0;
				} else {
					acorrsinus = 12.0 * OMEGA + 55.0;
				}
				param.Corr_delta_f = std::min(acorrsinus - (acorrsinus - acorrB1) * (B2 / B1), 70.0);
				if ((CSXT <= 25000) && (CSXR <= 25000)) {
					CDFN = param.Corr_delta_f;
				} else {
					// Annex3.5 wideband
					// acorrB1:
					if (OMEGA < 0.5) {
						acorrB1 = 0.0;
					} else if (OMEGA <= 2.0) {
						acorrB1 = 33.3 * OMEGA - 16.7;
					} else {
						acorrB1 = 10.0 * OMEGA + 30.0;
					}

					// acorrsinus:
					if (OMEGA < 0.5) {
						acorrsinus = 0.0;
					} else if (OMEGA <= 1.25) {
						acorrsinus = 66.7 * OMEGA - 33.3;
					} else if (OMEGA <= 1.75) {
						acorrsinus = 20.0 * OMEGA + 25.0;
					} else {
						acorrsinus = 4.8 * OMEGA + 51.6;
					}
					param.Corr_delta_f = std::min(acorrsinus - (acorrsinus - acorrB1) * (B2 / B1), 70.0);
					if ((CSXR >= 200000) || (CSXT >= 200000)) {
						CDFW = param.Corr_delta_f;
					} else {
						// Annex3.6
						param.Corr_delta_f = CDFN + ((CDFW - CDFN) / 175.0) * (B1 / 1000.0 - 25.0);
					}
				}
			}
		}
	}

	param.Perm_FS += param.Corr_delta_f;

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

	// Calculation of antenna correction factors "Rx_ant_corr" and "Rx_ant_type_corr":
	param.V_angle_Rx_Tx = std::atan2(hTx + hAntTx - (hRx + hAntRx), (1e3 * param.Distance)) * 180/ M_PI;
	if ((param.Ant_typ_V_Rx != "000ND00") || (param.Ant_typ_H_Rx != "000ND00")) {
		try {
			Rx_Elevation = std::stod(param.Ele_Rx_input);
		} catch (const std::invalid_argument&) {
			if (param.Ant_typ_V_Rx != "000ND00") {
				param.HCM_error = 1042;
				// Error in Rx elevation
				return;
			}
		}
		try {
			Rx_Azimuth = std::stod(param.Azi_Rx_input);
		} catch (const std::invalid_argument&) {
			if (param.Ant_typ_H_Rx != "000ND00") {
				param.HCM_error = 1043;
				// Error in Rx azimuth
				return;
			}
		}
		Antenna_correction(param.Dir_Rx_Tx, Rx_Azimuth, param.V_angle_Rx_Tx, Rx_Elevation, 
							param.Ant_typ_H_Rx, param.Ant_typ_V_Rx,
							param.H_diff_angle_Rx_Tx, param.V_diff_angle_Rx_Tx,  
							param.Rx_ant_corr, param.HCM_error);
		if (param.HCM_error != 0) return;
	}

	param.Perm_FS += param.Rx_ant_corr;

	if (param.Type_of_Rx_ant == "E") {
		param.Rx_ant_type_corr = 0.0;
	} else if (param.Type_of_Rx_ant == "I") {
		param.Rx_ant_type_corr = 2.1;
	} else {
		param.HCM_error = 1044;
		// Error in typ of Rx antenna (E/I)
		return;
	}

	param.Perm_FS += param.Rx_ant_type_corr;

	// Gain of Rx-antenna:
	try {
		GANT = std::stod(param.Rx_ant_gain);
	} catch (const std::invalid_argument&) {
		param.HCM_error = 1045;
		// Error in gain of Rx antenna
		return;
	}

	param.Perm_FS -= GANT;

	// Depolarisation loss:
	try {
		DPN = std::stod(param.Depol_loss);
	} catch (const std::invalid_argument&) {
		param.HCM_error = 1046;
		// Error in input data of depolarization loss
		return;
	}

	// automatic DPN calculation
	if (param.Depol_loss == "-9.9") {
		double X1 = param.Free_space_FS - param.Calculated_FS;
		if ((X1 < 50.0) && (hAntRx > 10) && (hAntTx > 10) &&
			(Tx_serv_area + Rx_serv_area == 0.0) &&
			((param.Rx_ant_corr <= 10.0) || (param.Tx_ant_corr <= 10.0))) {
			DPN = 25.0 - 0.5 * (X1);
		} else {
			DPN = 0.0;
		}
		param.Depol_loss = std::to_string(DPN);
	}

	param.Perm_FS += DPN;

	return;
}


#endif