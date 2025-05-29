#ifndef _HCM4AMobServStruct_h_
#define _HCM4AMobServStruct_h_


const double M_PI = 3.14159265358979323846264;

// Main structure
struct HCM4AMobileService_Parameters {
	// INPUT VALUES
	//	Calculation mode, possible values are:
	//	12	point to point calculation (t%=1%)
	//	11	point to point calculation (t%=50%
	//	10	point to point calculation (t%=10%)
	//	0	point to point calculation (t%=Channel occ.)
	//	-1	point to line calculation (t%=Channel occ., 10m)
	//	-9	point to line calculation (t%=10%, 3m)
	//	-10	point to line calculation (t%=10%, 10m)
	//	-11	point to line calculation (t%=50%, 3m)
	int C_Mode = 0;     // Integer*4
	std::string 
	Coo_Tx, // Character*15	Tx co-ordinates, format = ‘015E203052N2040’
	Coo_Rx, // Character*15	Rx co-ordinates, format = ‘015E203052N2040’ only required if C_mode is 0 or positive
	H_Tx_ant,//Character*4	Tx antenna height in m
	H_Rx_ant,//Character*4	Rx antenna height in m                      only required if C_mode is 0 or positive
	Tx_frequ,//Character*12	Tx frequency, format = ‘00147.77000M’
	Rx_frequ,//Character*12	Rx frequency, format = ‘00147.77000M’       only required if C_mode is 0 or positive
	Rad_of_Tx_serv_area,//Character*5	Radius of Tx service area
	Rad_of_Rx_serv_area,//Character*5	Radius of Rx service area       only required if C_mode is 0 or positive
	H_Tx_input,//Character*4	Input value of Tx site height (if available)
	H_Rx_input,//Character*4	Input value of Rx site height (if available)only required if C_mode is 0 or positive
	Max_power,//Character*6	Maximum radiated power
	Type_of_Tx_ant,//Character*1	Type of Tx reference antenna (E / I)
	Type_of_Rx_ant,//Character*1	Type of Rx reference antenna (E / I)   only required if C_mode is 0 or positive
	Chan_occup;//Character*1	Channel occupation (0 /1)
	double PD;//DoublePrecision	Point distance for the profile (default value = 0.1 km)
	std::string 
	Perm_FS_input,//Character*5	Input value of permissible field strength (if not filled in, the value is taken from the table in Annex 1)
	Max_CBR_D_input,//Character*3	Input value of maximum cross-border range (if not filled in, the value is taken from the table in Annex 1)
	Sea_temperature,//Character*1	Sea temperature (C / W)
	Topo_path,//Character*63	Path of the terrain height data (e.g. ‘C:\TOPO’)
	Morpho_path,//Character*63	Path of the morphological data (e.g. ‘C:\MORPHO’)
	Border_path;//Character*63	Path of the (border-) line data (e.g. ‘C:\BORDER’)
	int D_to_border;//Integer*4	Distance to border line (for the selection of the type of line calculation, 
	                //0 = calculation on the borderline, negative value = calculation 
	                //of the maximum cross-border range, positive value x = calculation on the x-km line  only required if C_mode is 0 or positive
	std::string
	Land_from,//Character*3	Country of Tx or country to calculate from
	Land_to,//Character*3	Country of Rx or country to calculate to
	Rx_ant_gain,//Character*4	Gain of Rx antenna only required if C_mode is 0 or positive
	Depol_loss,//Character*4	Depolarization loss only required if C_mode is 0 or positive
	Cor_fact_frequ_diff,//Character*4	Correction factor according to frequency Difference, 
	//only required if C_mode is 0 or positive, if missing, this value is calculated
	Azi_Tx_input,//Character*5	Tx azimuth
	Azi_Rx_input,//Character*5	Rx azimuth  only required if C_mode is 0 or positive
	Ele_Tx_input,//Character*5	Tx elevation
	Ele_Rx_input,//Character*5	Rx elevation    only required if C_mode is 0 or positive
	D_sea_input,//Character*5	Input value of distance over sea; if filled, the calculation of distance over sea is switched off
	Ant_typ_H_Tx,//Character*7	Horizontal antenna type of Tx
	Ant_typ_V_Tx,//Character*7	Vertical antenna type of Tx
	Ant_typ_H_Rx,//Character*7	Horizontal antenna type of Rxonly required if C_mode is 0 or positive
	Ant_typ_V_Rx,//Character*7	Vertical antenna type of Rx only required if C_mode is 0 or positive
	Desig_of_Tx_emis,//Character*9	Designation of emission of Tx
	Desig_of_Rx_emis;//Character*9	Designation of emission of Rx only required If C_mode is 0 or positive
	
	// OUTPUT VALUES
	int HCM_error;	  //	Integer*4	Error value, see list of HCM_error values
	 //	Logical*4(20)	List of Info(i) values, see list of Info values
		/*1	No height of Tx site is given or Tx is mobile; height is taken from the terrain database
		2	Height of Tx site differs from height of terrain database
		3	Height of Tx site differs more than 10%, calculated values may be (extremely) wrong!
		4	Frequency out of range of table in Annex 1
		5	Input value of permissible field strength is used
		6	Input value of maximum cross border range is used
		7	Distance between Tx and Rx is less than both service area radiuses; field strength is set to 999.9
		8	No height of Rx site is given or Rx is mobile/line, height is from the terrain database
		9	Height of Rx site differs from height of terrain data
		10	Rx site height differs more than 10%, calculated values may be (extremely) wrong!
		11	Free space field strength used because distance < 1 km
		12	Free space field strength is used, because 1st Fresnel zone is free
		13	Distance over sea is greater than total distance. Distance between Tx and Rx is used!
		14	Input value of correction factor according frequency difference is used
		15	Frequency difference outside definition range; 82 dB is used
		16	Calculated distance over sea is set to 0 because of missing morphological data.
		17	Tx channel spacing outside definition range, 25 kHz is used!
		18	Correction factors for the band 380 - 400 MHz are used.*/
	std::array<bool, 20> Info = {false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false};
	
