#include "MobileService.h"
#include <CtrlLib/CtrlLib.h>
#include "GisMap.h"
#include "GisCountries.h"
#include "Calculation.h"
#include "Common.h"
#include "EditorLayout.h"
#include "HcmTerrain.h"
#include "Excel.h"


GUI_APP_MAIN {
	/**
	* Main entry point for the application.
	* Initializes the application and runs the main service.
	*/
	SetDefaultCharset(CHARSET_UTF8);
	MobileService().Run();
}

void MobileServiceSettings::Save() {
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
	String ss = WinPath(GetExeFolder() + "\\settings.ini");
	FileOut f(ss);
	if (f) {
		f.PutLine(PathToTopo);
		f.PutLine(PathToMorpho);
		f.PutLine(PathToBorders);
		f.PutLine(LastPathForOpeningDialog);
	}
	f.Close();
}

void MobileServiceSettings::Load() {
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
	FileIn f(WinPath(GetExeFolder() + "\\settings.ini"));
	if (f) {
		PathToTopo = f.GetLine();
		if (!DirectoryExists(PathToTopo)) PathToTopo = GetExeFolder(); 
		PathToMorpho = f.GetLine();
		if (!DirectoryExists(PathToMorpho)) PathToMorpho = GetExeFolder(); 
		PathToBorders = f.GetLine();
		if (!DirectoryExists(PathToBorders)) PathToBorders = GetExeFolder(); 
		LastPathForOpeningDialog = f.GetLine();
		if (!DirectoryExists(LastPathForOpeningDialog)) LastPathForOpeningDialog = GetExeFolder(); 
		
	}
	f.Close();

	if (PathToTopo.IsEmpty()) PathToTopo = "c:\\Maps\\Topo\\";
	if (PathToMorpho.IsEmpty()) PathToMorpho = "c:\\Maps\\Morpho\\";
	if (PathToBorders.IsEmpty()) PathToBorders = "c:\\Maps\\Border\\";
	if (LastPathForOpeningDialog.IsEmpty()) LastPathForOpeningDialog = GetExeFolder(); 

	FileWithCountriesCodes = WinPath(GetExeFolder() + "\\Files\\ListOfCountries.txt");
}

StationsTabDlg::StationsTabDlg() {
	/**
	* Constructor for StationsTabDlg.
	* Initializes the dialog layout.
	*/
	typedef StationsTabDlg CLASSNAME;
	CtrlLayout(*this);
}

void MapsTabDlg::ChangeOptions() {
	/**
	* Updates the map options based on user selections.
	*
	* Parameters:
	* None
	*
	* Returns:
	* void
	*
	* Constraints:
	* - The function assumes that the options are correctly set.
	*/
	map.options.showTopo = optionTopo;
	map.options.showMorpho = optionMorpho;
	map.options.showDegreeLines = optionDegLines;
	map.options.showGoogleHybrid = optionGoogleHybrid;
	map.options.showGoogle = optionGoogle;
	Refresh();
}

MapsTabDlg::MapsTabDlg() {
	/**
	* Constructor for MapsTabDlg.
	* Initializes the dialog layout and sets up the status bar.
	*/
	typedef MapsTabDlg CLASSNAME;
	CtrlLayout(*this);
	AddFrame(statusBar);

	statusBar.AddFrame(infoLongLati.Right(250));
	statusBar.AddFrame(infoZoom.Right(90));
	statusBar.AddFrame(infoTopo.Right(110));
	statusBar.AddFrame(infoMorpho.Right(90));
	//statusBar.AddFrame(infoDop.Left(360));
	statusBar = "[ctrl] to show Height at point. [shift] to Distance. [double click][+ctrl] generate Heigth/Morpho Map";

	SetTimeCallback(-100, [=] {
		String txt = CoordinateToString(map.PointMoveGrad);
		txt.Insert(8, " ");
		txt += "  " + FormatDouble(map.PointMoveGrad.x, 8) << " " << FormatDouble(map.PointMoveGrad.y, 8);
		infoLongLati.Set(txt);
		infoZoom = "Zoom = " + AsString(map.zoom);
		infoTopo = "Height = " + AsString(map.lastTopo);
		infoMorpho = "Morpho = " + AsString(map.lastMorpho);
	});

	optionTopo.WhenAction = optionMorpho.WhenAction = optionGoogle.WhenAction =
		optionDegLines.WhenAction = optionGoogleHybrid.WhenAction = THISBACK(ChangeOptions);
}

