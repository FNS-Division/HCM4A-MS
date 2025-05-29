//!
//!	Line_calculation.							
//!												
//!	Subroutine to calculate the field strengs on a line
//!	or to calculate the cross border field strength.
//!
//!	Input values
//!
//!	Land_to			Country to calculate to
//!	Land_from		Country of Tx
//!	D_to_border		Distance to borderline (input)
//!					(negativ = Cross-border range calculation)      
//!	Border_path		Path where border data are stored
//!	B_L				Length of Border_path
//!	LongTx			Longitude of Tx
//!	LatTx			Latitude of Tx
//!
//!
//!	Output values
//!	HCM_error		Error value
//!	Calculated_FS	Calculated field strength
//!
//!	HCM_error values:
//!
//!	1047	Distance to borderline is too long
//!	1048	Selected line data not available
//!	1049	Error in (border-) line data

#ifndef _LineCalc_h
#define _LineCalc_h


#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <string>
#include <algorithm>

#include "HCM4AMobileServiceStruct.h"
#include "Calc_distance.h"
#include "Calc_direction.h"
#include "Position_of_mobile.h"
#include "P_to_P_calculation.h"
#include "HcmTerrain.h"



#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <vector>
#include <algorithm>




//Structure to hold coordinates
struct Coordinate {
    double longitude;
    double latitude;
};

//int Rec_N_list[3], Rec_N_list1[3];
//double FS_list[3], FS_list1[3];




void CBR_Coordinates(double& LongLi, double& LatLi, double& LongTx, double& LatTx, double d2b, double Tx_serv_area, HCM4AMobileService_Parameters& param) {
    double Lo, La;

    // Calculate the direction from Tx to Rx:
    CalcDirection(LongTx, LatTx, LongLi, LatLi, param.Dir_Tx_Rx);

    // In case of Tx is a mobile, calculate new Tx coordinates:
    if (Tx_serv_area > 0.0) {
        // Calculate the distance 'Distance' between point A and B
        CalcDistance(LongTx, LatTx, LongLi, LatLi, param.Distance);
        // Calculate new Tx coordinates like p2p:
        Calc_Tx_pos(LongTx, LatTx, LongLi, LatLi, Lo, La, Tx_serv_area, param);
        LongTx = Lo;
        LatTx = La;
	}
					
    if (d2b < 0.0) {
        New_coordinates(LongLi, LatLi, param.Dir_Tx_Rx, std::abs(d2b), LongLi, LatLi);
    }
}


bool Test_cut1(double LONG, double LAT, double N_LONG, double N_LAT, std::vector<Coordinate>& Co_cp, int N_cp) {
    double AX = LONG;
    double AY = LAT;
    double BX = N_LONG;
    double BY = N_LAT;

    int I = 2;
    // Take first two points:
    double CX = Co_cp[0].longitude;
    double CY = Co_cp[0].latitude;
    double DX = Co_cp[1].longitude;
    double DY = Co_cp[1].latitude;

    // Determine intersection:
    while (true) {
        double RN = (BX - AX) * (DY - CY) - (BY - AY) * (DX - CX);
        if (RN == 0.0) {
            CX = DX;
            CY = DY;
            I += 1;
            if (I > N_cp) 
            	return true;
            DX = Co_cp[I].longitude;
            DY = Co_cp[I].latitude;
            continue;
        }
        double R = ((AY - CY) * (DX - CX) - (AX - CX) * (DY - CY)) / RN;
        double S = ((AY - CY) * (BX - AX) - (AX - CX) * (BY - AY)) / RN;
        if ((R >= 0.0) && (R <= 1.0) && (S >= 0.0) && (S <= 1.0)) {
            return false;
        }

        CX = DX;
        CY = DY;
        I += 1;
        if (I > N_cp) 
        	return true;
        DX = Co_cp[I].longitude;
        DY = Co_cp[I].latitude;
    }
}


int count_in_Nlist = 0;
std::vector<int> recNList(3), recNList1(3);
std::vector<double> fsList(3), fsList1(3);

