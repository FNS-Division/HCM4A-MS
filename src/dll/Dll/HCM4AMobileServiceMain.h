//
//!	Harmonized Calculation Method for mobile services
//!
//!
//	SUBROUTINE HCM_MS
//!
//!	Input values
//!
//!	Coo_Tx				Geographical coordinates of Tx				(4C) (Tx)
//!	Coo_Rx				Geographical coordinates of Rx				(4C) (Rx)
//!	H_Tx_input			Height of transmitter above sea level		(4Z) (Tx)
//!	H_Rx_input			Height of receiver above sea level			(4Z) (Rx)
//!	Ant_typ_H_Tx		Type of antenna horizontal				   (9XH) (Tx)
//!	Ant_typ_V_Tx		Type of antenna vertical				   (9XV) (Tx)
//!	Azi_Tx_input		Azimuth of maximum radiation				(9A) (Tx)
//!	Ele_Tx_input		Elevation angle of main radiation			(9B) (Tx)
//!	H_Tx_ant			Antenna height transmitter					(9Y) (Tx)
//!	H_Rx_ant			Antenna height receiver						(9Y) (Rx)
//!	Type_of_Tx_ant		Type of antenna [E or I]				   (8B2) (Tx)
//!	Max_power			Maximum radiated power					   (8B1) (Tx)
//!	Tx_frequ			Transmitting frequency						(1A) (Tx)
//!	Chan_occup			Channel occupation [0 or 1]				   (10Z) (Tx)
//!	Sea_temperature		Cold or warm sea [C or W, default = C]
//!	Rad_of_Tx_serv_area	Radius of the service area Tx				(4D) (Tx)
//!	Rad_of_Rx_serv_area	Radius of the service area Rx				(4D) (Rx)
//!	D_sea_input			Distance over sea [km] (input value)
//!						[format 999.9 or blank]
//!	Rx_frequ			Reception frequency							(1Y) (Rx)
//!	Desig_of_Rx_emmis	Designation of emission						(7A) (Rx)
//!	Desig_of_Tx_emmis	Designation of emission						(7A) (Tx)
//!	Ant_typ_H_Rx		Type of antenna horizontal				   (9XH) (Rx)
//!	Ant_typ_V_Rx		Type of antenna vertical				   (9XV) (Rx)
//!	Azi_Rx_input		Azimuth of maximum radiation				(9A) (Rx)
//!	Ele_Rx_input		Elevation angle of main radiation			(9B) (Rx)
//!	Type_of_Rx_ant		Type of antenna [E or I]				   (8B2) (Rx)
//!	Rx_ant_gain			Gain of Rx-antenna							(9G) (Rx)
//!	Depol_loss			Depolarization loss [format 99.9 (positiv)]
//!	Perm_FS_input		Input value of permissible field strength
//!						[format 999.9]
//!	Cor_fact_frequ_diff	Input value of correction factor according
//!						frequency difference [dB][format 99.9]
//!	PD					Point distace for the profile (km)
//!
//!	C_mode				Mode of calculation
//!						99 = line as P2P
//!						12 = P2P non strict HCM t%=1
//!						11= P2P non strict HCM t%=50
//!						10 = P2P non strict HCM t%=10
//!						 0 = normal Vienna Agreement
//!						-1 = Coordination line calculation (h2 = 10m)
//!						-9 = P2L non strict HCM t%=10 h2=3
//!						-10 = P2L non strict HCM t%=10 h2=10
//!						-11 = P2L non strict HCM t%=50 h2=3
//!
//!	Land_to				Land to calculate to (Preface to the IFL)
//!	Land_from			Land to calculate from (Preface to the IFL)
//!
//!	D_to_border			Distance to borderline
//!						negativ   -> Cross border range
//!						0         -> Borderline calculation
//!						positiv x -> Calculation on x km -line
//!
//!	Max_CBR_D_input		Input value of "Maximum cross border range of harmful
//!						interference" [km] [format 100]
//!
//!	Topo_path			Path to the terrain elevation data, e.g. "D:\TOPO"
//!
//!
//!	Output values:
//!
//!	Version				Version number of HCM_MS_V7 subroutine
//!	HCM_error			Error value, see list
//!	Info				Information (field) (see list)
//!	LongTx				Longitude of Tx
//!	LatTx				Latitude of Tx
//!	H_AntTx				Antenna height of Tx
//!	Tx_frequency		Tx frequency in MHz
//!	Tx_serv_area		Radius of Tx service area
//!	H_Datab_Tx			Height of Tx site from terrain database
//!	H_Tx				Height of Tx site used during all calculations
//!						(from input value or from terrain database)
//!	LongRx				Longitude of Rx
//!	LatRx				Latitude of Rx
//!	H_AntRx				Antenna height of Rx
//!	Rx_frequency		Rx frequency in MHz
//!	Rx_serv_area		Radius of Rx service area
//!	H_Datab_Rx			Height of Rx site from terrain database
//!	H_Rx				Height of Rx site used during all calculations
//!						(from input value or from terrain database)
//!	Time_percentage		1, 10 or 50 % time percentage of the curves
//!	CBR_D				Maximum cross border range
//!	ERP_ref_Tx			E.R.P. of reference transmitter
//!	T_Prof				Terrain height profile
//!	M_Prof				Morphological profile
//!	PN					Number of profile points
//!	Heff				Total effective antenna height
//!	Heff_Tx				Effective antenna height of Tx
//!	Heff_Rx				Effective antenna height of Rx
//!	Dh					Terrain irregularity
//!	Dh_corr				Correction factor according to terrain irregularity
//!	Perm_FS				Permissible field strength (for calculations)
//!	Perm_FS_from_table	Permissible field strength from table in Annex 1
//!	Calculated_FS		Calculated field strength
//!	Distance			Distance between transmitting position and reception position
//!	Prot_margin			Protection margin
//!						(permissible field strength - calculated field strenght)
//!	Tx_TCA				Transmitter terrain clearance angle
//!	Rx_TCA				Receiver terrain clearance angle
//!	Tx_TCA_corr			Correctin factor according to Tx terrain clearance angle
//!	Rx_TCA_corr			Correctin factor according to Rx terrain clearance angle
//!	Land_FS				Land field strength
//!	Sea_FS				Sea field strength
//!	Tx_ant_corr			Correction according to Tx antennatype (horizontal and vertical)
//!	Rx_ant_corr			Correction according to Rx antennatype (horizontal and vertical)
//!	Tx_ant_type_corr	Correction according to Tx antennatype (E or I)
//!	Rx_ant_type_corr	Correction according to Rx antennatype (E or I)
//!	Corr_delta_f		Correction factor according to frequency difference
//!	Channel_sp_Tx		Channel spacing of Tx (kHz)
//!	Channel_sp_Rx		Channel spacing of Rx (kHz)
//!	Power_to_Rx			Radiated power in direction of Rx
//!	Free_space_FS		Free space field strength
//!	D_sea_calculated	Calculated distance over sea
//!	Dir_Tx_Rx			Direction Tx to Rx
//!	Dir_Rx_Tx			Direction Rx to Tx
//!	V_angle_Tx_Rx		Vertical angle Tx to Rx
//!	V_angle_Rx_Tx		Vertical angle Rx to Tx
//!	H_diff_angle_Tx_Rx	Horizontal difference angle Tx to Rx
//!	H_diff_angle_Rx_Tx	Horizontal difference angle Rx to Tx
//!	V_diff_angle_Tx_Rx	Vertical difference angle Tx to Rx
//!	V_diff_angle_Rx_Tx	Vertical difference angle Rx to Tx
//!	Delta_frequency		Frequency difference in kHz
//!	Coo_Tx_new			Calculated Tx co-ordinates
//!	Coo_Rx_new			Calculated Rx co-ordinates
//!
//!
//!	HCM_error values:
//!
//!	0		no error
//!	36		error opening terrain- or morphological data file (data not available)
//!	200		error in longitude (in 'Point_height' or 'Point_type' subroutine)
//!	210		error in latitude (in 'Point_height' or 'Point_type' subroutine)
//!	220		error reading record (in 'Point_height' or 'Point_type' subroutine)
//!	300		latitude is not in range of 0.0 - 90.0
//!			(in 'Point_height' or 'Point_type' subroutine)
//!	400		height is missing (-9999) (in 'Point_height' subroutine)
//!	1000	Distance between Tx and Rx = 0. Calculations not possible
//!	1001	Error in geographical coordinates (Tx longitude, degrees)
//!	1002	Error in geographical coordinates (Tx longitude, minutes)
//!	1003	Error in geographical coordinates (Tx longitude, seconds)
//!	1004	Error in geographical coordinates (Tx longitude, E/W)
//!	1005	Error in geographical coordinates (Tx latitude, degrees)
//!	1006	Error in geographical coordinates (Tx latitude, minutes)
//!	1007	Error in geographical coordinates (Tx latitude, seconds)
//!	1008	Error in geographical coordinates (Tx latitude, N/S)
//!	1009	Error in Tx antenna height
//!	1010	Error in transmitting frequency value
//!	1011	Error in transmitting frequency unit
//!	1012	Error in radius of service area of Tx
//!	1013	Error in input value height of Tx site above sea level
//!	1014	Error in geographical coordinates (Rx longitude, degrees)
//!	1015	Error in geographical coordinates (Rx longitude, minutes)
//!	1016	Error in geographical coordinates (Rx longitude, seconds)
//!	1017	Error in geographical coordinates (Rx longitude, E/W)
//!	1018	Error in geographical coordinates (Rx latitude, degrees)
//!	1019	Error in geographical coordinates (Rx latitude, minutes)
//!	1020	Error in geographical coordinates (Rx latitude, seconds)
//!	1021	Error in geographical coordinates (Rx latitude, N/S)
//!	1022	Error in Rx antenna height
//!	1023	Error in reception frequency value
//!	1024	Error in reception frequency unit
//!	1025	C_mode is out of range
//!	1026	Error in input value of permissible field strength
//!	1027	Error in input value of maximum cross border range
//!	1028	The distance is greater than 1000 km. Calculations not possible
//!	1029	Error in radius of Rx service area
//!	1030	Error in input value Rx site height above sea level
//!	1031	Error in Tx elevation
//!	1032	Error in Tx azimuth
//!	1033	Error in typ of Tx antenna (E/I)
//!	1034	Error in power
//!	1035	Error in input value of distance over sea
//!	1036	The 'xxx.ALL' borderline file for Tx is missing
//!	1037	The 'xxx.ALL' borderline file for Rx is missing
//!	1038	Error in type of antenna (TR20-08)
//!	1039	Error in input data of correction factor according frequency difference
//!	1040	Channel spacing outside definition range (Rx)
//!	1041	Channel spacing outside definition range (Tx)
//!	1042	Error in Rx elevation
//!	1043	Error in Rx azimuth
//!	1044	Error in Rx type of antenna ("E" or "I")
//!	1045	Error in gain of Rx antenna
//!	1046	Error in input data of depolarization loss
//!	1047	Distance to borderline is too long
//!	1048	Selected line data not available
//!	1049	Error in line data
//!	1050	No Agreement frequency and CBR input is missing
//!
//!	2000	wrong Figure_frequency (from Get_figure_FS_value)
//!	2001	wrong Time_percentage  (from Get_figure_FS_value)
//!	2002	wrong Sea_temperature  (from Get_figure_FS_value)
//!	2003	wrong Figure_Heff      (from Get_figure_FS_value)
//!	2004	wrong Figure_distance  (from Get_figure_FS_value)
//!
//!	Info values:
//!
//!	Info(1)		No height of Tx site is given,
//!				height is taken from the terrain database
//!	Info(2)		Height of Tx site differs from height of terrain database
//!	Info(3)		Height of Tx site differs more than 10%,
//!				calculated values may be (extremely) wrong!
//!	Info(4)		Frequency out of range of table in Annex 1
//!	Info(5)		Input value of permissible field strength is used
//!	Info(6)		Input value of maximum cross border range is used
//!	Info(7)		Distance between Tx and Rx is less than both service area radius;
//!				field strength is set to 999.9
//!	Info(8)		No height of Rx site is given, height is from the terrain database
//!	Info(9)		Height of Rx site differs from height of terrain data
//!	Info(10)	Rx site height differs more than 10%,
//!				calculated values may be (extremely) wrong!
//!	Info(11)	Free space field strength used because distance < 1km
//!	Info(12)	Free space field strength is used, because 1st Fresnel zone is free
//!	Info(13)	Distance over sea is greater than total distance.
//!				Distance between Tx and Rx is used!
//!	Info(14)	Input value of correction factor according frequency difference is used
//!	Info(15)	Frequency difference outside definition range; 82 dB is used
//!	Info(16)	Calculated distance over sea is set to 0 because of missing
//!				morphological data. !
//!	Info(17)	Channel spacing outside curve range, broadband formula is used!
//!	Info(18)	Correction factors for the band 380 - 400 MHz are used.
//!

