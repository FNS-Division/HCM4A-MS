#ifndef _HCM4AMobileService_editorLayout_h_
#define _HCM4AMobileService_editorLayout_h_

//#include "Common.h"
#include "MobileService.h"

bool MobileService::HeaderStringToControls(String source){
	try{
		int tmp1, tmp2, tmp3;
		String tmp = TrimBoth(source.Mid(0,2));		StationEditor.h01.SetText(tmp);
		tmp = TrimBoth(source.Mid(2,80));		StationEditor.h02.SetText(tmp);
		tmp = TrimBoth(source.Mid(82,1));		StationEditor.h03.SetText(tmp);
		tmp = ToUpper(source.Mid(83,3));
		int pos = StationEditor.h04.Find(tmp);
		if (pos != -1){
			StationEditor.h04.SetIndex(pos);
		} else {
			StationEditor.h04.Add(tmp);
			StationEditor.h04.SetIndex(StationEditor.h04.GetCount()-1);
		}
		tmp = TrimBoth(source.Mid(86,40));		StationEditor.h05.SetText(tmp);
		tmp = TrimBoth(source.Mid(126,20));		StationEditor.h06.SetText(tmp);
		tmp = TrimBoth(source.Mid(146,20));		StationEditor.h07.SetText(tmp);
		tmp = TrimBoth(source.Mid(166,20));		StationEditor.h08.SetText(tmp);
		tmp = TrimBoth(source.Mid(186,6));		StationEditor.h09.SetText(tmp);
		tmp = source.Mid(192,8);
			tmp1 = ScanInt(tmp.Mid(0,2));
			tmp2 = ScanInt(tmp.Mid(2,2));
			tmp3 = ScanInt(tmp.Mid(4,4));
		if(IsNull(tmp1) || IsNull(tmp2) || IsNull(tmp3)){StationEditor.h10.SetText("");}else{StationEditor.h10.SetDate(tmp3, tmp2, tmp1);};
		tmp = ToUpper(source.Mid(200,3));
		pos = StationEditor.h11.Find(tmp);
		if (pos != -1){
			StationEditor.h11.SetIndex(pos);
		} else {
			StationEditor.h11.Add(tmp);
			StationEditor.h11.SetIndex(StationEditor.h11.GetCount()-1);
		}
		tmp = TrimBoth(source.Mid(203,6));		StationEditor.h12.SetText(tmp);
		tmp = TrimBoth(source.Mid(209,3));		StationEditor.h13.SetText(tmp);
		tmp = TrimBoth(source.Mid(212,7));		StationEditor.h14.SetText(tmp);
	}catch(...){
		return false;
	}
	return true;
}