StationsTestDlg::StationsTestDlg() {
	/**
	* Constructor for StationsTestDlg.
	* Initializes the dialog layout.
	*/
	typedef StationsTestDlg CLASSNAME;
	CtrlLayout(*this);
	
}

ResultsLayoutDlg::ResultsLayoutDlg() {
	/**
	* Constructor for ResultsLayoutDlg.
	* Initializes the dialog layout.
	*/
	typedef ResultsLayoutDlg CLASSNAME;
	CtrlLayout(*this);
	SetDarkThemeEnabled(false);
}

String MobileService::GetNewStationFileName(bool iReferenceStation) {
	/**
	* Generates a new filename for a station.
	*
	* Parameters:
	* iReferenceStation (bool): Indicates if the station is a reference station.
	*
	* Returns:
	* String: The generated filename.
	*
	* Constraints:
	* - The function assumes that the path is correctly set.
	*/
	String fname = "", path = pathTest;
	if (iReferenceStation) path = pathRef;

	for (int i = 1; i <= 99999999; i++) {
		fname = path + "Station_" + Format("%08d", i) + cnfg.DefaultExtensionForRecord;
		if (!FileExists(fname)) return fname;
	}
	return path + ToUpper("Station_0" + cnfg.DefaultExtensionForRecord);
}

void MobileService::PrepareResultLayout() {
	/**
	* Prepares the result layout for calculations.
	*
	* Parameters:
	* None
	*
	* Returns:
	* void
	*
	* Constraints:
	* - The function assumes that the UI elements are correctly initialized.
	*/
	tabRes.drlPointToPoint.Clear();
	tabRes.drlPointToPoint.Add("0 = normal Agreement")
		.Add("10 = P2P non strict HCM t%=10")
		.Add("11= P2P non strict HCM t%=50")
		.Add("12 = P2P non strict HCM t%=1")
		.SetIndex(0);
	tabRes.drlLine//.Add("-1 = Coordination line calculation (h2 = 10m)")
		.Add("-9 = P2L non strict HCM t%=10 h2=3")
		.Add("-10 = P2L non strict HCM t%=10 h2=10")
		.Add("-11 = P2L non strict HCM t%=50 h2=3")
		.SetIndex(0);
	tabRes.drlCbr.Add("-1 = Coordination line calculation (h2 = 10m)")
		.Add("-9 = P2L non strict HCM t%=10 h2=3")
		.Add("-10 = P2L non strict HCM t%=10 h2=10")
		.Add("-11 = P2L non strict HCM t%=50 h2=3")
		.SetIndex(0);

	tabRes.edPtoP.SetText("0");
	tabRes.edDepolLoss.SetText("0");

	LoadFileToDropList(cnfg.FileWithCountriesCodes, tabRes.lineLandTo);
	tabRes.lineLandTo.SetIndex(0);
	tabRes.lineLandTo.SetIndex(0);
	tabRes.lineCross.SetText("6");
	tabRes.linePermFS.SetText("0");

	LoadFileToDropList(cnfg.FileWithCountriesCodes, tabRes.cbrLandTo);
	tabRes.cbrLandTo.SetIndex(0);
	tabRes.cbrLandTo.SetIndex(0);
	tabRes.cbrCross.SetText("6");
	tabRes.cbrPermFS.SetText("0");

	Font fnt;
	fnt.Height(22);
	tabRes.statusLabel.AlignCenter();
	tabRes.statusLabel.SetFont(fnt);
	tabRes.statusLabel.SetInk(Color(94, 33, 41));
	tabRes.statusLabel.SetText("The calculation results are published here.");

	tabRes.BtnPtoP << [=] {
		tabRes.statusLabel.SetText("The calculation results for P2P mode");
		Calculation(100);
	};
	tabRes.BtnLine << [=] {
		tabRes.statusLabel.SetText("The calculation results for Line mode");
		Calculation(200);
	};
	tabRes.BtnCbr << [=] {
		tabRes.statusLabel.SetText("The calculation results for CBR mode");
		Calculation(300);
	};
	tabRes.BtnExcel << [=]{
		std::vector<std::vector<std::string>> table;
		std::vector<int> columnWidths;
		
		
		
		table.resize(tabRes.table.GetCount() + 1);
		table[0].resize( tabRes.table.GetColumnCount());
		for (int j=0; j<tabRes.table.GetColumnCount(); ++j){
			table[0][j] = tabRes.table.HeaderTab(j).GetText().ToStd();
		}
		
		for (int i=0; i<tabRes.table.GetCount(); ++i){
			table[i+1].resize( tabRes.table.GetColumnCount());
			for (int j=0; j<tabRes.table.GetColumnCount(); ++j){
				table[i+1][j] = AsString(tabRes.table.Get(i,j)).ToStd();
			}
		}

		
		columnWidths.resize(tabRes.table.GetColumnCount());
		for (int j=0; j<tabRes.table.GetColumnCount(); ++j){
			columnWidths[j] =tabRes.table.GetScreenCellRect(0,j).Width();
		};
		FileSel fsExcel;
		fsExcel.Type("Excel files", "*.xls");
		fsExcel.ActiveDir(cnfg.LastPathForOpeningDialog);
		if (fsExcel.ExecuteSaveAs()){
			std::string Fname = fsExcel.GetFile(0).ToStd();
			saveToXLSX(table, columnWidths, Fname);
		}
		~fsExcel;
	};

	tabRes.table.WhenLeftDouble << [=] {
		int num = tabRes.table.GetCursor();
		if (debugFiles.GetCount() == 0) return;
		if (num > debugFiles.GetCount() - 1) return;
		WinExec("notepad " + debugFiles[num], SW_SHOW);
	};
	tabRes.table.OddRowColor(Color(255, 255, 191), Black());
	tabRes.table.EvenRowColor(Color(220, 236, 249), Black());
}

