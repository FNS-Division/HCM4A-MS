#ifndef _HCM4AMobServ_gisObjects_h_
#define _HCM4AMobServ_gisObjects_h_

using namespace Upp;


struct CountryGeoJon {
    String CountryName;
    Array<Array<Pointf>> Border;
    /**
     * Represents a country with its name and border coordinates.
     *
     * Parameters:
     * CountryName (String): The name of the country.
     * Border (Array<Array<Pointf>>): The border coordinates of the country.
     *
     * Constraints:
     * - The border coordinates should be in a valid format.
     */
};

struct GeoBorder {
    String CountryName;
    String Distance;
    Array<Pointf> Points;
    Array<Pointf> Points10;
    /**
     * Represents a geographical border with associated points.
     *
     * Parameters:
     * CountryName (String): The name of the country.
     * Distance (String): The distance related to the border.
     * Points (Array<Pointf>): The points defining the border.
     * Points10 (Array<Pointf>): Additional points related to the border.
     *
     * Constraints:
     * - The points should be in a valid format.
     */
};

struct GeoTopoMorpho {
    bool IsTopo;
    Point PointLD;
    String Filename;
    Array<int16> data;
    /**
     * Represents topographical and morphological data.
     *
     * Parameters:
     * IsTopo (bool): Indicates if the data is topographical.
     * PointLD (Point): A point related to the data.
     * Filename (String): The name of the file containing the data.
     * data (Array<int16>): The data values.
     *
     * Constraints:
     * - The data should be in a valid format.
     */
};

class GeoData {
private:
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
     * - The function assumes that the GeoJSON file exists and is correctly formatted.
     */
    void LoadGeoJson();

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
     * - The function assumes that the border file exists and is correctly formatted.
     */
    void LoadBorders();

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
    void LoadListOfCountries();

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
    void LoadTopoMorho();

public:
    int ERROR_HEIGHT = -9999;
    Array<CountryGeoJon> CountryGeoJson;
    Array<GeoBorder> Borders;
    Array<String> ListOfCountries;
    Array<GeoTopoMorpho> TopoMorpho;
    
    String pathToTopo, pathToMorpho, pathToGeoJson, pathToBorders, fileWithCountriesCodes = "";

    String lastStatus;

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
    void LoadGeoData();
};

#endif
