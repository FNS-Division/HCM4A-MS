#ifndef _HCM4AMobileService_Common_h_
#define _HCM4AMobileService_Common_h_

#include <iomanip>
#include <iostream>
#include <sstream>

String PutSepToEndPath(String path) {
	/**
	* Ensures that the path ends with a directory separator.
	*
	* Parameters:
	* path (String): The path to modify.
	*
	* Returns:
	* String: The modified path with a separator at the end.
	*
	* Constraints:
	* - The function assumes that the path is valid.
	*/
	if (path.GetCount() < 2) return path;

	if (path[path.GetCount()] == DIR_SEP) return path;
	return path + DIR_SEP;
}

String MakeStringBlank(String source, int len) {
	/**
	* Pads a string to a specified length by adding spaces to the beginning.
	*
	* Parameters:
	* source (String): The original string.
	* len (int): The desired length of the string.
	*
	* Returns:
	* String: The padded string.
	*
	* Constraints:
	* - The function assumes that the length is positive.
	*/
	String ss = source;
	if (source.GetCount() < len) {
		for (int i = 0; i < (len - source.GetCount()); i++) {
			ss = " " + ss;
		}
	}
	return ss;
}

void LoadFileToDropList(const String& filePath, DropList& dropList) {
	/**
	* Loads the contents of a file into a drop list.
	*
	* Parameters:
	* filePath (const String&): The path to the file.
	* dropList (DropList&): The drop list to populate.
	*
	* Returns:
	* void
	*
	* Constraints:
	* - The function assumes that the file exists and is readable.
	*/
	FileIn file(filePath);
	if (!file) {
		PromptOK("Failed to open file: " + filePath);
		file.Close();
		return;
	}
	dropList.Clear();
	String line;
	while (!file.IsEof()) {
		line = TrimBoth(file.GetLine());
		dropList.Add(line);
	}
	file.Close();
}

Pointf ReadCoordinate(String source) {
	/**
	* Reads geographical coordinates from a string.
	*
	* Parameters:
	* source (String): The string containing the coordinates.
	*
	* Returns:
	* Pointf: The parsed coordinates.
	*
	* Constraints:
	* - The function assumes that the input string is correctly formatted.
	*/
	Pointf pnt;
	try {
		String tmp;
		tmp = source.Mid(0, 3);
		pnt.x = ScanDouble(tmp);
		tmp = source.Mid(4, 2);
		pnt.x += ScanDouble(tmp) / 60;
		tmp = source.Mid(6, 2);
		pnt.x += ScanDouble(tmp) / 3600;
		if (ToUpper(source.Mid(3, 1)) == "W") pnt.x = pnt.x * -1;
	} catch (...) {
		return Pointf(0, 0);
	}
	try {
		String tmp;
		tmp = source.Mid(8, 2);
		pnt.y = ScanDouble(tmp);
		tmp = source.Mid(11, 2);
		pnt.y += ScanDouble(tmp) / 60;
		tmp = source.Mid(13, 2);
		pnt.y += ScanDouble(tmp) / 3600;
		if (ToUpper(source.Mid(10, 1)) == "S") pnt.y = pnt.y * -1;
	} catch (...) {
		return Pointf(0, 0);
	}
	return pnt;
}