void MobileService::MainMenu(Bar& bar) {
	/**
	* Initializes the main menu of the application.
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
	bar.Add("Paths", [=] { SettingsLO.Execute(); });
	bar.Add("Exit", [=] { Close(); });
}

void MobileService::RefreshSettings() {
	/**
	* Refreshes the settings and updates the UI.
	*
	* Parameters:
	* None
	*
	* Returns:
	* void
	*
	* Constraints:
	* - The function assumes that the settings are correctly loaded.
	*/
	SettingsLO.EdBorderPath.SetText(cnfg.PathToBorders);
	SettingsLO.EdTopoPath.SetText(cnfg.PathToTopo);
	SettingsLO.EdMorphoPath.SetText(cnfg.PathToMorpho);

	bool flag = !PathIsEqual(tabMap.map.geodata.pathToBorders, cnfg.PathToBorders) ||
				!PathIsEqual(tabMap.map.geodata.pathToMorpho, cnfg.PathToMorpho) ||
				!PathIsEqual(tabMap.map.geodata.pathToTopo, cnfg.PathToTopo);

	tabMap.map.geodata.pathToBorders = cnfg.PathToBorders;
	tabMap.map.geodata.fileWithCountriesCodes = cnfg.FileWithCountriesCodes;
	tabMap.map.geodata.pathToMorpho = cnfg.PathToMorpho;
	tabMap.map.geodata.pathToTopo = cnfg.PathToTopo;
	tabMap.map.ter.PathToTopo = cnfg.PathToTopo;
	tabMap.map.ter.PathToMorpho = cnfg.PathToMorpho;
		
	cnfg.Save();

	if (flag) {
		tabMap.map.geodata.LoadGeoData();
	}
}

