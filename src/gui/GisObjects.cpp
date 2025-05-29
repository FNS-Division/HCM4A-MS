#ifndef _HCM4AMobServ_gisObjects_cpp_
#define _HCM4AMobServ_gisObjects_cpp_

#include <CtrlLib/CtrlLib.h>
#include <Core/Core.h>

#include "GisObjects.h"


String GetTopoMorphoFileName(double longitude, double latitude, bool isTopo) {
    /**
     * Generates a filename for topographical or morphological data based on coordinates.
     *
     * Parameters:
     * longitude (double): The longitude coordinate.
     * latitude (double): The latitude coordinate.
     * isTopo (bool): Indicates if the data is topographical.
     *
     * Returns:
     * String: The generated filename.
     *
     * Constraints:
     * - The function assumes valid coordinate values.
     */
    std::string filename;
    filename += (longitude >= 0) ? 'E' : 'W';
    if (std::abs(longitude) >= 100) {
        filename += "";
    } else if (std::abs(longitude) >= 10) {
        filename += "0";
    } else {
        filename += "00";
    }
    filename += std::to_string(static_cast<int>(std::abs(longitude))).substr(0, 3);
    filename += (latitude > -1) ? 'N' : 'S';
    if (std::abs(latitude) < 10) {
        filename += "0";
    }
    filename += std::to_string(static_cast<int>(std::abs(latitude))).substr(0, 2);
    filename += (std::abs(latitude) < 50) ? ".33" : ".63";
    filename += isTopo ? 'E' : 'M';
    return filename;
}



void FindFiles(const String& directory, const String& mask, Array<String>& result) {
    /**
     * Finds files in a directory matching a given mask.
     *
     * Parameters:
     * directory (const String&): The directory to search in.
     * mask (const String&): The file mask to match.
     * result (Array<String>&): The array to store the matching file names.
     *
     * Returns:
     * void
     *
     * Constraints:
     * - The directory must exist and be accessible.
     */
    FindFile ff(AppendFileName(directory, "*"));
    while (ff) {
        String fullPath = AppendFileName(directory, ff.GetName());
        if (ff.IsFile() && PatternMatchMulti(mask, ff.GetName())) {
            result.Add(fullPath);
        } else if (ff.IsFolder() && ff.GetName() != "." && ff.GetName() != "..") {
            FindFiles(fullPath, mask, result);
        }
        ff.Next();
    }
}

void GeoData::LoadGeoData() {
    /**
     * Loads all geographical data.
     *
     * Parameters:
     * None
     *
     * Returns:
     * void
     *
     * Constraints:
     * - The function assumes that all necessary files exist and are correctly formatted.
     */
    // LoadGeoJson();
    LoadBorders();
    LoadListOfCountries();
    LoadTopoMorho();
}

void GeoData::LoadTopoMorho() {
    /**
     * Loads topographical and morphological data.
     *
     * Parameters:
     * None
     *
     * Returns:
     * void
     *
     * Constraints:
     * - The function assumes that the topographical and morphological files exist and are correctly formatted.
     */
    Array<String> files;
    String directory = pathToTopo;
    String mask = "*.33E";
    FindFiles(directory, mask, files);
    mask = "*.63E";
    FindFiles(directory, mask, files);
    directory = pathToMorpho;
    mask = "*.33M";
    FindFiles(directory, mask, files);
    mask = "*.63M";
    FindFiles(directory, mask, files);

    TopoMorpho.SetCount(files.GetCount());

    for (int ifi = 0; ifi < files.GetCount(); ifi++) {
        TopoMorpho[ifi].Filename = files[ifi];
        TopoMorpho[ifi].IsTopo = (files[ifi].Find(".33E") >= 0) || (files[ifi].Find(".63E") >= 0) ;
        String ss = GetFileName(files[ifi]);
        ss = ss.Left(ss.FindFirstOf("."));

        bool flagE = true, flagS = true;
        int ewPos = ss.Find("E");
        int snPos = ss.Find("S");
        if (ewPos < 0) {
            ewPos = ss.Find("W");
            flagE = false;
        }
        if (snPos < 0) {
            snPos = ss.Find("N");
            flagS = false;
        }

        TopoMorpho[ifi].PointLD.x = StrInt(ss.Mid(ewPos + 1, snPos - ewPos - 1));
        TopoMorpho[ifi].PointLD.y = StrInt(ss.Mid(snPos + 1));
        if (!flagE) {
            TopoMorpho[ifi].PointLD.x *= -1;
        }
        if (flagS) {
            TopoMorpho[ifi].PointLD.y *= -1;
        }
    }
}