bool MobileService::StationStringToControls(String source, int numStation){
	indexOpenedStation = -1;
	try{
		String tmp;
		int tmp1, tmp2, tmp3;
		int offet = numStation * 219 + 219;
		tmp = TrimBoth(source.Mid(offet+0,11));		StationEditor.tx1Afreq.SetText(tmp);
		tmp = TrimBoth(source.Mid(offet+11,1));
		int pos = StationEditor.tx1Adfreqn.Find(tmp);	(pos != -1) ? StationEditor.tx1Adfreqn.SetIndex(pos) : StationEditor.tx1Adfreqn.SetIndex(1);
		tmp = TrimBoth(source.Mid(offet+12,1));
		pos = StationEditor.tx1Zfrcat.Find(tmp);	(pos != -1) ? StationEditor.tx1Zfrcat.SetIndex(pos) : StationEditor.tx1Zfrcat.SetIndex(1);
		tmp = TrimBoth(source.Mid(offet+13,2));
		pos = StationEditor.tx6Aclas.Find(tmp);	(pos != -1) ? StationEditor.tx6Aclas.SetIndex(pos) : StationEditor.tx6Aclas.SetIndex(1);
		tmp = TrimBoth(source.Mid(offet+15,2));
		pos = StationEditor.tx6Bnatu.Find(tmp);	(pos != -1) ? StationEditor.tx6Bnatu.SetIndex(pos) : StationEditor.tx6Bnatu.SetIndex(1);
		tmp = TrimBoth(source.Mid(offet+17,2));		StationEditor.tx6Zcat.SetText(tmp);
		tmp = TrimBoth(source.Mid(offet+19,1));
		pos = StationEditor.tx10Zch.Find(tmp);	(pos != -1) ? StationEditor.tx10Zch.SetIndex(pos) : StationEditor.tx10Zch.SetIndex(1);
		
		tmp = TrimBoth(source.Mid(offet+20,8));		StationEditor.tx2Cdate.SetText(tmp);
		tmp = source.Mid(offet+20,8);
			tmp1 = ScanInt(tmp.Mid(0,2));
			tmp2 = ScanInt(tmp.Mid(2,2));
			tmp3 = ScanInt(tmp.Mid(4,4));
		if(IsNull(tmp1) || IsNull(tmp1) || IsNull(tmp1)){StationEditor.tx2Cdate.SetText("");}else{StationEditor.tx2Cdate.SetDate(tmp3, tmp2, tmp1);};
		
		tmp = TrimBoth(source.Mid(offet+28,20));		StationEditor.tx4Aname.SetText(tmp);
		tmp = TrimBoth(source.Mid(offet+48,3));
			pos = StationEditor.tx4Bcntr.Find(tmp);
			if (pos != -1){
				StationEditor.tx4Bcntr.SetIndex(pos);
			} else {
				StationEditor.tx4Bcntr.Add(tmp);
				StationEditor.tx4Bcntr.SetIndex(StationEditor.tx4Bcntr.GetCount()-1);
			}
		tmp = TrimBoth(source.Mid(offet+51,8));		StationEditor.tx4Clong.SetText(tmp);
		tmp = TrimBoth(source.Mid(offet+59,7));		StationEditor.tx4Clati.SetText(tmp);
		tmp = TrimBoth(source.Mid(offet+66,5));		StationEditor.tx4Dradi.SetText(tmp);
		tmp = TrimBoth(source.Mid(offet+71,4));		StationEditor.tx4Zhei.SetText(tmp);
		tmp = TrimBoth(source.Mid(offet+75,9));		StationEditor.tx7Adesi.SetText(tmp);
		tmp = TrimBoth(source.Mid(offet+84,6));		StationEditor.tx8B1max.SetText(tmp);
		tmp = TrimBoth(source.Mid(offet+90,1));
		pos = StationEditor.tx8B2ref.Find(tmp);	(pos != -1) ? StationEditor.tx8B2ref.SetIndex(pos) : StationEditor.tx8B2ref.SetIndex(1);
		tmp = TrimBoth(source.Mid(offet+91,5));		StationEditor.tx9Aazim.SetText(tmp);
		tmp = TrimBoth(source.Mid(offet+96,5));		tmp.Replace(" ",""); StationEditor.tx9Belev.SetText(tmp);
		tmp = TrimBoth(source.Mid(offet+101,2));
		pos = StationEditor.tx9Dpol.Find(tmp);	(pos != -1) ? StationEditor.tx9Dpol.SetIndex(pos) : StationEditor.tx9Dpol.SetIndex(1);
		tmp = TrimBoth(source.Mid(offet+103,4));		StationEditor.tx9Ggain.SetText(tmp);
		tmp = TrimBoth(source.Mid(offet+107,4));		StationEditor.tx9Yheit.SetText(tmp);
		tmp = TrimBoth(source.Mid(offet+111,7));		StationEditor.tx9Xhori.SetText(tmp);
		tmp = TrimBoth(source.Mid(offet+118,7));		StationEditor.tx9Xvert.SetText(tmp);
		tmp = TrimBoth(source.Mid(offet+125,11));	StationEditor.tx1Yfreq.SetText(tmp);
		tmp = TrimBoth(source.Mid(offet+136,1));
		pos = StationEditor.tx1Yfreq2.Find(tmp);	(pos != -1) ? StationEditor.tx1Yfreq2.SetIndex(pos) : StationEditor.tx1Yfreq2.SetIndex(1);
		
		tmp = TrimBoth(source.Mid(offet+137,50));	StationEditor.tx13Zrema.SetText(tmp);
		tmp = TrimBoth(source.Mid(offet+187,1));
		pos = StationEditor.tx13Ystat.Find(tmp);	(pos != -1) ? StationEditor.tx13Ystat.SetIndex(pos) : StationEditor.tx13Ystat.SetIndex(1);
	
		tmp = source.Mid(offet+188,8);
			tmp1 = ScanInt(tmp.Mid(0,2));
			tmp2 = ScanInt(tmp.Mid(2,2));
			tmp3 = ScanInt(tmp.Mid(4,4));
		if(IsNull(tmp1) || IsNull(tmp1) || IsNull(tmp1)){StationEditor.tx2Wdate.SetText("");}else{StationEditor.tx2Wdate.SetDate(tmp3, tmp2, tmp1);};
	
		tmp = source.Mid(offet+196,8);
			tmp1 = ScanInt(tmp.Mid(0,2));
			tmp2 = ScanInt(tmp.Mid(2,2));
			tmp3 = ScanInt(tmp.Mid(4,4));
		if(IsNull(tmp1) || IsNull(tmp1) || IsNull(tmp1)){StationEditor.tx2Zdate.SetText("");}else{StationEditor.tx2Zdate.SetDate(tmp3, tmp2, tmp1);};
	
		tmp = TrimBoth(source.Mid(offet+204,3));
			pos = StationEditor.tx13Xcc.Find(tmp);
			if (pos != -1){
				StationEditor.tx13Xcc.SetIndex(pos);
			} else {
				StationEditor.tx13Xcc.Add(tmp);
				StationEditor.tx13Xcc.SetIndex(StationEditor.tx13Xcc.GetCount()-1);
			}
		tmp = TrimBoth(source.Mid(offet+207,2));	StationEditor.tx13Xzz.SetText(tmp);
		tmp = TrimBoth(source.Mid(offet+209,6));	StationEditor.tx13Xpp.SetText(tmp);
		tmp = TrimBoth(source.Mid(offet+215,2));	StationEditor.tx13Xff.SetText(tmp);
		tmp = TrimBoth(source.Mid(offet+217,1));	StationEditor.tx13Xr.SetText(tmp);
		tmp = TrimBoth(source.Mid(offet+218,1));	StationEditor.tx13Xo.SetText(tmp);
	}catch(...){
		return false;
	}
	indexOpenedStation = numStation;
	return true;
}