String CoordinateToString(Pointf point) {
	/**
	* Converts geographical coordinates to a formatted string.
	*
	* Parameters:
	* point (Pointf): The coordinates to convert.
	*
	* Returns:
	* String: The formatted coordinates string.
	*
	* Constraints:
	* - The function assumes that the coordinates are valid.
	*/
	std::string coo = "               ";
	double lo = point.x;
	double la = point.y;

	if (lo >= 0.0) {
		coo[3] = 'E';
	} else {
		coo[3] = 'W';
		lo = -lo;
	}

	if (la >= 0.0) {
		coo[10] = 'N';
	} else {
		coo[10] = 'S';
		la = -la;
	}

	int i = static_cast<int>(lo);
	lo = (lo - i) * 60.0;
	int j = static_cast<int>(lo);
	lo = (lo - j) * 60.0;
	int k = std::round(lo);
	if (k >= 60) {
		k -= 60;
		j += 1;
		if (j >= 60) {
			j -= 60;
			i += 1;
		}
	}

	// Write the longitude part
	std::ostringstream oss;
	oss << std::setw(3) << std::setfill('0') << i;
	coo.replace(0, 3, oss.str());
	oss.str("");
	oss.clear();
	oss << std::setw(2) << std::setfill('0') << j;
	coo.replace(4, 2, oss.str());
	oss.str("");
	oss.clear();
	oss << std::setw(2) << std::setfill('0') << k;
	coo.replace(6, 2, oss.str());

	i = static_cast<int>(la);
	la = (la - i) * 60.0;
	j = static_cast<int>(la);
	la = (la - j) * 60.0;
	k = std::round(la);
	if (k >= 60) {
		k -= 60;
		j += 1;
		if (j >= 60) {
			j -= 60;
			i += 1;
		}
	}

	// Write the latitude part
	oss.str("");
	oss.clear();
	oss << std::setw(2) << std::setfill('0') << i;
	coo.replace(8, 2, oss.str());
	oss.str("");
	oss.clear();
	oss << std::setw(2) << std::setfill('0') << j;
	coo.replace(11, 2, oss.str());
	oss.str("");
	oss.clear();
	oss << std::setw(2) << std::setfill('0') << k;
	coo.replace(13, 2, oss.str());

	return coo;
}

double ReadFreq(String source) {
	/**
	* Reads a frequency value from a string.
	*
	* Parameters:
	* source (String): The string containing the frequency.
	*
	* Returns:
	* double: The parsed frequency value.
	*
	* Constraints:
	* - The function assumes that the input string is correctly formatted.
	*/
	try {
		double koef = 1.0;
		String tmp = ToUpper(source);
		if (tmp.Find('M') >= 0) tmp.Replace("M", "");
		if (tmp.Find('K') >= 0) {
			tmp.Replace("K", "");
			koef = 0.001;
		}
		if (tmp.Find('G') >= 0) {
			tmp.Replace("G", "");
			koef = 1000;
		}
		tmp.Replace(" ", "");
		if (tmp.GetLength() < 1) return 0;

		return ScanDouble(tmp) * koef;
	} catch (...) {
		return 0;
	}
}

String SetFreqString(String txt) {
	/**
	* Formats a frequency value as a string.
	*
	* Parameters:
	* txt (String): The frequency value to format.
	*
	* Returns:
	* String: The formatted frequency string.
	*
	* Constraints:
	* - The function assumes that the input string is correctly formatted.
	*/
	txt = TrimBoth(txt);
	if (txt.GetLength() == 0) return MakeStringBlank("", 11);
	if (txt == "-") return MakeStringBlank("", 11);
	try {
		double dd = std::stod(txt.ToStd());
		return MakeStringBlank(Format("%.5mf", dd), 11);
	} catch (...) {
		return MakeStringBlank("", 11);
	}
}

String SetMaxPowerString(String txt) {
	/**
	* Formats a maximum power value as a string.
	*
	* Parameters:
	* txt (String): The maximum power value to format.
	*
	* Returns:
	* String: The formatted maximum power string.
	*
	* Constraints:
	* - The function assumes that the input string is correctly formatted.
	*/
	txt = TrimBoth(txt);
	if (txt.GetLength() == 0) return MakeStringBlank("", 6);
	if (txt == "-") return MakeStringBlank("", 6);
	try {
		double dd = std::stod(txt.ToStd());
		return MakeStringBlank(Format("%.3mf", dd).Mid(0, 6), 6);
	} catch (...) {
		return MakeStringBlank("", 6);
	}
}

