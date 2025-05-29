#ifndef _HCM4AMobServ_h
#define _HCM4AMobServ_h

#include <CtrlLib/CtrlLib.h>

using namespace Upp;

#define LAYOUTFILE <HCM4AMobileService/MobileService.lay>
#include <CtrlCore/lay.h>
#include "GisMap.h"
#include "GisObjects.h"







//#ifdef PLATFORM_WIN32
//	#define DLLFILENAME "hcm4mfs.dll"
//#else
//	#define DLLFILENAME "hcm4mfs.so"
//#endif
//#define DLIHEADER   <APP_PATH/hcm4ams.dli>
//#define DLIMODULE   Hcm4AmsDll
//







struct StationsTabDlg : WithStationsLayout<ParentCtrl> {
    /**
     * Constructor for StationsTabDlg.
     * Initializes the dialog for managing stations.
     */
    StationsTabDlg();
};

struct MapsTabDlg : WithMapsLayout<ParentCtrl> {
public:
    GisMap map;
    StatusBar statusBar;
    InfoCtrl infoLongLati, infoZoom, infoTopo, infoMorpho, infoDop;
    Option showTopoTile, showMorphoTile, showDegLines;

    /**
     * Constructor for MapsTabDlg.
     * Initializes the dialog for managing maps.
     */
    MapsTabDlg();

    /**
     * Changes the options for the map display.
     *
     * Parameters:
     * None
     *
     * Returns:
     * void
     *
     * Constraints:
     * - The function assumes that the map and options are correctly initialized.
     */
    void ChangeOptions();
};

struct StationsTestDlg : WithStationsTestLayout<ParentCtrl> {
    /**
     * Constructor for StationsTestDlg.
     * Initializes the dialog for testing stations.
     */
    StationsTestDlg();
};

struct ResultsLayoutDlg : WithResultsLayout<ParentCtrl> {
    /**
     * Constructor for ResultsLayoutDlg.
     * Initializes the dialog for displaying results.
     */
    ResultsLayoutDlg();
};

struct MobileServiceSettings {
    String PathToTopo = "";
    String PathToMorpho = "";
    String PathToBorders = "";
    String FileWithCountriesCodes = "";
    String DefaultExtensionForRecord = "*.txt";
    String LastPathForOpeningDialog = "";

    /**
     * Saves the current settings to a file.
     *
     * Parameters:
     * None
     *
     * Returns:
     * void
     *
     * Constraints:
     * - The function assumes that the settings are correctly populated.
     */
    void Save();

    /**
     * Loads the settings from a file.
     *
     * Parameters:
     * None
     *
     * Returns:
     * void
     *
     * Constraints:
     * - The function assumes that the settings file exists and is correctly formatted.
     */
    void Load();
};

struct MobileStation {
    String Name = "";
    double Longitude, Latitude = 0;
    double FreqTx, FreqRx, Azimut, Heigth, Radius = 0;
    bool isRefList;
    String Filename = "";
    String Land = "";
    int IndexInFile = 0;
};

class MobileService : public WithMobileServiceLayout<TopWindow> {
public:
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
    void FindFiles(const String& directory, const String& mask, Array<String>& result);

    struct mapCoord {
        int id;
        double longitude;
        double latitude;
        int zoom;
    };

    Vector<mapCoord> mapTreeCoordArray;
    Vector<String> debugFiles;

    String pathRef, pathTest;
    Array<String> ListRefStationFiles, ListTestStationFiles;
    Array<MobileStation> StationsList;

    String OpenedFileName = "";
    int indexOpenedStation = -1;

    /**
     * Refreshes the list of stations.
     *
     * Parameters:
     * None
     *
     * Returns:
     * void
     *
     * Constraints:
     * - The function assumes that the stations list is correctly populated.
     */
    void RefreshListOfStations();

    /**
     * Reads station data from the controls.
     *
     * Parameters:
     * None
     *
     * Returns:
     * String: The station data as a string.
     *
     * Constraints:
     * - The function assumes that the controls are correctly populated.
     */
    String ReadStationFromControls();