String MobileService::ReadStationHeaderFromControls(){
	String ss, tmp;
	ss = Format("%02d",atoi(StationEditor.h01.GetText().ToString()));
	ss += MakeStringBlank(StationEditor.h02.GetText().ToString(), 80);
	ss += MakeStringBlank(StationEditor.h03.GetText().ToString(), 1);
	ss += MakeStringBlank(StationEditor.h04.GetValue(StationEditor.h04.GetIndex()), 3);
	ss += MakeStringBlank(StationEditor.h05.GetText().ToString(), 40);
	ss += MakeStringBlank(StationEditor.h06.GetText().ToString(), 20);
	ss += MakeStringBlank(StationEditor.h07.GetText().ToString(), 20);
	ss += MakeStringBlank(StationEditor.h08.GetText().ToString(), 20);
	ss += Format("%06d",atoi(StationEditor.h09.GetText().ToString()));
		tmp = StationEditor.h10.GetText().ToString();
		tmp.Replace("/","");
	ss += MakeStringBlank(tmp, 8);
	ss += MakeStringBlank(StationEditor.h11.GetValue(StationEditor.h11.GetIndex()), 3);
	ss += Format("%06d",atoi(StationEditor.h12.GetText().ToString()));
		tmp = StationEditor.h13.GetText().ToString();
		if (IsEmpty(tmp)) tmp = "1.0";
	ss += tmp;
	ss += MakeStringBlank("", 7);
	return ss;
}
String MobileService::ReadStationFromControls(){
	String ss, tmp;
	ss = SetFreqString(StationEditor.tx1Afreq.GetText().ToString());
	ss += StationEditor.tx1Adfreqn.GetValue(StationEditor.tx1Adfreqn.GetIndex()).ToString();
	ss += StationEditor.tx1Zfrcat.GetValue(StationEditor.tx1Zfrcat.GetIndex()).ToString();
	ss += MakeStringBlank(StationEditor.tx6Aclas.GetValue(StationEditor.tx6Aclas.GetIndex()).ToString(), 2);
	ss += MakeStringBlank(StationEditor.tx6Bnatu.GetValue(StationEditor.tx6Bnatu.GetIndex()).ToString(), 2);
	ss += MakeStringBlank(StationEditor.tx6Zcat.GetText().ToString(), 2);
	ss += StationEditor.tx10Zch.GetValue(StationEditor.tx10Zch.GetIndex()).ToString();
		tmp = StationEditor.tx2Cdate.GetText().ToString();
		tmp.Replace("/","");
	ss += MakeStringBlank(tmp, 8);
	ss += MakeStringBlank(StationEditor.tx4Aname.GetText().ToString(), 20);
	ss += MakeStringBlank(StationEditor.tx4Bcntr.GetValue(StationEditor.tx4Bcntr.GetIndex()), 3);
	ss += MakeStringBlank(StationEditor.tx4Clong.GetText().ToString(), 8);
	ss += MakeStringBlank(StationEditor.tx4Clati.GetText().ToString(), 7);
	ss += MakeStringBlank(StationEditor.tx4Dradi.GetText().ToString(), 5);
	ss += MakeStringBlank(StationEditor.tx4Zhei.GetText().ToString(), 4);
	ss += MakeStringBlank(StationEditor.tx7Adesi.GetText().ToString(), 9);
	ss += SetMaxPowerString(StationEditor.tx8B1max.GetText().ToString());
	ss += MakeStringBlank(StationEditor.tx8B2ref.GetValue(StationEditor.tx8B2ref.GetIndex()), 1);
	ss += SetAzimutString(StationEditor.tx9Aazim.GetText().ToString());
	ss += SetElevationString(StationEditor.tx9Belev.GetText().ToString());
	ss += MakeStringBlank(StationEditor.tx9Dpol.GetValue(StationEditor.tx9Dpol.GetIndex()), 2);
	ss += MakeStringBlank(StationEditor.tx9Ggain.GetText().ToString(), 4);
	ss += MakeStringBlank(StationEditor.tx9Yheit.GetText().ToString(), 4);
	ss += MakeStringBlank(StationEditor.tx9Xhori.GetText().ToString(), 7);
	ss += MakeStringBlank(StationEditor.tx9Xvert.GetText().ToString(), 7);
	ss += SetFreqString(StationEditor.tx1Yfreq.GetText().ToString());
	ss += StationEditor.tx1Yfreq2.GetValue(StationEditor.tx1Yfreq2.GetIndex()).ToString();
	
	ss += MakeStringBlank(StationEditor.tx13Zrema.GetText().ToString(), 50);
	ss += MakeStringBlank(StationEditor.tx13Ystat.GetValue(StationEditor.tx13Ystat.GetIndex()), 1);
		tmp = StationEditor.tx2Wdate.GetText().ToString();
		tmp.Replace("/","");
	ss += MakeStringBlank(tmp, 8);
		tmp = StationEditor.tx2Zdate.GetText().ToString();
		tmp.Replace("/","");
	ss += MakeStringBlank(tmp, 8);
	ss += MakeStringBlank(StationEditor.tx13Xcc.GetValue(StationEditor.tx13Xcc.GetIndex()), 3);
	ss += MakeStringBlank(StationEditor.tx13Xzz.GetText().ToString(), 2);
	ss += MakeStringBlank(StationEditor.tx13Xpp.GetText().ToString(), 6);
	ss += MakeStringBlank(StationEditor.tx13Xff.GetText().ToString(), 2);
	ss += MakeStringBlank(StationEditor.tx13Xr.GetText().ToString(), 1);
	ss += MakeStringBlank(StationEditor.tx13Xo.GetText().ToString(), 1);
	return ss;
}

