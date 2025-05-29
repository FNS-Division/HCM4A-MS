#ifndef _HCM4AMobileService_Calculation_h_
#define _HCM4AMobileService_Calculation_h_

//#include "Dll/HCM4AMobileServiceDLL.h"



#include <Functions4U/Functions4U.h>
#define DLLFILENAME "hcm4mfs.dll"
#define DLIHEADER   <HCM4AMobileService/hcm4mfs.dli>
#define DLIMODULE   Hcm4AmsDll
#include <Core/dli.h>




#include "HCM4AMobileServiceStruct.h"
#include "ErrorCodes.h"
#include "Common.h"

bool ExtractTxFromFile(String fname, HCM4AMobileService_Parameters& param, String &stationName, int index) {
	/**
	* Extracts transmitter parameters from a file.
	*
	* Parameters:
	* fname (String): The name of the file to read from.
	* param (HCM4AMobileService_Parameters&): Reference to a structure to store extracted parameters.
	* stationName (String&): Reference to a string to store the station name.
	* index (int): The index of the transmitter in the file.
	*
	* Returns:
	* bool: True if extraction is successful, false otherwise.
	*
	* Constraints:
	* - The file must exist and be readable.
	* - The file size must be a multiple of 219 bytes and greater than 219 bytes.
	* - The function does not handle non-numeric inputs or invalid file formats.
	*/
	FileIn f(fname);
	int fs = f.GetSize();
	if (((fs % 219) != 0) && (fs > 219)) {
		f.Close();
		return false;
	}
	f.Seek(0);
	String allString = f.GetLine();
	String tmp = "";
	int tmp1, tmp2, tmp3 = 0;
	try {
		int offet = 219 + index * 219;

		if (TrimBoth(allString.Mid(offet + 0, 12)).GetCount() < 2) { // NO FREQ TX
			f.Close();
			return false;
		};

		stationName = TrimLeft(TrimRight(allString.Mid(offet + 28, 20)));
		param.Coo_Tx = allString.Mid(offet + 51, 15);
		param.H_Tx_input = "    ";
		param.Ant_typ_H_Tx = allString.Mid(offet + 111, 7);
		param.Ant_typ_V_Tx = allString.Mid(offet + 118, 7);
		param.Azi_Tx_input = allString.Mid(offet + 91, 5);
		param.Ele_Tx_input = allString.Mid(offet + 96, 5);
		param.Ele_Tx_input.erase(std::remove(param.Ele_Tx_input.begin(), param.Ele_Tx_input.end(), ' '), param.Ele_Tx_input.end());
		
		param.H_Tx_ant = allString.Mid(offet + 107, 4);
		param.Type_of_Tx_ant = allString.Mid(offet + 90, 1);
		param.Max_power = allString.Mid(offet + 84, 6);
		param.Tx_frequ = allString.Mid(offet + 0, 12);
		param.Chan_occup = allString.Mid(offet + 19, 1);
		param.Rad_of_Tx_serv_area = allString.Mid(offet + 66, 5);
		param.Desig_of_Tx_emis = allString.Mid(offet + 75, 9);
		param.Land_from = allString.Mid(offet + 48, 3);
		param.Desig_of_Tx_emis = allString.Mid(offet + 75, 9);
	} catch (...) {
		f.Close();
		return false;
	}
	f.Close();
	return true;
}

