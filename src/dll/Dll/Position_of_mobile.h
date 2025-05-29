//!	Subroutine to calculate the new position of Tx (New_LongTx, New_LatTx)
//!	and/or Rx (New_LongRx, New_latRx) if at least one is a mobile and
//!	to test the overlapping situation.
//!
//!	Possible HCM_Errors:
//!	1036	The 'xxx.ALL' borderline file for Tx is missing
//!	1037	The 'xxx.ALL' borderline file for Rx is missing
//!
//!	Info(7)	Calculated field strength is set to 999.9 because
//!			the distance is 0 or less than the radius of the
//!			service areas (overlapping).
//!
//!	**********************************************************************
//!
//	SUBROUTINE Position_of_mobile ( LongTx, LatTx, LongRx, LatRx, &
//					New_LongTx, New_LatTx, New_LongRx, New_LatRx )
//!
//	IMPLICIT			NONE
//!
//!	Include the interface definitions:
//	INCLUDE				'HCM_MS_V7_definitions.F90'
//!
//!
//	DOUBLE PRECISION	LongTx, LatTx, LongRx, LatRx
//	DOUBLE PRECISION	New_LongTx, New_LatTx, New_LongRx, New_LatRx
//!
//	REAL				DP1
//	INTEGER				N_Cut_Rx,N_Cut_Tx
//!
//!	************************************************************************

#ifndef _Position_of_mobile_h
#define _Position_of_mobile_h

#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <utility>

#include "Calc_direction.h"
#include "Calc_distance.h"
#include "HCM4AMobileServiceStruct.h"


		
					
void New_coordinates(double longitude, double latitude, double direction, double distance, double &nLong, double &nLat) {
    if (distance == 0.0) {
        nLong = longitude;
        nLat = latitude;
        return;
    }
	double degtorad = M_PI / 180.0;
	double radtodeg = 180.0 / M_PI;
	
    double dp = distance / 111.2;
    double t1 = cos(latitude * degtorad) * sin(dp * degtorad);
    double t2 = (sin(latitude * degtorad) * cos(dp * degtorad)) / t1;
    double t = t1 * (cos(direction * degtorad) + t2);
    if (t > 1.0) t = 1.0;
    if (t < -1.0) t = -1.0;
    nLat = asin(t) * radtodeg;

    if (direction == 0.0 || direction == 360.0 || direction == 180.0) {
        nLong = longitude;
        return;
    }

    t = (cos(dp * degtorad) - sin(latitude * degtorad) * sin(nLat * degtorad)) / (cos(latitude * degtorad) * cos(nLat * degtorad));
    if (t > 1.0) t = 1.0;
    if (t < -1.0) t = -1.0;
    nLong = acos(t) * radtodeg;
    if (direction < 180.0) nLong = longitude + nLong;
    if (direction > 180.0) nLong = longitude - nLong;
}

static std::map<std::string, std::vector<std::vector<std::pair<double, double>>>> borderCache;
void TestCut(double dir, double longitude, double latitude,
             double serviceArea, int &nCut, HCM4AMobileService_Parameters& param) {
    // Глобальный кэш для хранения данных границ
    static std::map<std::string, std::vector<std::vector<std::pair<double, double>>>> borderCache;
    static const double B = 180.0 / M_PI;

    nCut = 0;
    double nLong, nLat;
    New_coordinates(longitude, latitude, dir, serviceArea, nLong, nLat);
    double AX = longitude;  // Начальная точка (долгота)
    double AY = latitude;   // Начальная точка (широта)
    double BX = nLong;      // Конечная точка (долгота)
    double BY = nLat;       // Конечная точка (широта)

    std::string filename = param.Border_path + param.Land_from + ".all";

    // Загрузка данных в кэш при первом обращении
    if (borderCache.find(filename) == borderCache.end()) {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            param.HCM_error = 1036;
            return;
        }

        union BorderRecord {
            char bytes[176];
            double coords[22];
        } buffer;

        std::vector<std::vector<std::pair<double, double>>> records;
        while (file.read(buffer.bytes, 176)) {
            std::vector<std::pair<double, double>> points;
            for (int i = 0; i < 22; i += 2) {
                double x = buffer.coords[i] * B;   // Преобразование в градусы
                double y = buffer.coords[i+1] * B;
                points.emplace_back(x, y);
            }
            records.push_back(points);
        }
        file.close();
        borderCache[filename] = std::move(records);
    }

    // Получение данных из кэша
    const auto& records = borderCache.at(filename);

    // Проверка пересечений с границами
    for (const auto& record : records) {
        for (size_t i = 1; i < record.size()-1; ++i) {
            double CX = record[i-1].first;  // Предыдущая точка границы
            double CY = record[i-1].second;
            double DX = record[i].first;    // Текущая точка границы
            double DY = record[i].second;

            // Вычисление параметров пересечения
            const double RN = (BX - AX) * (DY - CY) - (BY - AY) * (DX - CX);
            if (RN != 0.0) {
                const double R = ((AY - CY) * (DX - CX) - (AX - CX) * (DY - CY)) / RN;
                const double S = ((AY - CY) * (BX - AX) - (AX - CX) * (BY - AY)) / RN;
                if (R >= 0.0 && R <= 1.0 && S >= 0.0 && S <= 1.0) {
                    nCut++;
                }
            }
        }
    }
}