String SetAzimutString(String txt) {
	/**
	* Formats an azimuth value as a string.
	*
	* Parameters:
	* txt (String): The azimuth value to format.
	*
	* Returns:
	* String: The formatted azimuth string.
	*
	* Constraints:
	* - The function assumes that the input string is correctly formatted.
	*/
	txt = TrimBoth(txt);
	if (txt.GetLength() == 0) return MakeStringBlank("", 5);
	if (txt == "-") return MakeStringBlank("", 5);
	try {
		double dd = std::stod(txt.ToStd());
		return MakeStringBlank(Format("%.2mf", dd).Mid(0, 5), 5);
	} catch (...) {
		return MakeStringBlank("", 5);
	}
}

String SetElevationString(String txt) {
	/**
	* Formats an elevation value as a string.
	*
	* Parameters:
	* txt (String): The elevation value to format.
	*
	* Returns:
	* String: The formatted elevation string.
	*
	* Constraints:
	* - The function assumes that the input string is correctly formatted.
	*/
	txt = TrimBoth(txt);
	if (txt.GetLength() == 0) return MakeStringBlank("", 5);
	if (txt == "-") return MakeStringBlank("", 5);
	try {
		double dd = std::stod(txt.ToStd());
		txt = MakeStringBlank(Format("%.1mf", abs(dd)),5);
		if (dd < 0) txt = "-" + txt.Mid(1,4);
		return txt;
	} catch (...) {
		return MakeStringBlank("", 5);
	}
}

int GetCountStationsInFile(String filename) {
	/**
	* Gets the number of stations in a file.
	*
	* Parameters:
	* filename (String): The name of the file.
	*
	* Returns:
	* int: The number of stations in the file.
	*
	* Constraints:
	* - The function assumes that the file exists and is correctly formatted.
	*/
	if (!FileExists(filename)) return 0;
	FileIn f(filename);
	int fs = f.GetSize();
	f.Close();
	if ((fs % 219) != 0) return 0;
	return trunc(fs / 219) - 1;
}

String ReadStationFromFile(String filename, int index) {
	/**
	* Reads a station record from a file.
	*
	* Parameters:
	* filename (String): The name of the file.
	* index (int): The index of the station record.
	*
	* Returns:
	* String: The station record.
	*
	* Constraints:
	* - The function assumes that the file exists and is correctly formatted.
	*/
	String result = "";

	int cnt = GetCountStationsInFile(filename);
	if (cnt == 0) return result;
	if (index > (cnt - 1)) return result;

	FileIn f(filename);
	result = f.GetLine();
	f.Close();

	result = result.Mid(219 + 219 * index, 219);
	return result;
}

void DuplicateLastStationToFile(String filename) {
	/**
	* Duplicates the last station record in a file.
	*
	* Parameters:
	* filename (String): The name of the file.
	*
	* Returns:
	* void
	*
	* Constraints:
	* - The function assumes that the file exists and is correctly formatted.
	*/
	int cnt = GetCountStationsInFile(filename);
	if (cnt == 0) return;

	FileAppend f;
	f.Open(filename);
	f.Seek(0);
	String result = f.GetLine();
	f.Seek(f.GetSize());
	result = result.Mid(cnt * 219, 219);
	f.Put(result);
	f.Close();
}

void ChangeNameToStation(String filename, int index, String newName) {
	/**
	* Changes the name of a station record in a file.
	*
	* Parameters:
	* filename (String): The name of the file.
	* index (int): The index of the station record.
	* newName (String): The new name for the station.
	*
	* Returns:
	* void
	*
	* Constraints:
	* - The function assumes that the file exists and is correctly formatted.
	*/
	int cnt = GetCountStationsInFile(filename);
	if (cnt == 0) return;
	if (index > cnt) return;

	FileAppend f;
	f.Open(filename);
	f.Seek(0);
	String result = f.GetLine();

	String name = result.Mid((index) * 219 + 28, 20);
	String part1 = result.Mid(0, (index) * 219 + 28);
	String part2 = result.Mid((index) * 219 + 28 + 20, result.GetCount() - (index) * 219 + 28);
	newName = MakeStringBlank(newName + "_" + TrimBoth(name), 20).Mid(0, 20);
	result = part1 + newName + part2;

	f.Seek(0);
	f.Put(result);
	f.Close();
}