	double Calculated_FS,	  //	Real	Calculated field strength
	Perm_FS,	  //	Real	Permissible field strength
	Prot_margin,	  //	Real	Protection margin
	Free_space_FS,	  //	Real	Free space field strength
	Distance,	  //	DoublePrecision	Distance between Tx position and Rx position
	D_sea_calculated,	  //	DoublePrecision	Calculated distance over sea (or from input value)
	Dir_Tx_Rx,	  //	DoublePrecision	Horizontal direction from Tx to Rx
	Dir_Rx_Tx,	  //	DoublePrecision	Horizontal direction from Rx to Tx
	V_angle_Tx_Rx,	  //	DoublePrecision	Vertical direction from Tx to Rx
	V_angle_Rx_Tx,	  //	DoublePrecision	Vertical direction from Rx to Tx
	H_diff_angle_Tx_Rx,	  //	DoublePrecision	Horizontal difference angle from Tx to Rx
	H_diff_angle_Rx_Tx,	  //	DoublePrecision	Horizontal difference angle from Rx to Tx
	V_diff_angle_Tx_Rx,	  //	DoublePrecision	Vertical difference angle from Tx to Rx
	V_diff_angle_Rx_Tx,	  //	DoublePrecision	Vertical difference angle from Rx to Tx
	Delta_frequency,	  //	DoublePrecision	Frequency difference in kHz
	Heff_Tx,	  //	Real	Tx effective antenna height
	Heff_Rx,	  //	Real	Rx effective antenna height
	Heff,	  //	Real	Total effective antenna height
	Dh,	  //	Real	Terrain irregularity
	Dh_corr,	  //	Real	Correction factor according to the terrain irregularity
	Tx_TCA,	  //	Real	Tx clearance angle
	Rx_TCA,	  //	Real	Rx clearance angle
	Tx_TCA_corr,	  //	Real	Correction factor according to the Tx clearance angle
	Rx_TCA_corr,	  //	Real	Correction factor according to the Rx clearance angle
	ERP_ref_Tx,	  //	Real	ERP of the reference transmitter
	Land_FS,	  //	Real	Land field strength
	Sea_FS,	  //	Real	Sea field strength
	Tx_ant_corr,	  //	Real	Correction factor according to the Tx antenna type (horizontal and vertical)
	Rx_ant_corr,	  //	Real	Correction factor according to the Rx antenna type (horizontal and vertical)
	Tx_ant_type_corr,	  //	Real	Correction factor according to the Tx reference antenna type (E / I)
	Rx_ant_type_corr,	  //	Real	Correction factor according to the Rx reference antenna type (E / I)
	Perm_FS_from_table,	  //	Real	Permissible field strength from the Table in Annex 1
	Corr_delta_f,	  //	Real	Correction factor according to the Frequency difference (calculated or from the input value)
	Channel_sp_Tx,	  //	Real	Channel spacing of Tx in kHz
	Channel_sp_Rx,	  //	Real	Channel spacing of Rx in kHz
	Power_to_Rx,	  //	Real	Power in direction of Rx
	CBR_D;	  //	Real	Maximum cross-border range in km (from input value or from table in Annex 1)
	std::string Version,	  //	Character*5	 version number
	Coo_Tx_new,	  //	Character*15	Calculated Tx co-ordinates
	Coo_Rx_new;	  //	Character*15	Calculated Rx co-ordinates (or line co-ordinates)
	std::array<int, 10002> T_Prof;	  //	Integer*2 (10002)	Terrain height profile
	std::array<int, 10002> M_Prof;	  //	Integer*2 (10002)	Morphological profile
	int PN,	  //	Integer*2	Number of profile points
	H_Datab_Tx,	  //	Integer*2	Tx site height from terrain database
	H_Datab_Rx;	  //	Integer*2	Rx site height from terrain database

};


struct InfoMessage {
    int code;
    std::string message;
};

std::string getInfoMessage(int InfoCode) {
    static const InfoMessage infoMessages[] = {
        {1, "No height of Tx site is given or Tx is mobile; height is taken from the terrain database"},
        {2, "Height of Tx site differs from height of terrain database"},
        {3, "Height of Tx site differs more than 10%%, calculated values may be (extremely) wrong!"},
        {4, "Frequency out of range of table in Annex 1"},
        {5, "Input value of permissible field strength is used"},
        {6, "Input value of maximum cross border range is used"},
        {7, "Distance between Tx and Rx is less than both service area radiuses; field strength is set to 999.9"},
        {8, "No height of Rx site is given or Rx is mobile/line, height is from the terrain database"},
        {9, "Height of Rx site differs from height of terrain data"},
        {10, "Rx site height differs more than 10%%, calculated values may be (extremely) wrong!"},
        {11, "Free space field strength used because distance < 1 km"},
        {12, "Free space field strength is used, because 1st Fresnel zone is free"},
        {13, "Distance over sea is greater than total distance. Distance between Tx and Rx is used!"},
        {14, "Input value of correction factor according frequency difference is used"},
        {15, "Frequency difference outside definition range; 82 dB is used"},
        {16, "Calculated distance over sea is set to 0 because of missing morphological data."},
        {17, "Tx channel spacing outside definition range, 25 kHz is used!"},
        {18, "Correction factors for the band 380 - 400 MHz are used."}
    };

    for (const auto& info : infoMessages) {
        if (info.code == InfoCode) {
            return info.message;
        }
    }

    return "Unknown info code";
}

#endif