void NearestLinePoint(double longitude, double latitude, double &nLong, double &nLat, 
                      double longX, double latX, double serviceArea, HCM4AMobileService_Parameters& param) {
    double maxdi = 1.0e7;
    static const double B = 180.0 / M_PI;
    std::string filename = param.Border_path + param.Land_from + ".all";

    // Загрузка данных в кэш при первом обращении
    if (borderCache.find(filename) == borderCache.end()) {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            param.HCM_error = 1036;
            return;
        }

        union BorderRecord {
            char bytes[176];
            double coords[22];
        } buffer;

        std::vector<std::vector<std::pair<double, double>>> records;
        while (file.read(buffer.bytes, 176)) {
            std::vector<std::pair<double, double>> points;
            for (int i = 0; i < 22; i += 2) { // Все 11 точек в записи
                points.emplace_back(buffer.coords[i] * B, buffer.coords[i+1] * B);
            }
            records.push_back(points);
        }
        file.close();
        borderCache[filename] = std::move(records);
    }


    const auto& records = borderCache.at(filename);

 
	double dis, di;
	for (const auto& record : records) {
	    // Пропускаем последний элемент в record
	    for (size_t i = 0; i < record.size() - 1; ++i) {
	        const auto& [x, y] = record[i];
	        CalcDistance(longX, latX, x, y, di);
	        if (di <= serviceArea) {
	            CalcDistance(longitude, latitude, x, y, dis);
	            if (dis < maxdi) {
	                maxdi = dis;
	                nLong = x;
	                nLat = y;
	            }
	        }
	    }
	}
}



void Calc_Tx_pos(double longTx, double latTx, double longRx, double latRx, 
				double &newLongTx, double &newLatTx, double Tx_serv_area, HCM4AMobileService_Parameters& param) {
					
    double dp1 = param.Distance;
    if (dp1 > Tx_serv_area) dp1 = Tx_serv_area;
    int nCut;
    TestCut(param.Dir_Tx_Rx, longTx, latTx, dp1, nCut, param);
			
    if (nCut % 2 == 0) {
        New_coordinates(longTx, latTx, param.Dir_Tx_Rx, dp1, newLongTx, newLatTx);
    } else {
        NearestLinePoint(longRx, latRx, newLongTx, newLatTx, longTx, latTx, Tx_serv_area, param);
    }
}

void Calc_Rx_pos(double longTx, double latTx, double longRx, double latRx, double &newLongRx, 
					double &newLatRx, double Rx_serv_area,  HCM4AMobileService_Parameters& param) {
    double dp1 = param.Distance;
    if (dp1 > Rx_serv_area) dp1 = Rx_serv_area;
    int nCut;
    TestCut(param.Dir_Rx_Tx, longRx, latRx, dp1, nCut, param);
    if (nCut % 2 == 0) {
        New_coordinates(longRx, latRx, param.Dir_Rx_Tx, dp1, newLongRx, newLatRx);
    } else {
        NearestLinePoint(longTx, latTx, newLongRx, newLatRx, longRx, latRx, Rx_serv_area, param);
    }
}