void MobileService::StationDeleteFromFile(int index){
	DeleteStationFromFile(OpenedFileName, index);
}

void MobileService::StationSaveToFile(int index){
	int count = GetCountStationsInFile(OpenedFileName);
	if (count==0) return;
	if (index>=count) return;
	
	FileIn f(OpenedFileName);
	f.Seek(0);
	String source = f.GetLine();
	f.Close();
	
	String header = ReadStationHeaderFromControls();
	String station = ReadStationFromControls();
	String context = "";
	
	if (count == 1) {
		context = header + station;
	} else {
		int offset = 0, countRead = 0, controlLen;
		String leftS, rightS;
		if (count > 1){
			offset = 219;
			countRead = 219 * index;
			leftS = source.Mid(offset, countRead);
			controlLen = leftS.GetLength();
			
			offset = 219 + 219 * (index + 1);
			countRead = 219 * (count - index - 1);
			rightS = source.Mid(offset, countRead);
			controlLen = rightS.GetLength();
		}
		context = header + leftS + station + rightS;
	}

	FileOut fo(OpenedFileName);
	fo.Seek(0);
	fo.Put(context);
	fo.Close();
}

void MobileService::RefreshListOfStations(){
	StationEditor.table.Clear();
	
	int count = GetCountStationsInFile(OpenedFileName);
	if (count==0) return;
	
	FileIn f(OpenedFileName);
	f.Seek(0);
	String source = f.GetLine();
	f.Close();
	
	String name = "";
	int offet = 219;
	for (int i=0; i<count; i++){
		offet = i*219 + 219;
		name = TrimBoth(source.Mid(offet+28,20));
		StationEditor.table.Add(i+1,name);
	}
}