    /**
     * Reads the station header from the controls.
     *
     * Parameters:
     * None
     *
     * Returns:
     * String: The station header as a string.
     *
     * Constraints:
     * - The function assumes that the controls are correctly populated.
     */
    String ReadStationHeaderFromControls();

    /**
     * Saves a station to a file.
     *
     * Parameters:
     * index (int): The index of the station to save.
     *
     * Returns:
     * void
     *
     * Constraints:
     * - The function assumes that the station list is correctly populated.
     */
    void StationSaveToFile(int index);

    /**
     * Loads the header from a file.
     *
     * Parameters:
     * None
     *
     * Returns:
     * void
     *
     * Constraints:
     * - The function assumes that the file exists and is correctly formatted.
     */
    void LoadHeader();

    /**
     * Loads a station from a file.
     *
     * Parameters:
     * index (int): The index of the station to load.
     *
     * Returns:
     * void
     *
     * Constraints:
     * - The function assumes that the file exists and is correctly formatted.
     */
    void LoadStation(int index);

    /**
     * Clears the header controls.
     *
     * Parameters:
     * None
     *
     * Returns:
     * void
     *
     * Constraints:
     * - The function assumes that the controls are correctly initialized.
     */
    void ClearHeaderControls();

    /**
     * Clears the station controls.
     *
     * Parameters:
     * None
     *
     * Returns:
     * void
     *
     * Constraints:
     * - The function assumes that the controls are correctly initialized.
     */
    void ClearStationControls();

    /**
     * Deletes a station from a file.
     *
     * Parameters:
     * index (int): The index of the station to delete.
     *
     * Returns:
     * void
     *
     * Constraints:
     * - The function assumes that the file exists and is correctly formatted.
     */
    void StationDeleteFromFile(int index);

    TabCtrl mainTab;
    StationsTabDlg tabStations;
    StationsTestDlg tabStationsTest;
    ResultsLayoutDlg tabRes;
    MapsTabDlg tabMap;
    WithSettingsLayout<TopWindow> SettingsLO;
    FileSel fs;
    FileSel faSettings;
    MobileServiceSettings cnfg;
    TabCtrl::Style s = TabCtrl::StyleDefault();
    MenuBar menu;

    /**
     * Initializes the main menu.
     *
     * Parameters:
     * bar (Bar&): The menu bar to initialize.
     *
     * Returns:
     * void
     *
     * Constraints:
     * - The function assumes that the menu bar is correctly initialized.
     */
    void MainMenu(Bar& bar);

    /**
     * Reads a file with station data.
     *
     * Parameters:
     * filename (String): The name of the file to read.
     * index (int): The index of the station in the file.
     * isRefList (bool): Whether the station is in the reference list.
     *
     * Returns:
     * void
     *
     * Constraints:
     * - The function assumes that the file exists and is correctly formatted.
     */
    void ReadFileWithStation(String filename, int index, bool isRefList);

    /**
     * Refreshes the settings.
     *
     * Parameters:
     * None
     *
     * Returns:
     * void
     *
     * Constraints:
     * - The function assumes that the settings are correctly populated.
     */
    void RefreshSettings();

    /**
     * Prepares the settings.
     *
     * Parameters:
     * None
     *
     * Returns:
     * void
     *
     * Constraints:
     * - The function assumes that the settings are correctly initialized.
     */
    void PrepareSettings();

    /**
     * Refreshes the map tree.
     *
     * Parameters:
     * None
     *
     * Returns:
     * void
     *
     * Constraints:
     * - The function assumes that the map tree is correctly initialized.
     */
    void RefreshMapTree();

    /**
     * Prepares the map tree.
     *
     * Parameters:
     * None
     *
     * Returns:
     * void
     *
     * Constraints:
     * - The function assumes that the map tree is correctly initialized.
     */
    void PrepareMapTree();

    /**
     * Refreshes the stations lists.
     *
     * Parameters:
     * None
     *
     * Returns:
     * void
     *
     * Constraints:
     * - The function assumes that the stations lists are correctly populated.
     */
    void RefreshStationsLists();