bool ExtractRxFromFile(String fname, HCM4AMobileService_Parameters& param, String &stationName, int index) {
	/**
	* Extracts receiver parameters from a file.
	*
	* Parameters:
	* fname (String): The name of the file to read from.
	* param (HCM4AMobileService_Parameters&): Reference to a structure to store extracted parameters.
	* stationName (String&): Reference to a string to store the station name.
	* index (int): The index of the receiver in the file.
	*
	* Returns:
	* bool: True if extraction is successful, false otherwise.
	*
	* Constraints:
	* - The file must exist and be readable.
	* - The file size must be a multiple of 219 bytes and greater than 219 bytes.
	* - The function does not handle non-numeric inputs or invalid file formats.
	*/
	FileIn f(fname);
	int fs = f.GetSize();
	if (((fs % 219) != 0) && (fs > 219)) {
		f.Close();
		return false;
	}
	f.Seek(0);
	String allString = f.GetLine();
	String tmp = "";
	int tmp1, tmp2, tmp3 = 0;
	try {
		int offet = 219 + index * 219;

		if (TrimBoth(allString.Mid(offet + 125, 12)).GetCount() < 2) { // NO FREQ RX
			f.Close();
			return false;
		};

		stationName = TrimBoth(allString.Mid(offet + 28, 20));
		param.Coo_Rx = allString.Mid(offet + 51, 15);
		param.H_Rx_input = "    ";
		param.H_Rx_ant = allString.Mid(offet + 107, 4);
		param.Rad_of_Rx_serv_area = allString.Mid(offet + 66, 5);
		param.Rx_frequ = allString.Mid(offet + 125, 12);
		param.Desig_of_Rx_emis = allString.Mid(offet + 75, 9);
		param.Ant_typ_H_Rx = allString.Mid(offet + 111, 7);
		param.Ant_typ_V_Rx = allString.Mid(offet + 118, 7);
		param.Azi_Rx_input = allString.Mid(offet + 91, 5);
		param.Ele_Rx_input = allString.Mid(offet + 96, 5);
		param.Ele_Rx_input.erase(std::remove(param.Ele_Rx_input.begin(), param.Ele_Rx_input.end(), ' '), param.Ele_Rx_input.end());
		
		param.Type_of_Rx_ant = allString.Mid(offet + 90, 1);
		param.Land_to = allString.Mid(offet + 48, 3);
		param.Rx_ant_gain = allString.Mid(offet + 103, 4);
	} catch (...) {
		f.Close();
		return false;
	}
	f.Close();
	return true;
}

std::string MakeDLLString(HCM4AMobileService_Parameters& param, bool isPtoP,  String debugPath) {
	/**
	* Creates a formatted string for DLL input from the parameters.
	*
	* Parameters:
	* param (HCM4AMobileService_Parameters&): Reference to a structure containing parameters.
	* debugPath (String): The path for debugging output.
	*
	* Returns:
	* std::string: A formatted string to be used as input for the DLL.
	*
	* Constraints:
	* - The function assumes that the input parameters are correctly formatted.
	* - The function does not validate the input parameters.
	*/
	std::string ss = "";
	ss += MakeStringBlank(param.Coo_Tx, 15);
	isPtoP ? ss += MakeStringBlank(param.Coo_Rx, 15) : ss += MakeStringBlank("", 15);
	ss += MakeStringBlank(param.H_Tx_input, 4);
	isPtoP ? ss += MakeStringBlank(param.H_Rx_input, 4) : ss += MakeStringBlank("", 4);
	ss += MakeStringBlank(param.Ant_typ_H_Tx, 7);
	ss += MakeStringBlank(param.Ant_typ_V_Tx, 7);
	ss += MakeStringBlank(param.Azi_Tx_input, 5);
	ss += MakeStringBlank(param.Ele_Tx_input, 5);
	ss += MakeStringBlank(param.H_Tx_ant, 4);
	ss += MakeStringBlank(param.H_Rx_ant, 4);
	ss += MakeStringBlank(param.Type_of_Tx_ant, 1);
	ss += MakeStringBlank(param.Max_power, 6);
	ss += MakeStringBlank(param.Tx_frequ, 12);
	ss += MakeStringBlank(param.Chan_occup, 1);
	ss += MakeStringBlank(param.Sea_temperature, 1);
	ss += MakeStringBlank(param.Rad_of_Tx_serv_area, 5);
	isPtoP ? ss += MakeStringBlank(param.Rad_of_Rx_serv_area, 5) : ss += MakeStringBlank("", 5);
	ss += MakeStringBlank(param.D_sea_input, 5);
	isPtoP ? ss += MakeStringBlank(param.Rx_frequ, 12) : ss += MakeStringBlank("", 12);
	isPtoP ? ss += MakeStringBlank(param.Desig_of_Rx_emis, 9) : ss += MakeStringBlank("", 9);
	ss += MakeStringBlank(param.Desig_of_Tx_emis, 9);
	isPtoP ? ss += MakeStringBlank(param.Ant_typ_H_Rx, 7) : ss += MakeStringBlank("", 7);
	isPtoP ? ss += MakeStringBlank(param.Ant_typ_V_Rx, 7) : ss += MakeStringBlank("", 7);
	isPtoP ? ss += MakeStringBlank(param.Azi_Rx_input, 5) : ss += MakeStringBlank("", 5);
	isPtoP ? ss += MakeStringBlank(param.Ele_Rx_input, 5) : ss += MakeStringBlank("", 5);
	isPtoP ? ss += MakeStringBlank(param.Type_of_Rx_ant, 1) : ss += MakeStringBlank("", 1);
	isPtoP ? ss += MakeStringBlank(param.Rx_ant_gain, 4) : ss += MakeStringBlank("", 4);
	ss += MakeStringBlank(param.Depol_loss, 4);
	ss += MakeStringBlank(param.Perm_FS_input, 5);
	ss += MakeStringBlank("", 4);//MakeStringBlank(param.Cor_fact_frequ_diff, 4);
	ss += MakeStringBlank(param.Land_to, 3);
	ss += MakeStringBlank(param.Land_from, 3);
	ss += MakeStringBlank(param.Max_CBR_D_input, 3);
	ss += MakeStringBlank(param.Topo_path, 163).ToStd();
	ss += MakeStringBlank(param.Border_path, 163).ToStd();
	ss += MakeStringBlank(param.Morpho_path, 163).ToStd();
	ss += MakeStringBlank("", 5).ToStd(); // version
	ss += MakeStringBlank(param.Coo_Tx, 15).ToStd(); // for Tx coord calculated
	isPtoP ? ss += MakeStringBlank(param.Coo_Rx, 15).ToStd() : ss += MakeStringBlank("", 15); // for Rx coord calculated
	ss += MakeStringBlank("", 20).ToStd(); // Info values (field, 20 values)
	ss += debugPath.ToStd();

	return ss;
}

