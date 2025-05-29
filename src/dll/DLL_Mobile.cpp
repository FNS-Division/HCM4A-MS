#include <windows.h>
#include "DLL_Mobile.h"



#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <cctype>
#include <vector>

#include "Dll/HCM4AMobileServiceStruct.h"
#include "Dll/HCM4AMobileServiceMain.h"




void HCM4AMobileServiceDLL(int I_C_mode, int I_bor_dis, double I_PD,
				double &I_Distance, short &I_H_Datab_Tx,
				short &I_H_Datab_Rx, int &I_HCM_error, double &I_Heff, double &I_Dh, double &I_Dh_corr,
				double &I_Power_to_Rx, double &I_Free_space_FS, double &I_Land_FS, double &I_Sea_FS,
				double &I_Tx_ant_corr, double &I_Tx_ant_type_corr, double &I_Dir_Tx_Rx,
				double &I_V_angle_Tx_Rx, double &I_Tx_TCA, double &I_Rx_TCA, double I_Tx_TCA_corr,
				double &I_Rx_TCA_corr, double &I_D_sea_calculated, double &I_Rx_ant_corr,
				double &I_Rx_ant_type_corr, double &I_Delta_frequency, double &I_Corr_delta_f,
				double &I_Calculated_FS, double &I_Perm_FS, double &I_CBR_D, double &I_ERP_ref_Tx,
				double &I_Prot_margin, std::string& I_str) {

     const int MAX_STR_LEN = 432;
    if (I_str.length() < MAX_STR_LEN) {
        I_HCM_error = 3000;
        return;
    }

    for (auto& ch : I_str) {
        if (ch >= 'a' && ch <= 'z') {
            ch = toupper(ch);
        } else if (ch == '\0') {
            ch = ' ';
        }
    }
    
	HCM4AMobileService_Parameters param;

	param.Version  = "1.18";
	
	param.C_Mode = I_C_mode;
	param.PD = I_PD;
	param.D_to_border = I_bor_dis;
	
    param.Coo_Tx = I_str.substr(0, 15);
    param.Coo_Rx = I_str.substr(15, 15);
    param.H_Tx_input = I_str.substr(30, 4);
    param.H_Rx_input = I_str.substr(34, 4);
    param.Ant_typ_H_Tx = I_str.substr(38, 7);
    param.Ant_typ_V_Tx = I_str.substr(45, 7);
    param.Azi_Tx_input = I_str.substr(52, 5);
    param.Ele_Tx_input = I_str.substr(57, 5);
    param.H_Tx_ant = I_str.substr(62, 4);
    param.H_Rx_ant = I_str.substr(66, 4);
    param.Type_of_Tx_ant = I_str.substr(70, 1);
    param.Max_power = I_str.substr(71, 6);
    param.Tx_frequ = I_str.substr(77, 12);
    param.Chan_occup = I_str.substr(89, 1);
    param.Sea_temperature = I_str.substr(90, 1);
    param.Rad_of_Tx_serv_area = I_str.substr(91, 5);
    param.Rad_of_Rx_serv_area = I_str.substr(96, 5);
    param.D_sea_input = I_str.substr(101, 5);
    param.Rx_frequ = I_str.substr(106, 12);
    param.Desig_of_Rx_emis = trim(I_str.substr(118, 9));
    param.Desig_of_Tx_emis = trim(I_str.substr(127, 9));
    param.Ant_typ_H_Rx = I_str.substr(136, 7);
    param.Ant_typ_V_Rx = I_str.substr(143, 7);
    param.Azi_Rx_input = I_str.substr(150, 5);
    param.Ele_Rx_input = I_str.substr(155, 5);
    param.Type_of_Rx_ant = I_str.substr(160, 1);
    param.Rx_ant_gain = I_str.substr(161, 4);
    param.Depol_loss = I_str.substr(165, 4);
    param.Perm_FS_input = I_str.substr(169, 5);
    param.Cor_fact_frequ_diff = I_str.substr(174, 4);
    param.Land_to = I_str.substr(178, 3);
    param.Land_from = I_str.substr(181, 3);
    param.Max_CBR_D_input = I_str.substr(184, 3);
    param.Topo_path = I_str.substr(187, 163);
    param.Border_path = I_str.substr(350, 163);
    param.Morpho_path = I_str.substr(513, 163);
    param.Topo_path = trim(param.Topo_path);
    param.Border_path = trim(param.Border_path);
    param.Morpho_path = trim(param.Morpho_path);
    param.HCM_error = 0;
    

    bool DEBUG = false;
    
    std::string pathToDebug = trim(I_str.substr(731));
    if (std::filesystem::exists(pathToDebug) && std::filesystem::is_directory(pathToDebug)) pathToDebug += "\\DEBUG.TXT";
    
    std::ofstream debugFile(pathToDebug);
    if (debugFile.is_open()) {
        DEBUG = true;
        debugFile << "\n                       R e s u l t s\n\n";
        debugFile << "  Mode of calculation            : " << param.C_Mode << " (";
        switch (param.C_Mode) {
            case 99: debugFile << "Point to point like line calc.)\n"; break;
            case 12: debugFile << "p2p non strict HCM t%=1.)\n"; break;
            case 11: debugFile << "p2p non strict HCM t%=50.)\n"; break;
            case 10: debugFile << "p2p non strict HCM t%=10.)\n"; break;
            case 0: debugFile << "Normal Agreement)\n"; break;
            case -1: debugFile << "Agreement line calc. h2=10m)\n"; break;
            case -9: debugFile << "p2l non strict HCM t%=10 h2=3.)\n"; break;
            case -10: debugFile << "p2l non strict HCM t%=10 h2=10.)\n"; break;
            case -11: debugFile << "p2l non strict HCM t%=50 h2=3.)\n"; break;
        }
        debugFile << "\n                     Input data :\n\n";
        debugFile << " Geographical co-ordinates of Tx: " << param.Coo_Tx.substr(0, 8) << " " << param.Coo_Tx.substr(8, 7) << "\n";
        if (param.C_Mode >= 0) {
            debugFile << " Geographical co-ordinates of Rx: " << param.Coo_Rx.substr(0, 8) << " " << param.Coo_Rx.substr(8, 7) << "\n";
        }
        debugFile << " Height above sea level of Tx   : " << param.H_Tx_input << " m\n";
        if (param.C_Mode >= 0) {
            debugFile << " Height above sea level of Rx   : " << param.H_Rx_input << " m\n";
        }
        debugFile << " Type of antenna horiz. of Tx   : " << param.Ant_typ_H_Tx << "\n";
        debugFile << " Type of antenna verti. of Tx   : " << param.Ant_typ_V_Tx << "\n";
        debugFile << " Azimuth of max. radiation of Tx: " << param.Azi_Tx_input << " degr.\n";
        debugFile << " Elevation of max. radia. of Tx : " << param.Ele_Tx_input << " degr.\n";
        debugFile << " Antenna height of Tx           : " << param.H_Tx_ant << " m\n";
        if (param.C_Mode >= 0) {
            debugFile << " Antenna height of Rx           : " << param.H_Rx_ant << " m\n";
        }
        debugFile << " Type of antenna Tx             : " << param.Type_of_Tx_ant << "\n";
        debugFile << " Maximum radiated power         : " << param.Max_power << " dBW\n";
        debugFile << " Transmitting frequency         : " << param.Tx_frequ << "Hz\n";
        debugFile << " Designation of emission Tx     : " << param.Desig_of_Tx_emis << "\n";
        debugFile << " Channel occupation             : " << param.Chan_occup << "\n";
        debugFile << " Cold or warm sea               : " << param.Sea_temperature << "\n";
        debugFile << " Distance over sea              : " << param.D_sea_input << " km\n";
        debugFile << " Radius of the service area Tx  : " << param.Rad_of_Tx_serv_area << " km\n";
        debugFile << " Country code of Tx station     : " << param.Land_from << "\n";
        if (param.C_Mode >= 0) {
            debugFile << " Reception frequency            : " << param.Rx_frequ << "Hz\n";
            debugFile << " Designation of emission Rx     : " << param.Desig_of_Rx_emis << "\n";
            debugFile << " Type of antenna horiz. of Rx   : " << param.Ant_typ_H_Rx << "\n";
            debugFile << " Type of antenna verti. of Rx   : " << param.Ant_typ_V_Rx << "\n";
            debugFile << " Azimuth of max. radiation of Rx: " << param.Azi_Rx_input << " degr.\n";
            debugFile << " Elevation of max. radia. of Rx : " << param.Ele_Rx_input << " degr.\n";
            debugFile << " Type of antenna Rx             : " << param.Type_of_Rx_ant << "\n";
            debugFile << " Gain of Rx-antenna             : " << param.Rx_ant_gain << " dB\n";
            debugFile << " Depolarization loss            : " << param.Depol_loss << " dB\n";
            debugFile << " Country code of Rx station     : " << param.Land_to << "\n";
            debugFile << " Input value of corr. f.delta f.: " << param.Cor_fact_frequ_diff << " dB\n";
            debugFile << " Radius of the service area Rx  : " << param.Rad_of_Rx_serv_area << " km\n";
        } else {
            debugFile << " Country (code) to calc. to     : " << param.Land_to << "\n";
            debugFile << " Input value of dist. to border : " << param.D_to_border << " km\n";
            debugFile << " Input value of max. crossb. r. :  " << param.Max_CBR_D_input << " km\n";
        }
        debugFile << " Input value of permissible fs. : " << param.Perm_FS_input << " dBuV/m\n\n";
    }
    
    
    auto TimeStart = std::chrono::high_resolution_clock::now();
    HCM4AMobileService_Main(param);
    auto TimeEnd = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> TimeElapsed = TimeEnd - TimeStart;

    I_str.replace(676, 6, param.Version); // Version number

    std::vector<bool> Info(20, false);
    for (int i = 0; i < 20; ++i) {
        I_str[682 + i] = Info[i] ? 'T' : 'F';
    }

    I_str.replace(702, 15, param.Coo_Tx_new); // Calculated co-ordinates of the transmitter
    I_str.replace(717, 15, param.Coo_Rx_new); // Calculated co-ordinates of the receiver
    I_str.replace(165, 4, param.Depol_loss); // Depolarization loss
    I_str[90] = param.Sea_temperature[0]; // calculated Sea temperature

    I_Distance = param.Distance; // Distance
    I_H_Datab_Tx = param.H_Datab_Tx; // Heigth of TX above sea level (terrain database)
    I_H_Datab_Rx = param.H_Datab_Rx; // Heigth of RX above sea level (terrain database)
    I_HCM_error = param.HCM_error; // Error value
    I_Heff = param.Heff; // Effective antenna height
    I_Dh = param.Dh; // Terrain irregularity
    I_Dh_corr = param.Dh_corr; // Correction factor according terrain irregularity
    I_Power_to_Rx = param.Power_to_Rx; // Power in the direction of the receiver
    I_Free_space_FS = param.Free_space_FS; // Free space field strength
    I_Land_FS = param.Land_FS; // Land field strength
    I_Sea_FS = param.Sea_FS; // Sea field strength
    I_Tx_ant_corr = param.Tx_ant_corr; // Correction according Tx antenna (H + V)
    I_Tx_ant_type_corr = param.Tx_ant_type_corr; // Correction according antenna type (E/I) TX
    I_Dir_Tx_Rx = param.Dir_Tx_Rx; // Horizontal direction TX to RX
    I_V_angle_Tx_Rx = param.V_angle_Tx_Rx; // Vertical direction TX to RX
    I_Tx_TCA = param.Tx_TCA; // TX clearance angle
    I_Rx_TCA = param.Rx_TCA; // RX clearance angle
    I_Tx_TCA_corr = param.Tx_TCA_corr; // TX clearance angle correction factor
    I_Rx_TCA_corr = param.Rx_TCA_corr; // RX clearance angle correction factor
    I_D_sea_calculated = param.D_sea_calculated; // Distance over sea calculated
    I_Rx_ant_corr = param.Rx_ant_corr; // Correction according Rx antenna (H + V)
    I_Rx_ant_type_corr = param.Rx_ant_type_corr; // Correction according antenna type (E/I) Rx
    I_Delta_frequency = param.Delta_frequency; // Frequency difference (Hz)
    I_Corr_delta_f = param.Corr_delta_f; // Correction factor according frequency difference
    I_Calculated_FS = param.Calculated_FS; // Calculated field strength
    I_Perm_FS = param.Perm_FS; // Permissible field strength
    I_CBR_D = param.CBR_D; // Maximum range of harmful interference
    I_ERP_ref_Tx = param.ERP_ref_Tx; // Power of the reference transmitter
    I_Prot_margin = param.Prot_margin; // Protection margin (EP - EC)

    if (DEBUG) {
        debugFile << "\n Version of the HCM module      : V" << param.Version << "\n\n";
        debugFile << "               Important output :\n\n";
        debugFile << "  Error value                    : " << param.HCM_error << "\n";
        if (param.HCM_error != 0) {
            switch (param.HCM_error) {
                case 999: debugFile << " Error in Point_heigt or Point_type subroutine (database).\n"; break;
                case 1000: debugFile << " Distance is 0.0 km. Calculations not possible !\n"; break;
                case 1001: case 1002: case 1003: case 1004:
                case 1005: case 1006: case 1007: case 1008:
                    debugFile << " Error in geographical Tx co-ordinates !\n"; break;
                case 1009: debugFile << " Error in input data of Tx antenna height !\n"; break;
                case 1010: debugFile << " Error in transmitting frequency value !\n"; break;
                case 1011: debugFile << " Error in transmitting frequency unit !\n"; break;
                case 1012: debugFile << " Error in input data of service area Tx!\n"; break;
                case 1013: debugFile << " Error in input data of height of transmitter !\n"; break;
                case 1014: case 1015: case 1016: case 1017:
                case 1018: case 1019: case 1020: case 1021:
                    debugFile << " Error in geographical Rx co-ordinates !\n"; break;
                case 1022: debugFile << " Error in input data of Rx antenna height !\n"; break;
                case 1023: debugFile << " Error in reception frequency value !\n"; break;
                case 1024: debugFile << " Error in reception frequency unit !\n"; break;
                case 1025: debugFile << " Mode of calculation is not available !\n"; break;
                case 1026: debugFile << " Error in input data of permissible field strength !\n"; break;
                case 1027: debugFile << " Error in input value of maximum crossborder range!\n"; break;
                case 1028: debugFile << " Distance greater than 1000 km. Calculations not possible !\n"; break;
                case 1029: debugFile << " Error in input data of service area Rx!\n"; break;
                case 1030: debugFile << " Error in input data of height of receiver !\n"; break;
                case 1031: debugFile << " Error in input data of Tx elevation !\n"; break;
                case 1032: debugFile << " Error in input-data of Tx azimuth !\n"; break;
                case 1033: debugFile << " Error in input data of Tx antenna type (E or I) !\n"; break;
                case 1034: debugFile << " Error in input data of maximal radiated power !\n"; break;
                case 1035: debugFile << " Error in input value of distance over sea !\n"; break;
                case 1036: debugFile << " xxx.ALL borderline file for Tx is missing!\n"; break;
                case 1037: debugFile << " xxx.ALL borderline file for Rx is missing!\n"; break;
                case 1038: debugFile << " Error in input data of antenna type (TR25-08), (Tx or Rx) !\n"; break;
                case 1039: debugFile << " Error in input data of correction factor according frequency difference !\n"; break;
                case 1040: debugFile << " Channel spacing outside definition range (Rx) !\n"; break;
                case 1041: debugFile << " Channel spacing outside definition range (Tx) !\n"; break;
                case 1042: debugFile << " Error in input data of elevation (Rx) !\n"; break;
                case 1043: debugFile << " Error in input data of azimuth (Rx) !\n"; break;
                case 1044: debugFile << " Error in type of antenna Rx (E/I)!\n"; break;
                case 1045: debugFile << " Error in input data of gain of Rx antenna !\n"; break;
                case 1046: debugFile << " Error in input data of depolarization loss !\n"; break;
                case 1047: debugFile << " Error in input value of borderline distance  (max. 999 km)!\n"; break;
                case 1048: debugFile << " Selected (border-) line data not available !\n"; break;
                case 1049: debugFile << " Error in line data !\n"; break;
                case 1050: debugFile << " No Agreement frequency or C_Mode and important input is missing !\n"; break;
                case 2000: debugFile << " Error in looking up FS-figures !\n"; break;
            }
        } else {
            debugFile << " Tx co-ordinates calculated     : " << param.Coo_Tx_new << "\n";
            if (param.C_Mode < 0) {
                if (param.D_to_border == 0) {
                    debugFile << " The maximum field strength at\n";
                    debugFile << "  the border line                : " << param.Calculated_FS << " dBuV/m\n";
                }
                if (param.D_to_border > 0) {
                    debugFile << " The maximum field strength at\n";
                    debugFile << "  the " << param.D_to_border << " km line                : " << param.Calculated_FS << " dBuV/m\n";
                }
                if (param.D_to_border < 0) {
                    debugFile << " The maximum field strength at\n";
                    debugFile << "  the cross border range         : " << param.Calculated_FS << " dBuV/m\n";
                }
                debugFile << " The co-ordinates of the\n";
                debugFile << "  (border-) line point are       : " << param.Coo_Rx_new << "\n";
                debugFile << " The distance to the (border-)\n";
                debugFile << "  line point is                  : " << param.Distance << " km.\n";
                debugFile << " Direction to the (border-)\n";
                debugFile << "  line point                     : " << param.Dir_Tx_Rx << " degr.\n";
                debugFile << "  Permissible field strength     : " << param.Perm_FS << " dBuV/m.\n";
                debugFile << "  The protection margin is       : " << param.Prot_margin << " dB.\n";
                if (param.D_to_border == 0) {
                    debugFile << " Calculation is performed on the borderline.\n";
                }
                if (param.D_to_border < 0) {
                    debugFile << " Calculation is performed on the cross border range.\n";
                }
                if (param.D_to_border > 0) {
                    debugFile << "  Calculation is performed on the " << param.D_to_border << " km-line.\n";
                }
                debugFile << "  Max. range of harmful interfer.: " << param.CBR_D << " km\n";
                debugFile << "  E.R.P of reference transmitter : " << param.ERP_ref_Tx << " dBW\n";
            } else {
                debugFile << " Rx co-ordinates calculated     : " << param.Coo_Rx_new << "\n";
                debugFile << "  Calculated field strength      : " << param.Calculated_FS << " dBuV/m\n";
                debugFile << "  Permissible field strength     : " << param.Perm_FS << " dBuV/m\n";
                debugFile << "  Protection margin\n";
                debugFile << "  (Perm_FS - Calculated_FS)      : " << param.Prot_margin << " dB\n";
                debugFile << "  Distance Tx -> Rx              : " << param.Distance << " km\n";
            }
            debugFile << "  Distance over sea              : " << param.D_sea_calculated << " km\n\n";
            debugFile << "                    Information :\n\n";

            if (Info[0]) {
                debugFile << " No height of Tx is given or Tx is mobile !\n";
                debugFile << " The height is taken from terrain database !\n\n";
            }
            if (Info[1]) {
                debugFile << " Height of transmitter differs from terrain database !\n\n";
            }
            if (Info[2]) {
                debugFile << " The transmitter height differs more than 10% \n";
                debugFile << " from the terrain database!\n";
                debugFile << " Calculations may be (extremely) wrong !\n\n";
            }
            if (Info[3]) {
                debugFile << " No HCM TX frequency or non strict C_Mode!\n";
                debugFile << " Permissible field strength, max CBR and\n";
                debugFile << " ERP of the ref. TX are taken from input!\n\n";
            }
            if (Info[4]) {
                debugFile << " Input value of permissible field strength is used !\n\n";
            }
            if (Info[5]) {
                debugFile << " Input value for the maximum crossborder range is used !\n\n";
            }
            if (Info[6]) {
                debugFile << " The field stength is set to 999.9, because of service area overlapping.\n\n";
            }
            if (Info[7]) {
                debugFile << " No height of Rx is given or Rx is mobile / line!\n";
                debugFile << " The height is taken from terrain database !\n\n";
            }
            if (Info[8]) {
                debugFile << " Height of receiver differs from terrain database !\n\n";
            }
            if (Info[9]) {
                debugFile << " The receiver height differs more than 10% from the terrain database!\n";
                debugFile << " Calculations may be (extremely) wrong !\n\n";
            }
            if (Info[10]) {
                debugFile << " Free space field strength is used because distance is less than 1 km !\n\n";
            }
            if (Info[11]) {
                debugFile << " Free space field strength is used because first fresnel zone is free !\n\n";
            }
            if (Info[12]) {
                debugFile << " Distance over sea is greater than total distance; distance between\n";
                debugFile << " Tx and Rx is used !\n\n";
            }
            if (Info[13]) {
                debugFile << " Input value of corr. factor accord. frequency difference is used !\n\n";
            }
            if (Info[14]) {
                debugFile << " Frequency difference outside definition range !\n";
                debugFile << " 82 dB value is used for correction factor !\n\n";
            }
            if (Info[15]) {
                debugFile << " Calculated distance over sea is set to 0\n";
                debugFile << " because of missing morphological data !\n\n";
            }
            if (Info[16]) {
                debugFile << " Channel spacing outside curve range, broadband formula is used!\n";
            }
            if (Info[17]) {
                debugFile << " Correction factors for the band 380 - 400 MHz are used.\n";
            }
            debugFile << "\n\n";
            debugFile << "              Additional output :\n\n";
            debugFile << "  Calc. Time                     : " << TimeElapsed.count() << " s\n";
            debugFile << "  Height of Tx (terrain database): " << param.H_Datab_Tx << " m\n";
            debugFile << "  Transmitter clearance angle    : " << param.Tx_TCA << " degree\n";
            debugFile << "  Tx clearance angle corr. factor: " << param.Tx_TCA_corr << " dB\n";
            debugFile << "  Effective antenna height of Tx : " << param.Heff_Tx << " m\n";
            debugFile << "  Effective antenna height of Rx : " << param.Heff_Rx << " m\n";
            debugFile << "  Effective antenna height (CCIR): " << param.Heff << " m\n";
            debugFile << "  Delta height  (CCIR)           : " << param.Dh << " m\n";
            debugFile << "  Correct. factor delta h (CCIR) : " << param.Dh_corr << " dB\n";
            debugFile << "  Values from the " << "Time_percentage" << "% curves\n";
            debugFile << "   Land field strength           : " << param.Land_FS << " dBuV/m\n";
            debugFile << "   Sea field strength (temp=" << param.Sea_temperature << ")   : " << param.Sea_FS << " dBuV/m\n";
            debugFile << "   Correction factor Tx antenna   : " << param.Tx_ant_corr << " dB\n";
            debugFile << "   Corr. factor antenna type Tx   : " << param.Tx_ant_type_corr << " dB\n";
            debugFile << "   Direction Tx -> Rx             : " << param.Dir_Tx_Rx << " degree\n";
            debugFile << "   Angle vertical Tx to Rx        : " << param.V_angle_Tx_Rx << " degree\n";
            debugFile << "   Diff.angle hori. (Tx->Rx - Azi): " << param.H_diff_angle_Tx_Rx << " degree\n";
            debugFile << "   Diff.angle vert. (Tx->Rx - Ele): " << param.V_diff_angle_Tx_Rx << " degree\n";
            if (param.C_Mode >= 0) {
                debugFile << "   Height of Rx (terrain database): " << param.H_Datab_Rx << " m\n";
                debugFile << "   Receiver clearance angle       : " << param.Rx_TCA << " degree\n";
                debugFile << "   Rx clearance angle corr. factor: " << param.Rx_TCA_corr << " dB\n";
                debugFile << "   Direction Rx -> Tx             : " << param.Dir_Rx_Tx << " degree\n";
                debugFile << "   Angle vertical Rx to Tx        : " << param.V_angle_Rx_Tx << " degree\n";
                debugFile << "   Diff.angle hori.(Rx->Tx - AziR): " << param.H_diff_angle_Rx_Tx << " degree\n";
                debugFile << "   Diff.angle vert.(Rx->Tx - EleR): " << param.V_diff_angle_Rx_Tx << " degree\n";
                debugFile << "   Correction factor Rx antenna   : " << param.Rx_ant_corr << " dB\n";
                debugFile << "   Corr. factor antenna type Rx   : " << param.Rx_ant_type_corr << " dB\n";
                debugFile << "   Delta frequency                : " << param.Delta_frequency / 1000.0 << " kHz\n";
                debugFile << "   Cannel spacing of Rx           : " << param.Channel_sp_Rx / 1000.0 << " kHz\n";
                debugFile << "   Cannel spacing of Tx           : " << param.Channel_sp_Tx / 1000.0 << " kHz\n";
                debugFile << "   Corr. factor according delta f : " << param.Corr_delta_f << " dB\n";
                debugFile << "   Depolarization loss            : " << param.Depol_loss << " dB\n";
            }
            debugFile << "   Permissible field str. of table\n";
            debugFile << "   (0, if there is an input)      : " << param.Perm_FS_from_table << " dBuV/m\n";
            debugFile << "   Power in direction of Rx       : " << (param.Power_to_Rx - param.Tx_ant_corr) << " dBW\n";
            debugFile << "   Free space field strength      : " << param.Free_space_FS << " dBuV/m\n\n\n\n";
          
                debugFile << "\n Number of profile points  " << param.PN << "\n";
                debugFile << " profile sampling distance " << param.PD << " km\n\n";
                try{
	                if (param.HCM_error == 0){
		                for (int i = 0; i < param.PN; i++) {
		                    debugFile << i << "\t" << param.T_Prof[i] << "\n" ;
		                }
	                }
                } catch(...){
                    
                };

        }
        debugFile.close();
    }
}