void MobileService::PrepareSettings() {
	/**
	* Prepares the settings UI.
	*
	* Parameters:
	* None
	*
	* Returns:
	* void
	*
	* Constraints:
	* - The function assumes that the settings UI is correctly initialized.
	*/
	SettingsLO.BtnBorderPath << [=] {
		faSettings.Set(cnfg.PathToBorders + "\\need.this");
		if (faSettings.ExecuteSelectDir("Choose folder with Borders [*.ALL; *.000 etc.]")) {
			cnfg.PathToBorders = PutSepToEndPath(faSettings.Get().Mid(0, 163));
			RefreshSettings();
		}
	};
	SettingsLO.BtnMorphoPath << [=] {
		faSettings.Set(cnfg.PathToMorpho + "\\need.this");
		if (faSettings.ExecuteSelectDir("Choose folder with MORPHO maps [*.33M *.63M]")) {
			cnfg.PathToMorpho = PutSepToEndPath(faSettings.Get().Mid(0, 163));
			RefreshSettings();
		}
	};
	SettingsLO.BtnTopoPath << [=] {
		faSettings.Set(cnfg.PathToTopo + "\\need.this");
		if (faSettings.ExecuteSelectDir("Choose folder with TOPO maps [*.33E *.63E]")) {
			cnfg.PathToTopo = PutSepToEndPath(faSettings.Get().Mid(0, 163));
			RefreshSettings();
		}
	};
	SettingsLO.ok << [=] { RefreshSettings(); };
}

void MobileService::FindFiles(const String& directory, const String& mask, Array<String>& result) {
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
			result.Add(ToUpper(fullPath));
		} else if (ff.IsFolder() && ff.GetName() != "." && ff.GetName() != "..") {
			FindFiles(fullPath, mask, result);
		}
		ff.Next();
	}
}

void MobileService::ReadFileWithStation(String filename, int index, bool isRefList) {
	/**
	* Reads a file and extracts station data.
	*
	* Parameters:
	* filename (String): The name of the file to read.
	* index (int): The index of the station in the file.
	* isRefList (bool): Indicates if the station is in the reference list.
	*
	* Returns:
	* void
	*
	* Constraints:
	* - The function assumes that the file exists and is correctly formatted.
	*/
	int countStations = GetCountStationsInFile(filename);
	MobileStation station;
	int offet = 0;
	Pointf pnt;
	String allString;

	for (int i = 0; i < countStations; i++) {
		try {
			allString = ReadStationFromFile(filename, i);

			station.Name = TrimLeft(TrimRight(allString.Mid(offet + 28, 20)));
			isRefList ? station.Filename = ListRefStationFiles[index] : station.Filename = ListTestStationFiles[index];
			station.isRefList = isRefList;
			station.IndexInFile = i;
			pnt = ReadCoordinate(allString.Mid(offet + 51, 15));
			station.Longitude = pnt.x;
			station.Latitude = pnt.y;
			try {
				station.Azimut = ScanDouble(allString.Mid(offet + 91, 5));
			} catch (...) {
				station.Azimut = 0;
			}
			try {
				station.Heigth = ScanDouble(allString.Mid(offet + 107, 4));
			} catch (...) {
				station.Heigth = 0;
			}
			station.FreqTx = ReadFreq(allString.Mid(offet + 0, 12));
			station.FreqRx = ReadFreq(allString.Mid(offet + 125, 12));
			station.Land = allString.Mid(offet + 48, 3);
			try {
				station.Radius = ScanDouble(allString.Mid(offet + 66, 5));
			} catch (...) {
				station.Radius = 0;
			}
			StationsList.Add(station);
		} catch (...) {
			// Handle exceptions
		}
	}
}

void MobileService::ReadAllFileStations() {
	/**
	* Reads all station files and populates the station list.
	*
	* Parameters:
	* None
	*
	* Returns:
	* void
	*
	* Constraints:
	* - The function assumes that the station files exist and are correctly formatted.
	*/
	String fname = "";
	StationsList.Clear();

	int countPathToStations = WinPath(pathRef).ToString().GetCount();
	tabStations.RefTable.Clear();
	for (int i = 0; i < ListRefStationFiles.GetCount(); i++) {
		fname = ListRefStationFiles.At(i);
		ReadFileWithStation(fname, i, true);
	}

	for (int i = 0; i < ListTestStationFiles.GetCount(); i++) {
		fname = ListTestStationFiles.At(i);
		ReadFileWithStation(fname, i, false);
	}
}

