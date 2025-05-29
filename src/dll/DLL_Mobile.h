#ifndef _DLL_Mobile_DLL_Mobile_h
#define _DLL_Mobile_DLL_Mobile_h

#include "Dll/HCM4AMobileServiceDLL.h"

#if defined(_WIN32)
  #define LIBRARY_API __declspec(dllexport)
#else
  #define LIBRARY_API
#endif

#define DllImport extern "C" __declspec(dllimport)
#define DllExport extern "C" __declspec(dllexport)

#ifdef flagDLL
	#define DLLIMPORT __declspec(dllexport)
#else
	#define DLLIMPORT __declspec(dllimport)
#endif



extern "C" void LIBRARY_API HCM4AMobileServiceDLL(int I_C_mode, int I_bor_dis, double I_PD,
				double &I_Distance, short &I_H_Datab_Tx,
				short &I_H_Datab_Rx, int &I_HCM_error, double &I_Heff, double &I_Dh, double &I_Dh_corr,
				double &I_Power_to_Rx, double &I_Free_space_FS, double &I_Land_FS, double &I_Sea_FS,
				double &I_Tx_ant_corr, double &I_Tx_ant_type_corr, double &I_Dir_Tx_Rx,
				double &I_V_angle_Tx_Rx, double &I_Tx_TCA, double &I_Rx_TCA, double I_Tx_TCA_corr,
				double &I_Rx_TCA_corr, double &I_D_sea_calculated, double &I_Rx_ant_corr,
				double &I_Rx_ant_type_corr, double &I_Delta_frequency, double &I_Corr_delta_f,
				double &I_Calculated_FS, double &I_Perm_FS, double &I_CBR_D, double &I_ERP_ref_Tx,
				double &I_Prot_margin, std::string& I_str);
									 
									 
#endif