void MobileService::ClearHeaderControls(){
	String tmp = "";
	StationEditor.h01.SetText(tmp);
	StationEditor.h02.SetText(tmp);
	StationEditor.h03.SetText(tmp);
	StationEditor.h05.SetText(tmp);
	StationEditor.h06.SetText(tmp);
	StationEditor.h07.SetText(tmp);
	StationEditor.h08.SetText(tmp);
	StationEditor.h09.SetText(tmp);
	StationEditor.h12.SetText(tmp);
	StationEditor.h13.SetText(tmp);
	StationEditor.h14.SetText(tmp);
}
void MobileService::ClearStationControls(){
	String tmp = "";
	StationEditor.tx1Afreq.SetText(tmp);
	StationEditor.tx6Zcat.SetText(tmp);
	StationEditor.tx4Aname.SetText(tmp);
	StationEditor.tx4Clong.SetText(tmp);
	StationEditor.tx4Clati.SetText(tmp);
	StationEditor.tx4Dradi.SetText(tmp);
	StationEditor.tx4Zhei.SetText(tmp);
	StationEditor.tx7Adesi.SetText(tmp);
	StationEditor.tx8B1max.SetText(tmp);
	StationEditor.tx9Aazim.SetText(tmp);
	StationEditor.tx9Ggain.SetText(tmp);
	StationEditor.tx9Yheit.SetText(tmp);
	StationEditor.tx9Xhori.SetText(tmp);
	StationEditor.tx9Xvert.SetText(tmp);
	StationEditor.tx1Yfreq.SetText(tmp);
	StationEditor.tx13Zrema.SetText(tmp);
	StationEditor.tx13Xzz.SetText(tmp);
	StationEditor.tx13Xpp.SetText(tmp);
	StationEditor.tx13Xff.SetText(tmp);
	StationEditor.tx13Xr.SetText(tmp);
	StationEditor.tx13Xo.SetText(tmp);
}

void MobileService::LoadHeader(){
	if (!FileExists(OpenedFileName)) return;
	if (!FileExists(OpenedFileName)) return;
	FileIn f(OpenedFileName);
	int fs = f.GetSize();
	if ( ((fs % 219) != 0) && (fs > 219)) {
		f.Close();
		return;
	}
	f.Seek(0);
	String allString = f.GetLine();
	f.Close();
	
	ClearHeaderControls();
	HeaderStringToControls(allString);
}

void MobileService::LoadStation(int index){
	
	if (!FileExists(OpenedFileName)) return;
	FileIn f(OpenedFileName);
	int fs = f.GetSize();
	if ( ((fs % 219) != 0) && (fs > 219)) {
		f.Close();
		return;
	}
	f.Seek(0);
	String allString = f.GetLine();
	f.Close();
	
	fs = GetCountStationsInFile(OpenedFileName);
	if (index>=fs){
		PromptOK("There is no such Station index = " + AsString(index) + " in file " + OpenedFileName );
		return;
		
	}
	ClearStationControls();
	StationStringToControls(allString, index);
	StationEditor.table.SetCursor(index);
}
	