void GeoData::LoadListOfCountries() {
    /**
     * Loads the list of countries.
     *
     * Parameters:
     * None
     *
     * Returns:
     * void
     *
     * Constraints:
     * - The function assumes that the list of countries file exists and is correctly formatted.
     */
    ListOfCountries.Clear();

    FileIn file(fileWithCountriesCodes);
    if (!file) {
        return;
    }
    while (!file.IsEof()) {
        String line = file.GetLine();
        if (!line.IsEmpty()) {
            ListOfCountries.Add(line);
        }
    }
}

void GeoData::LoadBorders() {
    /**
     * Loads border data.
     *
     * Parameters:
     * None
     *
     * Returns:
     * void
     *
     * Constraints:
     * - The function assumes that the border files exist and are correctly formatted.
     */
    String directory = pathToBorders;

    Array<String> files;
    String mask = "*.*";
    FindFiles(directory, mask, files);

    Borders.Clear();
    FileIn file;
    int index = 0;

    for (int ifi = 0; ifi < files.GetCount(); ifi++) {
        if (!file.Open(files[ifi])) {
            continue;
        }

        Borders.Add();

        Borders[index].CountryName = GetFileName(files[ifi]);
        Borders[index].Distance = GetFileExt(files[ifi]);
        Borders[index].CountryName.Replace(Borders[index].Distance, "");
        Borders[index].Distance.Replace(".", "");

        Pointf point;
        int counter = 0;
        file.Seek(0);
        while (!file.IsEof()) {
            if (file.Get(&point.x, sizeof(point.x)) != sizeof(point.x)) {
                break;
            }
            point.x = point.x * 57.2957795130;
            if (file.Get(&point.y, sizeof(point.y)) != sizeof(point.y)) {
                break;
            }
            point.y = point.y * 57.2957795130;
            counter++;

            counter % 11 != 0 ? Borders[index].Points.Add(point) : Borders[index].Points10.Add(point);
        }
        index++;
    }
    file.Close();
}

void GeoData::LoadGeoJson() {
    /**
     * Loads GeoJSON data.
     *
     * Parameters:
     * None
     *
     * Returns:
     * void
     *
     * Constraints:
     * - The function assumes that the GeoJSON files exist and are correctly formatted.
     */
    String directory = pathToBorders;
    String mask = "*.geojson";
    Array<String> files;
    FindFiles(directory, mask, files);
    CountryGeoJson.Clear();
    CountryGeoJon cntr;
    for (int ifi = 0; ifi < files.GetCount(); ifi++) {
        String jsonData = LoadFile(files[ifi]);
        if (jsonData.IsEmpty()) {
            Cout() << "Error loading GeoJSON.\n";
            return;
        }
        Value json = ParseJSON(jsonData);
        if (json.IsError()) {
            Cout() << "Error parsing JSON.\n";
            return;
        }
        if (json["type"] != "FeatureCollection") {
            Cout() << "Error: there are no FeatureCollection.\n";
            return;
        }
        const Value& features = json["features"];
        for (int i = 0; i < features.GetCount(); i++) {
            const Value& feature = features[i];
            if (feature["geometry"]["type"] != "Polygon" && feature["geometry"]["type"] != "MultiPolygon") {
                continue;
            }
            String name = feature["properties"]["name"];
            cntr.CountryName = name;

            const Value& coordinates = feature["geometry"]["coordinates"];

            if (feature["geometry"]["type"] == "Polygon") {
                cntr.Border.Add();
                for (int j = 0; j < coordinates[0].GetCount(); j++) {
                    double x = coordinates[0][j][0];
                    double y = coordinates[0][j][1];

                    cntr.Border[0].Add(Pointf(x, y));
                }
            } else if (feature["geometry"]["type"] == "MultiPolygon") {
                for (int j = 0; j < coordinates.GetCount(); j++) {
                    cntr.Border.Add();
                    for (int k = 0; k < coordinates[j][0].GetCount(); k++) {
                        double x = coordinates[j][0][k][0];
                        double y = coordinates[j][0][k][1];
                        cntr.Border[j].Add(Pointf(x, y));
                    }
                }
            }
        }
    }
}

#endif