void MobileService::PrepareMapTree() {
	/**
	* Prepares the map tree for displaying stations.
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
	tabMap.mapTree.WhenCursor = [=] {
		
		int cur = tabMap.mapTree.GetCursor();
		if (cur < 0) return;

		for (int i = 0; i < mapTreeCoordArray.GetCount(); i++) {
			if (mapTreeCoordArray[i].id != cur) continue;
			tabMap.map.LookAt(mapTreeCoordArray[i].longitude, mapTreeCoordArray[i].latitude, mapTreeCoordArray[i].zoom);
			break;
		}
	};
}

void MobileService::RefreshMapTree() {
	/**
	* Refreshes the map tree with station data.
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
	MapStation mapS;
	tabMap.map.MapStation.Clear();
	for (int i = 0; i < StationsList.GetCount(); i++) {
		mapS.Longitude = StationsList[i].Longitude;
		mapS.Latitude = StationsList[i].Latitude;
		mapS.Azimut = StationsList[i].Azimut;
		mapS.Name = StationsList[i].Name;
		mapS.Radius = StationsList[i].Radius;
		mapS.isRef = StationsList[i].isRefList;
		tabMap.map.MapStation.Add(mapS);
	}

	mapTreeCoordArray.Clear();
	tabMap.mapTree.Clear();
	tabMap.mapTree.SetRoot(Null, "Stations");

	int refTree = tabMap.mapTree.Add(0, Null, "Reference list");
	int testTree = tabMap.mapTree.Add(0, Null, "Test list");
	int id;
	for (int i = 0; i < StationsList.GetCount(); i++) {
		if (StationsList[i].isRefList) {
			id = tabMap.mapTree.Add(refTree, Null, StationsList[i].Name);
		} else {
			id = tabMap.mapTree.Add(testTree, Null, StationsList[i].Name);
		}
		mapTreeCoordArray.Add(mapCoord{ id, StationsList[i].Longitude, StationsList[i].Latitude, 12 });
	}

	tabMap.mapTree.Open(0);
	tabMap.mapTree.Open(refTree);
	tabMap.mapTree.Open(testTree);
}

void MobileService::PrepareStationsLists() {
	/**
	* Prepares the stations lists for display.
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
	TreeCtrl::Node node;
	node.Set("Stations");
	tabMap.mapTree.SetRoot(node);

	tabStations.RefTable.Clear();
	tabStations.RefTable.AddColumnAt(0, "№", 20);
	tabStations.RefTable.AddColumnAt(1, "Station name", 120);
	tabStations.RefTable.AddColumnAt(2, "Coordinates", 120);
	tabStations.RefTable.AddColumnAt(3, "Country", 30);
	tabStations.RefTable.AddColumnAt(4, "Tx freq [MHz]", 40);
	tabStations.RefTable.AddColumnAt(5, "Rx freq [MHz]", 40);
	tabStations.RefTable.AddColumnAt(6, "Azimuth [grad]", 40);
	tabStations.RefTable.AddColumnAt(7, "Ant. height [m]", 40);
	tabStations.RefTable.AddColumnAt(8, "Radius [km]", 40);
	tabStations.RefTable.OddRowColor(Color(255, 255, 191), Black());
	tabStations.RefTable.EvenRowColor(Color(220, 236, 249), Black());

	tabStationsTest.TestTable.OddRowColor(Color(255, 255, 191), Black());
	tabStationsTest.TestTable.EvenRowColor(Color(220, 236, 249), Black());

	tabStationsTest.TestTable.Clear();
	for (int i = 0; i < tabStations.RefTable.GetColumnCount(); i++)
		tabStationsTest.TestTable.AddColumn(tabStations.RefTable.ColumnAt(i).HeaderTab().GetText());
	tabStationsTest.TestTable.ColumnWidths(tabStations.RefTable.GetColumnWidths());

	tabStations.RefTable.WhenLeftDouble << [=] {
		int num = tabStations.RefTable.GetCursor();
		if (num >= 0) ShowStationEditor(11, num);
		RefreshStationsLists();
	};

	tabStations.BtnOpen << [=] {
		fs.ActiveDir(cnfg.LastPathForOpeningDialog);
		if (fs.ExecuteOpen()) {
			FileCopy(fs.GetFile(0), pathRef + "stations.txt");
			cnfg.LastPathForOpeningDialog = GetFileFolder(fs.GetFile(0));
			cnfg.Save();
		}
		RefreshStationsLists();
	};
	tabStations.BtnSave << [=] {
		fs.ActiveDir(cnfg.LastPathForOpeningDialog);
		if (fs.ExecuteSaveAs()) {
			FileCopy(pathRef + "stations.txt", fs.GetFile(0));
			cnfg.LastPathForOpeningDialog = GetFileFolder(fs.GetFile(0));
			cnfg.Save();
		}
		RefreshStationsLists();
	};
	
	tabStations.BtnNew << [=] {
		AddEmptyStationToFile(pathRef + "stations.txt");
		RefreshStationsLists();
	};
	
	tabStations.BtnClear << [=] {
		DeleteFileA(pathRef + "stations.txt");
		RefreshStationsLists();
	};
	tabStations.BtnAppend << [=] {
		fs.ActiveDir(cnfg.LastPathForOpeningDialog);
		if (fs.ExecuteOpen()) {
			MergeFilesWithStations(pathRef + "stations.txt", fs.GetFile(0));
			cnfg.LastPathForOpeningDialog = GetFileFolder(fs.GetFile(0));
			cnfg.Save();
		}
		RefreshStationsLists();
	};

	tabStations.BtnDelete1 << [=] {
		int num = tabStations.RefTable.GetCursor();
		if (num >= 0) DeleteStationFromFile(pathRef + "stations.txt", num);
		RefreshStationsLists();
	};

	tabStationsTest.TestTable.WhenLeftDouble << [=] {
		int num = tabStationsTest.TestTable.GetCursor();
		if (num >= 0) ShowStationEditor(21, num);
		RefreshStationsLists();
	};

	tabStationsTest.BtnOpen << [=] {
		fs.ActiveDir(cnfg.LastPathForOpeningDialog);
		if (fs.ExecuteOpen()) {
			FileCopy(fs.GetFile(0), pathTest + "stations.txt");
			cnfg.LastPathForOpeningDialog = GetFileFolder(fs.GetFile(0));
			cnfg.Save();
		}
		RefreshStationsLists();
	};
	tabStationsTest.BtnSave << [=] {
		fs.ActiveDir(cnfg.LastPathForOpeningDialog);
		if (fs.ExecuteSaveAs()) {
			FileCopy(pathTest + "stations.txt", fs.GetFile(0));
			cnfg.LastPathForOpeningDialog = GetFileFolder(fs.GetFile(0));
			cnfg.Save();
		}
		RefreshStationsLists();
	};
	tabStationsTest.BtnClear << [=] {
		DeleteFileA(pathTest + "stations.txt");
		RefreshStationsLists();
	};
	tabStationsTest.BtnNew << [=] {
		AddEmptyStationToFile(pathTest + "stations.txt");
		RefreshStationsLists();
	};
	tabStationsTest.BtnDelete1 << [=] {
		int num = tabStationsTest.TestTable.GetCursor();
		if (num >= 0) DeleteStationFromFile(pathTest + "stations.txt", num);
		RefreshStationsLists();
	};
	tabStationsTest.BtnAppend << [=] {
		fs.ActiveDir(cnfg.LastPathForOpeningDialog);
		if (fs.ExecuteOpen()) {
			MergeFilesWithStations(pathTest + "stations.txt", fs.GetFile(0));
			cnfg.LastPathForOpeningDialog = GetFileFolder(fs.GetFile(0));
			cnfg.Save();
		}
		RefreshStationsLists();
	};
}

void MobileService::RefreshStationsLists() {
	/**
	* Refreshes the stations lists with updated data.
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
	ListRefStationFiles.Clear();
	ListTestStationFiles.Clear();
	FindFiles(pathRef, "*" + cnfg.DefaultExtensionForRecord, ListRefStationFiles);
	FindFiles(pathTest, "*" + cnfg.DefaultExtensionForRecord, ListTestStationFiles);

	ReadAllFileStations();

	tabMap.mapTree.Clear();

	tabStations.RefTable.Clear();
	tabStationsTest.TestTable.Clear();

	int countRef = 0, countTest = 0;
	String coord = "";
	for (int i = 0; i < StationsList.GetCount(); i++) {
		coord = CoordinateToString(Pointf(StationsList[i].Longitude, StationsList[i].Latitude));
		coord.Insert(8, " ");
		if (StationsList[i].isRefList) {
			countRef++;
			tabStations.RefTable.Add(
				countRef,
				StationsList[i].Name,
				Format("%.6mf", StationsList[i].Longitude) + "   " +
				Format("%.6mf", StationsList[i].Latitude) + "\t\t" + coord,
				StationsList[i].Land,
				(StationsList[i].FreqTx > 0) ? AsString(StationsList[i].FreqTx) : "",
				(StationsList[i].FreqRx > 0) ? AsString(StationsList[i].FreqRx) : "",
				AsString(StationsList[i].Azimut),
				AsString(StationsList[i].Heigth),
				AsString(StationsList[i].Radius)
			);
		} else {
			countTest++;
			tabStationsTest.TestTable.Add(
				countTest,
				StationsList[i].Name,
				Format("%.6mf", StationsList[i].Longitude) + "   " +
				Format("%.6mf", StationsList[i].Latitude) + "\t\t" + coord,
				StationsList[i].Land,
				(StationsList[i].FreqTx > 0) ? AsString(StationsList[i].FreqTx) : "",
				(StationsList[i].FreqRx > 0) ? AsString(StationsList[i].FreqRx) : "",
				AsString(StationsList[i].Azimut),
				AsString(StationsList[i].Heigth),
				AsString(StationsList[i].Radius)
			);
		}
	}
	RefreshMapTree();
}


double generateRandomDouble(double min, double max) {
	static bool initialized = false;
	if (!initialized) {
		std::srand(std::time(nullptr));
		initialized = true;
	}
	return min + (max - min) * (std::rand() / (RAND_MAX + 1.0));
}

void MobileService::Close(){
	std::abort();
}
MobileService::MobileService() {
	/**
	* Constructor for MobileService.
	* Initializes the mobile service application.
	*/
	SetDarkThemeEnabled(false);
	
	PrepareSettings();
	cnfg.Load();
	pathRef = WinPath(GetExeFolder() + "\\Stations\\RefList\\");
	pathTest = WinPath(GetExeFolder() + "\\Stations\\TestList\\");

	RefreshSettings();

	PrepareResultLayout();

	if (!FileExists(cnfg.FileWithCountriesCodes)) {
		ErrorOK("NO FILE WITH COUNTRIES CODES. TRY TO RE-INSTALL PROGRAM");
		Close();
	}

	fs.Type("Station files", cnfg.DefaultExtensionForRecord);

	AddFrame(menu);
	menu.Set([=](Bar& bar) { MainMenu(bar); });

	CtrlLayout(*this, "CalcMS4A version 1.18");
	Zoomable(true).Sizeable().SetMinSize(Size(1200, 768));

	mainTab.SizePos();
	mainTab.Add(tabStationsTest.SizePos(), "Test List");
	mainTab.Add(tabStations.SizePos(), "Reference List");
	mainTab.Add(tabMap.SizePos(), "Maps");
	mainTab.Add(tabRes.SizePos(), "Calculations");
	s.tabheight = 30;
	s.font = StdFont().Height(18);
	mainTab.SetStyle(s);
	
	mainTab.SetDarkThemeEnabled(false);
	tabRes.SetDarkThemeEnabled(false);
	tabMap.SetDarkThemeEnabled(false);
	tabStationsTest.SetDarkThemeEnabled(false);
	
	
	CtrlLayoutOKCancel(SettingsLO, t_("Settings"));
	CtrlLayout(StationEditor, t_("Station Editor"));

	PrepareMapTree();
	
	PrepareStationsLists();
	RefreshStationsLists();

	tabMap.map.LookAt(-21, 28, 4);

	PrepareStationEditor();
	SetStationDefault();


	SetDateFormat("%3:02d/%2:02d/%1:4d");
	SetDateScan("dmy");
}
