//!
//!	Dh_calculation
//!	Subroutine to calculate the terrain irregularity 'Dh'.
//!
//!	Input values:
//!				Distance    Distance in km
//!				T_Prof(i)	Height of the profile point(i) [first point (i = 1) is
//!							the height of the transmitter site, last point = height
//!							of the receiver site] from subroutine 'PROFILE'
//!				PD			Point distance (gridsize) of profile in km
//!				PN			Number of profile points from subroutine 'PROFILE'
//!				
//!
//!
//!	Output values:
//!				Dh			Terrain irregularity in meter


	
#ifndef _DhCalcH
#define _DhCalcH

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

#include "HCM4AMobileServiceStruct.h"


void DhCalculation(HCM4AMobileService_Parameters& param) {
	std::vector<int> HI(1670);
	int PStart, PStop;
	int I, J, K, L, N, S1, S2, S3;

	// Prepare useful values
	N = 0;
	I = static_cast<int>(std::round(4.5 / param.PD));
	L = static_cast<int>(std::round(25.0 / param.PD));

	// Starting number of profile point 'PStart' and ending number 'PStop'
	if (param.Distance <= 50.0) {
		// Between 10 km and 50 km: Normal calculation minus two times 4.5 km (transmitter and receiver).
		PStart = I + 1;
		PStop = param.PN - I;
		for (J = PStart; J <= PStop; ++J) {
			HI[N++] = param.T_Prof[J];
		}
	} else {
		// Distance > 50 km
		// Calculation of two parts: 4.5 to 25 km and distance - 25 km to distance - 4.5 km.
		// 1st part:
		PStart = I + 1;
		PStop = L + 1;
		for (J = PStart; J <= PStop; ++J) {
			HI[N++] = param.T_Prof[J];
		}
		// 2nd part:
		PStart = param.PN - L;
		PStop = param.PN - I;
		for (J = PStart; J <= PStop; ++J) {
			HI[N++] = param.T_Prof[J];
		}
	}

	// Sort all heights:
	S1 = N / 2;
	while (S1 != 0) {
		S2 = N - S1;
		for (S3 = 1; S3 <= S2; ++S3) {
			for (K = S3; K >= 1; K -= S1) {
				L = K + S1;
				if (HI[K] < HI[L]) break;
				std::swap(HI[K], HI[L]);
			}
		}
		S1 /= 2;
	}

	// Subtract 10% at the top and at the bottom:
	// Remaining difference between the lowest and the highest value is the value of 'Dh':
	if (N < 5) {
		param.Dh = static_cast<double>(HI[N - 1] - HI[0]);
	} else {
		param.Dh = static_cast<double>(HI[N - static_cast<int>(std::round(static_cast<double>(N) / 10.0))] - HI[static_cast<int>(std::round(static_cast<double>(N) / 10.0)) - 1]);
	}
}


#endif