void PositionOfMobile(double LongTx, double LatTx, double LongRx, double LatRx,
                     double &newLongTx, double &newLatTx, double &newLongRx, double &newLatRx,
                     double Tx_serv_area, double Rx_serv_area, bool p2p, HCM4AMobileService_Parameters& param) {
    int nCutTx = 0;
    int nCutRx = 0;

    CalcDistance(LongTx, LatTx, LongRx, LatRx, param.Distance);
    CalcDirection(LongRx, LatRx, LongTx, LatTx, param.Dir_Rx_Tx);
    CalcDirection(LongTx, LatTx, LongRx, LatRx, param.Dir_Tx_Rx);

    if (Tx_serv_area > 0.0 && param.C_Mode < 0) {
        Calc_Tx_pos(LongTx, LatTx, LongRx, LatRx, newLongTx, newLatTx, Tx_serv_area, param );
        return;
    }

    if (Tx_serv_area > 0.0) {
        double dp1 = param.Distance;
        if (dp1 > Tx_serv_area) dp1 = Tx_serv_area;
        if (param.D_to_border == 0 && !p2p) {
            nCutTx = 1;
        } else {
            TestCut(param.Dir_Tx_Rx, LongTx, LatTx, dp1, nCutTx, param);
            if (param.HCM_error != 0) {
                param.HCM_error = 1036;
                return;
            }
        }
    }

    if (Rx_serv_area > 0.0) {
        double dp1 = param.Distance;
        if (dp1 > Rx_serv_area) dp1 = Rx_serv_area;
        TestCut(param.Dir_Rx_Tx, LongRx, LatRx, dp1, nCutRx, param);
        if (param.HCM_error != 0) {
            param.HCM_error = 1037;
            return;
        }
    }

    if (nCutTx == 0 && nCutRx == 0) {
        if (param.Distance <= Tx_serv_area + Rx_serv_area) {
            param.Distance = 0.0;
            param.Info[7] = true;
        } else {
            if (Tx_serv_area > 0.0) {
                New_coordinates(LongTx, LatTx, param.Dir_Tx_Rx, Tx_serv_area, newLongTx, newLatTx);
            }
            if (Rx_serv_area > 0.0) {
                New_coordinates(LongRx, LatRx, param.Dir_Rx_Tx, Rx_serv_area, newLongRx, newLatRx);
            }
        }
        return;
    }

    if (Tx_serv_area > 0.0 && Rx_serv_area > 0.0) {
        if (Tx_serv_area > Rx_serv_area) {
            Calc_Tx_pos(LongTx, LatTx, LongRx, LatRx, newLongTx, newLatTx, Tx_serv_area, param );
            CalcDistance(newLongTx, newLatTx, LongRx, LatRx, param.Distance);
            CalcDirection(newLongTx, newLatTx, LongRx, LatRx, param.Dir_Tx_Rx);
            CalcDirection(LongRx, LatRx, newLongTx, newLatTx, param.Dir_Rx_Tx);
            Calc_Rx_pos(newLongTx, newLatTx, LongRx, LatRx, newLongRx, newLatRx, Rx_serv_area, param );
        } else {
            Calc_Rx_pos(LongTx, LatTx, LongRx, LatRx, newLongRx, newLatRx, Rx_serv_area, param );
            CalcDistance(newLongRx, newLatRx, LongTx, LatTx, param.Distance);
            CalcDirection(LongTx, LatTx, newLongRx, newLatRx, param.Dir_Tx_Rx);
            CalcDirection(newLongRx, newLatRx, LongTx, LatTx, param.Dir_Rx_Tx);
            Calc_Tx_pos(LongTx, LatTx, newLongRx, newLatRx, newLongTx, newLatTx, Tx_serv_area, param );
        }
    } else if (Tx_serv_area > 0.0) {
        Calc_Tx_pos(LongTx, LatTx, LongRx, LatRx, newLongTx, newLatTx, Tx_serv_area, param );
    } else if (Rx_serv_area > 0.0) {
        Calc_Rx_pos(LongTx, LatTx, LongRx, LatRx, newLongRx, newLatRx, Rx_serv_area, param );
    }
}



#endif