void Manage_List(int N_rec, double Calculated_FS, bool cbr ) {
	
	if (cbr){
	    if (count_in_Nlist < 3) {
	        recNList[count_in_Nlist] = N_rec;
	        fsList[count_in_Nlist] = Calculated_FS;
	        count_in_Nlist++;
	    } else {
	        // More than MAX entries:
	        // Find list-number of lowest field strength:
	        int J = 0;
	        double FS_x = 999.9;
	        for (int I = 0; I < 3; ++I) {
	            if (fsList[I] < FS_x) {
	                J = I;
	                FS_x = fsList[I];
	            }
	        }
	        // J = number of list entry with the lowest field strength
	        if (Calculated_FS > fsList[J]) {
	            fsList[J] = Calculated_FS;
	            recNList[J] = N_rec;
	        }
	    }
	    return;
		
		
	}
    if (count_in_Nlist < 3) {
        recNList[count_in_Nlist] = N_rec;
        fsList[count_in_Nlist] = Calculated_FS;
        count_in_Nlist++;
    } else {
        // More than MAX entries:
        // Find list-number of lowest field strength:
        int J = 0;
        double FS_x = 999.9;
        for (int I = 0; I < 3; ++I) {
            if (fsList[I] < FS_x) {
                J = I;
                FS_x = fsList[I];
            }
        }
        // J = number of list entry with the lowest field strength
        if (Calculated_FS > fsList[J]) {
            fsList[J] = Calculated_FS;
            recNList[J] = N_rec;
        }
    }
}