#ifndef _HCM4AMobileServiceMain_H
#define _HCM4AMobileServiceMain_H

#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <algorithm>

#include "HCM4AMobileServiceStruct.h"
#include "Line_calculation.h"
#include "P_to_P_calculation.h"
#include "Permissible_FS_calculation.h"
#include "HcmTerrain.h"

struct Coordinates {
	double longitude;
	double latitude;
};

struct AntennaS {
	int height;
	std::string type;
	double azimuth;
	double elevation;
};

struct Frequency {
	double value;
	char unit;
};


 

void HCM4AMobileService_Main(HCM4AMobileService_Parameters &param)
{
	// Read all input data
	Coordinates Coo_Tx, Coo_Rx;
	double Tx_serv_area, Rx_serv_area;
	Frequency Tx_frequency, Rx_frequency;
	double LongTx, LatTx, LongRx, LatRx;
	int Time_percentage, CI;
	int IOS;
	double hRx, hTx;
	
	HcmTerrain ter;
	
	ter.PathToTopo = param.Topo_path;
	ter.PathToMorpho = param.Morpho_path;
	
	param.PN = 0;

	// Correct country codes for filenames
	if(param.Land_to[2] == ' ')		param.Land_to[2] = '_';
	if(param.Land_to[1] == ' ')		param.Land_to[1] = '_';
	if(param.Land_from[2] == ' ')	param.Land_from[2] = '_';
	if(param.Land_from[1] == ' ')	param.Land_from[1] = '_';
	
	if (trim(param.Ant_typ_V_Rx).length() == 0) param.Ant_typ_V_Rx = "000ND00";
	if (trim(param.Ant_typ_H_Rx).length() == 0) param.Ant_typ_H_Rx = "000ND00";
	if (trim(param.Ant_typ_V_Tx).length() == 0) param.Ant_typ_V_Tx = "000ND00";
	if (trim(param.Ant_typ_H_Tx).length() == 0) param.Ant_typ_H_Tx = "000ND00";
	
	if (trim(param.Perm_FS_input).length() == 0){
		
	} else {
		try {
			param.Perm_FS = std::stod(trim(param.Perm_FS_input));
		}
		catch(...) {
			param.HCM_error = 1003;
			return;
		}	
	}
	

	// Get the longitude of transmitter 'LongTx'
	try {
		std::stringstream(param.Coo_Tx.substr(0, 3)) >> LongTx;
		std::stringstream(param.Coo_Tx.substr(4, 2)) >> CI;
		LongTx += CI / 60.0;
	}
	catch(...) {
		param.HCM_error = 1001;
		return;
	}
	try {
		std::stringstream(param.Coo_Tx.substr(6, 2)) >> CI;
		LongTx += CI / 3600.0;
	}
	catch(...) {
		param.HCM_error = 1003;
		return;
	}
	if(param.Coo_Tx[3] == 'W')
		LongTx = -LongTx;
	if(param.Coo_Tx[3] != 'E' && param.Coo_Tx[3] != 'W') {
		param.HCM_error = 1004;
		return;
	}

	// Get the latitude of transmitter 'LatTx'
	try {
		std::stringstream(param.Coo_Tx.substr(8, 2)) >> LatTx;
		std::stringstream(param.Coo_Tx.substr(11, 2)) >> CI;
		LatTx += CI / 60.0;
	}
	catch(...) {
		param.HCM_error = 1005;
		return;
	}
	try {
		std::stringstream(param.Coo_Tx.substr(13, 2)) >> CI;
		LatTx += CI / 3600.0;
	}
	catch(...) {
		param.HCM_error = 1007;
		return;
	}
	if(param.Coo_Tx[10] == 'S')
		LatTx = -LatTx;
	if(param.Coo_Tx[10] != 'N' && param.Coo_Tx[10] != 'S') {
		param.HCM_error = 1008;
		return;
	}

	// Get the antenna height of transmitter 'H_AntTx'
	try {
		double tmpH = std::stod(trim(param.H_Tx_ant));
	}
	catch(...) {
		param.HCM_error = 1009;
		return;
	}

	// Get the transmitting frequency 'Tx_frequency'
	try {
		Tx_frequency.value = std::stod(trim(param.Tx_frequ.substr(0, 11)));
		if(param.Tx_frequ[11] == 'M') {}
		else if(param.Tx_frequ[11] == 'K') {
			Tx_frequency.value /= 1.0e3;
		}
		else if(param.Tx_frequ[11] == 'G') {
			Tx_frequency.value *= 1.0e3;
		}
		else {
			param.HCM_error = 1011;
			return;
		}
	}
	catch(...) {
		param.HCM_error = 1010;
		return;
	}

	// Get the radius of the Tx service area
	try {
		Tx_serv_area = std::stod(trim(param.Rad_of_Tx_serv_area));
	}
	catch(...) {
		param.HCM_error = 1012;
		return;
	}

	// Calculate 'Tx_ant_type_corr'
	param.Tx_ant_type_corr = 0.0;
	if(param.Type_of_Tx_ant == "E") {
		param.Tx_ant_type_corr = 0.0;
	}
	else if(param.Type_of_Tx_ant == "I") {
		param.Tx_ant_type_corr = 2.1;
	}
	else {
		param.HCM_error = 1033;
		return;
	}

	// Get power
	try {
		param.Power_to_Rx = std::stod(trim(param.Max_power));
	}
	catch(...) {
		param.HCM_error = 1034;
		return;
	}
	param.Power_to_Rx -= param.Tx_ant_type_corr;

	// Read data according to p2p or p2l
	if(param.C_Mode >= 0) {
		// Read data for point to point calculations
		//Distance between Tx and Rx = 0. Calculations not possible
		if(param.Coo_Tx == param.Coo_Rx) {
			param.HCM_error = 1000;
			return;
		}

		// Get the longitude of receiver 'LongRx'
		try {
			LongRx = std::stod(trim(param.Coo_Rx.substr(0, 3)));
		}
		catch(...) {
			param.HCM_error = 1014;
			return;
		}
		try {
			CI = std::stod(trim(param.Coo_Rx.substr(4, 2)));
			LongRx += CI / 60.0;
		}
		catch(...) {
			param.HCM_error = 1015;
			return;
		}
		try {
			CI = std::stod(trim(param.Coo_Rx.substr(6, 2)));
			LongRx += CI / 3600.0;
		}
		catch(...) {
			param.HCM_error = 1016;
			return;
		}
		if(param.Coo_Rx[3] == 'W')
			LongRx = -LongRx;
		if(param.Coo_Rx[3] != 'E' && param.Coo_Rx[3] != 'W') {
			param.HCM_error = 1017;
			return;
		}
		
		
		

		// Get the latitude of receiver 'LatRx'
		try {
			LatRx = std::stod(trim(param.Coo_Rx.substr(8, 2)));
		}
		catch(...) {
			param.HCM_error = 1018;
			return;
		}
		try {
			CI = std::stod(trim(param.Coo_Rx.substr(11, 2)));
			LatRx += CI / 60.0;
		}
		catch(...) {
			param.HCM_error = 1019;
			return;
		}
		try {
			CI = std::stod(trim(param.Coo_Rx.substr(13, 2)));
			LatRx += CI / 3600.0;
		}
		catch(...) {
			param.HCM_error = 1200;
			return;
		}
		if(param.Coo_Rx[10] == 'S')
			LatRx = -LatRx;
		if(param.Coo_Rx[10] != 'N' && param.Coo_Rx[10] != 'S') {
			param.HCM_error = 1021;
			return;
		}

		// Get the antenna height of receiver 'H_AntRx'
		try {
			double tmpR = std::stod(trim(param.H_Rx_ant));
		}
		catch(...) {
			param.HCM_error = 1022;
			return;
		}


		// Get reception frequency 'Rx_frequency'
		try {
			Rx_frequency.value = std::stod(trim(param.Rx_frequ.substr(0, 11)));
		}
		catch(...) {
			param.HCM_error = 1023;
			return;
		}
		if(param.Rx_frequ[11] == 'M') {}
		else if(param.Rx_frequ[11] == 'K') {
			Rx_frequency.value /= 1.0e3;
		}
		else if(param.Rx_frequ[11] == 'G') {
			Rx_frequency.value *= 1.0e3;
		}
		else {
			param.HCM_error = 1024;
			return;
		}

		// Get radius of the Rx service area
		if(param.C_Mode != 99) {
			try {
				Rx_serv_area = std::stod(trim(param.Rad_of_Rx_serv_area));
			}
			catch(...) {
				param.HCM_error = 1029;
				return;
			}
		}
		else {
			Rx_serv_area = 0.0;
		}
	}
	else {
		// Safe defaults for p2l
		param.H_Rx_ant = "10";
		Rx_frequency.value = 0;
		Rx_serv_area = 0.0;
		param.Desig_of_Rx_emis = "       ";
		param.Cor_fact_frequ_diff = "    ";
	}

	// Test point distance (if <30, set it to 100 m)
	if(param.PD < 0.03)
		param.PD = 0.1;

	// Set C_mode depending parameters
	switch(param.C_Mode) {
	case 12:
		Time_percentage = 1;
		param.CBR_D = -999.9;
		param.ERP_ref_Tx = -999.9;
		param.Perm_FS = -999.9;
		param.Info[4] = true;
		break;
	case 11:
		Time_percentage = 50;
		param.CBR_D = -999.9;
		param.ERP_ref_Tx = -999.9;
		param.Perm_FS = -999.9;
		param.Info[4] = true;
		break;
	case 10:
		Time_percentage = 10;
		param.CBR_D = -999.9;
		param.ERP_ref_Tx = -999.9;
		param.Perm_FS = -999.9;
		param.Info[4] = true;
		break;
	case 99:
	case 0:
	case -1:
		if(Tx_frequency.value >= 29.7 && Tx_frequency.value <= 47.0) {
			param.Perm_FS = 0.0;
			param.CBR_D = 100.0;
			param.ERP_ref_Tx = 3.0;
			param.Info[4] = false;
		}
		else if(Tx_frequency.value >= 68.0 && Tx_frequency.value <= 74.8) {
			param.Perm_FS = 6.0;
			param.CBR_D = 100.0;
			param.ERP_ref_Tx = 9.0;
			param.Info[4] = false;
		}
		else if(Tx_frequency.value >= 75.2 && Tx_frequency.value <= 87.5) {
			param.Perm_FS = 6.0;
			param.CBR_D = 100.0;
			param.ERP_ref_Tx = 9.0;
			param.Info[4] = false;
		}
		else if(Tx_frequency.value >= 146.0 && Tx_frequency.value <= 149.9) {
			param.Perm_FS = 12.0;
			param.CBR_D = 80.0;
			param.ERP_ref_Tx = 12.0;
			param.Info[4] = false;
		}
		else if(Tx_frequency.value >= 150.05 && Tx_frequency.value <= 174.0) {
			param.Perm_FS = 12.0;
			param.CBR_D = 80.0;
			param.ERP_ref_Tx = 12.0;
			param.Info[4] = false;
		}
		else if(Tx_frequency.value >= 380.0 && Tx_frequency.value <= 385.0) {
			param.Perm_FS = 18.0;
			param.CBR_D = 50.0;
			param.ERP_ref_Tx = 14.0;
			param.Info[4] = false;
			param.Info[18] = true;
		}
		else if(Tx_frequency.value >= 390.0 && Tx_frequency.value <= 395.0) {
			param.Perm_FS = 18.0;
			param.CBR_D = 50.0;
			param.ERP_ref_Tx = 14.0;
			param.Info[4] = false;
			param.Info[18] = true;
		}
		else if(Tx_frequency.value >= 406.1 && Tx_frequency.value <= 430.0) {
			param.Perm_FS = 20.0;
			param.CBR_D = 50.0;
			param.ERP_ref_Tx = 16.0;
			param.Info[4] = false;
		}
		else if(Tx_frequency.value >= 440.0 && Tx_frequency.value <= 470.0) {
			param.Perm_FS = 20.0;
			param.CBR_D = 50.0;
			param.ERP_ref_Tx = 16.0;
			param.Info[4] = false;
		}
		else if(Tx_frequency.value >= 862.0 && Tx_frequency.value <= 960.0) {
			param.Perm_FS = 26.0;
			param.CBR_D = 30.0;
			param.ERP_ref_Tx = 13.0;
			param.Info[4] = false;
		}
		else if(Tx_frequency.value >= 1710.0 && Tx_frequency.value <= 1785.0) {
			param.Perm_FS = 35.0;
			param.CBR_D = 15.0;
			param.ERP_ref_Tx = 13.0;
			param.Info[4] = false;
		}
		else if(Tx_frequency.value >= 1805.0 && Tx_frequency.value <= 1880.0) {
			param.Perm_FS = 35.0;
			param.CBR_D = 15.0;
			param.ERP_ref_Tx = 13.0;
			param.Info[4] = false;
		}
		else {
			param.Perm_FS = -999.9;
			param.CBR_D = -999.9;
			param.ERP_ref_Tx = -999.9;
			param.Info[4] = true;
		}

		if((param.Info[18] && param.Desig_of_Tx_emis.substr(0, 7) == "25K0G7W" &&
		    param.Desig_of_Rx_emis.substr(0, 7) == "25K0G7W") ||
		   param.Chan_occup == "0") {
			Time_percentage = 10;
		}
		else {
			Time_percentage = 1;
		}
		break;
	case -9:
		Time_percentage = 10;
		param.H_Rx_ant = "3";
		param.CBR_D = -999.9;
		param.ERP_ref_Tx = -999.9;
		param.Perm_FS = -999.9;
		param.Info[4] = true;
		break;
	case -10:
		Time_percentage = 10;
		param.H_Rx_ant = "10";
		param.CBR_D = -999.9;
		param.ERP_ref_Tx = -999.9;
		param.Perm_FS = -999.9;
		param.Info[4] = true;
		break;
	case -11:
		Time_percentage = 50;
		param.H_Rx_ant = "3";
		param.CBR_D = -999.9;
		param.ERP_ref_Tx = -999.9;
		param.Perm_FS = -999.9;
		param.Info[4] = true;
		break;
	default:
		param.HCM_error = 1025;
		return;
	}

	param.Perm_FS_from_table = param.Perm_FS;


	// Input value Max_CBR_D
	int IMR = -1000;
	if(param.Max_CBR_D_input != "   ") {
		
		try {
			IMR = std::stoi(trim(param.Max_CBR_D_input));
		}
		catch(...) {
			param.HCM_error = 1027;
			return;
		}
		//if (IMR != 0){
			param.CBR_D = IMR*1.0;
		//}
		param.Info[6] = true;
	}
	else {
		if(param.Info[4] && param.C_Mode < 0 && param.D_to_border < 0) {
			param.HCM_error = 1050;
			return;
		}
	}
	
	// Look, if there is an input of permissible field strength
	//if (IMR != 0){
		if(param.Perm_FS_input != "     ") {
			param.Info[5] = true;
			
			try {
				param.Perm_FS = std::stod(trim(param.Perm_FS_input));
			}
			catch(...) {
				param.HCM_error = 1026;
				return;
			}
		}
		else {
			if(param.Info[4]) {
				param.HCM_error = 1026;
				return;
			}
		}
	//}



	// Sea temperature
	if(param.Sea_temperature != "W" && param.Sea_temperature != "C") {
		double aLatM;
		if(param.C_Mode >= 0) {
			aLatM = std::abs(LatTx + LatRx) / 2.0;
		}
		else {
			aLatM = LatTx;
		}
		param.Sea_temperature = 'C';
		if(aLatM < 56.0)
			param.Sea_temperature = 'I';
		if(aLatM <= 35.0)
			param.Sea_temperature = 'W';
	}

	// Prepare control values
	bool p2p = (param.C_Mode >= 0 && param.C_Mode != 99);
	bool with_morpho = false;
	
	if(trim(param.D_sea_input).length() < 1) {
		with_morpho = true;
	}
	else {
		with_morpho = false;
		try {
			param.D_sea_calculated = std::stod(trim(param.D_sea_input));
		}
		catch(...) {
			param.HCM_error = 1035;
			return;
		}
	}


	param.Prot_margin = 0;
	if(param.C_Mode < 0) {
		LineCalculation(LongTx, LatTx, LongRx, LatRx, Tx_serv_area, Time_percentage, Tx_frequency.value, hTx, hRx, param, ter);
	}
	else {
		P_to_P_calculation(LongTx, LatTx, LongRx, LatRx, Time_percentage, Tx_frequency.value,
		                   Rx_frequency.value, p2p, hTx, hRx, param, ter);
	}
	
	if (param.HCM_error > 0) return;
	Permissible_FS_calculation(Tx_frequency.value, Rx_frequency.value, Tx_serv_area, Rx_serv_area, p2p, hTx, hRx, param);

	param.Prot_margin = param.Perm_FS - param.Calculated_FS;
}

#endif