bool MobileService::LoadStationToEditor(String filename, int index){
	SetStationClear();
	if (!FileExists(filename)){
		PromptOK("ERROR NO FILE EXISTS " + filename );
		return false;
	}
	FileIn f(filename);
	int fs = f.GetSize();
	if ( ((fs % 219) != 0) && (fs > 219)) {
		PromptOK("ERROR FILE HAVE NOT CORRECT LENGTH" + filename );
		f.Close();
		return false;
	}
	f.Close();
	OpenedFileName = filename;
	
	LoadHeader();
	RefreshListOfStations();
	
	LoadStation(index);
	StationEditor.table.SetCursor(index);
	return true;
}



bool MobileService::SaveStationEditorToFile(String filename){
	RealizeDirectory(GetFileFolder(filename));
	FileOut f(filename);
	String ss;
	ss = ReadStationHeaderFromControls();
	ss += ReadStationFromControls();

	f.Put(ss);
	f.Close();
	return true;
}


bool MobileService::ValidateStationEditor(){
	return true;
}

void MobileService::ShowStationEditor(int mode, int index){
	 //REFERENCE LIST >> EDIT
	 //TEST LIST >> EDIT
	if (mode==11 || mode==21){
		
		String fname = "";
		int indexRef = -1, indexTest = -1, indexCursor = 0;
		
		for (int i=0; i<StationsList.GetCount(); i++){
			StationsList[i].isRefList ? indexRef++ : indexTest++;
			if ((mode==11) & (index == indexRef)) {
				fname = StationsList[i].Filename;
				indexCursor = StationsList[i].IndexInFile;
				break;	
			}
			if ((mode!=11) & (index == indexTest)) {
				fname = StationsList[i].Filename;
				indexCursor = StationsList[i].IndexInFile;
				break;	
			}
		}

		if (LoadStationToEditor(fname,indexCursor)) StationEditor.Execute();
	}
}

