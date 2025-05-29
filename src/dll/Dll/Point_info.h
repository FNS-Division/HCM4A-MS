//!	Subroutine to read the info of a given point from the database.
//!
//!
//!	Input values:
//!			Long		DOUBLE PRECISION	longitude of point	(-180.0....+180.0)
//!			Lat			DOUBLE PRECISION	latitude of point	(-90.0...+90.0)
//!			Topo_path	CHARACTER*63		path of terrain database, e.g. 'D:\TOPO'
//!			Morpho_path	CHARACTER*63		path of morpho database, e.g. 'D:\MORPHO'
//!
//!
//!	Output values:
//!			Height		INTEGER*2			height of point in meter
//!			M_Type		INTEGER*2			type of morphologie
//!			Error		INTEGER*4			error value
//!
//!	Possible error values
//!		  0 no error
//!		 36 error opening file (no data)
//!		220 error reading record
//!		400 height is missing (-9999)
//!
//!	Terrain-data location: Topo_path\Subdir\
//!	Morpho-data location: Morpho_path\Subdir\
//!		where Subdir is first 4 char of filename
//!
//!	Topo_Path e.g. = D:\TOPO
//!	Morpho_Path e.g. = D:\MORPHO
//!
//!	Filenames are (example) E009N50.63E or E012N45.33M where
//!		E009 or E012  is  9 or 12 degree east,
//!		N50  or  N45  is 50 or 45 degree north,
//!		33   or   63  is the resolution in east-west (3 or 6)
//!					  and in north-south direction (always 3) 
//!					  in seconds and
//!				   E  for elevation-data,
//!				   M  for morphological-data.
//!
//!	!!!!   Longitudes are [0° - 360°[    !!!!!
//!
//!	Filenames correspond to the south-west corner of a 1 * 1
//!	degree block in !both! hemispheres. 
//!   allways the lowest long(0..360)/lat(-90..+90) = 1st record in file)
//!
//!	Record-No. (R) in the files:
//!
//!       133 134 135 136 137 138 139 140 141 142 143 144                    
//!       121                                         132                    
//!       109                                         120                    
//!       097                                         108                    
//!       085                                         096                    
//! (BV)  073                                         084                    
//!       061                                         072                    
//!       049                                         060                    
//!       037                                         048                    
//!       025                                         036                    
//!       013                                         024                    
//!       001 002 003 004 005 006 007 008 009 010 011 012
//!                            (BH)
//!    int e = trunc(ev) * (resh) + trunc(eh) + (resh*101*bv)+(12*resh*101*bv);
//!	one record: 
//!                Field H_F_3(I,J)
//!
//!         I=1,J=101   ********************* I=101,J=101
//!                     *********************
//!              North  *********************
//!                     *********************
//!                (EV) *********************
//!                     *********************
//!                     *********************
//!             South   *********************
//!                     *********************
//!         I=1,J=1     ********************* I=101,J=1
//!                             (EH)  
//!                   West                 East
//!                                          
//!
//!                 Field H_F_6(I,J)
//!
//!         I=1,J=101   ********************* I=51,J=101
//!                     *********************
//!              North  *********************
//!                     *********************
//!                     *********************
//!                     *********************
//!                     *********************
//!             South   *********************
//!                     *********************
//!         I=1,J=1     ********************* I=51,J=1
//!
//!                   West                 East
//!
//!
//!             Heights surrounding point "P" :
//!
//!                   P3            P4
//!
//!                            P
//!
//!
//!                   P1(E)         P2          

	
#ifndef _PointInfoH
#define _PointInfoH



#include <cmath>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>



// Function to convert longitude and latitude to filename
std::string createFilename(double longitude, double latitude, bool isTopo) {
    std::string filename;
    filename += (longitude >= 0) ? 'E' : 'W';
    if (std::abs(longitude) >= 100){
        filename += "";
    }else if(std::abs(longitude) >= 10){
        filename += "0";
    }else{
		filename += "00";
    };
    filename += std::to_string(static_cast<int>(std::abs(longitude))).substr(0, 3);
    filename += (latitude >= 0) ? 'N' : 'S';
    if (std::abs(latitude) < 10){
        filename += "0";
    };
    filename += std::to_string(static_cast<int>(std::abs(latitude))).substr(0, 2);
    filename += (std::abs(latitude) < 50) ? ".33" : ".63";
    filename += isTopo ? 'E' : 'M';
    return filename;
}


