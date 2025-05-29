#ifndef _ErrorCodes_h
#define _ErrorCodes_h


#include <iostream>
#include <string>


struct ErrorMessage {
    int code;
    std::string message;
};

std::string getErrorMessage(int errorCode) {
    static const ErrorMessage errorMessages[] = {
        {0, "No error"},
        {36, "Error opening terrain- or morphological data file (data not available)"},
        {200, "Error in longitude (in 'Point_height' or 'Point_type' subroutine)"},
        {210, "Error in latitude (in 'Point_height' or 'Point_type' subroutine)"},
        {220, "Error reading record (in 'Point_height' or 'Point_type' subroutine)"},
        {300, "Latitude is not in range of 0.0 - 90.0 (in 'Point_height' or 'Point_type' subroutine)"},
        {400, "Height is missing (-9999) (in 'Point_height' subroutine)"},
        {1000, "Distance between Tx and Rx = 0. Calculations not possible"},
        {1001, "Error in geographical coordinates (Tx longitude, degrees)"},
        {1002, "Error in geographical coordinates (Tx longitude, minutes)"},
        {1003, "Error in geographical coordinates (Tx longitude, seconds)"},
        {1004, "Error in geographical coordinates (Tx longitude, E/W)"},
        {1005, "Error in geographical coordinates (Tx latitude, degrees)"},
        {1006, "Error in geographical coordinates (Tx latitude, minutes)"},
        {1007, "Error in geographical coordinates (Tx latitude, seconds)"},
        {1008, "Error in geographical coordinates (Tx latitude, N/S)"},
        {1009, "Error in Tx antenna height"},
        {1010, "Error in transmitting frequency value"},
        {1011, "Error in transmitting frequency unit"},
        {1012, "Error in radius of service area of Tx"},
        {1013, "Error in input value height of Tx site above sea level"},
        {1014, "Error in geographical coordinates (Rx longitude, degrees)"},
        {1015, "Error in geographical coordinates (Rx longitude, minutes)"},
        {1016, "Error in geographical coordinates (Rx longitude, seconds)"},
        {1017, "Error in geographical coordinates (Rx longitude, E/W)"},
        {1018, "Error in geographical coordinates (Rx latitude, degrees)"},
        {1019, "Error in geographical coordinates (Rx latitude, minutes)"},
        {1020, "Error in geographical coordinates (Rx latitude, seconds)"},
        {1021, "Error in geographical coordinates (Rx latitude, N/S)"},
        {1022, "Error in Rx antenna height"},
        {1023, "Error in reception frequency value"},
        {1024, "Error in reception frequency unit"},
        {1025, "C_mode is out of range"},
        {1026, "Error in input value of permissible field strength"},
        {1027, "Error in input value of maximum cross border range"},
        {1028, "The distance is greater than 1000 km. Calculations not possible"},
        {1029, "Error in radius of Rx service area"},
        {1030, "Error in input value Rx site height above sea level"},
        {1031, "Error in Tx elevation"},
        {1032, "Error in Tx azimuth"},
        {1033, "Error in type of Tx antenna (E/I)"},
        {1034, "Error in power"},
        {1035, "Error in input value of distance over sea"},
        {1036, "The 'xxx.ALL' borderline file for Tx is missing"},
        {1037, "The 'xxx.ALL' borderline file for Rx is missing"},
        {1038, "Error in type of antenna (TR20-08)"},
        {1039, "Error in input data of correction factor according frequency difference"},
        {1040, "Channel spacing outside definition range (Rx)"},
        {1041, "Channel spacing outside definition range (Tx)"},
        {1042, "Error in Rx elevation"},
        {1043, "Error in Rx azimuth"},
        {1044, "Error in Rx type of antenna (\"E\" or \"I\")"},
        {1045, "Error in gain of Rx antenna"},
        {1046, "Error in input data of depolarization loss"},
        {1047, "Distance to borderline is too long"},
        {1048, "Selected line data not available"},
        {1049, "Error in line data"},
        {1050, "non strict HCM Cmode and important technical data missing"},
        {2000, "wrong Figure_frequency (from Get_figure_FS_value)"},
        {2001, "wrong Time_percentage (from Get_figure_FS_value)"},
        {2002, "wrong Sea_temperature (from Get_figure_FS_value)"},
        {2003, "wrong Figure_Heff (from Get_figure_FS_value)"},
        {2004, "wrong Figure_distance (from Get_figure_FS_value)"},
        {2005, "wrong Get_figure_FS_value (error in curves)"},
        {3100, "x-km line calculation doesn't support variable Tx coordinates"}
        
    };

    for (const auto& error : errorMessages) {
        if (error.code == errorCode) {
            return error.message;
        }
    }

    return "Unknown error code";
}


#endif