void MobileService::PrepareStationEditor(){
	Time date = GetSysTime();
	
	LoadFileToDropList(cnfg.FileWithCountriesCodes,StationEditor.h04);
	StationEditor.h04.SetIndex(0);
	LoadFileToDropList(cnfg.FileWithCountriesCodes,StationEditor.h11);
	StationEditor.h11.SetIndex(0);
	StationEditor.ok << [=] {StationEditor.Break(IDOK);};

	StationEditor.h10.Format("%3:02d/%2:02d/%1:4d");
	StationEditor.h10.SetDate(date.year,date.month,date.day);
	
	LoadFileToDropList(cnfg.FileWithCountriesCodes,StationEditor.tx4Bcntr);
	StationEditor.tx4Bcntr.SetIndex(0);
	LoadFileToDropList(cnfg.FileWithCountriesCodes,StationEditor.tx13Xcc);
	StationEditor.tx13Xcc.SetIndex(0);
	StationEditor.tx1Adfreqn.Clear();
	StationEditor.tx1Adfreqn.Add("K").Add("M").Add("G").SetIndex(1);
	StationEditor.tx1Yfreq2.Clear();
	StationEditor.tx1Yfreq2.Add("K").Add("M").Add("G").SetIndex(1);
	StationEditor.tx1Zfrcat.Clear();
	StationEditor.tx1Zfrcat.Add("1").Add("2").Add("3").Add("4")
		.Add("5").Add("6").Add("7").Add("8").SetIndex(0);
	StationEditor.tx6Aclas.Clear();
	StationEditor.tx6Aclas.Add("FB").Add("FC").Add("FL").Add("FP")
		.Add("FS").Add("FW").Add("FX").Add("ML")
		.Add("MO").Add("MR").Add("MS").SetIndex(0);
	StationEditor.tx6Bnatu.Clear();
	StationEditor.tx6Bnatu.Add("CO").Add("CP").Add("CR").Add("CV").Add("OT").SetIndex(1);
	StationEditor.tx13Ystat.Clear();
	StationEditor.tx13Ystat.Add("A").Add("B").Add("C")
		.Add("D").Add("E").Add("F").Add("G").Add("H")
		.Add("M").Add("P").Add("R").Add("W").Add("Z").SetIndex(0);
	StationEditor.tx9Dpol.Clear();
	StationEditor.tx9Dpol.Add("H").Add("V").Add("SR")
		.Add("SL").Add("CR").Add("CL").Add("D").Add("M").SetIndex(0);
	StationEditor.tx10Zch.Clear();
	StationEditor.tx10Zch.Add("0").Add("1").SetIndex(0);
	StationEditor.tx8B2ref.Clear();
	StationEditor.tx8B2ref.Add("E").Add("I").SetIndex(0);
	
	StationEditor.tx2Cdate.Format("%3:02d/%2:02d/%1:4d");
	StationEditor.tx2Wdate.Format("%3:02d/%2:02d/%1:4d");
	StationEditor.tx2Zdate.Format("%3:02d/%2:02d/%1:4d");
	
	StationEditor.table.Clear();
	StationEditor.table.AddColumn("№",0, 10);
	StationEditor.table.AddColumn("Station name",20);
	
	StationEditor.table.OddRowColor(Color(255,255,191), Black());
	StationEditor.table.EvenRowColor(Color(220,236,249), Black());
	
	StationEditor.btnSave << [=] {
		int num = StationEditor.table.GetCursor();
		if (num<0) return;
		StationSaveToFile(num);
		RefreshListOfStations();
		LoadStation(num);
	};
	
	StationEditor.BtnAdd << [=] {
		if (!FileExists(OpenedFileName)) return;
		//AddEmptyStationToFile(OpenedFileName);
		int num = StationEditor.table.GetCursor();
		if (num < 0) return;

		DuplicateIndexStationToFile(OpenedFileName,num);
		int count = GetCountStationsInFile(OpenedFileName);
		ChangeNameToStation(OpenedFileName,count,"Copy");
		
		RefreshListOfStations();
		LoadStation(GetCountStationsInFile(OpenedFileName)-1);
	};
	StationEditor.BtnDelete << [=] {
		int num = StationEditor.table.GetCursor();
		if (num<0) return;
		StationDeleteFromFile(num);
		RefreshListOfStations();
	};
	StationEditor.table.WhenLeftClick  << [=] {
		int num = StationEditor.table.GetCursor();
		if (num<0) return;
		LoadStation(num);
	};
	StationEditor.table.WhenLeftDouble << StationEditor.table.WhenLeftClick;
	StationEditor.table.WhenCursor << StationEditor.table.WhenLeftClick;
	
}



void MobileService::SetStationDefault(){
	SetStationClear();
	StationEditor.h01 = 50;
	StationEditor.h02.SetText("test content");
	StationEditor.h03.SetText("A");
	StationEditor.h04.SetIndex(0);
	StationEditor.h05.SetText("test@test.com");
	StationEditor.h06.SetText("+1100100200");
	StationEditor.h07.SetText("+1100100333");
	StationEditor.h08.SetText("Test Person");
	StationEditor.h09 = 1;
	Time time = GetSysTime();
	StationEditor.h10.SetDate(time.year,time.month,time.day);
	StationEditor.h11.SetIndex(0);
	StationEditor.h12 = 654321;
	StationEditor.h13.SetText("1.0");
	StationEditor.h14.SetText("");
}
void MobileService::SetStationClear(){
	StationEditor.h01.Clear();
	StationEditor.h02.SetText("");
	StationEditor.h03.SetText("");
	StationEditor.h04.SetIndex(0);
	StationEditor.h05.SetText("");
	StationEditor.h06.SetText("");
	StationEditor.h07.SetText("");
	StationEditor.h08.SetText("");
	StationEditor.h09.Clear();
	Time time = GetSysTime();
	StationEditor.h10.SetDate(time.year,time.month,time.day);
	StationEditor.h11.SetIndex(0);
	StationEditor.h12.Clear();
	StationEditor.h13.SetText("1.0");
	StationEditor.h14.SetText("");
}

#endif