// Function to read the info of a given point from the database
PointInfoResult Point_info2(double longitude, double latitude, const std::string& topo_path, const std::string& morpho_path) {
	using myType = uint16_t;
	PointInfoResult result = {-9999, 8, 0};
	
	std::string filename = createFilename(longitude, latitude, true);
    std::string morpho_filename = createFilename(longitude, latitude, false);
    filename = topo_path + "\\" + filename.substr(0, 4) + "\\" + filename;
    morpho_filename = morpho_path + "\\" + morpho_filename.substr(0, 4) + "\\" + morpho_filename;
    
    std::ifstream topo_file(filename, std::ios::binary);
	std::ifstream morpho_file(morpho_filename, std::ios::binary);
	
	if (!topo_file) {
        result.Error = 36;
        return result;
    }
  
    int lats  = static_cast<int> (latitude * 3600);			//change format of latitude in to seconds
    int longs = static_cast<int> (longitude * 3600);		//change format of longitude in to seconds

	double blockx = (longs - floor(longitude) * 3600)*12/3600;	//get horizontal block number
	double blocky = (lats -  floor(latitude)  * 3600)*12/3600;	//get vertical block number
	int block_nr = static_cast<int>(blocky) * 12 + static_cast<int>(blockx);	//block number from beginning
	int stepx = latitude < 50 ? 100 : 50;					//if latitude > 50, in the block we have 50 points in x direction

	double dotx = (blockx - floor(blockx)) * stepx;			//point x coordinate inside the block
	double doty = (blocky - floor(blocky))*100;				//point y coordinate inside the block
	int dot_nr = static_cast<int>(doty) * (stepx+1) + static_cast<int>(dotx);	//point number from beginning of the block

	long offset = block_nr * (stepx + 1) * 202 + dot_nr * 2;		//offset of the first point in bytes
	topo_file.seekg(offset, std::ios::beg);
	short h1,h2,h3,h4;
	if (!topo_file.read(reinterpret_cast<char*>(&h1), sizeof(h1))) {
        result.Error = 220;
        return result;
    }
	if (!topo_file.read(reinterpret_cast<char*>(&h2), sizeof(h2))) {
        result.Error = 220;
        return result;
    }
    topo_file.seekg((stepx + 1 - 2)*2, std::ios::cur);
	if (!topo_file.read(reinterpret_cast<char*>(&h3), sizeof(h3))) {
        result.Error = 220;
        return result;
    }
	if (!topo_file.read(reinterpret_cast<char*>(&h4), sizeof(h4))) {
        result.Error = 220;
        return result;
    }
	if (h1 == -9999 || h2 == -9999 || h3 == -9999 || h4 == -9999)
	{
		result.Error = 5;
		return result;
	}
	double h12 = h1 + (h2-h1)*(dotx - floor(dotx));
	double h34 = h3 + (h4-h3)*(dotx - floor(dotx));
	short h = h12 + (h34-h12)*(doty - floor(doty));
	result.Height = h;
	
	if (morpho_file) {
		morpho_file.seekg(offset, std::ios::beg);
		morpho_file.read(reinterpret_cast<char*>(&result.M_Type), sizeof(result.M_Type));
    }
    
	return result;
}


PointInfoResult Point_info(double longitude, double latitude, const std::string& topo_path, const std::string& morpho_path) {
	using myType = uint16_t;
	PointInfoResult result = {-9999, 8, 0};
	
	std::string filename = createFilename(longitude, latitude, true);
    std::string morpho_filename = createFilename(longitude, latitude, false);
    filename = topo_path + "\\" + filename.substr(0, 4) + "\\" + filename;
    morpho_filename = morpho_path + "\\" + morpho_filename.substr(0, 4) + "\\" + morpho_filename;
    
	FILE* fileh = fopen(filename.c_str(), "rb");
	if(fileh == NULL){
		return result;
	}
	
	int lats  = static_cast<int> (latitude * 3600);						// change format of latitude in to seconds
    int longs = static_cast<int> (longitude * 3600);	
    
	double blockx = (longs - floor(longitude) * 3600)*12/3600;	// get horizontal block number
	double blocky = (lats -  floor(latitude)  * 3600)*12/3600;	// get vertical block number
	int block_nr = static_cast<int>(blocky) * 12 + static_cast<int>(blockx);	// block number from beginning
	int stepx = latitude < 50 ? 100 : 50;						// if latitude > 50, in the block we have 50 points in x direction
	double dotx = (blockx - floor(blockx)) * stepx;				// point x coordinate inside the block
	double doty = (blocky - floor(blocky))*100;					// point y coordinate inside the block
	int dot_nr = static_cast<int>(doty) * (stepx+1) + static_cast<int>(dotx);	// point number from beginning of the block
	long offset = block_nr * (stepx + 1) * 202 + dot_nr * 2;	// offset of the first point in bytes

	short h1, h2, h3, h4;
	fseek(fileh, offset, SEEK_SET);
	// read 1st point
	if(fread(&h1, 2, 1, fileh)!=1){
		fclose(fileh);
		return result;
	}
	// read 2nd point
	if(fread(&h2, 2, 1, fileh)!=1){
		fclose(fileh);
		return result;
	}
	// move cursor by 1 line - 2 data points
	fseek(fileh,(stepx + 1 - 2)*2, SEEK_CUR);
	// read 3rd point
	if(fread(&h3, 2, 1, fileh)!=1){
		fclose(fileh);
		return result;
	}
	// read 4th point
	if(fread(&h4, 2, 1, fileh)!=1){
		fclose(fileh);
		return result;
	}
	if(h1 == -9999 || h2 == -9999 || h3 == -9999 || h4 == -9999){
		fclose(fileh);
		return result;
	}
	double h12 = h1 + (h2-h1)*(dotx - floor(dotx));
	double h34 = h3 + (h4-h3)*(dotx - floor(dotx));
	result.Height = static_cast<short>( h12 + (h34-h12)*(doty - floor(doty)));// get hight [double]
	result.M_Type = 0;
	result.Error = 0;
	fclose(fileh);
	return result;
	

}

#endif