#ifndef _GisImplementation_gisMap_h_
#define _GisImplementation_gisMap_h_

#include "GisObjects.h"
#include "HcmTerrain.h"


struct GisOptions {
	bool showTopo = false;
	bool showMorpho = false;
	bool showDegreeLines = false;
	bool showGoogleHybrid = false;
	bool showGoogle = false;
};

struct MapStation {
	String Name = "";
	double Longitude, Latitude = 0;
	double Azimut, Radius = 0;
	bool isRef = false;
};

class GisMap : public Ctrl {
	private:
		const double MAX_LATITUDE = 85.05; // spherical mercator restriction
		const double MAX_LONGITUDE = 180.0; // spherical mercator restriction
		const int MAX_ZOOM = 20;
		bool isDragging = false;
		bool isDistShowing = false;
		bool flagDrawHeightMap = false;
		Point PointCenterPx, PointMouseUpPx, PointCenterHeigthMap, PointMovePx, PointDist = Point(0.0, 0.0);
		
		int pxInCm = 52; // TODO: need to check this in other systems
		double metresIn1px = 0;
		int width, height;
		
		
		
		Event<> downloadComplete;
		Time latTimeUpdate = GetSysTime();
		
		double RoundDigit(double digit, int places);
		
		void Paint(Draw& w);
		void DrawDistance(Draw& w);
		void DrawCountries(Draw& w);
		void DrawClosedBorders(Draw& w);
		void DrawTopoRect(Draw& w);
		void DrawStations(Draw& w);
		void DrawTopoRect(bool isTopo, Draw& w);
		void DrawHeightMap(Draw& w);
		
		void DrawLines(Draw& w);
		void DrawOrder(Draw& w);	
		void DrawGoogle(Draw& w);
		
		void DrawProfile(Draw& w);
		
		void MouseWheel(Point p, int zdelta, dword keyflags);
		void LeftDrag(Point pos, dword flags);
		void LeftUp(Point pos, dword flags);
		void LeftDouble(Point pos, dword flags);
		
		void MouseMove(Point p, dword keyflag);
			
		Color GetRandomColor60(int Seed = -1);
		Color GetColorForCountry(const String& country);
		Color GetHeightColor(double Height);
		Color GetMorphoColor(int moprho);
		void CheckMapCenter();
	public:
		Pointf PointMouseDownPx, PointCenterGrad = Point(0.0, 0.0);
		Pointf PointMoveGrad = Point(0.0, 0.0);
		Array<MapStation> MapStation;
		int zoom = 8;
		int lastTopo = 0;
		int lastMorpho = 0;
		GisOptions options;
		GeoData geodata;
		HcmTerrain ter;
		
		
		Pointf WebMercatorToXY(Pointf pnt);
		Pointf XyToWebMercator(Pointf pnt);
		Pointf DegToPx(Pointf pnt);
		Pointf PxToDeg(Pointf pnt);
		
		void ZoomChange(int zm);
			
		
		void ToCenter();
		void LookAt( double longitude, double latitide, int zoom = -1 );
		
		GisMap();
};

#endif