void DuplicateIndexStationToFile(String filename, int index) {
	/**
	* Duplicates a station record at a specific index in a file.
	*
	* Parameters:
	* filename (String): The name of the file.
	* index (int): The index of the station record to duplicate.
	*
	* Returns:
	* void
	*
	* Constraints:
	* - The function assumes that the file exists and is correctly formatted.
	*/
	int cnt = GetCountStationsInFile(filename);
	if (cnt == 0) return;
	if (index >= cnt) return;

	FileAppend f;
	f.Open(filename);
	f.Seek(0);
	String result = f.GetLine();
	f.Seek(f.GetSize());
	result = result.Mid((index + 1) * 219, 219);
	f.Put(result);
	f.Close();
}

void DeleteStationFromFile(String filename, int index) {
	/**
	* Deletes a station record from a file.
	*
	* Parameters:
	* filename (String): The name of the file.
	* index (int): The index of the station record to delete.
	*
	* Returns:
	* void
	*
	* Constraints:
	* - The function assumes that the file exists and is correctly formatted.
	*/
	int count = GetCountStationsInFile(filename);
	if (count == 0) return;
	if (index >= count) return;
	if ((index == 0) & (count == 1)) return;

	FileIn f(filename);
	f.Seek(0);
	String context = f.GetLine();
	f.Close();

	if (index + 1 == count) {
		context = context.Mid(0, 219 * (index + 1));
	} else {
		context = context.Mid(0, 219 * (index + 1)) + context.Mid(219 * (index + 2), 219 * (count - index - 1));
	}

	FileOut fo(filename);
	fo.Seek(0);
	fo.Put(context);
	fo.Close();
}

void AddEmptyStationToFile(String filename) {
	/**
	* Adds an empty station record to a file.
	*
	* Parameters:
	* filename (String): The name of the file.
	*
	* Returns:
	* void
	*
	* Constraints:
	* - The function assumes that the file exists and is correctly formatted.
	*/
	if (!FileExists(filename)) {
		FileOut fo(filename);
		fo.Close();
	}
	FileAppend f(filename);
	int fs = f.GetSize();
	f.Seek(fs);
	if (fs == 0){
		std::string dynamicString(219, ' ');
		f.Put(dynamicString);
	} else {
		if ((fs % 219) != 0) {
			f.Close();
			return;
		}
	}	
	String newStation;// = "  598.00000M2FLOT L014032025 Change this name 01D__008E111753N0716    6      91K9G2W27.540I124.0 -8.0 M      40006KA14002EB29           M                                                  B14022025        D__250001010111";
	newStation = " 1000.00000M2FLOT L001012025         New StationETH 00E000000N0000    0              0.000I 0.00  0.0 M       0                         M                                                  B01012025        ETH            ";
	f.Put(newStation);
	fs = f.GetSize();
	f.Close();
}

void MergeFilesWithStations(String mainFile, String addingFile) {
	/**
	* Merges station records from one file into another.
	*
	* Parameters:
	* mainFile (String): The main file to merge into.
	* addingFile (String): The file to merge from.
	*
	* Returns:
	* void
	*
	* Constraints:
	* - The function assumes that the files exist and are correctly formatted.
	*/
	int cnt = GetCountStationsInFile(addingFile);
	if (cnt == 0) return;

	String st = "";
	if (!FileExists(mainFile)) {
		for (int i = 0; i < 219; i++) st += " ";
		FileOut fi;
		fi.Open(mainFile);
		fi.Put(st);
		fi.Close();
	}

	FileAppend f;
	f.Open(mainFile);
	int fs = f.GetSize();
	if ((fs % 219) != 0) {
		f.Close();
		return;
	}
	f.Seek(fs);

	for (int i = 0; i < cnt; i++) {
		st = ReadStationFromFile(addingFile, i);
		f.Put(st);
	}
	f.Close();
}

#endif