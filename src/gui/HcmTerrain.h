#ifndef _HCM4AMobileService_HcmTerrain_h_
#define _HCM4AMobileService_HcmTerrain_h_

#include <algorithm>
#include <cmath>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <filesystem>

struct PointHT {
	int x;
	int y;

	bool operator==(const PointHT& other) const { return x == other.x && y == other.y; }
};

struct HcmTerrain {
	const double ERROR_HEIGHT = -9999.0;
	
	std::string PathToTopo = "";   // "C:\\Maps\\Topo\\"
	std::string PathToMorpho = ""; // "C:\\Maps\\Morpho\\"

	std::vector<std::vector<std::byte>> fileContentsH;
	std::vector<std::vector<std::byte>> fileContentsM;

	std::vector<PointHT> openedPointsH;
	std::vector<PointHT> openedPointsM;

	constexpr static double DEG_TO_RAD = M_PI / 180.0;
	constexpr static double RAD_TO_DEG = 180.0 / M_PI;
	const double EARTH_RADIUS_KM = 6371.29;

	// Profile variables
	std::vector<double> T_Prof; // Terrain profile heights
	std::vector<int> M_Prof;    // Morphology profile types
	std::vector<double> Longis;
	std::vector<double> Latis;
	std::vector<double> Dists;

	double stepProfile = 0.1; // Step distance in kilometers
	int countSteps = 0;
	
	bool isErrorHeigth(double height){
		return fabs(height - ERROR_HEIGHT) < 1e-3;
	}

	std::string generateFileName(const double& longitude, const double& latitude, bool isTopo)
	{
		int LOD = static_cast<int>(std::floor(longitude));
		int LAD = static_cast<int>(std::floor(latitude));
		char filename[16];
		snprintf(filename, sizeof(filename), "%c%03d%c%02d.%s",
				(LOD >= 0) ? 'e' : 'w', std::abs(LOD),
				(LAD >= 0) ? 'n' : 's', std::abs(LAD),
				(std::abs(LAD) < 50) ? "33" : "63");
		std::string pref = isTopo ? "E" : "M";
		if (PathToTopo.substr(PathToTopo.size() - 1) != "\\"){
			PathToTopo = PathToTopo + "\\";
		}
		if (PathToMorpho.substr(PathToMorpho.size() - 1) != "\\"){
			PathToMorpho = PathToMorpho + "\\";
		}
		
		return isTopo ? PathToTopo + std::string(filename).substr(0,4) + "\\" + filename + pref : 
						PathToMorpho + std::string(filename).substr(0,4) + "\\" + filename + pref;
	}

	const std::vector<std::byte>& getFileContentByPoint(const double& longitude,
														const double& latitude, bool isTopo)
	{
		auto& fileContents = isTopo ? fileContentsH : fileContentsM;
		auto& openedPoints = isTopo ? openedPointsH : openedPointsM;

		PointHT pb;
		pb.x = std::floor(longitude);
		pb.y = std::floor(latitude);

		auto it = std::find(openedPoints.begin(), openedPoints.end(), pb);
		if(it != openedPoints.end()) {
			size_t index = std::distance(openedPoints.begin(), it);
			return fileContents[index];
		}

		std::string fileName = generateFileName(longitude, latitude, isTopo);
		std::ifstream file(fileName, std::ios::binary);
		if(!file.is_open()) {
			static std::vector<std::byte> emptyVector;
			return emptyVector;
		}

		file.seekg(0, std::ios::end);
		size_t fileSize = file.tellg();
		file.seekg(0, std::ios::beg);

		std::vector<std::byte> content(fileSize);
		file.read(reinterpret_cast<char*>(content.data()), fileSize);

		openedPoints.push_back(pb);
		fileContents.push_back(std::move(content));

		return fileContents.back();
	}

	double GetHeigth(const double& longitude, const double& latitude){
		const std::vector<std::byte>& content =
			getFileContentByPoint(longitude, latitude, true);
		if(content.empty())
			return ERROR_HEIGHT;
		
		int LOD = floor(longitude);
		int LOR = lround((longitude - LOD) * 3599.0);
		int LAD = floor(latitude);
		int LAR = lround((latitude - LAD) * 3599.0);
	
		double BH = (LOR / 300);
		double BV = (LAR / 300);
		double EH = (LOR - BH * 300) / (300.0 / ((std::abs(LAD) < 50) ? 100 : 50));
		double EV = (LAR - BV * 300) / 3.0;
	
		int R = 12 * BV + BH + 1;
		int RESH = (std::abs(LAD) < 50) ? 101 : 51;
		
		int offset = (R - 1) * 202 * RESH + (int(EV) * RESH + int(EH)) * sizeof(short);
		short H1 = *reinterpret_cast<const short*>(&content[offset]);
		short H2 = *reinterpret_cast<const short*>(&content[offset + sizeof(short)]);
	
		offset = (R - 1) * 202 * RESH + (int(EV) * RESH + int(EH) + RESH) * sizeof(short);
		short H3 = *reinterpret_cast<const short*>(&content[offset]);
		short H4 = *reinterpret_cast<const short*>(&content[offset + sizeof(short)]);
			
		if (std::min({H1, H2, H3, H4}) == -9999) {
			return ERROR_HEIGHT;
		}
		double H12 = (double)H1 + (double)(H2 - H1) * fmod(EH, 1.0);
		double H34 = (double)H3 + (double)(H4 - H3) * fmod(EH, 1.0);
		return lround(H12 + (H34 - H12) * fmod(EV, 1.0));

	}