void LineCalculation(double LongTx, double LatTx, double LongRx, double LatRx, double Tx_serv_area, 
	double Time_percentage, double Tx_frequency, double hTx, double hRx, HCM4AMobileService_Parameters& param, HcmTerrain& ter ) {
		
    int ios, nRec,  nCp, nAll;
    int nList1, teststep;
    int i, j, k, recNX, recX;
    std::vector<double> nRecord(22), nFile(30800);
    double  fsX, d2b;
    std::string borderFile, tmpS;
    bool takeIt, testCut1;
    std::vector<Coordinate> coCp(1400);
	double longRxTmp, latRxTmp;
	int shift = 0;
	
	double RB = 180.0 / M_PI;
    bool flp = false;
    recNX = -1;
    fsX = -999.9;
	int N_List = 0;
	int N_List1 = 0;
	
	double ddd = 0;

	
	
//	P_to_P_calculation(LongTx, LatTx, 9.510556, 47.5125, Time_percentage, Tx_frequency, Tx_frequency, false, hTx, hRx, param);
//	return;



	// Select line data
	std::string BorderFile = std::string(param.Land_from) + std::string(param.Land_to) + ".";

	// 1. Cross border range:
	d2b = param.D_to_border;
	if (param.CBR_D >= 0) {
		d2b = 0.0 - param.CBR_D;
		BorderFile += "cbr";
	} else {
		if (param.D_to_border > 999) {
			param.HCM_error = 1047;
			// Distance to borderline is too long
			return;
		} else {
			tmpS = std::to_string(param.D_to_border);
			if (tmpS.length() == 1){ BorderFile += "00" + tmpS;};
			if (tmpS.length() == 2){ BorderFile += "0" + tmpS;};
			if (tmpS.length() == 3){ BorderFile += tmpS;};
		}
	}
	//d2b = param.D_to_border;

	// Get additionally all borderline centerpoints of the affected country:
	std::ifstream file(param.Border_path + BorderFile.substr(0, 7) + "000", std::ios::binary);
	if (!file) {
		param.HCM_error = 1048;
		// Selected line data not available
		return;
	}
    // Store all center points table 'Co_cp(i,j)'
    nCp = 0;
    while (nCp < 1400 && file.read(reinterpret_cast<char*>(nRecord.data()), 176)) {
        coCp[nCp].longitude = nRecord[20] * RB;
        coCp[nCp].latitude = nRecord[21] * RB;
        nCp++;
    }
    file.close();

    nCp--;

	file.open(param.Border_path + BorderFile, std::ios::binary);
	    if (!file) {
	        param.HCM_error = 1048;
	        // Selected line data not available
	        return;
	    }

    // Open line file and cache full line
    nAll = 0;
    while (nAll < 1400 && file.read(reinterpret_cast<char*>(&nFile[nAll * 22]), 176)) {
        nAll++;
    }
    file.close();


    nAll--;

    for (i = 0; i < 3; ++i) {
        fsList[i] = 0.0;
        recNList[i] = 0;
        recNList1[i] = 0;
		fsList[i] = 0.0;
    }



    if (d2b<0){
		count_in_Nlist = 0;
		fsX = -999;
		
		for (j = 0; j <= nAll; ++j) {// for all points of country edge
			longRxTmp = nFile[22 * j + 20] * RB;
			latRxTmp = nFile[22 * j + 21] * RB;
			
			double lo = LongTx;
			double la = LatTx;

			CBR_Coordinates(longRxTmp, latRxTmp, lo, la, d2b, Tx_serv_area, param);
			if (Test_cut1(longRxTmp, latRxTmp, lo, la, coCp, nCp)) continue;
			
			P_to_P_calculation(lo, la, longRxTmp, latRxTmp, Time_percentage, Tx_frequency, Tx_frequency, false, hTx, hRx, param, ter);
			if (param.HCM_error == 1028) continue; // Distance > 1000 km
			
            if (param.Calculated_FS >= fsX) {
                fsX = param.Calculated_FS;
                Manage_List(j, param.Calculated_FS, true);
            }
		}
		
		count_in_Nlist = 0;
		recNList1[0] = recNList[0];
		recNList1[1] = recNList[1];
		recNList1[2] = recNList[2];
		
		fsX = -999;
		double dlong, dlati = 0;
		double dlongTx, dlatiTx = 0;
		
		for (j = 0; j < 3; ++j) {
			for (i = 0; i < 20; i++) {
				longRxTmp = nFile[recNList1[j]*22 + i] * RB;
				latRxTmp = nFile[recNList1[j]*22 + i + 1] * RB;
				
				double lo = LongTx;
				double la = LatTx;

				CBR_Coordinates(longRxTmp, latRxTmp, lo, la, d2b, Tx_serv_area, param);
				if (Test_cut1(longRxTmp, latRxTmp, lo, la, coCp, nCp)) continue;
            
				P_to_P_calculation(lo, la, longRxTmp, latRxTmp, Time_percentage, Tx_frequency, Tx_frequency, false, hTx, hRx, param, ter);
				if (param.HCM_error == 1028) continue; // Distance > 1000 km
				
		        if (param.Calculated_FS >= fsX) {
		            fsX = param.Calculated_FS;
		            dlong = longRxTmp;
		            dlati = latRxTmp;
		            dlongTx = lo;
		            dlatiTx = la;
		        }
			
			}
		}
		
		fsX = -999;
		P_to_P_calculation(dlongTx, dlatiTx, dlong, dlati, Time_percentage, Tx_frequency, Tx_frequency, false, hTx, hRx, param, ter);
		return;

     



   
    }

















    // Check if file 'HCM_LP' exists
    std::ifstream checkFile("HCM_LP");
    takeIt = checkFile.good();
    checkFile.close();

    if (!takeIt) goto label80;

    // Test routine to calculate to each point

    for (j = 0; j <= nAll; ++j) {
        for (k = 0; k < 20; k += 2) {
            longRxTmp = nFile[22 * j + k] * RB;
            latRxTmp = nFile[22 * j + k + 1] * RB;
            
            double lo = LongTx;
            double la = LatTx;
            CBR_Coordinates(longRxTmp, latRxTmp, lo, la, d2b, Tx_serv_area, param);
            
            //if ((param.D_to_border != 0) && Test_cut1(longRxTmp, latRxTmp, lo, la, coCp, nCp)) continue;
            if (Test_cut1(longRxTmp, latRxTmp, lo, la, coCp, nCp)) continue;

            P_to_P_calculation(lo, la, longRxTmp, latRxTmp, Time_percentage, Tx_frequency, Tx_frequency, false, hTx, hRx, param, ter);
            if (param.HCM_error == 1028) continue; // Distance > 1000 km

            if (param.Calculated_FS >= fsX) {
                fsX = param.Calculated_FS;
                recX = k;
                recNX = j;
                if (param.HCM_error != 0 || param.Info[7]) goto label75;
            }
        }
    }

label75:
    goto label140;

label80:
    if (param.D_to_border > 0) {
        teststep = 1;
        nRec = 0;
    } else {
        teststep = 5;
        nRec = 2;
    }
    shift = nRec;

label90:
	
    for (nRec = shift; nRec <= nAll - shift; nRec += teststep) {
        longRxTmp = nFile[22 * nRec + 20] * RB;
        latRxTmp = nFile[22 * nRec + 21] * RB;
        
        double lo = LongTx;
        double la = LatTx;
        CBR_Coordinates(longRxTmp, latRxTmp, lo, la, d2b, Tx_serv_area, param);

        
        if ((param.D_to_border != 0) && (Test_cut1(longRxTmp, latRxTmp, lo, la, coCp, nCp ))) continue;
        P_to_P_calculation(lo, la, longRxTmp, latRxTmp, Time_percentage, Tx_frequency, Tx_frequency, false, hTx, hRx, param, ter);
        
        if (param.HCM_error == 1028) continue; // Distance > 1000 km

        Manage_List(nRec, param.Calculated_FS, false);
        if (param.HCM_error != 0 || param.Info[7]) goto label125;
    }

    if (teststep == 5) {
        if (count_in_Nlist == 0) {
            teststep = 1;
            nRec = 0;
            goto label90;
        } else {
            nList1 = count_in_Nlist;
            count_in_Nlist = 0;
            for (i = 0; i < 3; ++i) {
                recNList1[i] = recNList[i];
                recNList[i] = 0;
                fsList1[i] = fsList[i];
                fsList[i] = 0.0;
            }
        }

        for (i = 0; i < nList1; ++i) {
            shift = recNList1[i];
            
            for (j = shift - 2; j <= shift + 2; ++j) {
                if (j == recNList1[i]) {
                    param.Calculated_FS = fsList1[i];
                } else {
                   	
                    longRxTmp = nFile[j * 22 + 20] * RB;
                    latRxTmp = nFile[j * 22 + 21] * RB;
                    double lo = LongTx;
                    double la = LatTx;
                    CBR_Coordinates(longRxTmp, latRxTmp, lo, la, d2b, Tx_serv_area, param);

                    //if ((param.D_to_border != 0) && 
                    //РАБОТАЛО if (Test_cut1(longRxTmp, latRxTmp, lo, la, coCp, nCp )) continue;
                    
                    if ((param.D_to_border != 0) && (Test_cut1(longRxTmp, latRxTmp, lo, la, coCp, nCp ))) continue;
                    
                    P_to_P_calculation(lo, la, longRxTmp, latRxTmp, Time_percentage, Tx_frequency, Tx_frequency, false, hTx, hRx, param, ter);
                    if (param.HCM_error == 1028) continue; // Distance > 1000 km
                }
                Manage_List(j,  param.Calculated_FS, false);
                if (param.HCM_error != 0 || param.Info[7]) goto label125;
            }
        }
    }

label125:
    fsX = -999.9;
    if (count_in_Nlist > 0) {
        for (i = 0; i < count_in_Nlist; ++i) {
            j = recNList[i];
            for (k = 0; k < 20; k += 2) {
                longRxTmp = nFile[22 * j + k] * RB;
                latRxTmp = nFile[22 * j + k + 1] * RB;
                double lo = LongTx;
                double la = LatTx;
                CBR_Coordinates(longRxTmp, latRxTmp, lo, la, d2b, Tx_serv_area, param);

                //if ((param.D_to_border != 0) && Test_cut1(longRxTmp, latRxTmp, lo, la, coCp, nCp )) continue;
                if ((param.D_to_border != 0) && (Test_cut1(longRxTmp, latRxTmp, lo, la, coCp, nCp ))) continue;

                P_to_P_calculation(lo, la, longRxTmp, latRxTmp, Time_percentage, Tx_frequency, Tx_frequency, false, hTx, hRx, param, ter);
                if (param.HCM_error == 1028) continue; // Distance > 1000 km

                if (param.Calculated_FS >= fsX) {
                    fsX = param.Calculated_FS;
                    recX = k;
                    recNX = j;
                    if (param.HCM_error != 0 || param.Info[7]) goto label140;
                }
            }
        }
    }

label140:
    if (recNX >= 0) {
        longRxTmp = nFile[22 * recNX + recX] * RB;
        latRxTmp = nFile[22 * recNX + recX + 1] * RB;
        double lo = LongTx;
        double la = LatTx;
        CBR_Coordinates(longRxTmp, latRxTmp, lo, la, d2b, Tx_serv_area, param);

        flp = true;
        P_to_P_calculation(lo, la, longRxTmp, latRxTmp, Time_percentage, Tx_frequency, Tx_frequency, false, hTx, hRx, param, ter);
    } else {
        param.HCM_error = 1047;
    }
}







#endif