    /**
     * Prepares the stations lists.
     *
     * Parameters:
     * None
     *
     * Returns:
     * void
     *
     * Constraints:
     * - The function assumes that the stations lists are correctly initialized.
     */
    void PrepareStationsLists();

    /**
     * Reads all file stations.
     *
     * Parameters:
     * None
     *
     * Returns:
     * void
     *
     * Constraints:
     * - The function assumes that the files exist and are correctly formatted.
     */
    void ReadAllFileStations();

    /**
     * Prepares the result layout.
     *
     * Parameters:
     * None
     *
     * Returns:
     * void
     *
     * Constraints:
     * - The function assumes that the result layout is correctly initialized.
     */
    void PrepareResultLayout();

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
     */
    void Calculation(int mode);

    WithStationEditorLayout<TopWindow> StationEditor;

    /**
     * Sets the default station.
     *
     * Parameters:
     * None
     *
     * Returns:
     * void
     *
     * Constraints:
     * - The function assumes that the station editor is correctly initialized.
     */
    void SetStationDefault();

    /**
     * Clears the station editor.
     *
     * Parameters:
     * None
     *
     * Returns:
     * void
     *
     * Constraints:
     * - The function assumes that the station editor is correctly initialized.
     */
    void SetStationClear();

    /**
     * Prepares the station editor.
     *
     * Parameters:
     * None
     *
     * Returns:
     * void
     *
     * Constraints:
     * - The function assumes that the station editor is correctly initialized.
     */
    void PrepareStationEditor();

    /**
     * Loads a station to the editor.
     *
     * Parameters:
     * filename (String): The name of the file to read.
     * index (int): The index of the station in the file.
     *
     * Returns:
     * bool: True if the station is successfully loaded, false otherwise.
     *
     * Constraints:
     * - The function assumes that the file exists and is correctly formatted.
     */
    bool LoadStationToEditor(String filename, int index);

    /**
     * Converts a station string to controls.
     *
     * Parameters:
     * source (String): The source string.
     * numStation (int): The number of the station.
     *
     * Returns:
     * bool: True if the conversion is successful, false otherwise.
     *
     * Constraints:
     * - The function assumes that the source string is correctly formatted.
     */
    bool StationStringToControls(String source, int numStation);

    /**
     * Converts a header string to controls.
     *
     * Parameters:
     * source (String): The source string.
     *
     * Returns:
     * bool: True if the conversion is successful, false otherwise.
     *
     * Constraints:
     * - The function assumes that the source string is correctly formatted.
     */
    bool HeaderStringToControls(String source);

    /**
     * Saves the station editor to a file.
     *
     * Parameters:
     * filename (String): The name of the file to save.
     *
     * Returns:
     * bool: True if the save is successful, false otherwise.
     *
     * Constraints:
     * - The function assumes that the file can be written to.
     */
    bool SaveStationEditorToFile(String filename);

    /**
     * Validates the station editor.
     *
     * Parameters:
     * None
     *
     * Returns:
     * bool: True if the validation is successful, false otherwise.
     *
     * Constraints:
     * - The function assumes that the station editor is correctly populated.
     */
    bool ValidateStationEditor();

    /**
     * Gets a new station file name.
     *
     * Parameters:
     * iReferenceStation (bool): Whether the station is a reference station.
     *
     * Returns:
     * String: The new station file name.
     *
     * Constraints:
     * - The function assumes that the file name can be generated.
     */
    String GetNewStationFileName(bool iReferenceStation);

    /**
     * Shows the station editor.
     *
     * Parameters:
     * mode (int): The mode of the editor.
     * index (int): The index of the station (default is 0).
     *
     * Returns:
     * void
     *
     * Constraints:
     * - The function assumes that the station editor is correctly initialized.
     */
    void ShowStationEditor(int mode, int index = 0);

    /**
     * Constructor for MobileService.
     * Initializes the mobile service.
     */
    void Close();
    MobileService();
};

#endif