	int16_t GetMorpho(const double& longitude, const double& latitude)
	{
		if(PathToMorpho.empty())
			return 0;
		const std::vector<std::byte>& content =
			getFileContentByPoint(longitude, latitude, false);
		if(content.empty())
			return 0;
		
		int LOD = floor(longitude);
		int LOR = lround((longitude - LOD) * 3599.0);
		int LAD = floor(latitude);
		int LAR = lround((latitude - LAD) * 3599.0);
	
		double BH = (LOR / 300);
		double BV = (LAR / 300);
		double EH = (double)(LOR - BH * 300) / (300.0 / ((std::abs(LAD) < 50) ? 100 : 50));
		double EV = (double)(LAR - BV * 300) / 3.0;
	
		int R = 12 * BV + BH + 1;
		int RESH = (std::abs(LAD) < 50) ? 101 : 51;
		
		int offset = (R - 1) * 202 * RESH + (int(EV) * RESH + int(EH)) * sizeof(short);
		short M = *reinterpret_cast<const short*>(&content[offset]);
		return M;
	}

	double Distance(const double& longiA, const double& latiA, const double& longiB,
					const double& latiB)
	{
		double rg = M_PI / 180.0;
		double delta_lat = latiB - latiA;
		double delta_lon = longiB - longiA;
		// Haversine formula components
		double a = pow(sin(delta_lat / 2.0 * rg), 2) +
				cos(latiA * rg) * cos(latiB * rg) * pow(sin(delta_lon / 2.0 * rg), 2);
		return 2.0 * EARTH_RADIUS_KM * atan2(sqrt(a), sqrt(1.0 - a));
	}