void MobileService::Calculation(int mode) {
	/**
	* Performs calculations for the mobile service based on the specified mode.
	*
	* Parameters:
	* mode (int): The mode of calculation (e.g., PointToPoint, Line, CBR).
	*
	* Returns:
	* void
	*
	* Constraints:
	* - The function assumes that the input parameters and files are correctly formatted.
	* - The function does not handle invalid input parameters or file formats.
	*/
	double I_Distance; short I_H_Datab_Tx;
	short I_H_Datab_Rx; int I_HCM_error; double I_Heff; double I_Dh; double I_Dh_corr;
	double I_Power_to_Rx; double I_Free_space_FS; double I_Land_FS; double I_Sea_FS;
	double I_Tx_ant_corr; double I_Tx_ant_type_corr; double I_Dir_Tx_Rx;
	double I_V_angle_Tx_Rx; double I_Tx_TCA; double I_Rx_TCA; double I_Tx_TCA_corr;
	double I_Rx_TCA_corr; double I_D_sea_calculated; double I_Rx_ant_corr;
	double I_Rx_ant_type_corr; double I_Delta_frequency; double I_Corr_delta_f;
	double I_Calculated_FS; double I_Perm_FS; double I_CBR_D; double I_ERP_ref_Tx;
	double I_Prot_margin; std::string I_str;

	String frTx, frRx, tmpS;

	String stationName1, stationName2;
	Progress prg;
	prg.Delay(0);

	HCM4AMobileService_Parameters param;
	String country1, country2, debugPath, debugFile;
	Time time;
	double permissibleFiledStrength = 0;
	
	
	
	
	auto clearVars = [&]() {
		I_Distance = 0;
		I_H_Datab_Tx = 0;
		I_H_Datab_Rx = 0;
		I_HCM_error = 0;
		I_Heff = 0; 
		I_Dh = 0;
		I_Dh_corr = 0;
		I_Power_to_Rx = 0;
		I_Free_space_FS = 0;
		I_Land_FS = 0;
		I_Sea_FS = 0;
		I_Tx_ant_corr = 0;
		I_Tx_ant_type_corr = 0; 
		I_Dir_Tx_Rx = 0;
		I_V_angle_Tx_Rx = 0;
		I_Tx_TCA = 0;
		I_Rx_TCA = 0;
		I_Tx_TCA_corr = 0;
		I_Rx_TCA_corr = 0;
		I_D_sea_calculated = 0; 
		I_Rx_ant_corr = 0;
		I_Rx_ant_type_corr = 0;
		I_Delta_frequency = 0;
		I_Corr_delta_f = 0;
		I_Calculated_FS = 0;
		I_Perm_FS = 0;
		I_CBR_D = 0;
		I_ERP_ref_Tx = 0;
		I_Prot_margin = 0;
		I_str = "";
	};

		

	tabRes.table.Clear();
	if (tabRes.table.GetColumnCount() == 0) {
		tabRes.table.AddColumn("№", 0);
		tabRes.table.AddColumn("Tx name", 1);
		tabRes.table.AddColumn("Tx land", 2);
		tabRes.table.AddColumn("Tx Antenna H", 3);
		tabRes.table.AddColumn("Tx Azimut", 4);
		tabRes.table.AddColumn("Tx max. power", 5);
		tabRes.table.AddColumn("Tx radius", 6);
		tabRes.table.AddColumn("Tx freq", 7);
		tabRes.table.AddColumn("Rx name", 8);
		tabRes.table.AddColumn("Rx land", 9);
		tabRes.table.AddColumn("Rx Antenna H", 10);
		tabRes.table.AddColumn("Rx Azimut", 11);
		tabRes.table.AddColumn("Rx radius", 12);
		tabRes.table.AddColumn("Rx freq", 13);
		tabRes.table.AddColumn(" ", 14);
		tabRes.table.AddColumn("Distance", 15);
		tabRes.table.AddColumn("Azimut", 16);
		tabRes.table.AddColumn("Max f.s", 17);
		tabRes.table.AddColumn("Permis.f.s", 18);
		tabRes.table.AddColumn("Protect.magrin", 19);
		tabRes.table.AddColumn("Comment", 20);
		tabRes.table.ColumnWidths("12 44 20 20 20 20 20 20 44 20 20 20 20 20 1 20 20 20 20 20 70");
	}

	debugPath = GetExeFolder() + "\\debug\\";
	RealizePath(debugPath);
	debugFiles.Clear();

	param.PD = 0.1;
	param.Topo_path = PutSepToEndPath(cnfg.PathToTopo).ToStd();
	param.Border_path = PutSepToEndPath(cnfg.PathToBorders).ToStd();
	param.Morpho_path = PutSepToEndPath(cnfg.PathToMorpho).ToStd();
	param.Sea_temperature = "W";
	param.D_sea_input = "     ";
	param.Cor_fact_frequ_diff = "    ";
	param.Max_CBR_D_input = "   ";
	param.Land_to = "   ";
	param.Land_from = "   ";
	
	if (Hcm4AmsDll()){
		
		
		
	} else {
		PromptOK("error while loading DLL");
		Hcm4AmsDll();
		return;
	}

	if (mode == 100) { // PointToPoint
		int idx = tabRes.drlPointToPoint.GetIndex();
		param.C_Mode = 0;
		if (idx == 1) param.C_Mode = 10;
		if (idx == 2) param.C_Mode = 11;
		if (idx == 3) param.C_Mode = 12;
		param.Perm_FS_input = MakeStringBlank(tabRes.edPtoP.GetData().ToString(), 5);
		if (StationsList.GetCount() < 1) { ErrorOK("There are no stations"); return; };



		int index = 1;
		prg.Set(0, StationsList.GetCount());
		for (int i = 0; i < StationsList.GetCount(); i++) {
			prg.Step();

			if (!StationsList[i].isRefList) continue;

			if (!ExtractRxFromFile(StationsList[i].Filename, param, stationName2, StationsList[i].IndexInFile)) {
				continue;
			}

			for (int j = 0; j < StationsList.GetCount(); j++) {
				if (StationsList[j].isRefList) continue;

				if (!ExtractTxFromFile(StationsList[j].Filename, param, stationName1, StationsList[j].IndexInFile)) {
					continue;
				}
				clearVars();

				time = GetSysTime();
				debugFile = AsString(time) + "_" + AsString(Random(1000));
				debugFile.Replace("/", "_");
				debugFile.Replace(" ", "_");
				debugFile.Replace(":", "_");
				debugFile = debugPath + debugFile + ".txt";
				debugFiles.Add(debugFile);
				FileOut f;
				f.Open(debugFile);
				f.Close();

				
				param.Depol_loss = MakeStringBlank(tabRes.edDepolLoss.GetData().ToString(), 4);
				I_str = MakeDLLString(param, true, debugFile);
					
				
				try {
					Hcm4AmsDll().
					HCM4AMobileServiceDLL(param.C_Mode, param.D_to_border, param.PD,
						I_Distance, I_H_Datab_Tx, I_H_Datab_Rx, I_HCM_error, I_Heff, I_Dh, I_Dh_corr, I_Power_to_Rx, I_Free_space_FS, I_Land_FS, I_Sea_FS,
						I_Tx_ant_corr, I_Tx_ant_type_corr, I_Dir_Tx_Rx, I_V_angle_Tx_Rx, I_Tx_TCA, I_Rx_TCA, I_Tx_TCA_corr,
						I_Rx_TCA_corr, I_D_sea_calculated, I_Rx_ant_corr, I_Rx_ant_type_corr, I_Delta_frequency, I_Corr_delta_f,
						I_Calculated_FS, I_Perm_FS, I_CBR_D, I_ERP_ref_Tx, I_Prot_margin, I_str);
				} catch (...) {

				}


				frTx = AsString(param.Tx_frequ);
				frTx.Replace(" ", ""); frTx.Replace(".00000", ""); frTx.Replace("0000", "");
				frRx = AsString(param.Rx_frequ);
				frRx.Replace(" ", ""); frRx.Replace(".00000", ""); frRx.Replace("0000", "");

				tabRes.table.Add(index, stationName1, param.Land_from, param.H_Tx_ant, param.Azi_Tx_input,
					param.Max_power, param.Rad_of_Tx_serv_area, frTx,
					stationName2, param.Land_to, param.H_Rx_ant, param.Azi_Rx_input, param.Rad_of_Rx_serv_area, frRx, "",
					Format("%.2mf", I_Distance),
					Format("%.1mf", I_Dir_Tx_Rx),
					Format("%.2mf", I_Calculated_FS),
					Format("%.2mf", I_Perm_FS),
					Format("%.2mf", I_Prot_margin),
					AsString(I_HCM_error) + " " + getErrorMessage(I_HCM_error)

				);
				index++;

			}

		}
	}

	if (mode == 200) {  //Line
		int idx = tabRes.drlLine.GetIndex();
		param.C_Mode = -1;
		if (idx == 0) param.C_Mode = -9;
		if (idx == 1) param.C_Mode = -10;
		if (idx == 2) param.C_Mode = -11;
		param.Land_from = "";
		param.Land_to = tabRes.lineLandTo.GetValue(tabRes.lineLandTo.GetIndex()).ToString().Mid(0, 3);
		param.D_to_border = atoi(tabRes.lineCross.GetText().ToString());
		param.Max_CBR_D_input = MakeStringBlank("", 3);
		param.Perm_FS_input = MakeStringBlank(tabRes.linePermFS.GetData().ToString(), 5);
		param.Depol_loss = "";

		if (StationsList.GetCount() < 1) { ErrorOK("There are no stations"); return; };

		int count = 1;
		for (int i = 0; i < StationsList.GetCount(); i++) if (!StationsList[i].isRefList) count++;

		int index = 1;
		prg.Set(0, count);
		for (int j = 0; j < StationsList.GetCount(); j++) {
			if (StationsList[j].isRefList) continue;
			clearVars();
			
			prg.Step();

			if (!ExtractTxFromFile(StationsList[j].Filename, param, stationName1, StationsList[j].IndexInFile)) {
				continue;
			}

			time = GetSysTime();
			debugFile = AsString(time) + "_" + AsString(Random(1000));
			debugFile.Replace("/", "_");
			debugFile.Replace(" ", "_");
			debugFile.Replace(":", "_");
			debugFile = debugPath + debugFile + ".txt";
			debugFiles.Add(debugFile);
			FileOut f;
			f.Open(debugFile);
			f.Close();

			I_str = MakeDLLString(param, false, debugFile);

			
			Hcm4AmsDll().
			HCM4AMobileServiceDLL(param.C_Mode, param.D_to_border, param.PD,
				I_Distance, I_H_Datab_Tx, I_H_Datab_Rx, I_HCM_error, I_Heff, I_Dh, I_Dh_corr, I_Power_to_Rx, I_Free_space_FS, I_Land_FS, I_Sea_FS,
				I_Tx_ant_corr, I_Tx_ant_type_corr, I_Dir_Tx_Rx, I_V_angle_Tx_Rx, I_Tx_TCA, I_Rx_TCA, I_Tx_TCA_corr,
				I_Rx_TCA_corr, I_D_sea_calculated, I_Rx_ant_corr, I_Rx_ant_type_corr, I_Delta_frequency, I_Corr_delta_f,
				I_Calculated_FS, I_Perm_FS, I_CBR_D, I_ERP_ref_Tx, I_Prot_margin, I_str);
			

			frTx = AsString(param.Tx_frequ);
			frTx.Replace(" ", ""); frTx.Replace(".00000", ""); frTx.Replace("0000", "");
			frRx = AsString(param.Rx_frequ);
			frRx.Replace(" ", ""); frRx.Replace(".00000", ""); frRx.Replace("0000", "");

			tabRes.table.Add(index, stationName1, param.Land_from, param.H_Tx_ant, param.Azi_Tx_input,
				param.Max_power, param.Rad_of_Tx_serv_area, frTx,
				"", param.Land_to, param.H_Rx_ant, param.Azi_Rx_input, param.Rad_of_Rx_serv_area, frRx, "",
				Format("%.2mf", I_Distance),
				Format("%.1mf", I_Dir_Tx_Rx),
				Format("%.2mf", I_Calculated_FS),
				Format("%.2mf", I_Perm_FS),
				Format("%.2mf", I_Prot_margin),
				AsString(I_HCM_error) + " " + getErrorMessage(I_HCM_error)

			);
			index++;

		}
	}
	if (mode == 300) {  //CBR
		int idx = tabRes.drlCbr.GetIndex();
		param.C_Mode = -1;
		if (idx == 1) param.C_Mode = -9;
		if (idx == 2) param.C_Mode = -10;
		if (idx == 3) param.C_Mode = -11;
		param.Land_from = "";
		param.Land_to = tabRes.cbrLandTo.GetValue(tabRes.cbrLandTo.GetIndex()).ToString().Mid(0, 3);
		param.D_to_border = atoi(tabRes.cbrCross.GetText().ToString());
		param.Max_CBR_D_input = AsString(param.D_to_border);
		param.Perm_FS_input = MakeStringBlank(tabRes.cbrPermFS.GetData().ToString(), 5);
		param.Depol_loss = "";

		if (StationsList.GetCount() < 1) { ErrorOK("There are no stations"); return; };

		int count = 0;
		for (int i = 0; i < StationsList.GetCount(); i++) if (!StationsList[i].isRefList) count++;
		int index = 1;

		prg.SetTotal(count);
		for (int j = 0; j < StationsList.GetCount(); j++) {
			if (StationsList[j].isRefList) continue;
			clearVars();
			
			prg.Step();

			if (!ExtractTxFromFile(StationsList[j].Filename, param, stationName1, StationsList[j].IndexInFile)) continue;

			time = GetSysTime();
			debugFile = AsString(time) + "_" + AsString(Random(1000));
			debugFile.Replace("/", "_");
			debugFile.Replace(" ", "_");
			debugFile.Replace(":", "_");
			debugFile = debugPath + debugFile + ".txt";
			debugFiles.Add(debugFile);
			FileOut f;
			f.Open(debugFile);
			f.Close();

			I_str = MakeDLLString(param, false, debugFile);

			
			Hcm4AmsDll().
			HCM4AMobileServiceDLL(param.C_Mode, param.D_to_border, param.PD,
				I_Distance, I_H_Datab_Tx, I_H_Datab_Rx, I_HCM_error, I_Heff, I_Dh, I_Dh_corr, I_Power_to_Rx, I_Free_space_FS, I_Land_FS, I_Sea_FS,
				I_Tx_ant_corr, I_Tx_ant_type_corr, I_Dir_Tx_Rx, I_V_angle_Tx_Rx, I_Tx_TCA, I_Rx_TCA, I_Tx_TCA_corr,
				I_Rx_TCA_corr, I_D_sea_calculated, I_Rx_ant_corr, I_Rx_ant_type_corr, I_Delta_frequency, I_Corr_delta_f,
				I_Calculated_FS, I_Perm_FS, I_CBR_D, I_ERP_ref_Tx, I_Prot_margin, I_str);

			frTx = AsString(param.Tx_frequ);
			frTx.Replace(" ", ""); frTx.Replace(".00000", ""); frTx.Replace("0000", "");
			frRx = AsString(param.Rx_frequ);
			frRx.Replace(" ", ""); frRx.Replace(".00000", ""); frRx.Replace("0000", "");

			tabRes.table.Add(index, stationName1, param.Land_from, param.H_Tx_ant, param.Azi_Tx_input,
				param.Max_power, param.Rad_of_Tx_serv_area, frTx,
				"", param.Land_to, param.H_Rx_ant, param.Azi_Rx_input, param.Rad_of_Rx_serv_area, frRx, "",
				Format("%.2mf", I_Distance),
				Format("%.1mf", I_Dir_Tx_Rx),
				Format("%.2mf", I_Calculated_FS),
				Format("%.2mf", I_Perm_FS),
				Format("%.2mf", I_Prot_margin),
				AsString(I_HCM_error) + " " + getErrorMessage(I_HCM_error)

			);
			index++;

		}
	}

	~prg;

}

#endif
