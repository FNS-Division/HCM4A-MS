//!	This subroutine constructs a terrain- and morphological profile from point A to
//!	point B in steps of 100 m. The heights or morphological information are stored
//!	in '[T|M]_Prof(i)'. The total number of points is in 'PN'. The first profile point
//!	'Prof(1)' is the height or morphological type of point A, the last profile point
//!	'Prof(PN)' is the height or morphological type of point B.
//!
//!	Input values:
//!		LongA		DOUBLE PRECISION	longitude of point A (starting point)
//!		LatA		DOUBLE PRECISION	latitude of point A
//!		LongB		DOUBLE PRECISION	longitude of point B (ending point)
//!		LatB		DOUBLE PRECISION	latitude of point B
//!		PD			DOUBLE PRECISION	point distance (grid size) of the profile
//!		Topo_path	CHARACTER*63		path of topo database, e.g. 'D:\TOPO'
//!		Morpho_path	CHARACTER*63		path of morpho database, e.g. 'D:\MORPHO'
//!
//!
//!	Output values:
//!		Prof(i)		INTEGER*2(10002)	field with heights or morpho information
//!		PN			INTEGER*2			number of profile points 
//!		Error		INTEGER*4			error value
//!
//!	Possible error values
//!		no own, only from Point_Info
//!		36		error opening terrain- or morphological data file (data not available)
//!		220		error reading record (in 'Point_height' or 'Point_type' subroutine)
//!		400		height is missing (-9999) (in 'Point_height' subroutine)
//!
//!	Called subroutines : Point_Info	

#ifndef _ProfileH
#define _ProfileH

#include <cmath>
#include <vector>
#include <string>
#include "Calc_direction.h"
#include "Calc_distance.h"
#include "Point_info.h"



// Degree-based trigonometric functions
constexpr double DEG_TO_RAD = M_PI / 180.0;
constexpr double RAD_TO_DEG = 180.0 / M_PI;
inline double deg_sin(double deg) { return sin(deg * DEG_TO_RAD); }
inline double deg_cos(double deg) { return cos(deg * DEG_TO_RAD); }
inline double deg_asin(double x) { return asin(x) * RAD_TO_DEG; }
inline double deg_atan2(double y, double x) { return atan2(y, x) * RAD_TO_DEG; }

void GetProfile(bool p2p,double hTx,double hRx,  double LongA, double LatA, double LongB, double LatB, HCM4AMobileService_Parameters& param) {
    double H_Rx_ant, H_Tx_ant = 0;
    double K = 0.0;
    std::vector<int> ArLong;
    

    // Calculate total distance and direction
    CalcDistance(LongA, LatA, LongB, LatB, param.Distance);
    CalcDirection(LongA, LatA, LongB, LatB, param.Dir_Tx_Rx);
    CalcDirection(LongB, LatB, LongA, LatA, param.Dir_Rx_Tx);

    // Calculate number of points (100m resolution, max 1000km = 10000 points)
    // [0] = Tx point
    // [Pn-1] = Rx point
    param.PN = static_cast<int>(param.Distance / param.PD);
    // if Distance mod state.PN ~ 0 then add only one point to TX because RX stay at step point
	//  like distance 15000m / 100m that meens RX point = last point
	// else add to point for TX and RX
	
	double PDa = (param.Distance / param.PN)/ 6372.93; // Convert to angular distance
    // Initialize profile arrays
    
    try {
        H_Tx_ant = std::stod(param.H_Tx_ant);
    } catch (const std::invalid_argument&) {
        param.HCM_error = 1009;
        return;
    }
    try {
        H_Rx_ant = std::stod(param.H_Rx_ant);
    } catch (const std::invalid_argument&) {
        param.HCM_error = 1022;
        return;
    }
    
    // Handle slant profile setup
    if (p2p) {
        //K = (H_Rx_ant - H_Tx_ant) / param.PN;
        K = (hRx - hTx) / param.PN;
        param.T_Prof[0] = 0;
        param.T_Prof[param.PN + 1] = 0;
    } else {
        //param.T_Prof[0] = H_Tx_ant;
        param.T_Prof[0] = hTx;
    }
    param.PN = param.PN+1;
    

    double CLA,SLA,SDI,SDIR,CDIR,DD,SIDD,CODD,lat,dlon,lon,CLB,SLB,SDIR_R,CDIR_R;
    
    CLA = deg_cos(LatA);
    SLA = deg_sin(LatA);
    SDIR = deg_sin(param.Dir_Tx_Rx);
    CDIR = deg_cos(param.Dir_Tx_Rx);

	// starts 1
	
    for (int PC = 1; PC < param.PN/2; PC++) {
        DD = PC * PDa;
        SIDD = sin(DD);
        CODD = cos(DD);
        
        lat = deg_asin(SLA * CODD + CLA * SIDD * CDIR);
        dlon = deg_atan2(SDIR * SIDD * CLA, CODD - SLA * (SLA * CODD + CLA * SIDD * CDIR));
        lon = LongA + dlon;

        // Get terrain/morpho info
        PointInfoResult result = Point_info(lon, lat, param.Topo_path, param.Morpho_path);
        if (result.Error != 0) {
            param.HCM_error = result.Error;
            return;
        }

        param.T_Prof[PC] = result.Height;
        param.M_Prof[PC] = result.M_Type;

        if (p2p) {
            param.T_Prof[PC] = param.T_Prof[PC] - static_cast<int>(std::round(hTx + K * static_cast<double>(PC - 1)));
        }

    }
    
    double mLongB = LongB;
    double mLatB = LatB;

    /// Calculate coordinates from Rx to midpoint
    CLB = deg_cos(mLatB);
    SLB = deg_sin(mLatB);
    SDIR_R = deg_sin(param.Dir_Rx_Tx);
    CDIR_R = deg_cos(param.Dir_Rx_Tx);

    for (int PC = param.PN/2; PC < param.PN - 1; PC++) {
        DD = (param.PN - PC - 1) * PDa;
        SIDD = sin(DD);
        CODD = cos(DD);
        
        lat = deg_asin(SLB * CODD + CLB * SIDD * CDIR_R);
        dlon = deg_atan2(SDIR_R * SIDD * CLB, CODD - SLB * (SLB * CODD + CLB * SIDD * CDIR_R));
        lon = mLongB + dlon;

        // Get terrain/morpho info
        PointInfoResult result = Point_info(lon, lat, param.Topo_path, param.Morpho_path);
        if (result.Error != 0) {
            param.HCM_error = result.Error;
            return;
        }

        param.T_Prof[PC] = result.Height;
        param.M_Prof[PC] = result.M_Type;
        
        if (p2p) {
            param.T_Prof[PC] = param.T_Prof[PC] - static_cast<int>(std::round(hRx + K * static_cast<double>(param.PN - PC)));
        }

    }


    // Set final point
//    PointInfoResult result = Point_info(LongB, LatB, param.Topo_path, param.Morpho_path);
//    if (result.Error != 0) {
//        param.HCM_error = result.Error;
//        return;
//    }
//	param.T_Prof[param.PN-1] = result.Height;
//	param.M_Prof[param.PN-1] = result.M_Type;
//    if (p2p) {
//        param.T_Prof[param.PN-1] -= static_cast<int16_t>(H_Rx_ant - K * (param.PN - 1));
//    }

}


#endif