	double Azimut(const double& longiA, const double& latiA, const double& longiB,
				const double& latiB)
	{
		double delta_lon = longiB - longiA;
		double rg = M_PI / 180.0;
		double Azi;

		// Compute numerator (y) and denominator (x) for atan2
		double y = sin(delta_lon * rg) * cos(latiB * rg);
		double x = cos(latiA * rg) * sin(latiB * rg) -
				sin(latiA * rg) * cos(latiB * rg) * cos(delta_lon * rg);

		// Calculate azimuth in degrees and adjust to [0, 360)
		Azi = atan2(y, x) * 180.0 / M_PI;
		if(Azi < 0.0)
			Azi += 360.0;
		return Azi;
	}
	void GetNextPointWGS84(double startLongitude, double startLatitude,
					double azimut, double distanceKm, double& newLongitude,
					double& newLatitude) {
		const double a = 6378137.0;
		const double f = 1.0 / 298.257223563;
		const double b = a * (1.0 - f);
		const double pi = 3.14159265358979323846;
		
		double phi1 = startLatitude * pi / 180.0;
		double lambda1 = startLongitude * pi / 180.0;
		double alpha1 = azimut * pi / 180.0;
		double s = distanceKm * 1000.0;
		
		double U1 = atan((1.0 - f) * tan(phi1));
		double sinU1 = sin(U1);
		double cosU1 = cos(U1);
		
		double sigma1 = atan2(tan(U1), cos(alpha1));
		double sinAlpha = cos(U1) * sin(alpha1);
		double cosSqAlpha = 1.0 - sinAlpha * sinAlpha;
		
		double uSq = cosSqAlpha * (a * a - b * b) / (b * b);
		double A = 1.0 + uSq / 16384.0 * (4096.0 + uSq * (-768.0 + uSq * (320.0 - 175.0 * uSq)));
		double B = uSq / 1024.0 * (256.0 + uSq * (-128.0 + uSq * (74.0 - 47.0 * uSq)));
		
		double sigma = s / (b * A);
		double sigmaPrev = 2.0 * pi;
		double cos2SigmaM, sinSigma, cosSigma, deltaSigma;
		
		while (fabs(sigma - sigmaPrev) > 1e-12) {
			cos2SigmaM = cos(2.0 * sigma1 + sigma);
			sinSigma = sin(sigma);
			cosSigma = cos(sigma);
			deltaSigma = B * sinSigma * (cos2SigmaM + B / 4.0 * 
					(cosSigma * (-1.0 + 2.0 * cos2SigmaM * cos2SigmaM) - 
						B / 6.0 * cos2SigmaM * (-3.0 + 4.0 * sinSigma * sinSigma) * 
						(-3.0 + 4.0 * cos2SigmaM * cos2SigmaM)));
			sigmaPrev = sigma;
			sigma = s / (b * A) + deltaSigma;
		}
		
		double tmp = sinU1 * sinSigma - cosU1 * cosSigma * cos(alpha1);
		double phi2 = atan2(sinU1 * cosSigma + cosU1 * sinSigma * cos(alpha1),
						(1.0 - f) * sqrt(sinAlpha * sinAlpha + tmp * tmp));
		
		double lambda = atan2(sinSigma * sin(alpha1),
						cosU1 * cosSigma - sinU1 * sinSigma * cos(alpha1));
		double C = f / 16.0 * cosSqAlpha * (4.0 + f * (4.0 - 3.0 * cosSqAlpha));
		double L = lambda - (1.0 - C) * f * sinAlpha * 
				(sigma + C * sinSigma * (cos2SigmaM + C * cosSigma * (-1.0 + 2.0 * cos2SigmaM * cos2SigmaM)));
		
		double lambda2 = lambda1 + L;
		
		newLatitude = phi2 * 180.0 / pi;
		newLongitude = fmod(lambda2 * 180.0 / pi + 540.0, 360.0) - 180.0;
	}
	void GetNextPoint(double startLongitude, double startLatitude,
											double azimut, double distanceKm, double& newLongitude,
											double& newLatitude)
	{
		newLongitude = startLongitude;
		newLatitude = startLatitude;
		
		if(distanceKm == 0)	return;

		double latRad = startLatitude * DEG_TO_RAD;
		double lonRad = startLongitude * DEG_TO_RAD;
		double bearingRad = azimut * DEG_TO_RAD;
		double angularDistance = distanceKm / EARTH_RADIUS_KM;

		double newLatRad = asin(sin(latRad) * cos(angularDistance) +
								cos(latRad) * sin(angularDistance) * cos(bearingRad));

		double deltaLonRad = atan2(sin(bearingRad) * sin(angularDistance) * cos(latRad),
								cos(angularDistance) - sin(latRad) * sin(newLatRad));

		double newLonRad = lonRad + deltaLonRad;
		// [-180, 180]
		newLonRad = fmod((newLonRad + 3 * M_PI), (2 * M_PI)) - M_PI;

		newLatitude = newLatRad * RAD_TO_DEG;
		newLongitude = newLonRad * RAD_TO_DEG;
	}
	void GetProfile(double LongA, double LatA, double LongB, double LatB, bool p2p = false,
					double hTx = 0, double hRx = 0)
	{
		// implemetation method from C# v7
		double lat1 = LatA * M_PI / 180.0;
		double lat2 = LatB * M_PI / 180.0;
		double deltaLat = (LatB - LatA) * M_PI / 180.0;
		double deltaLong = (LongB - LongA) * M_PI / 180.0;
		
		double aHav = pow(sin(deltaLat / 2.0), 2) + 
					cos(lat1) * cos(lat2) * pow(sin(deltaLong / 2.0), 2);
		double num2 = 2.0 * 6371.29 * atan2(sqrt(aHav), sqrt(1 - aHav));
			
		int num3 = static_cast<int>(num2 / 0.1);
		double num4 = num2 / num3 / 6371.29;
		num3++;
		
		double num = 0;
		int item, item2;
		if (p2p) {
			num = static_cast<double>(hRx - hTx) / (num3 - 1.0);
			item = 0;
			item2 = 0;
		} else {
			item = hRx;
			item2 = hTx;
		}

		double deltaLongDir = (LongB - LongA) * M_PI / 180.0;
		double latARad = LatA * M_PI / 180.0;
		double latBRad = LatB * M_PI / 180.0;
		
		double y = sin(deltaLongDir) * cos(latBRad);
		double x = cos(latARad) * sin(latBRad) - 
				sin(latARad) * cos(latBRad) * cos(deltaLongDir);
		double angleInDegree = (atan2(y, x) * 180.0 / M_PI);
		if (angleInDegree < 0.0) angleInDegree += 360.0;
	
		double num5 = sin(latARad);
		double num6 = cos(latARad);
		double num7 = sin(angleInDegree * M_PI / 180.0);
		double num8 = cos(angleInDegree * M_PI / 180.0);
		
		countSteps = num3;
		T_Prof.resize(countSteps);
		M_Prof.resize(countSteps);

		Longis.resize(countSteps);
		Latis.resize(countSteps);
		Dists.resize(countSteps);
		
	
		for (int i = 0; i <= static_cast<int>(round(static_cast<double>(num3) / 2.0)); i++) {
			double num9 = (i - 1) * num4;
			double num10 = sin(num9);
			double num11 = cos(num9);
			num9 = num5 * num11 + num6 * num10 * num8;
			double lat = (asin(num9) * 180.0 / M_PI);
			double longi = LongA + 
				(atan2(num7 * num10 * num6, num11 - num5 * num9) * 180.0 / M_PI);
			
			
			T_Prof[i] = GetHeigth(longi, lat);
			M_Prof[i] = GetMorpho(longi, lat);
			Longis[i] = longi;
			Latis[i] = lat;
			
			if (p2p) {
				T_Prof[i] = T_Prof[i] - static_cast<int>(round(hTx + num * (i - 1)));
			}
		}
	
		deltaLongDir = (LongA - LongB) * M_PI / 180.0;
		latARad = LatB * M_PI / 180.0;
		latBRad = LatA * M_PI / 180.0;
		
		y = sin(deltaLongDir) * cos(latBRad);
		x = cos(latARad) * sin(latBRad) - 
			sin(latARad) * cos(latBRad) * cos(deltaLongDir);
		angleInDegree = (atan2(y, x) * 180.0 / M_PI);
		if (angleInDegree < 0.0) angleInDegree += 360.0;
	
		num5 = sin(latARad);
		num6 = cos(latARad);
		num7 = sin(angleInDegree * M_PI / 180.0);
		num8 = cos(angleInDegree * M_PI / 180.0);
	
		for (int j = static_cast<int>(round(static_cast<double>(num3) / 2.0)) + 1; j < num3; j++) {
			double num9 = (num3 - j) * num4;
			double num10 = sin(num9);
			double num11 = cos(num9);
			num9 = num5 * num11 + num6 * num10 * num8;
			double lat = (asin(num9) * 180.0 / M_PI);
			double longi = LongB + 
				(atan2(num7 * num10 * num6, num11 - num5 * num9) * 180.0 / M_PI);
			
			T_Prof[j] = GetHeigth(longi, lat);
			M_Prof[j] = GetMorpho(longi, lat);
			Longis[j] = longi;
			Latis[j] = lat;
			
			if (p2p) {
				T_Prof[j] = T_Prof[j] - static_cast<int>(round(hRx - num * (num3 - j)));
			}
		}
}
	
	
	void GetProfileSphera(double LongA, double LatA, double LongB, double LatB, bool p2p = false,
					double hTx = 0, double hRx = 0)
	{
		T_Prof.clear();
		M_Prof.clear();

		// Calculate distance and directions
		double distance = Distance(LongA, LatA, LongB, LatB);
		double dir_A_B = Azimut(LongA, LatA, LongB, LatB);
		double dir_B_A = Azimut(LongB, LatB, LongA, LatA);

		// Calculate number of points
		countSteps = std::round(distance / stepProfile);
		if(countSteps < 2)
			return;
		double K = (hRx - hTx) / countSteps;

		T_Prof.resize(countSteps);
		M_Prof.resize(countSteps);

		Longis.resize(countSteps);
		Latis.resize(countSteps);
		Dists.resize(countSteps);
		double newLongitude, newLatitude;

		for(int PC = 0; PC < countSteps; ++PC) {
			if(PC < countSteps / 2) {
				GetNextPoint(LongA, LatA, dir_A_B, PC * stepProfile, newLongitude, newLatitude);
			}
			else {
				GetNextPoint(LongB, LatB, dir_B_A, (countSteps - PC) * stepProfile, newLongitude, newLatitude);
			}
			T_Prof[PC] = GetHeigth(newLongitude, newLatitude);
			M_Prof[PC] = GetMorpho(newLongitude, newLatitude);
			Longis[PC] = newLongitude;
			Latis[PC] = newLatitude;
			if(p2p) {
				if(PC < countSteps / 2) {
					T_Prof[PC] -= hTx + K * PC;
				}
				else {
					T_Prof[PC] -= hRx + K * (countSteps - PC);
				}
			}
		}
		
		
		
		double kLong = (LongB - LongA) / countSteps;
		double kLati = (LatB - LatA) / countSteps;
		
		for(int PC = 0; PC < countSteps; ++PC) {
			T_Prof[PC] = GetHeigth(LongA + kLong * PC, LatA + kLati * PC);
			M_Prof[PC] = GetMorpho(LongA + kLong * PC, LatA + kLati * PC);
			Longis[PC] = LongA + kLong * PC;
			Latis[PC] = LongA + kLong * PC;
		}

//		Dists[0] = 0;
//		for(int PC = 1; PC < countSteps; ++PC) {
//			Dists[PC] = Distance(Longis[PC - 1], Latis[PC - 1], Longis[PC], Latis[PC]);
//		}

	}
};

#endif
